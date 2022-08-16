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
 * @brief  eCAL base memory file interface
**/

#pragma once

#include <string>

#include "ecal_memfile_info.h"

namespace eCAL
{
  /**
   * @brief Shared memory file handler class. 
  **/
  class CMemoryFile
  {
  public:
    /**
     * @brief Constructor. 
    **/
    CMemoryFile();

    /**
     * @brief Destructor. 
    **/
    ~CMemoryFile();

    /**
     * @brief Create a new memory file. 
     *
     * @param name_    Unique file name. 
     * @param create_  Add file to system if not exists.
     * @param len_     Number of bytes to allocate (only if create_ == true). 
     *
     * @return  true if it succeeds, false if it fails. 
    **/
    bool Create(const char* name_, const bool create_, const size_t len_ = 0);

    /**
     * @brief Delete the associated memory file from system. 
     *
     * @param remove_  Remove file from system.
     *
     * @return  true if it succeeds, false if it fails. 
    **/
    bool Destroy(const bool remove_);

    /**
     * @brief Get memory file read access. 
     *
     * @param timeout_  The timeout in ms for access via mutex.
     *
     * @return  true if file exists and could be opened with read access. 
    **/
    bool GetReadAccess(const int timeout_);

    /**
     * @brief Release the read access. 
     *
     * @return  true if it succeeds, false if it fails. 
    **/
    bool ReleaseReadAccess();

    /**
     * @brief Get payload buffer pointer from an opened memory file for reading.
     *
     * @param buf_     The destination address.
     * @param len_     Expected length of the available payload.
     *
     * @return         Number of available bytes (or zero if it fails).
    **/
    size_t GetReadAddress(const void*& buf_, const size_t len_);

    /**
     * @brief Read bytes from an opened memory file.
     *
     * @param buf_     The destination address.
     * @param len_     The length of the allocated memory (has to be allocated by caller).
     * @param offset_  The offset where to start reading.
     *
     * @return         Number of copied bytes (or zero if it fails).
    **/
    size_t Read(void* buf_, const size_t len_, const size_t offset_);

    /**
     * @brief Get memory file write access.
     *
     * @param timeout_  The timeout in ms for access via mutex.
     *
     * @return  true if file exists and could be opened with read/write access.
    **/
    bool GetWriteAccess(const int timeout_);

    /**
     * @brief Release the write access.
     *
     * @return  true if it succeeds, false if it fails.
    **/
    bool ReleaseWriteAccess();

    /**
     * @brief Get payload buffer pointer from an opened memory file for writing.
     *
     * @param buf_     The destination address.
     * @param len_     Expected length of the available payload.
     *
     * @return         Number of available bytes (or zero if it fails).
    **/
    size_t GetWriteAddress(void*& buf_, const size_t len_);

    /**
     * @brief Write bytes to the memory file.
     *
     * @param buf_     The source address.
     * @param len_     The number of bytes to write.
     * @param offset_  The offset for writing the data.
     *
     * @return         Number of bytes copied to the memory file.
    **/
    size_t Write(const void* buf_, const size_t len_, const size_t offset_);

    /**
     * @brief Maximum data size of the whole memory file.
     *
     * @return  The size of the data object. 
    **/
    size_t MaxDataSize() const {return static_cast<size_t>(m_header.max_data_size);};

    /**
     * @brief Size of the stored data object (can be smaller than the size
     *        of the available data size.
     *
     * @return  The size of the data object. 
    **/
    size_t CurDataSize()     const {return static_cast<size_t>(m_header.cur_data_size);};

    bool IsCreated()         const {return(m_created);};
    std::string Name()       const {return(m_name);};

    bool IsOpened()          const {return(m_access_state != access_state::closed);};
    bool HasReadAccess()     const {return(m_access_state == access_state::read_access);};
    bool HasWriteAccess()    const {return(m_access_state == access_state::write_access);};

    struct SInternalHeader
    {
      unsigned short int_hdr_size  = sizeof(SInternalHeader);
      unsigned long  cur_data_size = 0;
      unsigned long  max_data_size = 0;
    };

  protected:
    bool GetAccess(int timeout_);

    enum class access_state
    {
      closed,
      read_access,
      write_access
    };
    bool            m_created;
    access_state    m_access_state;
    std::string     m_name;
    SInternalHeader m_header;
    SMemFileInfo    m_memfile_info;

  private:
    CMemoryFile(const CMemoryFile&);                 // prevent copy-construction
    CMemoryFile& operator=(const CMemoryFile&);      // prevent assignment
  };
};
