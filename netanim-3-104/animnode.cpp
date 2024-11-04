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

#include "animnode.h"
#include "animresource.h"

NS_LOG_COMPONENT_DEFINE ("AnimNode");
namespace netanim
{

AnimNodeMgr * pAnimNodeMgr = 0;

AnimNode::AnimNode (uint32_t nodeId, qreal x, qreal y, QString nodeDescription):m_nodeDescription (0),
  m_nodeId (nodeId),
  m_x (x),
  m_y (y),
  m_showNodeId (true),
  m_resourceId (-1),
  m_showNodeTrajectory (false)
{
  //setVisible (false);
  setZValue (ANIMNODE_ZVALUE);
  m_r = 255;
  m_g = 0;
  m_b = 0;
  if (nodeDescription == "")
    {
      nodeDescription = QString::number (nodeId);
    }
  m_nodeDescription = new QGraphicsTextItem (nodeDescription);
  m_nodeDescription->setFlag (QGraphicsItem::ItemIgnoresTransformations);
  setFlag (QGraphicsItem::ItemIsSelectable);
}

AnimNode::~AnimNode ()
{
  if (m_nodeDescription)
    {
      delete m_nodeDescription;
    }
}

void
AnimNode::showNodeId (bool show)
{
  m_showNodeId = show;
  m_nodeDescription->setVisible (m_showNodeId);
}

int
AnimNode::getResourceId ()
{
  return m_resourceId;
}


void
AnimNode::setResource (int resourceId)
{
  m_resourceId = resourceId;
  QString resourcePath = AnimResourceManager::getInstance ()->get (resourceId);
  //NS_LOG_DEBUG ("Res:" << resourcePath.toAscii ().data ());
  QPixmap pix (resourcePath, "png");
  setPixmap (pix);
  update ();
}

void
AnimNode::setColor (uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
  m_r = r;
  m_g = g;
  m_b = b;
  m_alpha = alpha;
  ResizeableItem::setColor (r, g, b, alpha);
  update ();
}

QColor
AnimNode::getColor ()
{
  QColor c (m_r, m_g, m_b, 255);
  return c;
}

qreal
AnimNode::getWidth ()
{
  return m_width;
}

void
AnimNode::setWidth (qreal width)
{
  m_width = width;
  update ();
}

void
AnimNode::setHeight (qreal height)
{
  m_height = height;
  update ();

}

qreal
AnimNode::getX ()
{
  return m_x;
}

qreal
AnimNode::getY ()
{
  return m_y;
}


void
AnimNode::setPos (qreal x, qreal y)
{
  m_x = x;
  m_y = y;
  QGraphicsItem::setPos (x, y);
}

void
AnimNode::setX (qreal x)
{
  m_x = x;
}


bool
AnimNode::getShowNodeTrajectory ()
{
  return m_showNodeTrajectory;
}

void
AnimNode::setShowNodeTrajectory (bool showNodeTrajectory)
{
  m_showNodeTrajectory = showNodeTrajectory;
}


void
AnimNode::setY (qreal y)
{
  m_y = y;
}

uint32_t
AnimNode::getNodeId ()
{
  return m_nodeId;
}

QGraphicsTextItem *
AnimNode::getDescription ()
{
  return m_nodeDescription;
}

QPointF AnimNode::getCenter ()
{
  return sceneBoundingRect ().center ();
}
void AnimNode::setNodeDescription (QString description)
{
  m_nodeDescription->setPlainText (description);
}

void AnimNode::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  ResizeableItem::paint (painter, option, widget);
}


void AnimNode::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
  ResizeableItem::mouseMoveEvent (event);
  if (m_nodeDescription)
    {
      m_nodeDescription->setPos (sceneBoundingRect ().bottomRight ());
      update ();
    }
}

AnimNode::Ipv4Vector_t
AnimNode::getIpv4Addresses ()
{
  return m_ipv4Vector;
}

AnimNode::MacVector_t
AnimNode::getMacAddresses ()
{
  return m_macVector;
}

void
AnimNode::addIpv4Address (QString ip)
{
  m_ipv4Vector.push_back (ip);
}

void
AnimNode::addMacAddress (QString mac)
{
  m_macVector.push_back (mac);
}

