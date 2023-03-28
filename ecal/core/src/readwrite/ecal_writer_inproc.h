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
 * @brief  inproc data writer
**/

#pragma once

#include "ecal_def.h"

#ifdef _MSC_VER
#pragma warning(push, 0) // disable proto warnings
#endif
#include <ecal/core/pb/ecal.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

//#include "io/ecal_inproc.h"
#include "readwrite/ecal_writer_base.h"

#include <string>
#include <vector>

namespace eCAL
{
  class CDataWriterInProc : public CDataWriterBase
  {
  public:
    ~CDataWriterInProc();

    SWriterInfo GetInfo() override;

    bool Create(const std::string& host_name_, const std::string& topic_name_, const std::string & topic_id_) override;
    // this virtual function is called during construction/destruction,
    // so, mark it as final to ensure that no derived classes override it.
    bool Destroy() final override;

    bool Write(const SWriterData& data_) override;

  protected:
  };
}
