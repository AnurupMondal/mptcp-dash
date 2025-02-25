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
 * Author: John Abraham <john.abraham.in@gmail.com>
 */

#include "animpacket.h"
#include "animnode.h"
#include "animatorview.h"
#include "logqt.h"

#define PI 3.14159265
NS_LOG_COMPONENT_DEFINE ("AnimPacket");

namespace netanim
{
AnimPacketMgr * pAnimPacketMgr = 0;

AnimPacket::AnimPacket (uint32_t fromNodeId,
                        uint32_t toNodeId,
                        qreal firstBitTx,
                        qreal firstBitRx,
                        qreal lastBitTx,
                        qreal lastBitRx,
                        bool isWPacket,
                        QString metaInfo,
                        bool showMetaInfo):
  m_fromNodeId (fromNodeId),
  m_toNodeId (toNodeId),
  m_firstBitTx (firstBitTx),
  m_firstBitRx (firstBitRx),
  m_lastBitTx (lastBitTx),
  m_lastBitRx (lastBitRx),
  m_isWPacket (isWPacket),
  m_infoText (0)
{
  m_fromPos = AnimNodeMgr::getInstance ()->getNode (fromNodeId)->getCenter ();
  m_toPos = AnimNodeMgr::getInstance ()->getNode (toNodeId)->getCenter ();
  //NS_LOG_DEBUG ("FromPos:" << m_fromPos);
  //NS_LOG_DEBUG ("ToPos:" << m_toPos);
  m_line = QLineF (m_fromPos, m_toPos);
  qreal propDelay = m_firstBitRx - m_firstBitTx;
  m_velocity = m_line.length ()/propDelay;
  m_cos = cos ((360 - m_line.angle ()) * PI/180);
  m_sin = sin ((360 - m_line.angle ()) * PI/180);
  setVisible(false);
  setZValue(ANIMPACKET_ZVAVLUE);

  m_infoText = new QGraphicsSimpleTextItem (this);
  if(showMetaInfo)
    {
      m_infoText->setText ("p");
      m_infoText->setFlag (QGraphicsItem::ItemIgnoresTransformations);


      qreal textAngle = m_line.angle ();
      if(textAngle < 90)
        {
          textAngle = 360-textAngle;
        }
      else if (textAngle > 270)
        {
          textAngle = 360-textAngle;
        }
      else
        {
          textAngle = 180-textAngle;
        }
      m_infoText->rotate (textAngle);
      m_infoText->setText (getShortMeta(metaInfo));
    }

}

AnimPacket::~AnimPacket ()
{
  if(m_infoText)
    {
      delete m_infoText;
    }
}

QString
AnimPacket::getShortMeta (QString metaInfo, int filter, bool & result)
{
  result = false;
  QString metaInfoString = metaInfo.toAscii ().data ();

  bool aodvResult = false;
  AodvInfo aodvInfo = parseAodv (metaInfoString, aodvResult);
  bool olsrResult = false;
  OlsrInfo olsrInfo = parseOlsr (metaInfoString, olsrResult);
  bool dsdvResult = false;
  DsdvInfo dsdvInfo = parseDsdv (metaInfoString, dsdvResult);
  bool tcpResult = false;
  TcpInfo tcpInfo = parseTcp (metaInfoString, tcpResult);
  bool udpResult = false;
  UdpInfo udpInfo = parseUdp (metaInfoString, udpResult);
  bool arpResult = false;
  ArpInfo arpInfo = parseArp (metaInfoString, arpResult);
  bool icmpResult = false;
  IcmpInfo icmpInfo = parseIcmp (metaInfoString, icmpResult);
  bool ipv4Result = false;
  Ipv4Info ipv4Info = parseIpv4 (metaInfoString, ipv4Result);
  bool wifiResult = false;
  WifiMacInfo wifiMacInfo = parseWifi (metaInfoString, wifiResult);
  bool pppResult = false;
  PppInfo pppInfo = parsePpp (metaInfoString, pppResult);
  bool ethernetResult = false;
  EthernetInfo ethernetInfo = parseEthernet (metaInfoString, ethernetResult);

  QString finalString = "";
  if (filter == AnimPacket::ALL)
    {
      finalString += aodvInfo.toShortString () +
                     olsrInfo.toShortString () +
                     dsdvInfo.toShortString () +
                     tcpInfo.toShortString () +
                     udpInfo.toShortString () +
                     icmpInfo.toShortString () +
                     ipv4Info.toShortString () +
                     arpInfo.toShortString () +
                     wifiMacInfo.toShortString () +
                     pppInfo.toShortString () +
                     ethernetInfo.toShortString ();
      result = true;
    }
  else
    {
      if (filter & AnimPacket::AODV)
        if (aodvResult)
          finalString += aodvInfo.toShortString ();
      if (filter & AnimPacket::OLSR)
        if (olsrResult)
          finalString += olsrInfo.toShortString ();
      if (filter & AnimPacket::DSDV)
        if (dsdvResult)
          finalString += dsdvInfo.toShortString ();
      if (filter & AnimPacket::TCP)
        if (tcpResult)
          finalString += tcpInfo.toShortString ();
      if (filter & AnimPacket::UDP)
        if (udpResult)
          finalString += udpInfo.toShortString ();
      if (filter & AnimPacket::ICMP)
        if (icmpResult)
          finalString += icmpInfo.toShortString ();
      if (filter & AnimPacket::IPV4)
        if (ipv4Result)
          finalString += ipv4Info.toShortString ();
      if (filter & AnimPacket::ARP)
        if (arpResult)
          finalString += arpInfo.toShortString ();
      if (filter & AnimPacket::WIFI)
        if (wifiResult)
          finalString += wifiMacInfo.toShortString ();
      if (filter & AnimPacket::PPP)
        if (pppResult)
          finalString += pppInfo.toShortString ();
      if (filter & AnimPacket::ETHERNET)
        if (ethernetResult)
          finalString += ethernetInfo.toShortString ();
      if (finalString != "")
        result = true;

    }
  return finalString;

}

QString
AnimPacket::getShortMeta (QString metaInfo)
{
  bool result = false;
  QString metaInfoString = metaInfo.toAscii ().data ();
  AodvInfo aodvInfo = parseAodv (metaInfoString, result);
  if (result)
    {
      return aodvInfo.toShortString ();
    }


  result = false;
  OlsrInfo olsrInfo = parseOlsr (metaInfoString, result);
  if(result)
    {
      return olsrInfo.toShortString ();
    }


  result = false;
  DsdvInfo dsdvInfo = parseDsdv (metaInfoString, result);
  if(result)
    {
      return dsdvInfo.toShortString ();
    }



  result = false;
  TcpInfo tcpInfo = parseTcp (metaInfoString, result);
  if(result)
    {
      return tcpInfo.toShortString ();
    }



  result = false;
  UdpInfo udpInfo = parseUdp (metaInfoString, result);
  if(result)
    {
      return udpInfo.toShortString ();
    }



  result = false;
  ArpInfo arpInfo = parseArp (metaInfoString, result);
  if(result)
    {
      return arpInfo.toShortString ();
    }




  result = false;
  IcmpInfo icmpInfo = parseIcmp (metaInfoString, result);
  if(result)
    {
      return icmpInfo.toShortString ();

    }


  result = false;
  Ipv4Info ipv4Info = parseIpv4 (metaInfoString, result);
  if(result)
    {
      return ipv4Info.toShortString ();
    }


  result = false;
  WifiMacInfo wifiMacInfo = parseWifi (metaInfoString, result);
  if(result)
    {
      return wifiMacInfo.toShortString ();
    }


  result = false;
  PppInfo pppInfo = parsePpp (metaInfoString, result);
  if(result)
    {
      return pppInfo.toShortString ();
    }

  result = false;
  EthernetInfo ethernetInfo = parseEthernet (metaInfoString, result);
  if(result)
    {
      return ethernetInfo.toShortString ();
    }
  return "";

}

uint32_t
AnimPacket::getFromNodeId ()
{
  return m_fromNodeId;
}

uint32_t
AnimPacket::getToNodeId ()
{
  return m_toNodeId;
}

qreal
AnimPacket::getFirstBitTx ()
{
  return m_firstBitTx;
}

qreal
AnimPacket::getFirstBitRx ()
{
  return m_firstBitRx;
}


qreal
AnimPacket::getLastBitRx ()
{
  return m_lastBitRx;
}

qreal
AnimPacket::getLastBitTx ()
{
  return m_lastBitTx;
}

bool
AnimPacket::getIsWPacket ()
{
  return m_isWPacket;
}

QGraphicsSimpleTextItem *
AnimPacket::getInfoTextItem ()
{
  return m_infoText;
}



PppInfo
AnimPacket::parsePpp(QString metaInfo, bool & result)
{
  PppInfo pppInfo;
  QRegExp rx("ns3::PppHeader.*");
  int pos = 0;
  if((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return pppInfo;
    }
  result = true;
  return pppInfo;

}


ArpInfo
AnimPacket::parseArp (QString metaInfo, bool & result)
{
  ArpInfo arpInfo;

  QRegExp rx ("ns3::ArpHeader\\s+\\((request|reply) source mac: ..-..-(..:..:..:..:..:..) source ipv4: (\\S+) (?:dest mac: ..-..-)?(..:..:..:..:..:.. )?dest ipv4: (\\S+)\\)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return arpInfo;
    }
  arpInfo.type = rx.cap (1).toAscii ().data ();
  arpInfo.sourceMac = rx.cap (2).toAscii ().data ();
  arpInfo.sourceIpv4 = rx.cap (3).toAscii ().data ();
  if(QString (rx.cap (4).toAscii ().data ()) != "")
    arpInfo.destMac = rx.cap (4).toAscii ().data ();
  arpInfo.destIpv4  = rx.cap (5).toAscii ().data ();
  result = true;
  return arpInfo;
  /*qDebug (" Type:" + arpInfo->type +
          " SMac:" + arpInfo->sourceMac +
          " SIp:"  + arpInfo->sourceIpv4 +
          " DMac:" + arpInfo->destMac +
          " DIp:"  + arpInfo->destIpv4);*/


}

EthernetInfo
AnimPacket::parseEthernet (QString metaInfo, bool & result)
{
  EthernetInfo ethernetInfo;
  QRegExp rx ("ns3::EthernetHeader \\( length/type\\S+ source=(..:..:..:..:..:..), destination=(..:..:..:..:..:..)\\)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return ethernetInfo;
    }
  ethernetInfo.sourceMac = rx.cap (1).toAscii ().data ();
  ethernetInfo.destMac = rx.cap (2).toAscii ().data ();
  result = true;
  return ethernetInfo;
}


IcmpInfo
AnimPacket::parseIcmp (QString metaInfo, bool & result)
{
  IcmpInfo icmpInfo;

  QRegExp rx ("ns3::Icmpv4Header \\(type=(.*), code=([^\\)]*)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return icmpInfo;
    }
  icmpInfo.type = rx.cap (1).toAscii ().data ();
  icmpInfo.code = rx.cap (2).toAscii ().data ();
  result = true;
  return icmpInfo;

}

UdpInfo
AnimPacket::parseUdp (QString metaInfo, bool & result)
{
  UdpInfo udpInfo;

  QRegExp rx ("ns3::UdpHeader \\(length: (\\S+) (\\S+) > (\\S+)\\)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return udpInfo;
    }
  udpInfo.length = rx.cap (1).toAscii ().data ();
  udpInfo.SPort = rx.cap (2).toAscii ().data ();
  udpInfo.DPort = rx.cap (3).toAscii ().data ();
  result = true;
  return udpInfo;
}

Ipv4Info
AnimPacket::parseIpv4 (QString metaInfo, bool & result)
{
  Ipv4Info ipv4Info;

  QRegExp rx ("ns3::Ipv4Header \\(tos (\\S+) DSCP (\\S+) ECN (\\S+) ttl (\\d+) id (\\d+) protocol (\\d+) .* length: (\\d+) (\\S+) > (\\S+)\\)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return ipv4Info;
    }
  ipv4Info.tos = rx.cap (1).toAscii ().data ();
  ipv4Info.Dscp = rx.cap (2).toAscii ().data ();
  ipv4Info.Ecn = rx.cap (3).toAscii ().data ();
  ipv4Info.Ttl = rx.cap (4).toAscii ().data ();
  ipv4Info.Id = rx.cap (5).toAscii ().data ();
  ipv4Info.protocol = rx.cap (6).toAscii ().data ();
  ipv4Info.length = rx.cap (7).toAscii ().data ();
  ipv4Info.SrcIp = rx.cap (8).toAscii ().data ();
  ipv4Info.DstIp = rx.cap (9).toAscii ().data ();
  result = true;
  return ipv4Info;
}

TcpInfo
AnimPacket::parseTcp (QString metaInfo, bool & result)
{
  TcpInfo tcpInfo;

  QRegExp rx ("ns3::TcpHeader \\((\\S+) > (\\S+) \\[([^\\]]*)\\] Seq=(\\S+) Ack=(\\S+) Win=(\\S+)\\)");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return tcpInfo;
    }
  tcpInfo.SPort = rx.cap (1).toAscii ().data ();
  tcpInfo.DPort = rx.cap (2).toAscii ().data ();
  tcpInfo.flags = rx.cap (3).toAscii ().data ();
  tcpInfo.seq = rx.cap (4).toAscii ().data ();
  tcpInfo.ack = rx.cap (5).toAscii ().data ();
  tcpInfo.window = rx.cap (6).toAscii ().data ();
  result = true;
  return tcpInfo;
}

WifiMacInfo
AnimPacket::parseWifi (QString metaInfo, bool & result)
{
  QRegExp rxCTL_ACK ("ns3::WifiMacHeader \\(CTL_ACK .*RA=(..:..:..:..:..:..)");
  WifiMacInfo wifiMacInfo;
  int pos = 0;
  if ((pos = rxCTL_ACK.indexIn (metaInfo)) != -1)
    {
      wifiMacInfo.type = "CTL_ACK";
      wifiMacInfo.Ra = rxCTL_ACK.cap (1).toAscii ().data ();
      result = true;
      return wifiMacInfo;

    }
  QRegExp rxCTL_RTS ("ns3::WifiMacHeader \\(CTL_RTS .*RA=(..:..:..:..:..:..), TA=(..:..:..:..:..:..)");
  pos = 0;
  if ((pos = rxCTL_RTS.indexIn (metaInfo)) != -1)
    {
      wifiMacInfo.type = "CTL_RTS";
      wifiMacInfo.Ra = rxCTL_RTS.cap (1).toAscii ().data ();
      wifiMacInfo.Sa = rxCTL_RTS.cap (2).toAscii ().data ();
      result = true;
      return wifiMacInfo;

    }

  QRegExp rxCTL_CTS ("ns3::WifiMacHeader \\(CTL_CTS .*RA=(..:..:..:..:..:..)");
  pos = 0;
  if ((pos = rxCTL_CTS.indexIn (metaInfo)) != -1)
    {
      wifiMacInfo.type = "CTL_CTS";
      wifiMacInfo.Ra = rxCTL_CTS.cap (1).toAscii ().data ();
      result = true;
      return wifiMacInfo;

    }

  QRegExp rx ("ns3::WifiMacHeader \\((\\S+) ToDS=(0|1), FromDS=(0|1), .*DA=(..:..:..:..:..:..), SA=(..:..:..:..:..:..), BSSID=(..:..:..:..:..:..)");
  pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return wifiMacInfo;
    }
  wifiMacInfo.type = rx.cap (1).toAscii ().data ();
  wifiMacInfo.toDs = rx.cap (2).toAscii ().data ();
  wifiMacInfo.fromDs = rx.cap (3).toAscii ().data ();
  wifiMacInfo.Da = rx.cap (4).toAscii ().data ();
  wifiMacInfo.Sa = rx.cap (5).toAscii ().data ();
  wifiMacInfo.Bssid = rx.cap (6).toAscii ().data ();