bool
AnimNode::hasIpv4 (QString ip)
{
  bool result = false;
  QStringList quads = ip.split (".");
  if (quads.count () == 4)
    {
      if (quads.at (3) == "255")
        return true;
      for (Ipv4Vector_t::const_iterator i = m_ipv4Vector.begin ();
          i != m_ipv4Vector.end ();
          ++i)
        {
          if (*i == ip)
            {
              //QDEBUG (ip);
              return true;
            }
        }
    }

  return result;
}


bool
AnimNode::hasMac (QString mac)
{
  bool result = false;
  QStringList bytes = mac.split (":");
  if (bytes.count () == 6)
    {
      for (MacVector_t::const_iterator i = m_macVector.begin ();
          i != m_macVector.end ();
          ++i)
        {
          if (*i == mac)
            {
              return true;
            }
        }
    }

  return result;
}


AnimNodeMgr::AnimNodeMgr ():
  m_minX (0),
  m_minY (0),
  m_maxX (0),
  m_maxY (0)
{

}

AnimNodeMgr * AnimNodeMgr::getInstance ()
{
  if (!pAnimNodeMgr)
    {
      pAnimNodeMgr = new AnimNodeMgr;
    }
  return pAnimNodeMgr;
}


AnimNode * AnimNodeMgr::add (uint32_t nodeId, qreal x, qreal y, QString nodeDescription)
{
  if (m_nodes.find (nodeId) != m_nodes.end ())
    {
      //NS_FATAL_ERROR ("NodeId:" << nodeId << " Already exists");
    }
  QPixmap pix (":/resources/ns3logo2.png","png");
  AnimNode * node = new AnimNode (nodeId, x, y, nodeDescription);
  node->setPos (x, y);
  //node->setPixmap (pix);
  m_nodes[nodeId] = node;
  m_minX = qMin (m_minX, x);
  m_minY = qMin (m_minY, y);
  m_maxX = qMax (m_maxX, x);
  m_maxY = qMax (m_maxY, y);

  return node;
}


void
AnimNodeMgr::setSize (qreal width, qreal height)
{
  for (NodeIdAnimNodeMap_t::const_iterator i = m_nodes.begin ();
      i != m_nodes.end ();
      ++i)
    {
      AnimNode * animNode = i->second;
      animNode->setSize (width, height);
    }
}

AnimNode * AnimNodeMgr::getNode (uint32_t nodeId)
{
  return m_nodes[nodeId];
}

uint32_t
AnimNodeMgr::getCount ()
{
  return m_nodes.size ();
}


QPointF
AnimNodeMgr::getMinPoint ()
{
  return QPointF (m_minX, m_minY);
}

QPointF
AnimNodeMgr::getMaxPoint ()
{
  qreal m = qMax (m_maxX, m_maxY);
  return QPointF (m, m);
}


void
AnimNodeMgr::systemReset ()
{
  m_nodes.clear ();
  m_minX = 0;
  m_minY = 0;
  m_maxX = 0;
  m_maxY = 0;
}


void
AnimNodeMgr::addIpv4Address (uint32_t nodeId, QString ip)
{
  getNode (nodeId)->addIpv4Address (ip);
}

void
AnimNodeMgr::addMacAddress (uint32_t nodeId, QString mac)
{
  getNode (nodeId)->addMacAddress (mac);
}

void
AnimNodeMgr::showNodeId (bool show)
{
  for (NodeIdAnimNodeMap_t::const_iterator i = m_nodes.begin ();
      i != m_nodes.end ();
      ++i)
    {
      AnimNode * animNode = i->second;
      animNode->showNodeId (show);
    }

}

AnimNodeMgr::TimePosVector_t
AnimNodeMgr::getPositions (uint32_t nodeId)
{
  return m_nodePositions[nodeId];
}

void
AnimNodeMgr::addAPosition (uint32_t nodeId, qreal t, QPointF pos)
{
  if (m_nodePositions.find (nodeId) == m_nodePositions.end ())
    {
      TimePosVector_t posVector;
      m_nodePositions[nodeId] = posVector;
    }
  TimePosVector_t & pv = m_nodePositions[nodeId];
  TimePosition_t tp;
  tp.p = pos;
  tp.t = t;
  pv.push_back (tp);
}



}

