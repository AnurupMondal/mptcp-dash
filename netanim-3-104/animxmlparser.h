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

#ifndef ANIMXMLPARSER_H
#define ANIMXMLPARSER_H


#include "common.h"
#include "animevent.h"

namespace netanim
{

enum ParsedElementType
{
  XML_INVALID,
  XML_ANIM,
  XML_TOPOLOGY,
  XML_NODE,
  XML_LINK,
  XML_NONP2P_LINK,
  XML_PACKET_RX,
  XML_WPACKET_RX,
  XML_LINKUPDATE,
  XML_NODEUPDATE,
  XML_RESOURCE,
  XML_BACKGROUNDIMAGE
};

struct ParsedElement
{
  ParsedElementType type;

  // Anim
  double version;

  // Topology
  qreal topo_width;
  qreal topo_height;

  // Node

  uint32_t nodeId;
  qreal node_x;
  qreal node_y;
  qreal node_batteryCapacity;
  uint8_t node_r;
  uint8_t node_g;
  uint8_t node_b;
  qreal node_width;
  qreal node_height;
  bool visible;

  // Link

  uint32_t link_fromId;
  uint32_t link_toId;

  // Link description

  QString fromNodeDescription;
  QString toNodeDescription;
  QString linkDescription;

  // Is Wpacket
  bool isWpacket;

  // Packet Rx
  double packetrx_fbTx;
  double packetrx_lbTx;
  uint32_t packetrx_fromId;
  double packetrx_toId;
  double packetrx_fbRx;
  double packetrx_lbRx;

  //meta-info
  QString meta_info;
  QString nodeDescription;


  // Update time
  double updateTime;

  // Has Color update
  bool hasColorUpdate;
  bool hasBattery; //!< Has battery with possible capacity

  // Background image properties
  QString fileName;
  qreal x;
  qreal y;
  qreal scaleX;
  qreal scaleY;
  qreal opacity;

  // Resource
  QString resourcePath;
  uint32_t resourceId;

  typedef enum
  {
    POSITION,
    COLOR,
    DESCRIPTION,
    SIZE,
    IMAGE
  } NodeUpdate_Type;
  // node update type
  NodeUpdate_Type nodeUpdateType;

};


class Animxmlparser
{
public:
  Animxmlparser (QString traceFileName);
  ~Animxmlparser ();
  ParsedElement parseNext ();
  bool isParsingComplete ();
  double getMaxSimulationTime ();
  void setMaxSimulationTime (qreal t);
  bool isFileValid ();
  uint64_t getRxCount ();
  void doParse ();
  qreal getLastPacketEventTime ();
  qreal getThousandthPacketTime ();
  qreal getFirstPacketTime ();
  QPointF getMinPoint ();
  QPointF getMaxPoint ();


private:
  QString m_traceFileName;
  bool m_parsingComplete;
  QXmlStreamReader * m_reader;
  QFile * m_traceFile;
  double m_maxSimulationTime;
  bool m_fileIsValid;
  qreal m_lastPacketEventTime;
  double m_version;
  qreal m_thousandThPacketTime;
  qreal m_firstPacketTime;

  qreal m_minNodeX;
  qreal m_minNodeY;
  qreal m_maxNodeX;
  qreal m_maxNodeY;

  ParsedElement parseAnim ();
  ParsedElement parseTopology ();
  ParsedElement parseNode ();
  ParsedElement parseLink ();
  ParsedElement parseNonP2pLink ();
  ParsedElement parseBackground ();
  ParsedElement parsePacket ();
  ParsedElement parseWPacket ();
  ParsedElement parseLinkUpdate ();
  ParsedElement parseNodeUpdate ();
  ParsedElement parseP ();
  ParsedElement parseWp ();
  ParsedElement parseResource ();
  void parseGeneric (ParsedElement &);

  void searchForVersion ();
};

} // namespace netanim
#endif // ANIMXMLPARSER_H
