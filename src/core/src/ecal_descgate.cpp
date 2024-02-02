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
 * @brief  eCAL description gateway class
**/

#include <ecal/ecal_log.h>
#include <ecal/ecal_config.h>

#include "ecal_descgate.h"
#include <cassert>
#include <algorithm>
#include <mutex>

namespace eCAL
{
  CDescGate::CDescGate() :
    m_topic_info_map  (std::chrono::milliseconds(Config::GetMonitoringTimeoutMs())),
    m_service_info_map(std::chrono::milliseconds(Config::GetMonitoringTimeoutMs()))
  {
  }
  CDescGate::~CDescGate() = default;

  void CDescGate::Create()
  {
  }

  void CDescGate::Destroy()
  {
  }

  bool CDescGate::ApplyTopicDescription(const std::string& topic_name_, const SDataTypeInformation& topic_info_, const QualityFlags description_quality_)
  {
    const std::unique_lock<std::shared_timed_mutex> lock(m_topic_info_map.sync);
    m_topic_info_map.map->remove_deprecated();

    const auto topic_info_it = m_topic_info_map.map->find(topic_name_);
    
    // new element (no need to check anything, just add it)
    if(topic_info_it == m_topic_info_map.map->end())
    {
      // create a new topic entry
      STopicInfoQuality& topic_info = (*m_topic_info_map.map)[topic_name_];
      topic_info.info               = topic_info_;
      topic_info.quality            = description_quality_;
      return true;
    }

    // we do not use the [] operator here to not update the timestamp
    // by accessing the map entry
    // 
    // a topic with the same name but different type name or different description
    // should NOT update the timestamp of an existing entry
    // 
    // otherwise there could be a scenario where a "lower quality topic" would keep a 
    // "higher quality topic" alive (even it is no more existing)
    STopicInfoQuality topic_info = (*topic_info_it).second;

    // first let's check whether the current information has a higher quality
    // if it has a higher quality, we overwrite it
    if (description_quality_ > topic_info.quality)
    {
      // overwrite attributes
      topic_info.info    = topic_info_;
      topic_info.quality = description_quality_;

      // update attributes and return
      (*m_topic_info_map.map)[topic_name_] = topic_info;
      return true;
    }

    // this is the same topic (topic name, topic type name, topic type description)
    if (topic_info.info == topic_info_)
    {
      // update timestamp (by just accessing the entry) and return
      (*m_topic_info_map.map)[topic_name_] = topic_info;
      return false;
    }

    // topic type name or topic description differ but we logged this before
    if (topic_info.type_missmatch_logged)
    {
      return false;
    }

    // topic type name or topic description differ and this is not logged yet
    // so we log the error and update the entry one time
    bool update_topic_info(false);

    // topic type name differs
    // we log the error and update the entry one time
    if (!topic_info_.encoding.empty()
      && !topic_info.info.encoding.empty()
      && (topic_info.info.encoding != topic_info_.encoding)
      )
    {
      std::string tencoding1 = topic_info.info.encoding;
      std::string tencoding2 = topic_info_.encoding;
      std::replace(tencoding1.begin(), tencoding1.end(), '\0', '?');
      std::replace(tencoding1.begin(), tencoding1.end(), '\t', '?');
      std::replace(tencoding2.begin(), tencoding2.end(), '\0', '?');
      std::replace(tencoding2.begin(), tencoding2.end(), '\t', '?');
      std::string msg = "eCAL Pub/Sub encoding mismatch for topic ";
      msg += topic_name_;
      msg += " (\'";
      msg += tencoding1;
      msg += "\' <> \'";
      msg += tencoding2;
      msg += "\')";
      eCAL::Logging::Log(log_level_warning, msg);

      // mark as logged
      topic_info.type_missmatch_logged = true;
      // and update its attributes
      update_topic_info = true;
    }

    // topic type name differs
    // we log the error and update the entry one time
    if (!topic_info_.name.empty()
      && !topic_info.info.name.empty()
      && (topic_info.info.name != topic_info_.name)
      )
    {
      std::string ttype1 = topic_info.info.name;
      std::string ttype2 = topic_info_.name;
      std::replace(ttype1.begin(), ttype1.end(), '\0', '?');
      std::replace(ttype1.begin(), ttype1.end(), '\t', '?');
      std::replace(ttype2.begin(), ttype2.end(), '\0', '?');
      std::replace(ttype2.begin(), ttype2.end(), '\t', '?');
      std::string msg = "eCAL Pub/Sub type mismatch for topic ";
      msg += topic_name_;
      msg += " (\'";
      msg += ttype1;
      msg += "\' <> \'";
      msg += ttype2;
      msg += "\')";
      eCAL::Logging::Log(log_level_warning, msg);

      // mark as logged
      topic_info.type_missmatch_logged = true;
      // and update its attributes
      update_topic_info = true;
    }

    // topic type description differs
    // we log the error and update the entry one time
    if ( !topic_info_.descriptor.empty()
      && !topic_info.info.descriptor.empty()
      && (topic_info.info.descriptor != topic_info_.descriptor)
      )
    {
      std::string msg = "eCAL Pub/Sub description mismatch for topic ";
      msg += topic_name_;
      eCAL::Logging::Log(log_level_warning, msg);

      // mark as logged
      topic_info.type_missmatch_logged = true;
      // and update its attributes
      update_topic_info = true;
    }

    // update topic info attributes
    if (update_topic_info)
    {
      (*m_topic_info_map.map)[topic_name_] = topic_info;
    }

    return false;
  }