  if(wifiMacInfo.type == "MGT_ASSOCIATION_REQUEST")
    {
      QRegExp rx ("ns3::MgtAssocRequestHeader \\(ssid=(\\S+),");
      int pos = 0;
      if ((pos = rx.indexIn (metaInfo)) == -1)
        {
          result = false;
          return wifiMacInfo;
        }
      wifiMacInfo.SSid = rx.cap (1).toAscii ().data ();
    }
  if(wifiMacInfo.type == "MGT_ASSOCIATION_RESPONSE")
    {
      QRegExp rx ("ns3::MgtAssocResponseHeader \\(status code=(\\S+), rates");
      int pos = 0;
      if ((pos = rx.indexIn (metaInfo)) == -1)
        {
          result = false;
          return wifiMacInfo;
        }
      wifiMacInfo.assocResponseStatus = rx.cap (1).toAscii ().data ();
    }
  result = true;
  return wifiMacInfo;
}

AodvInfo
AnimPacket::parseAodv (QString metaInfo, bool & result)
{
  AodvInfo aodvInfo;

  QRegExp rx ("ns3::aodv::TypeHeader \\((\\S+)\\) ");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return aodvInfo;
    }
  aodvInfo.type = rx.cap (1).toAscii ().data ();
  if(aodvInfo.type == "RREQ")
    {
      QRegExp rx ("ns3::aodv::RreqHeader \\(RREQ ID \\d+ destination: ipv4 (\\S+) sequence number (\\d+) source: ipv4 (\\S+) sequence number \\d+");
      int pos = 0;
      if ((pos = rx.indexIn (metaInfo)) == -1)
        {
          result = false;
          return aodvInfo;
        }
      aodvInfo.destination = rx.cap (1).toAscii ().data ();
      aodvInfo.seq = rx.cap (2).toAscii ().data ();
      aodvInfo.source = rx.cap (1).toAscii ().data ();

    }
  if(aodvInfo.type == "RREP")
    {
      QRegExp rx ("ns3::aodv::RrepHeader \\(destination: ipv4 (\\S+) sequence number (\\d+) source ipv4 (\\S+) ");
      int pos = 0;
      if ((pos = rx.indexIn (metaInfo)) == -1)
        {
          result = false;
          return aodvInfo;
        }
      aodvInfo.destination = rx.cap (1).toAscii ().data ();
      aodvInfo.seq = rx.cap (2).toAscii ().data ();
      aodvInfo.source = rx.cap (1).toAscii ().data ();
    }
  if(aodvInfo.type == "RERR")
    {
      QRegExp rx ("ns3::aodv::RerrHeader \\(([^\\)]+) \\(ipv4 address, seq. number):(\\S+) ");
      int pos = 0;
      if ((pos = rx.indexIn (metaInfo)) == -1)
        {
          result = false;
          return aodvInfo;
        }
      aodvInfo.rerrInfo = rx.cap (1).toAscii ().data ();
      aodvInfo.destination = rx.cap (2).toAscii ().data ();
    }
  result = true;
  return aodvInfo;

}

