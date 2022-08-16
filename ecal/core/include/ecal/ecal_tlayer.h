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
 * @file   ecal_tlayer.h
 * @brief  eCAL transport layer
**/

#pragma once

#include <ecal/ecal_os.h>

namespace eCAL
{
  namespace TLayer
  {
    /**
     * @brief eCAL transport layer types.
    **/
    enum eTransportLayer
    {
      tlayer_none       = 0,
      tlayer_udp_mc     = 1,
      tlayer_shm        = 4,
      tlayer_tcp        = 5,
      tlayer_inproc     = 42,
      tlayer_all        = 255
    };

    /**
     * @brief eCAL transport layer modes.
    **/
    enum eSendMode
    {
      smode_none = -1,
      smode_off  = 0,
      smode_on,
      smode_auto
    };

    /**
     * @brief eCAL transport layer state struct.
    **/
    struct ECAL_API STLayer
    {
      STLayer()
      {
        sm_udp_mc  = smode_none;
        sm_shm     = smode_none;
        sm_inproc  = smode_none;
        sm_tcp     = smode_none;
      }
      eSendMode sm_udp_mc;  //!< udp multicast
      eSendMode sm_shm;     //!< shared memory
      eSendMode sm_inproc;  //!< inner process (inner process memory forwarding only)
      eSendMode sm_tcp;     //!< tcp
    };
  }
}
