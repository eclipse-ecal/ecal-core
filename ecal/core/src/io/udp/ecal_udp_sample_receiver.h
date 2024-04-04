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
 * @brief  UDP sample receiver to receive messages of type eCAL::Sample
**/

#pragma once

#include "io/udp/ecal_udp_receiver_attr.h"
#include "ecal_udp_sample_receiver_asio.h"

#ifdef ECAL_CORE_NPCAP_SUPPORT
#include "ecal_udp_sample_receiver_npcap.h"
#endif

#include <memory>
#include <string>

namespace eCAL
{
  namespace UDP
  {
    class CSampleReceiver
    {
    public:
      CSampleReceiver(const SReceiverAttr& attr_, const HasSampleCallbackT& has_sample_callback_, const ApplySampleCallbackT& apply_sample_callback_);

      bool AddMultiCastGroup(const char* ipaddr_);
      bool RemMultiCastGroup(const char* ipaddr_);

    private:
      std::shared_ptr<CSampleReceiverAsio>  m_receiver_asio;

#ifdef ECAL_CORE_NPCAP_SUPPORT
      std::shared_ptr<CSampleReceiverNpcap> m_receiver_npcap;
#endif
    };
  }
}