DsdvInfo
AnimPacket::parseDsdv (QString metaInfo, bool & result)
{
  DsdvInfo dsdvInfo;

  QRegExp rx ("ns3::dsdv::DsdvHeader");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return dsdvInfo;
    }
  result = true;
  return dsdvInfo;

}

OlsrInfo
AnimPacket::parseOlsr (QString metaInfo, bool & result)
{
  OlsrInfo olsrInfo;

  QRegExp rx ("ns3::olsr::MessageHeader");
  int pos = 0;
  if ((pos = rx.indexIn (metaInfo)) == -1)
    {
      result = false;
      return olsrInfo;
    }
  result = true;
  return olsrInfo;
}







#if 0
void
AnimPacket::update (qreal t)
{
  qreal timeElapsed = t - getFirstBitTx ();
  qreal distanceTravelled = m_velocity * timeElapsed;
  m_distanceTraveled = distanceTravelled;
  qreal x = m_distanceTraveled * m_cos;
  qreal y = m_distanceTraveled * m_sin;
  m_head = QPointF (m_fromPos.x () + x,  m_fromPos.y () + y);
  //NS_LOG_DEBUG ("Upd Time:" << t << " Head:" << m_head << " Distance traveled:" << m_distanceTraveled << " time elapsed:" << timeElapsed  << " velocity:" << m_velocity);
}

