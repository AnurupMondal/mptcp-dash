/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Morteza Kheirkhah <m.kheirkhah@sussex.ac.uk>
 */

// Network topology
//
//       n0 ----------- n1
// - Flow from n0 to n1 using MpTcpBulkSendApplication.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcpBulkSendExample");

double old_time = 0.0;
bool first = true;
Time current = Time::FromInteger(3, Time::S);
EventId output;

static void
OutputTrace ()
{
 // *stream->GetStream() << newtime << " " << newval << std::endl;
 // old_time = newval;
}

static void CwndTracer (Ptr<OutputStreamWrapper>stream, uint32_t oldval, uint32_t newval)
{
  double new_time = Simulator::Now().GetSeconds();
  if (old_time == 0 && first)
  {
    double mycurrent = current.GetSeconds();
    *stream->GetStream() << new_time << " " << mycurrent << " " << newval << std::endl;
    first = false;
    output = Simulator::Schedule(current,&OutputTrace);
  }
  else
  {
    if (output.IsExpired())
    {
      *stream->GetStream() << new_time << " " << newval << std::endl;
      output.Cancel();
      output = Simulator::Schedule(current,&OutputTrace);
    }
  }
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  if (cwnd_tr_file_name.compare("") == 0)
     {
       NS_LOG_DEBUG ("No trace file for cwnd provided");
       return;
     }
  else
    {
      Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(cwnd_tr_file_name.c_str());
      Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",MakeBoundCallback (&CwndTracer, stream));
    }
}

int
main(int argc, char *argv[])
{
  Packet::EnablePrinting(); // Ensure packet metadata is included
  Packet::EnableChecking();
  
  LogComponentEnable("MpTcpSocketBase", LOG_INFO);

  Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(100));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(8)); // Sink
  //Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
  //Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));

  // LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL); 
  // LogComponentEnable ("MpTcpSocketBase", LOG_LEVEL_ALL);
  // LogComponentEnable ("MpTcpPacketSink", LOG_LEVEL_ALL);

  NodeContainer nodes;
  nodes.Create(2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
  
  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute("DataRate", StringValue("50Mbps"));
  pointToPoint1.SetChannelAttribute("Delay", StringValue("2ms"));

  std::vector<NetDeviceContainer> devices;
  NetDeviceContainer d0 = pointToPoint.Install(nodes);
  devices.push_back(d0);
  NetDeviceContainer d1 = pointToPoint1.Install(nodes);
  devices.push_back(d1);

  InternetStackHelper internet;
  internet.Install(nodes);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0 = ipv4.Assign(devices[0]);
  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1 = ipv4.Assign(devices[1]);

  uint16_t port = 9;
  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install(nodes.Get(1));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(10.0));

  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(i0.GetAddress(1)), port));
  source.SetAttribute("MaxBytes", UintegerValue(14000000));
  ApplicationContainer sourceApps = source.Install(nodes.Get(0));
  sourceApps.Start(Seconds(0.0));

  AnimationInterface anim("mptcp.xml");

  sourceApps.Stop(Seconds(10.0));
  
  pointToPoint.EnablePcapAll("mptcp");

  // Flow monitor
  std::string fileName = "cwnd_flow_monitor";
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();
  Simulator::Schedule(Seconds(0.00001), &TraceCwnd, fileName);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(20.0));
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO ("Done.");

}
