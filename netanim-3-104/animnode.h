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
#ifndef ANIMNODE_H
#define ANIMNODE_H

#include "common.h"
#include "resizeableitem.h"

namespace netanim
{

typedef
struct {
  qreal t;
  QPointF p;
} TimePosition_t;

class AnimNode: public ResizeableItem
{
public:
  typedef QVector <QString> Ipv4Vector_t;
  typedef QVector <QString> MacVector_t;

  AnimNode (uint32_t nodeId, qreal x, qreal y, QString nodeDescription);
  ~AnimNode ();
  void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
  void setNodeDescription (QString description);
  QPointF getCenter ();
  QGraphicsTextItem * getDescription ();
  qreal getX ();
  qreal getY ();
  void setX (qreal x);
  void setY (qreal y);
  bool getShowNodeTrajectory ();
  QColor getColor ();
  uint32_t getNodeId ();
  qreal getWidth ();
  int getResourceId ();
  Ipv4Vector_t getIpv4Addresses ();
  MacVector_t getMacAddresses ();
  void setWidth (qreal width);
  void setHeight (qreal height);
  void setColor (uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);
  void setResource (int resourceId);
  void setPos (qreal x, qreal y);
  void setShowNodeTrajectory (bool showNodeTrajectory);
  void addIpv4Address (QString ip);
  void addMacAddress (QString mac);
  bool hasIpv4 (QString ip);
  bool hasMac (QString mac);
  void showNodeId (bool show);
private:
  QGraphicsTextItem * m_nodeDescription;
  uint32_t m_nodeId;
  qreal m_x;
  qreal m_y;
  bool m_showNodeId;
  Ipv4Vector_t m_ipv4Vector;
  MacVector_t m_macVector;
  int m_resourceId;
  bool m_showNodeTrajectory;


};


class AnimNodeMgr
{
public:
  typedef std::map <uint32_t, AnimNode *> NodeIdAnimNodeMap_t;
  typedef QVector <TimePosition_t> TimePosVector_t;
  typedef std::map <uint32_t, TimePosVector_t> NodeIdPositionMap_t;

  static AnimNodeMgr * getInstance ();
  AnimNode * getNode (uint32_t nodeId);
  AnimNode * add (uint32_t nodeId, qreal x, qreal y, QString nodeDescription);
  uint32_t getCount ();
  QPointF getMinPoint ();
  QPointF getMaxPoint ();
  void systemReset ();
  void addIpv4Address (uint32_t nodeId, QString ip);
  void addMacAddress (uint32_t nodeId, QString mac);
  void setSize (qreal width, qreal height);
  void showNodeId (bool show);
  TimePosVector_t getPositions (uint32_t nodeId);
  void addAPosition (uint32_t nodeId, qreal t, QPointF pos);


private:
  AnimNodeMgr ();
  NodeIdAnimNodeMap_t m_nodes;
  qreal m_minX;
  qreal m_minY;
  qreal m_maxX;
  qreal m_maxY;
  NodeIdPositionMap_t m_nodePositions;

};


}
#endif // ANIMNODE_H
