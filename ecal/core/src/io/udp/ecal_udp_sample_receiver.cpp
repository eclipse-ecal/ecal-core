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

#include <array>
#include <chrono>
#include <iostream>
#include <memory>

namespace eCAL
{
  namespace UDP
  {
    CSampleReceiver::CSampleReceiver(const IO::UDP::SReceiverAttr& attr_, HasSampleCallbackT has_sample_callback_, ApplySampleCallbackT apply_sample_callback_) :
      m_has_sample_callback(std::move(has_sample_callback_)), m_apply_sample_callback(std::move(apply_sample_callback_)), m_broadcast(attr_.broadcast)
    {
      m_io_context = std::make_shared<asio::io_context>();
      m_work = std::make_shared<asio::io_context::work>(*m_io_context);
      m_socket     = std::make_shared<ecaludp::Socket>(*m_io_context, std::array<char, 4>{'E', 'C', 'A', 'L'});

      // create socket
      const asio::ip::udp::endpoint listen_endpoint(asio::ip::udp::v4(), static_cast<unsigned short>(attr_.port));
      {
        asio::error_code ec;
        m_socket->open(listen_endpoint.protocol(), ec);
        if (ec)
        {
          std::cerr << "CUDPReceiverAsio: Unable to open socket: " << ec.message() << std::endl;
          return;
        }
      }

      // set socket reuse
      {
        asio::error_code ec;
        m_socket->set_option(asio::ip::udp::socket::reuse_address(true), ec);
        if (ec)
        {
          std::cerr << "CUDPReceiverAsio: Unable to set reuse-address option: " << ec.message() << std::endl;
        }
      }

      // bind socket
      {
        asio::error_code ec;
        m_socket->bind(listen_endpoint, ec);
        if (ec)
        {
          std::cerr << "CUDPReceiverAsio: Unable to bind socket to " << listen_endpoint.address().to_string() << ":" << listen_endpoint.port() << ": " << ec.message() << std::endl;
          return;
        }
      }

      // set loopback option
      {
        const asio::ip::multicast::enable_loopback loopback(attr_.loopback);
        asio::error_code ec;
        m_socket->set_option(loopback, ec);
        if (ec)
        {
          std::cerr << "CUDPReceiverAsio: Unable to enable loopback: " << ec.message() << std::endl;
        }
      }

      // set receive buffer size (default = 1 MB)
      {
        int rcvbuf = 1024 * 1024;
        if (attr_.rcvbuf > 0) rcvbuf = attr_.rcvbuf;
        const asio::socket_base::receive_buffer_size recbufsize(rcvbuf);
        asio::error_code ec;
        m_socket->set_option(recbufsize, ec);
        if (ec)
        {
          std::cerr << "CUDPReceiverAsio: Unable to set receive buffer size: " << ec.message() << std::endl;
        }
      }

      // join multicast group
      AddMultiCastGroup(attr_.address.c_str());

      // start io_context
      m_work = std::make_shared<asio::io_context::work>(*m_io_context);

      // start receiving
      Receive();
    }

    CSampleReceiver::~CSampleReceiver()
    {
      m_work.reset();
    }

    bool CSampleReceiver::AddMultiCastGroup(const char* ipaddr_)
    {
      if (!m_broadcast)
      {
        asio::error_code ec;
        m_socket->set_option(asio::ip::multicast::join_group(asio::ip::make_address(ipaddr_)), ec);
        if (ec)
        {
          std::cerr << "CSampleReceiver: Unable to join multicast group: " << ec.message() << std::endl;
          return false;
        }
      }
      return true;
    }

    bool CSampleReceiver::RemMultiCastGroup(const char* ipaddr_)
    {
      if (!m_broadcast)
      {
        asio::error_code ec;
        m_socket->set_option(asio::ip::multicast::leave_group(asio::ip::make_address(ipaddr_)), ec);
        if (ec)
        {
          std::cerr << "CSampleReceiver: Unable to leave multicast group: " << ec.message() << std::endl;
          return false;
        }
      }
      return true;
    }

    void CSampleReceiver::Receive()
    {
      m_socket->async_receive_from(m_sender_endpoint,
        [this](const std::shared_ptr<ecaludp::OwningBuffer>& buffer, asio::error_code ec)
        {
          if (ec)
          {
            std::cout << "CSampleReceiver: Error receiving: " << ec.message() << std::endl;
            return;
          }

          // extract data from the buffer
          auto receive_buffer = static_cast<const char*>(buffer->data());
          bool processed = true;

          // read sample_name size
          unsigned short sample_name_size = 0;
          memcpy(&sample_name_size, receive_buffer, 2);

          // check for damaged data
          if (sample_name_size > buffer->size())
          {
            std::cout << "CSampleReceiver: Received damaged data. Wrong sample name size." << std::endl;
            processed = false;
          }
          else
          {
            // read sample_name
            const std::string sample_name(receive_buffer + sizeof(sample_name_size));

            // calculate payload offset
            auto payload_offset = sizeof(sample_name_size) + sample_name_size;

            // check for damaged data
            if (payload_offset > buffer->size())
            {
              std::cout << "CSampleSender: Received damaged data. Wrong payload buffer offset." << std::endl;
              processed = false;
            }
            else if (m_has_sample_callback(sample_name)) // if we are interested in the sample payload
            {
              // extract payload and its size
              auto payload_buffer = receive_buffer + payload_offset;
              auto payload_buffer_size = buffer->size() - payload_offset;

              // apply the sample payload
              m_apply_sample_callback(payload_buffer, payload_buffer_size);
            }
          }

          // recursively call Receive() to continue listening for data
          if (processed)
          {
            this->Receive();
          }
        });
    }
  }
}
