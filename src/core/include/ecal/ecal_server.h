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
 * @file   ecal_server.h
 * @brief  eCAL service interface
**/

#pragma once

#include <ecal/ecal_deprecate.h>
#include <ecal/ecal_os.h>
#include <ecal/ecal_callback.h>
#include <ecal/ecal_service_info.h>

#include <string>
#include <vector>
#include <memory>

namespace eCAL
{
  class CServiceServerImpl;

  /**
   * @brief Service Server wrapper class.
  **/
  class CServiceServer
  {
  public:
    /**
     * @brief Constructor. 
    **/
    ECAL_API CServiceServer();

    /**
     * @brief Constructor. 
     *
     * @param service_name_   Unique service name.
    **/
    ECAL_API CServiceServer(const std::string& service_name_);

    /**
     * @brief Destructor. 
    **/
    ECAL_API virtual ~CServiceServer();

    /**
     * @brief CServiceServers are non-copyable
    **/
    ECAL_API CServiceServer(const CServiceServer&) = delete;

    /**
     * @brief CServiceServers are non-copyable
    **/
    ECAL_API CServiceServer& operator=(const CServiceServer&) = delete;

    /**
     * @brief Creates this object. 
     *
     * @param service_name_   Unique service name.
     *
     * @return  True if successful. 
    **/
    ECAL_API bool Create(const std::string& service_name_);

    /**
     * @brief Destroys this object. 
     *
     * @return  True if successful. 
    **/
    ECAL_API bool Destroy();

    /**
     * @brief Add method type descriptions.
     *
     * @param method_     Service method name.
     * @param req_type_   Service method request type.
     * @param req_desc_   Service method request description.
     * @param resp_type_  Service method response type.
     * @param resp_desc_  Service method response description.
     *
     * @return  True if successful.
    **/
    ECAL_API bool AddDescription(const std::string& method_, const std::string& req_type_, const std::string& req_desc_, const std::string& resp_type_, const std::string& resp_desc_);

    /**
     * @brief Add method callback.
     *
     * @param method_     Service method name.
     * @param req_type_   Service method request type.
     * @param resp_type_  Service method response type.
     * @param callback_   Callback function for client request.
     *
     * @return  True if successful.
    **/
    ECAL_API bool AddMethodCallback(const std::string& method_, const std::string& req_type_, const std::string& resp_type_, const MethodCallbackT& callback_);

    /**
     * @brief Remove method callback.
     *
     * @param method_  Service method name.
     *
     * @return  True if successful.
    **/
    ECAL_API bool RemMethodCallback(const std::string& method_);

    /**
     * @brief Add server event callback function.
     *
     * @param type_      The event type to react on.
     * @param callback_  The callback function to add.
     *
     * @return  True if succeeded, false if not.
    **/
    ECAL_API bool AddEventCallback(eCAL_Server_Event type_, ServerEventCallbackT callback_);

    /**
     * @brief Remove server event callback function.
     *
     * @param type_  The event type to remove.
     *
     * @return  True if succeeded, false if not.
    **/
    ECAL_API bool RemEventCallback(eCAL_Server_Event type_);

    /**
     * @brief Retrieve service name.
     *
     * @return  The service name.
    **/
    ECAL_API std::string GetServiceName();

    /**
     * @brief Check connection state.
     *
     * @return  True if connected, false if not.
    **/
    ECAL_API bool IsConnected();

  private:
    std::shared_ptr<CServiceServerImpl> m_service_server_impl;
    bool                                m_created;
  };
} 
