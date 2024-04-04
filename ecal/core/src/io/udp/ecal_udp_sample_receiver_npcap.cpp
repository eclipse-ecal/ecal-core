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

#include "ecal_udp_sample_receiver_npcap.h"

#include <array>
#include <iostream>

namespace eCAL
{
  namespace UDP
  {
    CSampleReceiverNpcap::CSampleReceiverNpcap(const SReceiverAttr& attr_, const HasSampleCallbackT& has_sample_callback_, const ApplySampleCallbackT& apply_sample_callback_) :
      m_has_sample_callback(has_sample_callback_), m_apply_sample_callback(apply_sample_callback_),
      m_broadcast(attr_.broadcast)
    {
      // initialize io context
      m_io_context = std::make_shared<asio::io_context>();
      m_work       = std::make_shared<asio::io_context::work>(*m_io_context);

      // create the socket and set all socket options
      InitializeSocket(attr_);

      // join multicast group
      AddMultiCastGroup(attr_.address.c_str());

      // run the io context
      m_io_thread = std::thread([this] { m_io_context->run(); });

      // start receiving
      Receive();
    }

    CSampleReceiverNpcap::~CSampleReceiverNpcap()
    {
      // TODO: cancel async socket operations ??
      //m_socket_npcap->cancel();

      // stop io context
      //m_work.reset();
      m_io_context->stop();
      if (m_io_thread.joinable())
        m_io_thread.join();
    }

    bool CSampleReceiverNpcap::AddMultiCastGroup(const char* ipaddr_)
    {
      if (!m_broadcast)
      {
        m_socket->join_multicast_group(asio::ip::make_address_v4(ipaddr_));
      }
      return true;
    }

    bool CSampleReceiverNpcap::RemMultiCastGroup(const char* ipaddr_)
    {
      if (!m_broadcast)
      {
        m_socket->leave_multicast_group(asio::ip::make_address_v4(ipaddr_));
      }
      return true;
    }

    void CSampleReceiverNpcap::InitializeSocket(const SReceiverAttr& attr_)
    {
      // create socket
      m_socket = std::make_shared<ecaludp::SocketNpcap>(std::array<char, 4>{'E', 'C', 'A', 'L'});

      // TDOD: open socket ??

      // TODO: set socket reuse ??

      // set loopback option
      m_socket->set_multicast_loopback_enabled(attr_.loopback);

      // set receive buffer size (default = 1 MB)
      {
        int rcvbuf = 1024 * 1024;
        if (attr_.rcvbuf > 0) rcvbuf = attr_.rcvbuf;
        m_socket->set_receive_buffer_size(rcvbuf);
      }

      // bind socket
      const asio::ip::udp::endpoint listen_endpoint(asio::ip::udp::v4(), static_cast<unsigned short>(attr_.port));
      m_socket->bind(listen_endpoint);
    }

    void CSampleReceiverNpcap::Receive()
    {
      m_socket->async_receive_from(m_sender_endpoint,
        [this](const std::shared_ptr<ecaludp::OwningBuffer>& buffer, const ecaludp::Error& error)
        {
          // TODO: should be triggered by m_socket->cancel in destructor but cancel() currently not existing ??
          //if (error == asio::error::operation_aborted)
          //{
          //  m_socket_npcap->close();
          //  return;
          //}

          // this means "receive operation finished succesfully" ?
          if (error == ecaludp::Error::OK)
          {
            return;
          }
          else
          {
            // because we can not shutdown gracefully by using cancel() + close() we will ignore "SOCKET_CLOSED" error
            if (error != ecaludp::Error::SOCKET_CLOSED)
            {
              std::cout << "CSampleReceiverNpcap: Error receiving: " << error.ToString() << '\n';
              return;
            }
          }

          // extract data from the buffer
          const char* receive_buffer = static_cast<const char*>(buffer->data());
          bool processed = true;

          // read sample_name size
          unsigned short sample_name_size = 0;
          memcpy(&sample_name_size, receive_buffer, 2);

          // check for damaged data
          if (sample_name_size > buffer->size())
          {
            std::cout << "CSampleReceiverNpcap: Received damaged data. Wrong sample name size." << '\n';
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
              std::cout << "CSampleReceiverNpcap: Received damaged data. Wrong payload buffer offset." << '\n';
              processed = false;
            }
            else if (m_has_sample_callback(sample_name)) // if we are interested in the sample payload
            {
              // extract payload and its size
              const char* payload_buffer = receive_buffer + payload_offset;
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
