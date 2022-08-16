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
 * @brief  udp multicast reader and layer
**/

#pragma once

#include "readwrite/ecal_reader_layer.h"

#include "io/rcv_sample.h"

#include <map>
#include <memory>
#include <string>

namespace eCAL
{
  ////////////////
  // READER
  ////////////////
  class CDataReaderUDP : public CSampleReceiver
  {
  public:
    bool HasSample(const std::string& sample_name_);
    size_t ApplySample(const eCAL::pb::Sample& ecal_sample_, eCAL::pb::eTLayerType layer_);
  };

  ////////////////
  // LAYER
  ////////////////
  class CUDPReaderLayer : public CReaderLayer<CUDPReaderLayer>
  {
  public:
    CUDPReaderLayer();
    ~CUDPReaderLayer();

    void Initialize();

    void AddSubscription(const std::string& /*host_name_*/, const std::string& topic_name_, const std::string& /*topic_id_*/, QOS::SReaderQOS /*qos_*/);
    void RemSubscription(const std::string& /*host_name_*/, const std::string& topic_name_, const std::string& /*topic_id_*/);

    void SetConnectionParameter(SReaderLayerPar& /*par_*/) {}

  private:
    bool                       started;
    CUDPReceiver               rcv;
    CThread                    thread;
    CDataReaderUDP             reader;
    std::map<std::string, int> topic_name_mcast_map;
  };
};
