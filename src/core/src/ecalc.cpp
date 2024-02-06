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
 * @brief  Implementation of the eCAL dll interface
**/

#include <mutex>

#include <ecal/ecal.h>
#include <ecal/ecalc.h>

static int CopyBuffer(void* target_, int target_len_, const std::string& source_s_)
{
  if(target_ == nullptr) return(0);
  if(source_s_.empty())  return(0);
  if(target_len_ == ECAL_ALLOCATE_4ME)
  {
    void* buf_alloc = malloc(source_s_.size());
    if(buf_alloc == nullptr) return(0);
    int copied = CopyBuffer(buf_alloc, static_cast<int>(source_s_.size()), source_s_);
    if(copied > 0)
    {
      *((void**)target_) = buf_alloc; //-V206
      return(copied);
    }
    else
    {
      // copying buffer failed, so free allocated memory.
      free(buf_alloc);
    }
  }
  else
  {
    if(target_len_ < static_cast<int>(source_s_.size())) return(0);
    memcpy(target_, source_s_.data(), source_s_.size());
    return(static_cast<int>(source_s_.size()));
  }
  return(0);
}

/////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////
extern "C"
{
  ECALC_API const char* eCAL_GetVersionString()
  {
    return(ECAL_VERSION);
  }

  ECALC_API const char* eCAL_GetVersionDateString()
  {
    return(ECAL_DATE);
  }

  ECALC_API int eCAL_GetVersion(int* major_, int* minor_, int* patch_)
  {
    if((major_ == nullptr) || (minor_ == nullptr) || (patch_ == nullptr)) return(-1); 
    *major_ = ECAL_VERSION_MAJOR;
    *minor_ = ECAL_VERSION_MINOR;
    *patch_ = ECAL_VERSION_PATCH;
    return(0);
  }

  ECALC_API int eCAL_Initialize(int argc_, char **argv_, const char *unit_name_, unsigned int components_)
  {
    return(eCAL::Initialize(argc_, argv_, unit_name_, components_));
  }

  ECALC_API int eCAL_SetUnitName(const char *unit_name_)
  {
    return(eCAL::SetUnitName(unit_name_));
  }

  ECALC_API int eCAL_Finalize(unsigned int components_)
  {
    return(eCAL::Finalize(components_));
  }

  ECALC_API int eCAL_IsInitialized(unsigned int component_)
  {
    return(eCAL::IsInitialized(component_));
  }

  ECALC_API int eCAL_Ok()
  {
    return(eCAL::Ok());
  }

  ECALC_API void eCAL_FreeMem(void* mem_)
  {
    free(mem_);
  }
}

