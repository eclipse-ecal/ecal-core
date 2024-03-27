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

#include "ecal_udp_sample_receiver.h"

namespace eCAL
{
  namespace UDP
  {
    CSampleReceiver::CSampleReceiver(const IO::UDP::SReceiverAttr& attr_, HasSampleCallbackT has_sample_callback_, ApplySampleCallbackT apply_sample_callback_) :
      m_has_sample_callback(std::move(has_sample_callback_)), m_apply_sample_callback(std::move(apply_sample_callback_))
    {
    }

    CSampleReceiver::~CSampleReceiver()
    {
    }

    bool CSampleReceiver::AddMultiCastGroup(const char* ipaddr_)
    {
      return false;
    }

    bool CSampleReceiver::RemMultiCastGroup(const char* ipaddr_)
    {
      return false;
    }
  }
}