  void CDescGate::GetTopics(std::unordered_map<std::string, SDataTypeInformation>& topic_info_map_)
  {
    std::unordered_map<std::string, SDataTypeInformation> map;

    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_info_map.sync);
    m_topic_info_map.map->remove_deprecated();
    map.reserve(m_topic_info_map.map->size());

    for (const auto& topic_info : (*m_topic_info_map.map))
    {
      map.emplace(topic_info.first, topic_info.second.info);
    }
    topic_info_map_.swap(map);
  }

  void CDescGate::GetTopicNames(std::vector<std::string>& topic_names_)
  {
    topic_names_.clear();

    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_info_map.sync);
    m_topic_info_map.map->remove_deprecated();
    topic_names_.reserve(m_topic_info_map.map->size());

    for (const auto& topic_info : (*m_topic_info_map.map))
    {
      topic_names_.emplace_back(topic_info.first);
    }
  }

  bool CDescGate::GetDataTypeInformation(const std::string& topic_name_, SDataTypeInformation& topic_info_)
  {
    if (topic_name_.empty()) return(false);

    const std::shared_lock<std::shared_timed_mutex> lock(m_topic_info_map.sync);
    const auto topic_info_it = m_topic_info_map.map->find(topic_name_);

    if (topic_info_it == m_topic_info_map.map->end()) return(false);
    topic_info_ = (*topic_info_it).second.info;
    return(true);
  }
  
  bool CDescGate::ApplyServiceDescription(const std::string& service_name_
                                        , const std::string& method_name_
                                        , const SDataTypeInformation& request_type_information_
                                        , const SDataTypeInformation& response_type_information_
                                        , const QualityFlags description_quality_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    const std::unique_lock<std::shared_timed_mutex> lock(m_service_info_map.sync);
    m_service_info_map.map->remove_deprecated();

    auto service_info_map_it = m_service_info_map.map->find(service_method_tuple);
    if (service_info_map_it == m_service_info_map.map->end())
    {
      // create a new service entry
      SServiceMethodInfoQuality& service_info = (*m_service_info_map.map)[service_method_tuple];
      service_info.info.request_type   = request_type_information_;
      service_info.info.response_type  = response_type_information_;
      service_info.quality             = description_quality_;
      return true;
    }

    // let's check whether the current information has a higher quality
    // if it has a higher quality, we overwrite it
    bool ret_value(false);
    SServiceMethodInfoQuality service_info = (*service_info_map_it).second;
    if (description_quality_ > service_info.quality)
    {
      service_info.info.request_type   =  request_type_information_;
      service_info.info.response_type  = response_type_information_;
      service_info.quality             = description_quality_;
      ret_value = true;
    }

    // update service entry (and its timestamp)
    (*m_service_info_map.map)[service_method_tuple] = service_info;

    return ret_value;
  }

  void CDescGate::GetServices(std::map<std::tuple<std::string, std::string>, SServiceMethodInformation>& service_info_map_)
  {
    std::map<std::tuple<std::string, std::string>, SServiceMethodInformation> map;

    const std::shared_lock<std::shared_timed_mutex> lock(m_service_info_map.sync);
    m_service_info_map.map->remove_deprecated();

    for (const auto& service_info : (*m_service_info_map.map))
    {
      map.emplace(service_info.first, service_info.second.info);
    }
    service_info_map_.swap(map);
  }

  void CDescGate::GetServiceNames(std::vector<std::tuple<std::string, std::string>>& service_method_names_)
  {
    service_method_names_.clear();

    const std::shared_lock<std::shared_timed_mutex> lock(m_service_info_map.sync);
    m_service_info_map.map->remove_deprecated();
    service_method_names_.reserve(m_service_info_map.map->size());

    for (const auto& service_info : (*m_service_info_map.map))
    {
      service_method_names_.emplace_back(service_info.first);
    }
  }

  bool CDescGate::GetServiceTypeNames(const std::string& service_name_, const std::string& method_name_, std::string& req_type_name_, std::string& resp_type_name_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    const std::shared_lock<std::shared_timed_mutex> lock(m_service_info_map.sync);
    auto service_info_map_it = m_service_info_map.map->find(service_method_tuple);

    if (service_info_map_it == m_service_info_map.map->end()) return false;
    req_type_name_  = (*service_info_map_it).second.info.request_type.name;
    resp_type_name_ = (*service_info_map_it).second.info.response_type.name;
    return true;
  }

  bool CDescGate::GetServiceDescription(const std::string& service_name_, const std::string& method_name_, std::string& req_type_desc_, std::string& resp_type_desc_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    const std::shared_lock<std::shared_timed_mutex> lock(m_service_info_map.sync);
    auto service_info_map_it = m_service_info_map.map->find(service_method_tuple);

    if (service_info_map_it == m_service_info_map.map->end()) return false;
    req_type_desc_  = (*service_info_map_it).second.info.request_type.descriptor;
    resp_type_desc_ = (*service_info_map_it).second.info.response_type.descriptor;
    return true;
  }
}
