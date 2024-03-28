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
 * @brief  UDP sample sender to send messages of type eCAL::Sample
**/

#include "ecal_udp_sample_sender.h"

#include <array>
#include <chrono>
#include <iostream>
#include <memory>

namespace eCAL
{
  namespace UDP
  {
    CSampleSender::CSampleSender(const IO::UDP::SSenderAttr& attr_) :
      m_destination_endpoint(asio::ip::make_address(attr_.address), static_cast<unsigned short>(attr_.port))
    {
      m_io_context = std::make_shared<asio::io_context>();
      m_work       = std::make_shared<asio::io_context::work>(*m_io_context);
      m_socket     = std::make_shared<ecaludp::Socket>(*m_io_context, std::array<char, 4>{'E', 'C', 'A', 'L'});

      // open the socket
      {
        asio::error_code ec;
        m_socket->open(asio::ip::udp::v4(), ec);
        if (ec)
          std::cout << "Error opening socket: " << ec.message() << std::endl;
      }

      if (attr_.broadcast)
      {
        // set unicast packet TTL
        const asio::ip::unicast::hops ttl(attr_.ttl);
        asio::error_code ec;
        m_socket->set_option(ttl, ec);
        if (ec)
          std::cerr << "CSampleSender: Setting TTL failed: " << ec.message() << std::endl;
      }
      else
      {
        // set multicast packet TTL
        {
          const asio::ip::multicast::hops ttl(attr_.ttl);
          asio::error_code ec;
          m_socket->set_option(ttl, ec);
          if (ec)
            std::cerr << "CSampleSender: Setting TTL failed: " << ec.message() << std::endl;
        }

        // set loopback option
        {
          const asio::ip::multicast::enable_loopback loopback(attr_.loopback);
          asio::error_code ec;
          m_socket->set_option(loopback, ec);
          if (ec)
            std::cerr << "CSampleSender: Error setting loopback option: " << ec.message() << std::endl;
        }
      }

      if (attr_.broadcast)
      {
        asio::error_code ec;
        m_socket->set_option(asio::socket_base::broadcast(true), ec);
        if (ec)
          std::cerr << "CSampleSender: Setting broadcast mode failed: " << ec.message() << std::endl;
      }

      // run the io context
      m_io_thread = std::thread([this] { m_io_context->run(); });
    }

    CSampleSender::~CSampleSender()
    {
      // stop io context
      m_work.reset();
      if (m_io_thread.joinable())
        m_io_thread.join();
    }

    size_t CSampleSender::Send(const std::string& sample_name_, const std::vector<char>& serialized_sample_)
    {
      // ------------------------------------------------
      // emulate old protocol
      // 
      // s1 = size of the sample name
      // s2 = size of the serialized sample payload
      // 
      //  2 Bytes sample name size (unsigned short)
      // s1 Bytes sample name
      // s2 Bytes serialized sample
      // ------------------------------------------------
      unsigned short s1 = static_cast<unsigned short>(sample_name_.size());
      size_t         s2 = serialized_sample_.size();
      asio::const_buffer sample_name_size_asio_buffer(&s1, 2);
      asio::const_buffer sample_name_asio_buffer(sample_name_.data(), s1);
      asio::const_buffer serialized_sample_asio_buffer(serialized_sample_.data(), s2);

      std::mutex              send_mtx;
      std::condition_variable send_cond;
      bool                    send_finished(false);

      m_socket->async_send_to({ sample_name_size_asio_buffer, sample_name_asio_buffer, serialized_sample_asio_buffer }
        , m_destination_endpoint
        , [&send_mtx, &send_cond, &send_finished](asio::error_code ec)
        {
          if (ec)
          {
            std::cout << "CSampleSender: Error sending: " << ec.message() << std::endl;
          }

          // notify waiting main thread
          {
            std::lock_guard<std::mutex> lock(send_mtx);
            send_finished = true;
            send_cond.notify_all();
          }

        });

      // wait for completion handler of async_send_to
      {
        std::unique_lock<std::mutex> lock(send_mtx);
        send_cond.wait(lock, [&send_finished]()->bool {return send_finished; });
      }

      return 0;
    }
  }
}
