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

#include "ecal_pubgate.h"
#include "ecal_sample_to_topicinfo.h"
#include "ecal_globals.h"

#include <atomic>
#include <iterator>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>

namespace
{
  // TODO: remove me with new CDescGate
  bool ApplyTopicDescription(const std::string& topic_name_, const eCAL::SDataTypeInformation& topic_info_)
  {
    if (eCAL::g_descgate() != nullptr)
    {
      // Calculate the quality of the current info
      eCAL::CDescGate::QualityFlags quality = eCAL::CDescGate::QualityFlags::NO_QUALITY;
      if (!topic_info_.name.empty() || !topic_info_.encoding.empty())
        quality |= eCAL::CDescGate::QualityFlags::TYPE_AVAILABLE;
      if (!topic_info_.descriptor.empty())
        quality |= eCAL::CDescGate::QualityFlags::DESCRIPTION_AVAILABLE;
      quality |= eCAL::CDescGate::QualityFlags::INFO_COMES_FROM_CORRECT_ENTITY;

      return eCAL::g_descgate()->ApplyTopicDescription(topic_name_, topic_info_, quality);
    }
    return false;
  }
}

namespace eCAL
{
  //////////////////////////////////////////////////////////////////
  // CPubGate
  //////////////////////////////////////////////////////////////////
  std::atomic<bool> CPubGate::m_created;
  CPubGate::CPubGate() :
    m_share_type(true),
    m_share_desc(true)
  {
  }

  CPubGate::~CPubGate()
  {
    Destroy();
  }

  void CPubGate::Create()
  {
    if(m_created) return;
    m_created = true;
  }

  void CPubGate::Destroy()
  {
    if(!m_created) return;

    // destroy all remaining publisher
    const std::unique_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    for (auto iter = m_topic_name_datawriter_map.begin(); iter != m_topic_name_datawriter_map.end(); ++iter)
    {
      iter->second->Destroy();
    }

    m_created = false;
  }

  void CPubGate::ShareType(bool state_)
  {
    m_share_type = state_;
  }

  void CPubGate::ShareDescription(bool state_)
  {
    m_share_desc = state_;
  }

  bool CPubGate::Register(const std::string& topic_name_, const std::shared_ptr<CDataWriter>& datawriter_)
  {
    if(!m_created) return(false);

    // register writer and multicast group
    const std::unique_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    m_topic_name_datawriter_map.emplace(std::pair<std::string, std::shared_ptr<CDataWriter>>(topic_name_, datawriter_));

    return(true);
  }

  bool CPubGate::Unregister(const std::string& topic_name_, const std::shared_ptr<CDataWriter>& datawriter_)
  {
    if(!m_created) return(false);
    bool ret_state = false;

    const std::unique_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    auto res = m_topic_name_datawriter_map.equal_range(topic_name_);
    for(auto iter = res.first; iter != res.second; ++iter)
    {
      if(iter->second == datawriter_)
      {
        m_topic_name_datawriter_map.erase(iter);
        ret_state = true;
        break;
      }
    }

    return(ret_state);
  }

  void CPubGate::ApplyLocSubRegistration(const Registration::Sample& ecal_sample_)
  {
    if(!m_created) return;

    const auto& ecal_sample = ecal_sample_.topic;
    const std::string& topic_name = ecal_sample.tname;
    CDataWriter::SLocalSubscriptionInfo subscription_info;
    subscription_info.topic_id = ecal_sample.tid;
    subscription_info.process_id = std::to_string(ecal_sample.pid);
    const SDataTypeInformation topic_information{ eCALSampleToTopicInformation(ecal_sample_) };

    std::string reader_par;
#if 0
    for (const auto& layer : ecal_sample.tlayer())
    {
      // layer parameter as protobuf message
      // this parameter is not used at all currently
      // for local subscriber registrations
      reader_par = layer.par_layer().SerializeAsString();
    }
#endif

    // store description
    ApplyTopicDescription(topic_name, topic_information);

    // register local subscriber
    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    auto res = m_topic_name_datawriter_map.equal_range(topic_name);
    for(TopicNameDataWriterMapT::const_iterator iter = res.first; iter != res.second; ++iter)
    {
      iter->second->ApplyLocSubscription(subscription_info, topic_information, reader_par);
    }
  }

  void CPubGate::ApplyLocSubUnregistration(const Registration::Sample& ecal_sample_)
  {
    if (!m_created) return;

    const auto& ecal_sample = ecal_sample_.topic;
    const std::string& topic_name = ecal_sample.tname;
    CDataWriter::SLocalSubscriptionInfo subscription_info;
    subscription_info.topic_id = ecal_sample.tid;
    subscription_info.process_id = std::to_string(ecal_sample.pid);

    // unregister local subscriber
    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    auto res = m_topic_name_datawriter_map.equal_range(topic_name);
    for (TopicNameDataWriterMapT::const_iterator iter = res.first; iter != res.second; ++iter)
    {
      iter->second->RemoveLocSubscription(subscription_info);
    }
  }

  void CPubGate::ApplyExtSubRegistration(const Registration::Sample& ecal_sample_)
  {
    if(!m_created) return;

    const auto& ecal_sample = ecal_sample_.topic;
    const std::string& topic_name = ecal_sample.tname;
    CDataWriter::SExternalSubscriptionInfo subscription_info;
    subscription_info.host_name  = ecal_sample.hname;
    subscription_info.topic_id   = ecal_sample.tid;
    subscription_info.process_id = std::to_string(ecal_sample.pid);
    const SDataTypeInformation topic_information{ eCALSampleToTopicInformation(ecal_sample_) };

    std::string reader_par;
#if 0
    for (const auto& layer : ecal_sample.tlayer())
    {
      // layer parameter as protobuf message
      // this parameter is not used at all currently
      // for external subscriber registrations
      reader_par = layer.par_layer().SerializeAsString();
    }
#endif

    // store description
    ApplyTopicDescription(topic_name, topic_information);

    // register external subscriber
    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    auto res = m_topic_name_datawriter_map.equal_range(topic_name);
    for(TopicNameDataWriterMapT::const_iterator iter = res.first; iter != res.second; ++iter)
    {
      iter->second->ApplyExtSubscription(subscription_info, topic_information, reader_par);
    }
  }

  void CPubGate::ApplyExtSubUnregistration(const Registration::Sample& ecal_sample_)
  {
    if (!m_created) return;

    const auto& ecal_sample = ecal_sample_.topic;
    const std::string& topic_name = ecal_sample.tname;
    CDataWriter::SExternalSubscriptionInfo subscription_info;
    subscription_info.host_name = ecal_sample.hname;
    subscription_info.topic_id = ecal_sample.tid;
    subscription_info.process_id = std::to_string(ecal_sample.pid);

    // unregister external subscriber
    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    auto res = m_topic_name_datawriter_map.equal_range(topic_name);
    for (TopicNameDataWriterMapT::const_iterator iter = res.first; iter != res.second; ++iter)
    {
      iter->second->RemoveExtSubscription(subscription_info);
    }
  }

  void CPubGate::RefreshRegistrations()
  {
    if (!m_created) return;

    // refresh publisher registrations
    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_datawriter_sync);
    for (const auto& iter : m_topic_name_datawriter_map)
    {
      iter.second->RefreshRegistration();
    }
  }
}
