/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
*/

/**
 * @brief  eCAL map with time expiration
**/

#pragma once

#include <chrono>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace eCAL
{
  namespace Util
  {
    /**
    * @brief A time expiration map
    **/
    template<class Key,
      class T,
      class Compare = std::less<Key>,
      class Alloc = std::allocator<std::pair<const Key, T> > >
      class CExpMap
    {
    public:
      using clock_type = std::chrono::steady_clock;

      // Key access history, most recent at back 
      using key_tracker_type = std::list<std::pair<clock_type::time_point, Key>>;
      // Key to value and key history iterator 
      using key_to_value_type = std::map<Key, std::pair<T, typename key_tracker_type::iterator>>;

      using allocator_type = Alloc;
      using value_type = std::pair<const Key, T>;
      using reference = typename Alloc::reference;
      using const_reference = typename Alloc::const_reference;
      using difference_type = typename Alloc::difference_type;
      using size_type = typename Alloc::size_type;
      using key_type = Key;
      using mapped_type = T;

      class iterator : public std::iterator<std::bidirectional_iterator_tag, std::pair<Key, T>>
      {
        friend class const_iterator;

      public:
        iterator(const typename key_to_value_type::iterator _it)
          : it(_it)
        {}

        iterator& operator++()
        {
          it++;
          return *this;
        } //prefix increment

        iterator& operator--()
        {
          it--;
          return *this;
        } //prefix decrement

        std::pair<Key, T> operator*() const
        {
          return std::make_pair(it->first, it->second.first);
        }
        
        //friend void swap(iterator& lhs, iterator& rhs); //C++11 I think
        bool operator==(const iterator& rhs) const { return it == rhs.it; }
        bool operator!=(const iterator& rhs) const { return it != rhs.it; }

      private:
        typename key_to_value_type::iterator it;
       };

      class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const std::pair<Key, T>>
      {
      public:
        const_iterator(const iterator& other)
          : it(other.it)
        {}

        const_iterator(const typename key_to_value_type::const_iterator _it)
          : it(_it)
        {}
        
        const_iterator& operator++()
        {
          it++;
          return *this;
        } //prefix increment
        
        const_iterator& operator--()
        {
          it--;
          return *this;
        } //prefix decrement
          //reference operator*() const
        
        std::pair<Key, T> operator*() const
        {
          return std::make_pair(it->first, it->second.first);
        }
        
        //friend void swap(iterator& lhs, iterator& rhs); //C++11 I think
        bool operator==(const const_iterator& rhs) const { return it == rhs.it; }
        bool operator!=(const const_iterator& rhs) const { return it != rhs.it; }

      private:
        typename key_to_value_type::const_iterator it;
      };


      // Constructor specifies the timeout of the map
      CExpMap() : _timeout(std::chrono::milliseconds(5000)) {};
      CExpMap(clock_type::duration t) : _timeout(t) {};

      /**
      * @brief  set expiration time
      **/
      void set_expiration(clock_type::duration t) { _timeout = t; };

      // Iterators:
      iterator begin() noexcept
      {
        return iterator(_key_to_value.begin());
      }

      iterator end() noexcept
      {
        return iterator(_key_to_value.end());
      }

      const_iterator begin() const noexcept
      {
        return const_iterator(_key_to_value.begin());
      }

      const_iterator end() const noexcept
      {
        return const_iterator(_key_to_value.end());
      }

      // Const begin and end functions
      const_iterator cbegin() const noexcept {
        return const_iterator(_key_to_value.cbegin());
      }

      const_iterator cend() const noexcept {
        return const_iterator(_key_to_value.cend());
      }

      // Capacity
      bool empty() const noexcept
      {
        return _key_to_value.empty();
      }

      size_type size() const noexcept
      {
        return _key_to_value.size();
      }

      size_type max_size() const noexcept
      {
        return  _key_to_value.max_size();
      }

      // Element access
      // Obtain value of the cached function for k 
      T& operator[](const Key& k)
      {
        // Attempt to find existing record 
        typename key_to_value_type::iterator it
          = _key_to_value.find(k);

        if (it == _key_to_value.end())
        {
          // We don't have it: 
          // Evaluate function and create new record 
          T v{};
          auto ret = insert(k, v);
          it = ret.first;
        }
        else
        {
          // We do have it: 
          // Update access record by moving 
          // accessed key to back of list 
          update_timestamp(k);
        }

        // Return the retrieved value 
        return (*it).second.first;
      };

      mapped_type& at(const key_type& k)
      {
        return _key_to_value.at(k).first;
      }

      const mapped_type& at(const key_type& k) const
      {
        return _key_to_value.at(k).first;
      }

      // Modifiers
      std::pair<iterator, bool> insert(const value_type& val)
      {
        auto result = insert(val.first, val.second);
        return std::make_pair(iterator(result.first), result.second);
      }

      // Operations
      iterator find(const key_type& k)
      {
        return iterator(_key_to_value.find(k));
      }

      const_iterator find(const Key& k) const
      {
        return const_iterator(_key_to_value.find(k));
      }

      // Purge the timed out elements from the cache 
      void remove_deprecated(std::list<Key>* key_erased = nullptr) //-V826
      {
        // Assert method is never called when cache is empty 
        //assert(!_key_tracker.empty());
        clock_type::time_point eviction_limit = get_curr_time() - _timeout;

        auto it(_key_tracker.begin());

        while (it != _key_tracker.end() && it->first < eviction_limit)
        {
          if (key_erased != nullptr) key_erased->push_back(it->second);
          _key_to_value.erase(it->second); // erase the element from the map 
          it = _key_tracker.erase(it);     // erase the element from the list
        }
      }

      // Remove specific element from the cache
      bool erase(const Key& k)
      {
        auto it = _key_to_value.find(k);
        if (it != _key_to_value.end())
        {
          _key_tracker.erase(it->second.second); // erase the element from the list
          _key_to_value.erase(k);                // erase the element from the map
          return true;
        }
        return false;
      }

      // Remove all elements from the cache 
      void clear()
      {
        // Assert method is never called when cache is empty 
        //assert(!_key_tracker.empty());
        auto it(_key_tracker.begin());

        while (it != _key_tracker.end())
        {
          _key_to_value.erase(it->second); // erase the element from the map 
          it = _key_tracker.erase(it);     // erase the element from the list
        }
      }

    private:

      // Maybe pass the iterator instead of the key? or at least only get k once
      void update_timestamp(const Key& k)
      {
        _key_tracker.erase(_key_to_value.at(k).second);
        auto new_iterator = _key_tracker.emplace(_key_tracker.end(), std::make_pair(get_curr_time(), k));
        _key_to_value.at(k).second = new_iterator;
      }

      // Record a fresh key-value pair in the cache 
      std::pair<typename key_to_value_type::iterator, bool> insert(const Key& k, const T& v)
      {
        // sorted list, containing (pair ( timestamp, K))
        auto it = _key_tracker.emplace(_key_tracker.end(), std::make_pair(get_curr_time(), k));

        // entry mapping k -> pair (T, iterator(pair(timestamp, K)))
        auto ret = _key_to_value.emplace(
          std::make_pair(
            k,
            std::make_pair(v, it)
          )
        );
        // return iterator to newly inserted element.
        return ret;
      }

      clock_type::time_point get_curr_time()
      {
        return clock_type::now();
      }

      // Key access history 
      key_tracker_type _key_tracker;

      // Key-to-value lookup 
      key_to_value_type _key_to_value;

      // Timeout of map
      clock_type::duration _timeout;
    };
  }
}
