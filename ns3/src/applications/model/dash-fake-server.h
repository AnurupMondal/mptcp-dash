/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2015 Christian Kreuzberger, Alpen-Adria-Universitaet Klagenfurt
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Christian Kreuzberger <christian.kreuzberger@itec.aau.at>
//

// ns3 - HTTP Server Application class


#ifndef DASH_FAKE_SERVER_APPLICATION_H
#define DASH_FAKE_SERVER_APPLICATION_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/mp-tcp-socket-base.h"

#include <map>
#include <vector>

#include "http-server-fake-virtual-clientsocket.h"


#define CRLF "\r\n"

namespace ns3 {

class Address;
class RandomVariableStream;
class Socket;





class DASHFakeServerApplication : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  DASHFakeServerApplication ();
  virtual ~DASHFakeServerApplication ();

  void TxTrace(Ptr<Packet const> packet);
  void RxTrace(Ptr<Packet const> packet);

protected:
  virtual void DoDispose (void);

  bool m_active;

  uint64_t m_bytes_recv;
  uint64_t m_bytes_sent;


  uint64_t m_last_bytes_recv;
  uint64_t m_last_bytes_sent;


  std::string ImportDASHRepresentations (std::string mpdMetaDataFilename, int video_id);


  bool ConnectionRequested (Ptr<Socket> socket, const Address& address);
  void ConnectionAccepted (Ptr<Socket> socket, const Address& address);


  TracedCallback<Ptr<ns3::Application> /*App*/,
    uint64_t /* TxBytes*/,uint64_t /* RxBytes */, uint32_t /* ConnectionCount */> m_throughputTrace;


  /**
   * \brief Register this new socket and gets a new client ID for this socket, and register this socket
  */
  uint64_t RegisterSocket(Ptr<MpTcpSocketBase> socket);

private:
  std::map<Ptr<MpTcpSocketBase> /* socket */, uint64_t /* socket id */  > m_activeSockets;

  std::map<uint64_t /* socket id */, HttpServerFakeClientSocket* /* client_socket */ > m_activeClients;


  std::map<uint64_t /* socket id */, std::string /* packet buffer */ > m_activePackets;

  std::map<std::string, long> m_fileSizes;
  std::vector<std::string> m_virtualFiles;

  std::map<std::string, std::string> m_mpdFileContents;

  uint64_t m_lastSocketID;

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */


  void OnReadySend (Ptr<Socket> socket, unsigned int txSize);


  void FinishedCallback (uint64_t socket_id);
  void DoFinishSocket(uint64_t socket_id);


  uint16_t m_port; //!< Port on which we listen for incoming packets.
  Ptr<MpTcpSocketBase> m_socket; //!< IPv4 Socket

  std::string m_mpdDirectory;
  std::string m_mpdMetaDataFiles;
  std::string m_metaDataContentDirectory;
  std::string m_hostName;
  Address m_listeningAddress;

  EventId m_reportStatsTimer;
  void ReportStats();
};

} // namespace ns3

#endif /* HTTP_SERVER_APPLICATION_H */
