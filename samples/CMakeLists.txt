# ========================= eCAL LICENSE =================================
#
# Copyright (C) 2016 - 2019 Continental Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ========================= eCAL LICENSE =================================
cmake_minimum_required(VERSION 3.10)

project(eCAL_samples VERSION "${ECAL_VERSION_STRING}")
set (CMAKE_CXX_STANDARD 14)

# --------------------------------------------------------
# c samples
# --------------------------------------------------------
add_subdirectory(c/minimal/minimal_rec)
add_subdirectory(c/minimal/minimal_rec_cb)
add_subdirectory(c/minimal/minimal_snd)
add_subdirectory(c/services/minimal_client_c)
add_subdirectory(c/services/minimal_server_c)

# --------------------------------------------------------
# cpp samples
# --------------------------------------------------------
add_subdirectory(cpp/benchmarks/many_connections)
add_subdirectory(cpp/binary/binary_rec_cb)
add_subdirectory(cpp/binary/binary_snd)
if(HAS_CAPNPROTO)
add_subdirectory(cpp/capnp/addressbook_snd)
add_subdirectory(cpp/capnp/addressbook_rec)
add_subdirectory(cpp/capnp/addressbook_rec_cb)
add_subdirectory(cpp/capnp/addressbook_rec_dynamic)
endif(HAS_CAPNPROTO)
add_subdirectory(cpp/counter/counter_rec_cb)
add_subdirectory(cpp/counter/counter_snd)
add_subdirectory(cpp/datarate/datarate_rec)
add_subdirectory(cpp/datarate/datarate_snd)
add_subdirectory(cpp/event/event_rec)
add_subdirectory(cpp/event/event_snd)
add_subdirectory(cpp/latency/latency_rec)
add_subdirectory(cpp/latency/latency_snd)
add_subdirectory(cpp/minimal/minimal_rec)
add_subdirectory(cpp/minimal/minimal_rec_cb)
add_subdirectory(cpp/minimal/minimal_snd)
add_subdirectory(cpp/misc/process)
add_subdirectory(cpp/misc/proto_dyn)
add_subdirectory(cpp/misc/proto_dyn_json)
add_subdirectory(cpp/misc/time)
add_subdirectory(cpp/misc/timer)
add_subdirectory(cpp/monitoring/monitoring_get_services)
add_subdirectory(cpp/monitoring/monitoring_get_topics)
add_subdirectory(cpp/monitoring/monitoring_rec)
add_subdirectory(cpp/monitoring/monitoring_reg)
add_subdirectory(cpp/multiple/multiple_rec_cb)
add_subdirectory(cpp/multiple/multiple_snd)
add_subdirectory(cpp/orchestration/component1)
add_subdirectory(cpp/orchestration/component2)
add_subdirectory(cpp/orchestration/orchestrator)
add_subdirectory(cpp/performance/dynsize_snd)
add_subdirectory(cpp/performance/performance_rec)
add_subdirectory(cpp/performance/performance_rec_cb)
add_subdirectory(cpp/performance/performance_snd)
add_subdirectory(cpp/performance/pubsub_throughput)
add_subdirectory(cpp/person/person_rec)
add_subdirectory(cpp/person/person_rec_events)
add_subdirectory(cpp/person/person_rec_lambda_in_class)
add_subdirectory(cpp/person/person_snd)
add_subdirectory(cpp/person/person_snd_dyn)
add_subdirectory(cpp/person/person_snd_events)
add_subdirectory(cpp/person/person_snd_inproc)
add_subdirectory(cpp/person/person_snd_multicast)
add_subdirectory(cpp/person/person_snd_tcp)
add_subdirectory(cpp/pingpong/ping)
add_subdirectory(cpp/pingpong/pong)
add_subdirectory(cpp/services/latency_client)
add_subdirectory(cpp/services/latency_server)
add_subdirectory(cpp/services/math_client)
add_subdirectory(cpp/services/math_server)
add_subdirectory(cpp/services/minimal_client)
add_subdirectory(cpp/services/minimal_server)
add_subdirectory(cpp/services/ping_client)
add_subdirectory(cpp/services/ping_client_dyn)
add_subdirectory(cpp/services/ping_server)
