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

#include <ecal/ecal.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <tclap/CmdLine.h>

#include "binary_payload_writer.h"

// warmup runs not to measure
const int warmups(100);

// single test run
void do_run(const int runs, int snd_size /*kB*/)
{
  // log parameter
  std::cout << "--------------------------------------------"    << std::endl;
  std::cout << "Runs                    : " << runs              << std::endl;
  std::cout << "Message size            : " << snd_size << " kB" << std::endl;

  // initialize eCAL API
  eCAL::Initialize(0, nullptr, "latency_snd");

  // create publisher and subscriber
  eCAL::CPublisher pub("ping");

  // prepare send buffer
  CBinaryPayload payload(snd_size * 1024);

  // let them match
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // add some extra loops for warmup :-)
  int run(0);
  for (; run < runs+warmups; ++run)
  {
    // get microseconds
    auto snd_time  = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // send message (with receive timeout 100 ms)
    pub.Send(payload, snd_time);
  }

  // log test
  std::cout << "Messages sent           : " << run - warmups << std::endl;
  std::cout << "--------------------------------------------" << std::endl;

  // let the receiver do the evaluation
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // finalize eCAL API
  eCAL::Finalize();
}

int main(int argc, char **argv)
{
  try
  {
    // parse command line
    TCLAP::CmdLine cmd("latency_snd");
    TCLAP::ValueArg<int>         runs        ("r", "runs",        "Number of messages to send.",            false, 5000, "int");
    TCLAP::ValueArg<int>         size        ("s", "size",        "Messages size in kB.",                   false,   -1, "int");
    cmd.add(runs);
    cmd.add(size);
    cmd.parse(argc, argv);

    if(size < 0)
    {
      // automatic size mode
      for (int s = 1; s <= 32768; s *= 2) do_run(runs.getValue(), s);
    }
    else
    {
      // run single test
      do_run(runs.getValue(), size.getValue());
    }
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }

  return(0);
}