#else
void
AnimPacket::update (qreal t)
{
  m_currentTime = t;
  qreal midPointX = (m_toPos.x () + m_fromPos.x ())/2;
  qreal midPointY = (m_toPos.y () + m_fromPos.y ())/2;
  if (m_isWPacket)
    {
      m_head = QPointF (midPointX, midPointY);
    }
  else
    {
      if (t > getFirstBitRx ())
        {
          m_head = m_toPos;
        }
      else
        {
          qreal timeElapsed = t - getFirstBitTx ();
          qreal distanceTravelled = m_velocity * timeElapsed;
          qreal x = distanceTravelled * m_cos;
          qreal y = distanceTravelled * m_sin;
          m_head = QPointF (m_fromPos.x () + x, m_fromPos.y () + y);
        }

    }
  //m_head = QPointF (100,100);
  //NS_LOG_DEBUG ("m_head:" << m_head);
}

#endif
QRectF
AnimPacket::boundingRect () const
{
  return m_boundingRect;
  //QRectF r = QRectF(-m_boundingRect.width(),  -m_boundingRect.height(), m_boundingRect.width(), m_boundingRect.height());
  //return r;
}


void
AnimPacket::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  //NS_LOG_DEBUG ("Packet Transform:" << transform());
  //NS_LOG_DEBUG ("Device Transform:" << painter->deviceTransform());
  //NS_LOG_DEBUG ("Scene Transform:" << sceneTransform());
  QPen p;
  QTransform viewTransform = AnimatorView::getInstance ()->transform ();

  painter->save ();
  QPainterPath arrowTailPath;
  arrowTailPath.moveTo (0, 0);
  qreal mag = 5;
  qreal transformedMag = 2 * (10/viewTransform.m22 ());

  if (!m_isWPacket)
    {
      qreal timeElapsed = m_currentTime - getFirstBitTx ();
      mag = m_velocity * timeElapsed;
      qreal magLbTx = 0;
      //NS_LOG_DEBUG ("First Bit Mag:" << mag);
      if (m_currentTime > getLastBitTx ())
        {
          timeElapsed = m_currentTime - getLastBitTx ();
          magLbTx = m_velocity * timeElapsed;
        }
      if (m_currentTime > getFirstBitRx ())
        {
          qreal fullDistance = m_velocity * (getFirstBitRx () - getFirstBitTx ());
          mag = fullDistance;

        }
      mag -= magLbTx;
      //NS_LOG_DEBUG ("Mag:" << mag << " MagLbTx:" << magLbTx);
      arrowTailPath.lineTo (-mag, 0);
      arrowTailPath.addEllipse( QPointF (-mag, 0), transformedMag/10, transformedMag/10);
      //arrowTailPath.lineTo (-mag , 0);


    }
  else
    {
      arrowTailPath.lineTo (-mag * (10/viewTransform.m22 ()) , 0);
    }
  p.setColor (Qt::blue);
  //p.setWidthF (0.75);
  painter->setPen (p);
  painter->rotate (360 - m_line.angle ());
  painter->drawPath (arrowTailPath);
  painter->restore ();


  QPolygonF arrowHeadPolygon;

  QPainterPath arrowHeadPath;
  qreal arrowHeadLength = transformedMag;

  arrowHeadPolygon << QPointF (0, 0)
                   << QPointF (-arrowHeadLength * cos (PI/10), -arrowHeadLength * sin (PI/10))
                   << QPointF (-(arrowHeadLength/2) * cos (PI/10), 0)
                   << QPointF (-arrowHeadLength * cos (PI/10), arrowHeadLength * sin (PI/10));

  arrowHeadPath.lineTo (-arrowHeadLength * cos (PI/10), -arrowHeadLength * sin (PI/10));
  arrowHeadPath.moveTo (0, 0);
  arrowHeadPath.lineTo (-arrowHeadLength * cos (PI/10), arrowHeadLength * sin (PI/10));
  arrowHeadPath.moveTo (0, 0);

  arrowHeadPath.moveTo (0, 0);
  painter->save();
  QPen arrowHeadPen;
  //arrowHeadPen.setWidthF (0.75);

  QColor black (0, 0, 5, 130);
  arrowHeadPen.setColor (black);

  painter->setPen(arrowHeadPen);
  painter->rotate (360 - m_line.angle ());
  QBrush brush;
  brush.setColor (black);
  brush.setStyle (Qt::SolidPattern);
  painter->setBrush (brush);
  painter->drawPolygon (arrowHeadPolygon);
  painter->restore ();




  QPainterPath path;
  path.moveTo (0, 0);
  path.addPath (arrowHeadPath);
  path.moveTo (0, 0);
  path.addPath (arrowTailPath);


  m_boundingRect = path.boundingRect ();
  QTransform t;
  t.rotate(360 - m_line.angle ());
  m_boundingRect = t.mapRect (m_boundingRect);





  painter->save ();
  QTransform textTransform;
  qreal textAngle = m_line.angle ();
  if(textAngle < 90)
    {
      textAngle = 360-textAngle;
    }
  else if (textAngle > 270)
    {
      textAngle = 360-textAngle;
    }
  else
    {
      textAngle = 180-textAngle;
    }
  painter->rotate (textAngle);
  textTransform.rotate (textAngle);
  QPainterPath textPath;
  QFont f ;
  f.setKerning (false);
  f.setStyleHint (QFont::Times);

  f.setPointSizeF (15/viewTransform.m22 ());
  f.setFixedPitch (true);

  QPen p1 = painter->pen ();
  p1.setColor (Qt::red);
  p1.setStyle (Qt::SolidLine);
  QBrush brush2 = painter->brush ();
  brush2.setStyle (Qt::SolidPattern);
  brush2.setColor (Qt::black);
  p1.setWidthF (1/viewTransform.m22 ());
  p1.setBrush (brush2);
  painter->setBrush (brush2);
  painter->setPen (p1);
  textPath.addText (0, -2/viewTransform.m22 (), f, "Jo");
  painter->setTransform (m_infoText->transform ());
  QRectF textBoundingRect = textTransform.mapRect (textPath.boundingRect ());

  painter->restore ();
  m_boundingRect = QRectF (QPointF(qMin (m_boundingRect.left (), textBoundingRect.left ()),
                                  qMin (m_boundingRect.top (), textBoundingRect.top ())),
                          QPointF(qMax (m_boundingRect.right (), textBoundingRect.right ()),
                                  qMax (m_boundingRect.bottom (), textBoundingRect.bottom ())));


  //NS_LOG_DEBUG ("m_boundingRect:" << m_boundingRect);


}


QPointF
AnimPacket::getHead ()
{
  return m_head;
}

QPointF
AnimPacket::getFromPos ()
{
  return m_fromPos;
}

QPointF
AnimPacket::getToPos ()
{
  return m_toPos;
}

AnimPacketMgr::AnimPacketMgr ()
{
}
AnimPacketMgr *
AnimPacketMgr::getInstance ()
{
  if(!pAnimPacketMgr)
    {
      pAnimPacketMgr = new AnimPacketMgr;
    }
  return pAnimPacketMgr;
}

AnimPacket *
AnimPacketMgr::add (uint32_t fromId, uint32_t toId, qreal fbTx, qreal fbRx, qreal lbTx, qreal lbRx, bool isWPacket, QString metaInfo, bool showMetaInfo)
{
  AnimPacket * pkt = new AnimPacket (fromId, toId, fbTx, fbRx, lbTx, lbRx, isWPacket, metaInfo, showMetaInfo);
  return pkt;
}



}

