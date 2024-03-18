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

#include <vector>
#include <ecal/ecal.h>
#include <gtest/gtest.h>

#define MAX_PUBLISHER_COUNT 10000
constexpr const int waiting_time_thread = 20000;
constexpr const int parallel_threads = 1;

TEST(Util, ParallelUtilFunctions)
{
  eCAL::Initialize();

  auto create_publishers = []() {
    std::string topic_name = "Topic.EcalCreateMultiPublishers";
    std::string test_url = "Test.EcalCreateMultiPublishers";
    std::string node_name = "Node.EcalCreateMultiPublishers";
    std::atomic<int> call_back_count = 0;

    std::vector<std::unique_ptr<eCAL::CPublisher>> publishers;
    for (int pub_count = 0; pub_count < MAX_PUBLISHER_COUNT; pub_count++) {
      std::unique_ptr<eCAL::CPublisher> publisher = std::make_unique<eCAL::CPublisher>(topic_name + std::to_string(pub_count));
      publishers.push_back(std::move(publisher));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
    };

  auto get_topics_from_ecal = []() {
    size_t found_topics = 0;
    std::vector<std::string> tmp_topic_names;
    std::unordered_map<std::string, eCAL::SDataTypeInformation> topics;
    do {
      eCAL::Util::GetTopicNames(tmp_topic_names);
      eCAL::Util::GetTopics(topics);

      found_topics = tmp_topic_names.size();
      std::cout << "Number of topics found by ecal: " << found_topics << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (found_topics < MAX_PUBLISHER_COUNT);

    // do it again until all publishers are deleted
    do {
      eCAL::Util::GetTopicNames(tmp_topic_names);
      eCAL::Util::GetTopics(topics);

      found_topics = tmp_topic_names.size();
      std::cout << "Number of topics found by ecal: " << found_topics << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (found_topics != 0);
    };

  std::vector<std::thread> threads_container;
  threads_container.push_back(std::thread(create_publishers));

  for (size_t i = 0; i < parallel_threads; i++) {
    threads_container.push_back(std::thread(get_topics_from_ecal));
  }

  for (auto& th : threads_container) {
    th.join();
  }
}
