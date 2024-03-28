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
      m_has_sample_callback(std::move(has_sample_callback_)), m_apply_sample_callback(std::move(apply_sample_callback_))
    {
      m_io_context = std::make_shared<asio::io_context>();
      m_socket     = std::make_shared<ecaludp::Socket>(*m_io_context, std::array<char, 4>{'E', 'C', 'A', 'L'});

      // start receiver thread
      m_udp_receiver_thread = std::make_shared<eCAL::CCallbackThread>([this] { ReceiveThread(); });
      m_udp_receiver_thread->start(std::chrono::milliseconds(0));
    }

    CSampleReceiver::~CSampleReceiver()
    {
      // stop receiver thread
      m_udp_receiver_thread->stop();
    }

    bool CSampleReceiver::AddMultiCastGroup(const char* ipaddr_)
    {
      asio::error_code ec;
      m_socket->set_option(asio::ip::multicast::join_group(asio::ip::make_address(ipaddr_)), ec);
      if (ec)
      {
        std::cerr << "CSampleReceiver: Unable to join multicast group: " << ec.message() << std::endl;
        return false;
      }

      return true;
    }

    bool CSampleReceiver::RemMultiCastGroup(const char* ipaddr_)
    {
      asio::error_code ec;
      m_socket->set_option(asio::ip::multicast::leave_group(asio::ip::make_address(ipaddr_)), ec);
      if (ec)
      {
        std::cerr << "CSampleReceiver: Unable to leave multicast group: " << ec.message() << std::endl;
        return false;
      }

      return true;
    }

    // do we need this extra thread ?
    void CSampleReceiver::ReceiveThread()
    {
      m_socket->async_receive_from(m_sender_endpoint
        , [this](const std::shared_ptr<ecaludp::OwningBuffer>& buffer, asio::error_code ec)
        {
          if (ec)
          {
            std::cout << "CSampleSender: Error receiving: " << ec.message() << std::endl;
            return;
          }

          // apply sample
          if(m_apply_sample_callback) m_apply_sample_callback(static_cast<const char*>(buffer->data()), buffer->size());

          // log it
          //std::string received_string(static_cast<const char*>(buffer->data()), buffer->size());
          //std::cout << "Received " << buffer->size() << " bytes from " << m_sender_endpoint.address().to_string() << ":" << m_sender_endpoint.port() << ": " << received_string << std::endl;
        });
    }
  }
}
