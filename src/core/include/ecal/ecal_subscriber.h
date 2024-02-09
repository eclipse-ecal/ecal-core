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
 * @file   ecal_subscriber.h
 * @brief  eCAL subscriber interface
**/

#pragma once

#include <cstddef>
#include <ecal/ecal_callback.h>
#include <ecal/ecal_deprecate.h>
#include <ecal/ecal_os.h>
#include <ecal/ecal_types.h>

#include <memory>
#include <set>
#include <string>

namespace eCAL
{
  class CDataReader;

  /**
   * @brief eCAL subscriber class.
   *
   * The CSubscriber class is used to receive topics from matching eCAL publishers.
   * <br>
   * <br>
   * For receiving the topic payload the subscriber class provides a callback interface and overloaded receive methods.
  **/

  /**
   * @code
   *            // create subscriber, topic name "A"
   *            eCAL::CSubscriber sub("A");
   *
   *            //---------------------------------------------------------------------------------------
   *            // receive content with 100 ms timeout as standard string and read receive time stamp
   *            //---------------------------------------------------------------------------------------
   *            std::string rcv_s;
   *            long long time = 0;
   *            size_t rcv_len = sub.Receive(rcv_s, &time, 100);
   *
   *            //---------------------------------------------------------------------------------------
   *            // receive content with a callback functions
   *            //---------------------------------------------------------------------------------------
   *            void OnReceive(const char* topic_name_, const struct eCAL::SReceiveCallbackData* data_)
   *            {
   *              size_t      size = data_->size;
   *              const void* data = data_->buf;
   *              long long   time = data_->time;
   *            }
   *
   *            // add callback
   *            sub.AddReceiveCallback(std::bind(OnReceive, std::placeholders::_1, std::placeholders::_2));
   *
   *            // idle main thread
   *            while(eCAL::Ok())
   *            {
   *              // sleep 100 ms
   *              std::this_thread::sleep_for(std::chrono::milliseconds(100));
   *            }
   * @endcode
  **/

  class CSubscriber
  {
  public:
    /**
     * @brief Constructor. 
    **/
    ECAL_API CSubscriber();

    /**
     * @brief Constructor.
     *
     * @param topic_name_   Unique topic name.
     * @param topic_info_   Topic information (encoding, type, descriptor)
    **/
    ECAL_API CSubscriber(const std::string& topic_name_, const SDataTypeInformation& topic_info_);

    /**
     * @brief Constructor.
     * 
     * @param topic_name_   Unique topic name.
    **/
    ECAL_API explicit CSubscriber(const std::string& topic_name_);

    /**
     * @brief Destructor. 
    **/
    ECAL_API virtual ~CSubscriber();

    /**
     * @brief CSubscribers are non-copyable
    **/
    CSubscriber(const CSubscriber&) = delete;

    /**
     * @brief CSubscribers are non-copyable
    **/
    CSubscriber& operator=(const CSubscriber&) = delete;

    /**
     * @brief CSubscribers are move-enabled
    **/
    ECAL_API CSubscriber(CSubscriber&& rhs) noexcept;

    /**
     * @brief CSubscribers are move-enabled
    **/
    ECAL_API CSubscriber& operator=(CSubscriber&& rhs) noexcept;

    /**
     * @brief Creates this object.
     *
     * @param topic_name_   Unique topic name.
     *
     * @return  True if it succeeds, false if it fails.
    **/
    ECAL_API virtual bool Create(const std::string& topic_name_);

    /**
     * @brief Creates this object.
     *
     * @param topic_name_   Unique topic name.
     * @param topic_info_   Topic information (encoding, type, descriptor)
     *
     * @return  True if it succeeds, false if it fails.
    **/
    ECAL_API virtual bool Create(const std::string& topic_name_, const SDataTypeInformation& topic_info_);

    /**
     * @brief Destroys this object. 
     *
     * @return  true if it succeeds, false if it fails. 
    **/
    ECAL_API virtual bool Destroy();

    /**
     * @brief Set a set of id's to prefiltering topics (see CPublisher::SetID).
     *
     * @param id_set_  Set of id's.
     *
     * @return  True if it succeeds, false if it fails.
    **/
    ECAL_API bool SetID(const std::set<long long>& id_set_);

    /**
     * @brief Sets subscriber attribute. 
     *
     * @param attr_name_   Attribute name. 
     * @param attr_value_  Attribute value. 
     *
     * @return  True if it succeeds, false if it fails. 
     * @experimental
    **/
    ECAL_API bool SetAttribute(const std::string& attr_name_, const std::string& attr_value_);

    /**
     * @brief Removes subscriber attribute. 
     *
     * @param attr_name_   Attribute name. 
     *
     * @return  True if it succeeds, false if it fails.
    **/
    ECAL_API bool ClearAttribute(const std::string& attr_name_);

    /**
     * @brief Receive a message from the publisher (able to process zero length buffer).
     *
     * @param [out] buf_    Standard string for copying message content.
     * @param [out] time_   Time from publisher in us (default = nullptr).
     * @param rcv_timeout_  Maximum time before receive operation returns (in milliseconds, -1 means infinite).
     *
     * @return  True if it succeeds, false if it fails.
    **/
    ECAL_API bool ReceiveBuffer(std::string& buf_, long long* time_ = nullptr, int rcv_timeout_ = 0) const;

    /**
     * @brief Add callback function for incoming receives. 
     *
     * @param callback_  The callback function to add.
     *
     * @return  True if succeeded, false if not. 
    **/
    ECAL_API bool AddReceiveCallback(ReceiveCallbackT callback_);

    /**
     * @brief Remove callback function for incoming receives. 
     *
     * @return  True if succeeded, false if not. 
    **/
    ECAL_API virtual bool RemReceiveCallback();

    /**
     * @brief Add callback function for subscriber events.
     *
     * @param type_      The event type to react on.
     * @param callback_  The callback function to add.
     *
     * @return  True if succeeded, false if not.
    **/
    ECAL_API bool AddEventCallback(eCAL_Subscriber_Event type_, SubEventCallbackT callback_);

    /**
     * @brief Remove callback function for subscriber events.
     *
     * @param type_  The event type to remove.
     *
     * @return  True if succeeded, false if not.
    **/
    ECAL_API bool RemEventCallback(eCAL_Subscriber_Event type_);

    /**
     * @brief Query if this object is created. 
     *
     * @return  true if created, false if not. 
    **/
    ECAL_API bool IsCreated() const {return(m_created);}

    /**
     * @brief Query the number of publishers.
     *
     * @return  Number of publishers.
    **/
    ECAL_API size_t GetPublisherCount() const;

    /**
     * @brief Gets name of the connected topic. 
     *
     * @return  The topic name. 
    **/
    ECAL_API std::string GetTopicName() const;

    /**
     * @brief Gets description of the connected topic.
     *
     * @return  The topic information.
    **/
    ECAL_API virtual SDataTypeInformation GetDataTypeInformation() const;

    /**
     * @brief Dump the whole class state into a string. 
     *
     * @param indent_  Indentation used for dump. 
     *
     * @return  The dump sting. 
    **/
    ECAL_API std::string Dump(const std::string& indent_ = "") const;

  protected:
    // class members
    std::shared_ptr<CDataReader>     m_datareader;
    bool                             m_created;
    bool                             m_initialized;
  };
}
