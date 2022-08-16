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
#include "ecal_descgate.h"
#include <assert.h>
#include <algorithm>
#include <mutex>

namespace eCAL
{
  CDescGate::CDescGate()  = default;
  CDescGate::~CDescGate() = default;

  void CDescGate::Create()
  {
  }

  void CDescGate::Destroy()
  {
  }

  void CDescGate::ApplyTopicDescription(const std::string& topic_name_, const std::string& topic_type_, const std::string& topic_desc_)
  {
    std::unique_lock<std::shared_timed_mutex> lock(m_topic_name_desc_sync);
    TopicNameDescMapT::iterator iter = m_topic_name_desc_map.find(topic_name_);
    
    // new element (no need to check anything, just add it)
    if(iter == m_topic_name_desc_map.end())
    {
      STypeDesc type_desc;
      type_desc.set_type(topic_type_);
      type_desc.set_desc(topic_desc_);
      m_topic_name_desc_map[topic_name_] = type_desc;
    }
    else
    {
      // existing type for the same topic name should be equal !!
      // we log the error only one time
      auto& type_desc = iter->second;
      if(  !type_desc.match_fail
        && !topic_type_.empty()
        && !type_desc.type.empty()
        && (type_desc.type != topic_type_)
        )
      {
        std::string ttype1 = type_desc.type;
        std::string ttype2 = topic_type_;
        std::replace(ttype1.begin(), ttype1.end(), '\0', '?');
        std::replace(ttype1.begin(), ttype1.end(), '\t', '?');
        std::replace(ttype2.begin(), ttype2.end(), '\0', '?');
        std::replace(ttype2.begin(), ttype2.end(), '\t', '?');
        type_desc.match_fail = true;
        std::string msg = "eCAL Pub/Sub type mismatch for topic ";
        msg += topic_name_;
        msg += " (\'";
        msg += ttype1;
        msg += "\' <> \'";
        msg += ttype2;
        msg += "\')";
        eCAL::Logging::Log(log_level_warning, msg);
      }

      // existing description for the same topic name should be equal !!
      // we log the warning only one time
      if ( !type_desc.match_fail
        && !topic_desc_.empty()
        && !type_desc.desc.empty()
        && (type_desc.desc != topic_desc_)
        )
      {
        type_desc.match_fail = true;
        std::string msg = "eCAL Pub/Sub description mismatch for topic ";
        msg += topic_name_;
        eCAL::Logging::Log(log_level_warning, msg);
      }

      type_desc.set_type(topic_type_);
      type_desc.set_desc(topic_desc_);
    }
  }

  bool CDescGate::GetTopicTypeName(const std::string& topic_name_, std::string& topic_type_)
  {
    if(topic_name_.empty()) return(false);

    std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_desc_sync);
    TopicNameDescMapT::iterator iter = m_topic_name_desc_map.find(topic_name_);

    if(iter == m_topic_name_desc_map.end()) return(false);
    topic_type_ = iter->second.type;
    return(true);
  }

  bool CDescGate::GetTopicDescription(const std::string& topic_name_, std::string& topic_desc_)
  {
    if(topic_name_.empty()) return(false);

    std::shared_lock<std::shared_timed_mutex> lock(m_topic_name_desc_sync);
    TopicNameDescMapT::iterator iter = m_topic_name_desc_map.find(topic_name_);

    if(iter == m_topic_name_desc_map.end()) return(false);
    topic_desc_ = iter->second.desc;
    return(true);
  }
  
  void CDescGate::ApplyServiceDescription(const std::string& service_name_, const std::string& method_name_, const std::string& req_type_name_, const std::string& req_type_desc_, const std::string& resp_type_name_, const std::string& resp_type_desc_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    std::shared_lock<std::shared_timed_mutex> lock(m_service_method_desc_sync);
    ServiceMethodDescMapT::iterator iter = m_service_method_desc_map.find(service_method_tuple);
    if (iter == m_service_method_desc_map.end())
    {
      STypeDesc req_typedesc;
      req_typedesc.set_type(req_type_name_);
      req_typedesc.set_desc(req_type_desc_);

      STypeDesc resp_typedesc;
      resp_typedesc.set_type(resp_type_name_);
      resp_typedesc.set_desc(resp_type_desc_);

      m_service_method_desc_map[service_method_tuple] = std::make_tuple(req_typedesc, resp_typedesc);;
    }
    else
    {
      // do we need to check consistency ?
    }
  }

  bool CDescGate::GetServiceTypeNames(const std::string& service_name_, const std::string& method_name_, std::string& req_type_name_, std::string& resp_type_name_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    std::shared_lock<std::shared_timed_mutex> lock(m_service_method_desc_sync);
    ServiceMethodDescMapT::iterator iter = m_service_method_desc_map.find(service_method_tuple);

    if (iter == m_service_method_desc_map.end()) return false;
    auto req_typedesc_tuple = *iter;
    req_type_name_  = std::get<0>(req_typedesc_tuple.second).type;
    resp_type_name_ = std::get<1>(req_typedesc_tuple.second).type;

    return true;
  }

  bool CDescGate::GetServiceDescription(const std::string& service_name_, const std::string& method_name_, std::string& req_type_desc_, std::string& resp_type_desc_)
  {
    std::tuple<std::string, std::string> service_method_tuple = std::make_tuple(service_name_, method_name_);

    std::shared_lock<std::shared_timed_mutex> lock(m_service_method_desc_sync);
    ServiceMethodDescMapT::iterator iter = m_service_method_desc_map.find(service_method_tuple);

    if (iter == m_service_method_desc_map.end()) return false;
    auto req_typedesc_tuple = *iter;
    req_type_desc_  = std::get<0>(req_typedesc_tuple.second).desc;
    resp_type_desc_ = std::get<1>(req_typedesc_tuple.second).desc;

    return true;
  }
};