/////////////////////////////////////////////////////////
// Util
/////////////////////////////////////////////////////////
extern "C"
{
#if ECAL_CORE_MONITORING
  ECALC_API void eCAL_Util_ShutdownUnitName(const char* unit_name_)
  {
    std::string unit_name = unit_name_;
    eCAL::Util::ShutdownProcess(unit_name);
  }

  ECALC_API void eCAL_Util_ShutdownProcessID(int process_id_)
  {
    eCAL::Util::ShutdownProcess(process_id_);
  }

  ECALC_API void eCAL_Util_ShutdownProcesses()
  {
    eCAL::Util::ShutdownProcesses();
  }

  ECALC_API void eCAL_Util_ShutdownCore()
  {
    eCAL::Util::ShutdownCore();
  }
#endif // ECAL_CORE_MONITORING

  ECALC_API void eCAL_Util_EnableLoopback(int state_)
  {
    eCAL::Util::EnableLoopback(state_ != 0);
  }

  ECALC_API int eCAL_Util_GetTopicTypeName(const char* topic_name_, void* topic_type_, int topic_type_len_)
  {
    if(!topic_name_) return(0);
    if(!topic_type_) return(0);
    eCAL::SDataTypeInformation topic_info;
    if(eCAL::Util::GetTopicDataTypeInformation(topic_name_, topic_info))
    {
      return(CopyBuffer(topic_type_, topic_type_len_, topic_info.name));
    }
    return(0);
  }

  ECALC_API int eCAL_Util_GetTopicEncoding(const char* topic_name_, void* topic_encoding_, int topic_encoding_len_)
  {
    if (!topic_name_)     return(0);
    if (!topic_encoding_) return(0);
    eCAL::SDataTypeInformation topic_info;
    if (eCAL::Util::GetTopicDataTypeInformation(topic_name_, topic_info))
    {
      return(CopyBuffer(topic_encoding_, topic_encoding_len_, topic_info.encoding));
    }
    return(0);
  }

  ECALC_API int eCAL_Util_GetTopicDescription(const char* topic_name_, void* topic_desc_, int topic_desc_len_)
  {
    if(!topic_name_) return(0);
    if(!topic_desc_) return(0);
    eCAL::SDataTypeInformation topic_info;
    if (eCAL::Util::GetTopicDataTypeInformation(topic_name_, topic_info))
    {
      return(CopyBuffer(topic_desc_, topic_desc_len_, topic_info.descriptor));
    }
    return(0);
  }

  ECALC_API int eCAL_Util_GetServiceResponseTypeName(const char* service_name_, const char* method_name_, void* resp_type_, int resp_type_len_)
  {
    if (!service_name_) return(0);
    if (!method_name_)  return(0);
    if (!resp_type_)    return(0);
    std::string req_type, resp_type;
    if (eCAL::Util::GetServiceTypeNames(service_name_, method_name_, req_type, resp_type))
    {
      return(CopyBuffer(resp_type_, resp_type_len_, resp_type));
    }
    return 0;
  }

  ECALC_API int eCAL_Util_GetServiceRequestDescription(const char* service_name_, const char* method_name_, void* req_desc_, int req_desc_len_)
  {
    if (!service_name_) return(0);
    if (!method_name_)  return(0);
    if (!req_desc_)     return(0);
    std::string req_desc, resp_desc;
    if (eCAL::Util::GetServiceDescription(service_name_, method_name_, req_desc, resp_desc))
    {
      return(CopyBuffer(req_desc_, req_desc_len_, req_desc));
    }
    return 0;
  }

  ECALC_API int eCAL_Util_GetServiceResponseDescription(const char* service_name_, const char* method_name_, void* resp_desc_, int resp_desc_len_)
  {
    if (!service_name_) return(0);
    if (!method_name_)  return(0);
    if (!resp_desc_)    return(0);
    std::string req_desc, resp_desc;
    if (eCAL::Util::GetServiceDescription(service_name_, method_name_, req_desc, resp_desc))
    {
      return(CopyBuffer(resp_desc_, resp_desc_len_, resp_desc));
    }
    return 0;
  }
}

/////////////////////////////////////////////////////////
// Process
/////////////////////////////////////////////////////////
extern "C"
{
  ECALC_API void eCAL_Process_DumpConfig()
  {
    eCAL::Process::DumpConfig();
  }

  ECALC_API int eCAL_Process_GetHostName(void* name_, int name_len_)
 {
    std::string name = eCAL::Process::GetHostName();
    if(!name.empty())
    {
      return(CopyBuffer(name_, name_len_, name));
    }
    return(0);
  }

  ECALC_API int eCAL_Process_GetUnitName(void* name_, int name_len_)
  {
    std::string name = eCAL::Process::GetUnitName();
    if(!name.empty())
    {
      return(CopyBuffer(name_, name_len_, name));
    }
    return(0);
  }

  ECALC_API int eCAL_Process_GetTaskParameter(void* par_, int par_len_, const char* sep_)
  {
    std::string par = eCAL::Process::GetTaskParameter(sep_);
    if(!par.empty())
    {
      return(CopyBuffer(par_, par_len_, par));
    }
    return(0);
  }

  ECALC_API void eCAL_Process_SleepMS(long time_ms_)
  {
    eCAL::Process::SleepMS(time_ms_);
  }

  ECALC_API int eCAL_Process_GetProcessID()
  {
    return(eCAL::Process::GetProcessID());
  }

  ECALC_API int eCAL_Process_GetProcessName(void* name_, int name_len_)
  {
    std::string name = eCAL::Process::GetProcessName();
    if(!name.empty())
    {
      return(CopyBuffer(name_, name_len_, name));
    }
    return(0);
  }

  ECALC_API int eCAL_Process_GetProcessParameter(void* par_, int par_len_)
  {
    std::string par = eCAL::Process::GetProcessParameter();
    if(!par.empty())
    {
      return(CopyBuffer(par_, par_len_, par));
    }
    return(0);
  }

  ECALC_API void eCAL_Process_SetState(enum eCAL_Process_eSeverity severity_, enum eCAL_Process_eSeverity_Level level_, const char* info_)
  {
    eCAL::Process::SetState(severity_, level_, info_);
  }

  ECALC_API int eCAL_Process_StartProcess(const char* proc_name_, const char* proc_args_, const char* working_dir_, int create_console_, enum eCAL_Process_eStartMode process_mode_, int block_)
  {
    return(eCAL::Process::StartProcess(proc_name_, proc_args_, working_dir_, create_console_ != 0, process_mode_, block_ != 0));
  }

  ECALC_API int eCAL_Process_StopProcessName(const char* proc_name_)
  {
    return(eCAL::Process::StopProcess(proc_name_));
  }

  ECALC_API int eCAL_Process_StopProcessID(int proc_id_)
  {
    return(eCAL::Process::StopProcess(proc_id_));
  }
}

