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
 * @file   ecal.h
 * @brief  eCAL main header file
**/

#pragma once

// all ecal includes
#include <ecal/ecal_os.h>
#include <ecal/ecal_defs.h>
#include <ecal/ecal_callback.h>
#include <ecal/ecal_client.h>
#include <ecal/ecal_config.h>
#include <ecal/ecal_core.h>
#include <ecal/ecal_event.h>
#include <ecal/ecal_log.h>
#include <ecal/ecal_process.h>
#include <ecal/ecal_monitoring.h>
#include <ecal/ecal_publisher.h>
#include <ecal/ecal_qos.h>
#include <ecal/ecal_server.h>
#include <ecal/ecal_service_info.h>
#include <ecal/ecal_subscriber.h>
#include <ecal/ecal_time.h>
#include <ecal/ecal_timer.h>
#include <ecal/ecal_tlayer.h>
#include <ecal/ecal_util.h>

/* Legacy namespace to be compatible with eCAL < 4.9 code, will be removed in future eCAL versions*/
namespace eCAL
{
  namespace pb
  { 
  }
}
namespace eCALPB = eCAL::pb;

