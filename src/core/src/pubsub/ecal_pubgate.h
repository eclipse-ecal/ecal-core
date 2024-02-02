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
 * @brief  eCAL publisher gateway class
**/

#pragma once

#include "readwrite/ecal_writer.h"
#include "serialization/ecal_struct_sample_registration.h"

#include <atomic>
#include <shared_mutex>
#include <memory>
#include <string>
#include <unordered_map>

namespace eCAL
{
  class CPubGate
  {
  public:
    CPubGate();
    ~CPubGate();

    void Create();
    void Destroy();

    void ShareType(bool state_);
    bool TypeShared() const { return m_share_type; };

    void ShareDescription(bool state_);
    bool DescriptionShared() const { return m_share_desc; };

    bool Register(const std::string& topic_name_, const std::shared_ptr<CDataWriter>& datawriter_);
    bool Unregister(const std::string& topic_name_, const std::shared_ptr<CDataWriter>& datawriter_);

    void ApplyLocSubRegistration(const Registration::Sample& ecal_sample_);
    void ApplyLocSubUnregistration(const Registration::Sample& ecal_sample_);

    void ApplyExtSubRegistration(const Registration::Sample& ecal_sample_);
    void ApplyExtSubUnregistration(const Registration::Sample& ecal_sample_);

    void RefreshRegistrations();

  protected:
    static std::atomic<bool>  m_created;
    bool                      m_share_type;
    bool                      m_share_desc;

    using TopicNameDataWriterMapT = std::multimap<std::string, std::shared_ptr<CDataWriter>>;
    std::shared_timed_mutex   m_topic_name_datawriter_sync;
    TopicNameDataWriterMapT   m_topic_name_datawriter_map;
  };
}