/////////////////////////////////////////////////////////
// Logging
/////////////////////////////////////////////////////////
extern "C"
{
  ECALC_API void eCAL_Logging_SetLogLevel(enum eCAL_Logging_eLogLevel level_)
  {
    eCAL::Logging::SetLogLevel(level_);
  }

  ECALC_API enum eCAL_Logging_eLogLevel eCAL_Logging_GetLogLevel()
  {
    return(eCAL::Logging::GetLogLevel());
  }

  ECALC_API void eCAL_Logging_Log(const char* const msg_)
  {
    eCAL::Logging::Log(msg_);
  }
}

/////////////////////////////////////////////////////////
// Publisher
/////////////////////////////////////////////////////////
#if ECAL_CORE_PUBLISHER
static std::recursive_mutex g_pub_callback_mtx;
static void g_pub_event_callback(const char* topic_name_, const struct eCAL::SPubEventCallbackData* data_, const PubEventCallbackCT callback_, void* par_)
{
  const std::lock_guard<std::recursive_mutex> lock(g_pub_callback_mtx);
  SPubEventCallbackDataC data;
  data.type       = data_->type;
  data.time       = data_->time;
  data.clock      = data_->clock;
  data.tid        = data_->tid.c_str();
  data.tname      = data_->tdatatype.name.c_str();
  data.tencoding  = data_->tdatatype.encoding.c_str();
  data.tdesc      = data_->tdatatype.descriptor.c_str();
  callback_(topic_name_, &data, par_);
}

