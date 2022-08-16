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
#include <ecal/msg/flatbuffers/subscriber.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

// flatbuffers includes
#include <flatbuffers/flatbuffers.h>

// flatbuffers generated includes
#include "monster_generated.h"

void OnMonster(const char* topic_name_, const flatbuffers::FlatBufferBuilder& msg_, const long long time_)
{
  // create monster
  auto monster(Game::Sample::GetMonster(msg_.GetBufferPointer()));

  // print content
  std::cout << "topic name        : " << topic_name_               << std::endl;
  std::cout << "time              : " << time_                     << std::endl;
  std::cout                                                        << std::endl;
  std::cout << "monster pos x     : " << monster->pos()->x()       << std::endl;
  std::cout << "monster pos y     : " << monster->pos()->y()       << std::endl;
  std::cout << "monster pos z     : " << monster->pos()->z()       << std::endl;
  std::cout << "monster mana      : " << monster->mana()           << std::endl;
  std::cout << "monster hp        : " << monster->hp()             << std::endl;
  std::cout << "monster name      : " << monster->name()->c_str()  << std::endl;

  std::cout << "monster inventory : ";
  for(auto iter = monster->inventory()->begin(); iter != monster->inventory()->end(); ++iter)
  {
    std::cout << static_cast<int>(*iter) << " ";
  }
  std::cout << std::endl;

  std::cout << "monster color     : ";
  switch (monster->color())
  {
  case Game::Sample::Color_Red:
    std::cout << "Red";
    break;
  case Game::Sample::Color_Green:
    std::cout << "Green";
    break;
  case Game::Sample::Color_Blue:
    std::cout << "Blue";
    break;
  }
  std::cout << std::endl;

  std::cout << std::endl;
}


int main(int argc, char **argv)
{
  // initialize eCAL API
  eCAL::Initialize(argc, argv, "monster subscriber");

  // set process state
  eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "I feel good !");

  // create a subscriber (topic name "monster")
  eCAL::flatbuffers::CSubscriber<flatbuffers::FlatBufferBuilder> sub("monster");

  // add receive callback function (_1 = topic_name, _2 = msg, _3 = time)
  auto callback = std::bind(OnMonster, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  sub.AddReceiveCallback(callback);

  while(eCAL::Ok())
  {
    // sleep 100 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // finalize eCAL API
  eCAL::Finalize();
}
