/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2024 Continental Corporation
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

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

enum {
  CMN_MONITORING_TIMEOUT_MS   = (5000 + 100),
  CMN_REGISTRATION_REFRESH_MS = (1000)
};

TEST(core_cpp_registration_public, GetTopics)
{
  // initialize eCAL API
  eCAL::Initialize(0, nullptr, "core_cpp_registration_public");

  // enable loop back communication in the same process
  eCAL::Util::EnableLoopback(true);

  std::map<std::string, eCAL::SDataTypeInformation> topic_info_map;

  // create and check a few pub/sub entities
  {
    eCAL::SDataTypeInformation info_A1  { "typeA1"  ,"",  "descA1"   };
    eCAL::SDataTypeInformation info_A1_2{ "typeA1.2","",  "descA1.2" };
    eCAL::SDataTypeInformation info_A2  { "typeA2"  ,"",  "descA2"   };
    eCAL::SDataTypeInformation info_A3  { "typeA3"  ,"",  "descA3"   };

    eCAL::SDataTypeInformation info_B1  { "typeB1"  ,"",  "descB1"   };
    eCAL::SDataTypeInformation info_B1_2{ "typeB1.2","",  "descB1.2" };
    eCAL::SDataTypeInformation info_B2  { "typeB2"  ,"",  "descB2"   };

    // create 3 publisher
    eCAL::CPublisher pub1("A1", info_A1);
    eCAL::CPublisher pub2("A2", info_A2);
    eCAL::CPublisher pub3("A3", info_A3);

    // create a missmatching publisher
    // this should trigger a warning but not increase map size
    eCAL::CPublisher pub12("A1", info_A1_2);

    // create 2 subscriber
    eCAL::CSubscriber sub1("B1", info_B1);
    eCAL::CSubscriber sub2("B2", info_B2);

    // create a missmatching subscriber
    // this should trigger a warning but not increase map size
    eCAL::CSubscriber sub12("B1", info_B1_2);

    // let's register
    eCAL::Process::SleepMS(2 * CMN_REGISTRATION_REFRESH_MS);

    // get all topics
    eCAL::Registration::GetTopics(topic_info_map);

    // check size
    EXPECT_EQ(topic_info_map.size(), 5);

    // check types and descriptions
    for (auto& topic_info : topic_info_map)
    {
      eCAL::SDataTypeInformation registration_topic_info;
      eCAL::Registration::GetTopicDataTypeInformation(topic_info.first, registration_topic_info);
      eCAL::SDataTypeInformation expected_topic_info{ "type" + topic_info.first, "", "desc" + topic_info.first };
      EXPECT_EQ(registration_topic_info, expected_topic_info);
    }

    // wait a monitoring timeout long,
    eCAL::Process::SleepMS(CMN_MONITORING_TIMEOUT_MS);

    // the topics should not be expired
    eCAL::Registration::GetTopics(topic_info_map);

    // check size
    EXPECT_EQ(topic_info_map.size(), 5);

    // now destroy publisher pub1 and subscriber sub1
    // the entities pub12 and sub12 should replace them
    // by overwriting their type names and descriptions
    pub1.Destroy();
    sub1.Destroy();

    // let's register
    eCAL::Process::SleepMS(2 * CMN_REGISTRATION_REFRESH_MS);

    // size should be 5 again (because of pub12 and sub12 should have replaced pub1 and sub1 attributes now)
    EXPECT_EQ(topic_info_map.size(), 5);

    // check overwritten attributes
    {
      eCAL::SDataTypeInformation registration_topic_info;
      EXPECT_EQ(true, eCAL::Registration::GetTopicDataTypeInformation("A1", registration_topic_info));
      EXPECT_EQ(registration_topic_info, info_A1_2);
    }
    {
      eCAL::SDataTypeInformation registration_topic_info;
      EXPECT_EQ(true, eCAL::Registration::GetTopicDataTypeInformation("B1", registration_topic_info));
      EXPECT_EQ(registration_topic_info, info_B1_2);
    }
  }

  // let's unregister
  eCAL::Process::SleepMS(2 * CMN_REGISTRATION_REFRESH_MS);

  // get all topics again, now all topics 
  // should be removed from the map
  eCAL::Registration::GetTopics(topic_info_map);

  // check size
  EXPECT_EQ(topic_info_map.size(), 0);

  // finalize eCAL API
  eCAL::Finalize();
}

// This test creates a reall big number of publishers.
// It then checks, if they have all been seen using GetTopics()
// And the count is back to 0 upon completion.
TEST(core_cpp_registration_public, GetTopicsParallel)
{
  constexpr const int max_publisher_count(2000);
  constexpr const int waiting_time_thread(4000);
  constexpr const int parallel_threads(1);

  std::atomic<bool> testing_completed{ false };

  // initialize eCAL API
  eCAL::Initialize(0, nullptr, "core_cpp_registration_public");

  // enable loop back communication in the same process
  eCAL::Util::EnableLoopback(true);

  auto create_publishers = [&]() {
    std::string topic_name = "Test.ParallelUtilFunctions";
    std::atomic<int> call_back_count{ 0 };
    {
      std::vector<std::unique_ptr<eCAL::CPublisher>> publishers;
      for (int pub_count = 0; pub_count < max_publisher_count; pub_count++) {
        std::unique_ptr<eCAL::CPublisher> publisher = std::make_unique<eCAL::CPublisher>(topic_name + std::to_string(pub_count));
        publishers.push_back(std::move(publisher));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
    testing_completed = true;
    };

  auto get_topics_from_ecal = [&]() {
    size_t number_publishers_seen = 0;
    size_t max_number_publishers_seen = 0;

    std::set<std::string> tmp_topic_names;
    std::map<std::string, eCAL::SDataTypeInformation> topics;

    do {
      eCAL::Registration::GetTopicNames(tmp_topic_names);
      eCAL::Registration::GetTopics(topics);

      number_publishers_seen = tmp_topic_names.size();
      max_number_publishers_seen = std::max(max_number_publishers_seen, number_publishers_seen);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (!testing_completed);

    EXPECT_EQ(number_publishers_seen, 0);
    EXPECT_EQ(max_number_publishers_seen, max_publisher_count);
    };

  std::vector<std::thread> threads_container;
  threads_container.push_back(std::thread(create_publishers));

  for (size_t i = 0; i < parallel_threads; i++) {
    threads_container.push_back(std::thread(get_topics_from_ecal));
  }

  for (auto& th : threads_container) {
    th.join();
  }

  // finalize eCAL API
  eCAL::Finalize();
}