extern "C"
{
  ECALC_API ECAL_HANDLE eCAL_Pub_New()
  {
    auto* pub = new eCAL::CPublisher;
    return(pub);
  }

  ECALC_API int eCAL_Pub_Create(ECAL_HANDLE handle_, const char* topic_name_, const char* topic_type_name_, const char* topic_type_encoding_, const char* topic_desc_, int topic_desc_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    struct eCAL::SDataTypeInformation topic_info = { topic_type_name_, topic_type_encoding_, std::string(topic_desc_, static_cast<size_t>(topic_desc_len_)) };
    if (!pub->Create(topic_name_, topic_info)) return(0);
    return(1);
  }

  ECALC_API int eCAL_Pub_Destroy(ECAL_HANDLE handle_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    delete pub;
    return(1);
  }

  ECALC_API int eCAL_Pub_SetAttribute(ECAL_HANDLE handle_, const char* attr_name_, int attr_name_len_, const char* attr_value_, int attr_value_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    if (pub->SetAttribute(std::string(attr_name_, static_cast<size_t>(attr_name_len_)), std::string(attr_value_, static_cast<size_t>(attr_value_len_)))) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_ClearAttribute(ECAL_HANDLE handle_, const char* attr_name_, int attr_name_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    if (pub->ClearAttribute(std::string(attr_name_, static_cast<size_t>(attr_name_len_)))) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_ShareType(ECAL_HANDLE handle_, int state_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    pub->ShareType(state_ != 0);
    return(1);
  }

  ECALC_API int eCAL_Pub_ShareDescription(ECAL_HANDLE handle_, int state_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    pub->ShareDescription(state_ != 0);
    return(1);
  }

  ECALC_API int eCAL_Pub_SetID(ECAL_HANDLE handle_, long long id_)
  {
    if (handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    if (pub->SetID(id_)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_IsSubscribed(ECAL_HANDLE handle_)
  {
    if(handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    if(pub->IsSubscribed()) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_Send(ECAL_HANDLE handle_, const void* const buf_, int buf_len_, long long time_)
  {
    if(handle_ == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    const size_t ret = pub->Send(buf_, static_cast<size_t>(buf_len_), time_);
    if(static_cast<int>(ret) == buf_len_)
    {
      return(buf_len_);
    }
    return(0);
  }

  ECALC_API int eCAL_Pub_AddEventCallback(ECAL_HANDLE handle_, eCAL_Publisher_Event type_, PubEventCallbackCT callback_, void* par_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CPublisher* pub = static_cast<eCAL::CPublisher*>(handle_);
    auto callback = std::bind(g_pub_event_callback, std::placeholders::_1, std::placeholders::_2, callback_, par_);
    if (pub->AddEventCallback(type_, callback)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_RemEventCallback(ECAL_HANDLE handle_, eCAL_Publisher_Event type_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CPublisher* pub = static_cast<eCAL::CPublisher*>(handle_);
    if (pub->RemEventCallback(type_)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Pub_Dump(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if(handle_   == nullptr) return(0);
    auto* pub = static_cast<eCAL::CPublisher*>(handle_);
    const std::string dump = pub->Dump();
    if(!dump.empty())
    {
      return(CopyBuffer(buf_, buf_len_, dump));
    }
    return(0);
  }
}
#endif // ECAL_CORE_PUBLISHER

/////////////////////////////////////////////////////////
// Subscriber
/////////////////////////////////////////////////////////
#if ECAL_CORE_SUBSCRIBER
static std::recursive_mutex g_sub_callback_mtx;
static void g_sub_receive_callback(const char* topic_name_, const struct eCAL::SReceiveCallbackData* data_, const ReceiveCallbackCT callback_, void* par_)
{
  const std::lock_guard<std::recursive_mutex> lock(g_sub_callback_mtx);
  SReceiveCallbackDataC data{};
  data.buf   = data_->buf;
  data.size  = data_->size;
  data.id    = data_->id;
  data.time  = data_->time;
  data.clock = data_->clock;
  callback_(topic_name_, &data, par_);
}

static void g_sub_event_callback(const char* topic_name_, const struct eCAL::SSubEventCallbackData* data_, const SubEventCallbackCT callback_, void* par_)
{
  const std::lock_guard<std::recursive_mutex> lock(g_sub_callback_mtx);
  SSubEventCallbackDataC data;
  data.type        = data_->type;
  data.time        = data_->time;
  data.clock       = data_->clock;
  data.tid         = data_->tid.c_str();
  data.tname       = data_->tdatatype.name.c_str();
  data.tencoding   = data_->tdatatype.encoding.c_str();
  data.tdesc       = data_->tdatatype.descriptor.c_str();
  callback_(topic_name_, &data, par_);
}

extern "C"
{
  ECALC_API ECAL_HANDLE eCAL_Sub_New()
  {
    auto* sub = new eCAL::CSubscriber;
    return(sub);
  }

  ECALC_API int eCAL_Sub_Create(ECAL_HANDLE handle_, const char* topic_name_, const char* topic_type_name_, const char* topic_type_encoding_, const char* topic_desc_, int topic_desc_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    struct eCAL::SDataTypeInformation topic_info = { topic_type_name_, topic_type_encoding_, std::string(topic_desc_, static_cast<size_t>(topic_desc_len_)) };
    if (!sub->Create(topic_name_, topic_info)) return(0);
    return(1);
  }

  ECALC_API int eCAL_Sub_Destroy(ECAL_HANDLE handle_)
  {
    if(handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    delete sub;
    return(1);
  }

  ECALC_API int eCAL_Sub_SetID(ECAL_HANDLE handle_, const long long* id_array_, const int id_num_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    std::set<long long> id_set;
    if (id_array_ != nullptr)
    {
      for (size_t i = 0; i < static_cast<size_t>(id_num_); ++i)
      {
        id_set.insert(id_array_[i]);
      }
    }
    if (sub->SetID(id_set)) return(1);
    return(1);
  }

  ECALC_API int eCAL_Sub_SetAttribute(ECAL_HANDLE handle_, const char* attr_name_, int attr_name_len_, const char* attr_value_, int attr_value_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    if (sub->SetAttribute(std::string(attr_name_, static_cast<size_t>(attr_name_len_)), std::string(attr_value_, static_cast<size_t>(attr_value_len_)))) return(1);
    return(0);
  }

  ECALC_API int eCAL_Sub_ClearAttribute(ECAL_HANDLE handle_, const char* attr_name_, int attr_name_len_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    if (sub->ClearAttribute(std::string(attr_name_, static_cast<size_t>(attr_name_len_)))) return(1);
    return(0);
  }

  ECALC_API int eCAL_Sub_Receive(ECAL_HANDLE handle_, void* buf_, int buf_len_, long long* time_, int rcv_timeout_)
  {
    if (buf_len_ == ECAL_ALLOCATE_4ME)
    {
      return eCAL_Sub_Receive_Alloc(handle_, static_cast<void**>(buf_), time_, rcv_timeout_); //-V206
    }
    else
    {
      return eCAL_Sub_Receive_ToBuffer(handle_, buf_, buf_len_, time_, rcv_timeout_);
    }
  }

  ECALC_API int eCAL_Sub_Receive_ToBuffer(ECAL_HANDLE handle_, void* buf_, int buf_len_, long long* time_, int rcv_timeout_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);

    std::string buf;
    if (sub->ReceiveBuffer(buf, time_, rcv_timeout_))
    {
      return(CopyBuffer(buf_, buf_len_, buf));
    }
    return(0);
  }

  ECALC_API int eCAL_Sub_Receive_Alloc(ECAL_HANDLE handle_, void** buf_, long long* time_, int rcv_timeout_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);

    std::string buf;
    if (sub->ReceiveBuffer(buf, time_, rcv_timeout_))
    {
      return(CopyBuffer(buf_, ECAL_ALLOCATE_4ME, buf));
    }
    return(0);
  }

  ECALC_API int eCAL_Sub_Receive_Buffer_Alloc(ECAL_HANDLE handle_, void** buf_, int* buf_len_, long long* time_, int rcv_timeout_)
  {
    if (handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);

    std::string buf;
    if (sub->ReceiveBuffer(buf, time_, rcv_timeout_))
    {
      CopyBuffer(buf_, ECAL_ALLOCATE_4ME, buf);
      if (buf_len_) *buf_len_ = static_cast<int>(buf.size());
      return(1);
    }
    return(0);
  }

  ECALC_API int eCAL_Sub_AddReceiveCallback(ECAL_HANDLE handle_, ReceiveCallbackCT callback_, void* par_)
  {
    if(handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    auto callback = std::bind(g_sub_receive_callback, std::placeholders::_1, std::placeholders::_2, callback_, par_);
    if(sub->AddReceiveCallback(callback)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Sub_RemReceiveCallback(ECAL_HANDLE handle_)
  {
    if(handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    if(sub->RemReceiveCallback()) return(1);
    return(0);
  }

  ECALC_API int eCAL_Sub_AddEventCallback(ECAL_HANDLE handle_, eCAL_Subscriber_Event type_, SubEventCallbackCT callback_, void* par_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CSubscriber* sub = static_cast<eCAL::CSubscriber*>(handle_);
    auto callback = std::bind(g_sub_event_callback, std::placeholders::_1, std::placeholders::_2, callback_, par_);
    if (sub->AddEventCallback(type_, callback)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Sub_RemEventCallback(ECAL_HANDLE handle_, eCAL_Subscriber_Event type_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CSubscriber* sub = static_cast<eCAL::CSubscriber*>(handle_);
    if (sub->RemEventCallback(type_)) return(1);
    return(0);
  }
  
  ECALC_API int eCAL_Sub_GetTypeName(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CSubscriber* sub = static_cast<eCAL::CSubscriber*>(handle_);
    const eCAL::SDataTypeInformation datatype_info = sub->GetDataTypeInformation();
    int buffer_len = CopyBuffer(buf_, buf_len_, datatype_info.name);
    if (buffer_len != static_cast<int>(datatype_info.name.size()))
    {
      return(0);
    }
    else
    {
      return(buffer_len);
    }
  }

  ECALC_API int eCAL_Sub_GetEncoding(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CSubscriber* sub = static_cast<eCAL::CSubscriber*>(handle_);
    const eCAL::SDataTypeInformation datatype_info = sub->GetDataTypeInformation();
    int buffer_len = CopyBuffer(buf_, buf_len_, datatype_info.encoding);
    if (buffer_len != static_cast<int>(datatype_info.encoding.size()))
    {
      return(0);
    }
    else
    {
      return(buffer_len);
    }
  }

  ECALC_API int eCAL_Sub_GetDescription(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CSubscriber* sub = static_cast<eCAL::CSubscriber*>(handle_);
    const eCAL::SDataTypeInformation datatype_info = sub->GetDataTypeInformation();
    int buffer_len = CopyBuffer(buf_, buf_len_, datatype_info.descriptor);
    if (buffer_len != static_cast<int>(datatype_info.descriptor.size()))
    {
      return(0);
    }
    else
    {
      return(buffer_len);
    }
  }

  ECALC_API int eCAL_Sub_Dump(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if(handle_ == nullptr) return(0);
    auto* sub = static_cast<eCAL::CSubscriber*>(handle_);
    const std::string dump = sub->Dump();
    if(!dump.empty())
    {
      return(CopyBuffer(buf_, buf_len_, dump));
    }
    return(0);
  }
}
#endif // ECAL_CORE_SUBSCRIBER

/////////////////////////////////////////////////////////
// Time
/////////////////////////////////////////////////////////
ECALC_API int eCAL_Time_GetName(void* name_, int name_len_)
{
  const std::string name = eCAL::Time::GetName();
  if (!name.empty())
  {
    return(CopyBuffer(name_, name_len_, name));
  }
  return(0);
}

ECALC_API long long eCAL_Time_GetMicroSeconds()
{
  return(eCAL::Time::GetMicroSeconds());
}

ECALC_API long long eCAL_Time_GetNanoSeconds()
{
  return(eCAL::Time::GetNanoSeconds());
}

ECALC_API int eCAL_Time_SetNanoSeconds(long long time_)
{
  return(eCAL::Time::SetNanoSeconds(time_));
}

ECALC_API int eCAL_Time_IsTimeSynchronized()
{
  return(eCAL::Time::IsSynchronized());
}

ECALC_API int eCAL_Time_IsTimeMaster()
{
  return(eCAL::Time::IsMaster());
}

ECALC_API void eCAL_Time_SleepForNanoseconds(long long duration_nsecs_)
{
  eCAL::Time::SleepForNanoseconds(duration_nsecs_);
}

ECALC_API int eCAL_Time_GetStatus(int* error_, char** status_message_, const int max_len_)
{
  if (max_len_ == ECAL_ALLOCATE_4ME || max_len_ > 0)
  {
    std::string status_message;
    eCAL::Time::GetStatus(*error_, &status_message);

    if (!status_message.empty())
    {
      return CopyBuffer(status_message_, max_len_, status_message);
    }
    return 0;
  }
  else 
  {
    eCAL::Time::GetStatus(*error_, nullptr);
    return 0;
  }
}

/////////////////////////////////////////////////////////
// Timer
/////////////////////////////////////////////////////////
static std::recursive_mutex g_timer_callback_mtx;
static void g_timer_callback(const TimerCallbackCT callback_, void* par_)
{
  const std::lock_guard<std::recursive_mutex> lock(g_timer_callback_mtx);
  callback_(par_);
}

extern "C"
{
  ECALC_API ECAL_HANDLE eCAL_Timer_Create()
  {
    eCAL::CTimer* timer = new eCAL::CTimer;
    return(timer);
  }

  ECALC_API int eCAL_Timer_Destroy(ECAL_HANDLE handle_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CTimer* timer = static_cast<eCAL::CTimer*>(handle_);
    delete timer;
    timer = NULL;
    return(1);
  }

  ECALC_API int eCAL_Timer_Start(ECAL_HANDLE handle_, int timeout_, TimerCallbackCT callback_, int delay_, void* par_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CTimer* timer = static_cast<eCAL::CTimer*>(handle_);
    auto callback = std::bind(g_timer_callback, callback_, par_);
    if (timer->Start(timeout_, callback, delay_)) return(1);
    else                                         return(0);
  }

  ECALC_API int eCAL_Timer_Stop(ECAL_HANDLE handle_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CTimer* timer = static_cast<eCAL::CTimer*>(handle_);
    if (timer->Stop()) return(1);
    else              return(0);
  }
}

/////////////////////////////////////////////////////////
// Service Server
/////////////////////////////////////////////////////////
#if ECAL_CORE_SERVICE
extern "C"
{
  static std::recursive_mutex g_request_callback_mtx;
  static int g_method_callback(const std::string& method_, const std::string& req_type_, const std::string& resp_type_, const std::string& request_, std::string& response_, MethodCallbackCT callback_, void* par_)
  {
    const std::lock_guard<std::recursive_mutex> lock(g_request_callback_mtx);
    void* response(nullptr);
    int   response_len(ECAL_ALLOCATE_4ME);
    const int ret_state = callback_(method_.c_str(), req_type_.c_str(), resp_type_.c_str(), request_.c_str(), static_cast<int>(request_.size()), &response, &response_len, par_);
    if (response_len > 0)
    {
      response_ = std::string(static_cast<const char*>(response), static_cast<size_t>(response_len));
    }
    return ret_state;
  }

  static std::recursive_mutex g_server_event_callback_mtx;
  static void g_server_event_callback(const char* name_, const struct eCAL::SServerEventCallbackData* data_, const ServerEventCallbackCT callback_, void* par_)
  {
    const std::lock_guard<std::recursive_mutex> lock(g_server_event_callback_mtx);
    SServerEventCallbackDataC data;
    data.time = data_->time;
    data.type = data_->type;
    callback_(name_, &data, par_);
  }

  ECALC_API ECAL_HANDLE eCAL_Server_Create(const char* service_name_)
  {
    if (service_name_ == NULL) return(NULL);
    eCAL::CServiceServer* server = new eCAL::CServiceServer(service_name_);
    return(server);
  }

  ECALC_API int eCAL_Server_Destroy(ECAL_HANDLE handle_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    delete server;
    server = NULL;
    return(1);
  }

  ECALC_API int eCAL_Server_AddMethodCallback(ECAL_HANDLE handle_, const char* method_, const char* req_type_, const char* resp_type_, MethodCallbackCT callback_, void* par_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    auto callback = std::bind(g_method_callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, callback_, par_);
    return server->AddMethodCallback(method_, req_type_, resp_type_, callback);
  }

  ECALC_API_DEPRECATED int eCAL_Server_AddMethodCallbackC(ECAL_HANDLE handle_, const char* method_, const char* req_type_, const char* resp_type_, MethodCallbackCT callback_, void* par_)
  {
    return eCAL_Server_AddMethodCallback(handle_, method_, req_type_, resp_type_, callback_, par_);
  }

  ECALC_API int eCAL_Server_RemMethodCallback(ECAL_HANDLE handle_, const char* method_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    return server->RemMethodCallback(method_);
  }

  ECALC_API_DEPRECATED int eCAL_Server_RemMethodCallbackC(ECAL_HANDLE handle_, const char* method_)
  {
    return eCAL_Server_RemMethodCallback(handle_, method_);
  }

  ECALC_API int eCAL_Server_AddEventCallback(ECAL_HANDLE handle_, eCAL_Server_Event type_, ServerEventCallbackCT callback_, void* par_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    auto callback = std::bind(g_server_event_callback, std::placeholders::_1, std::placeholders::_2, callback_, par_);
    if (server->AddEventCallback(type_, callback)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Server_RemEventCallback(ECAL_HANDLE handle_, eCAL_Server_Event type_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    if (server->RemEventCallback(type_)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Server_GetServiceName(ECAL_HANDLE handle_, void* buf_, int buf_len_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceServer* server = static_cast<eCAL::CServiceServer*>(handle_);
    const std::string service_name = server->GetServiceName();
    int buffer_len = CopyBuffer(buf_, buf_len_, service_name);
    if (buffer_len != static_cast<int>(service_name.size()))
    {
      return(0);
    }
    else
    {
      return(buffer_len);
    }
  }
}

/////////////////////////////////////////////////////////
// Service Client
/////////////////////////////////////////////////////////
extern "C"
{
  static std::recursive_mutex g_response_callback_mtx;
  static void g_response_callback(const struct eCAL::SServiceResponse& service_response_, const ResponseCallbackCT callback_, void* par_)
  {
    const std::lock_guard<std::recursive_mutex> lock(g_response_callback_mtx);
    struct SServiceResponseC service_response;
    service_response.host_name = service_response_.host_name.c_str();
    service_response.service_name = service_response_.service_name.c_str();
    service_response.service_id = service_response_.service_id.c_str();
    service_response.method_name = service_response_.method_name.c_str();
    service_response.error_msg = service_response_.error_msg.c_str();
    service_response.ret_state = service_response_.ret_state;
    service_response.call_state = service_response_.call_state;
    service_response.response = service_response_.response.c_str();
    service_response.response_len = static_cast<int>(service_response_.response.size());
    callback_(&service_response, par_);
  }

  static std::recursive_mutex g_client_event_callback_mtx;
  static void g_client_event_callback(const char* name_, const struct eCAL::SClientEventCallbackData* data_, const ClientEventCallbackCT callback_, void* par_)
  {
    const std::lock_guard<std::recursive_mutex> lock(g_client_event_callback_mtx);
    SClientEventCallbackDataC data;
    data.time = data_->time;
    data.type = data_->type;
    callback_(name_, &data, par_);
  }

  ECALC_API ECAL_HANDLE eCAL_Client_Create(const char* service_name_)
  {
    if (service_name_ == NULL) return(NULL);
    eCAL::CServiceClient* client = new eCAL::CServiceClient(service_name_);
    return(client);
  }

  ECALC_API int eCAL_Client_Destroy(ECAL_HANDLE handle_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
    delete client;
    client = NULL;
    return(1);
  }

  ECALC_API int eCAL_Client_SetHostName(ECAL_HANDLE handle_, const char* host_name_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
    if (client->SetHostName(host_name_)) return(1);
    return(0);
  }

  ECALC_API int eCAL_Client_Call(ECAL_HANDLE handle_, const char* method_name_, const char* request_, int request_len_, int timeout_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
    if (client->Call(method_name_, std::string(request_, static_cast<size_t>(request_len_)), timeout_)) return(1);
    return(0);
  }

  // The C API variant is not able to return all service repsonses but only the first one !
  ECALC_API int eCAL_Client_Call_Wait(ECAL_HANDLE handle_, const char* method_name_, const char* request_, int request_len_, int timeout_, struct SServiceResponseC* service_response_, void* response_, int response_len_)
  {
    if (handle_ == NULL) return(0);
    eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
    eCAL::ServiceResponseVecT service_response_vec;
    if (client->Call(method_name_, std::string(request_, static_cast<size_t>(request_len_)), timeout_, &service_response_vec))
    {
      if (service_response_vec.size() > 0)
      {
        service_response_->host_name = NULL;
        service_response_->service_name = NULL;
        service_response_->service_id = NULL;
        service_response_->method_name = NULL;
        service_response_->error_msg = NULL;
        service_response_->ret_state = service_response_vec[0].ret_state;
        service_response_->call_state = service_response_vec[0].call_state;
        service_response_->response = NULL;
        service_response_->response_len = 0;
        return(CopyBuffer(response_, response_len_, service_response_vec[0].response));
      }
    }
    return(0);
  }
}

ECALC_API int eCAL_Client_Call_Async(ECAL_HANDLE handle_, const char* method_name_, const char* request_, int request_len_, int timeout_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  if (client->CallAsync(method_name_, std::string(request_, static_cast<size_t>(request_len_)), timeout_)) return(1);
  return(0);
}

int eCAL_Client_AddResponseCallback(ECAL_HANDLE handle_, ResponseCallbackCT callback_, void* par_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  auto callback = std::bind(g_response_callback, std::placeholders::_1, callback_, par_);
  return client->AddResponseCallback(callback);
}

int eCAL_Client_AddResponseCallbackC(ECAL_HANDLE handle_, ResponseCallbackCT callback_, void* par_)
{
  return eCAL_Client_AddResponseCallback(handle_, callback_, par_);
}


int eCAL_Client_RemResponseCallback(ECAL_HANDLE handle_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  return client->RemResponseCallback();
}

ECALC_API int eCAL_Client_AddEventCallback(ECAL_HANDLE handle_, eCAL_Client_Event type_, ClientEventCallbackCT callback_, void* par_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  auto callback = std::bind(g_client_event_callback, std::placeholders::_1, std::placeholders::_2, callback_, par_);
  if (client->AddEventCallback(type_, callback)) return(1);
  return(0);
}

ECALC_API int eCAL_Client_RemEventCallback(ECAL_HANDLE handle_, eCAL_Client_Event type_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  if (client->RemEventCallback(type_)) return(1);
  return(0);
}

ECALC_API int eCAL_Client_GetServiceName(ECAL_HANDLE handle_, void* buf_, int buf_len_)
{
  if (handle_ == NULL) return(0);
  eCAL::CServiceClient* client = static_cast<eCAL::CServiceClient*>(handle_);
  const std::string service_name = client->GetServiceName();
  const int buffer_len = CopyBuffer(buf_, buf_len_, service_name);
  if (buffer_len != static_cast<int>(service_name.size()))
  {
    return(0);
  }
  else
  {
    return(buffer_len);
  }
}
#endif // ECAL_CORE_SERVICE
