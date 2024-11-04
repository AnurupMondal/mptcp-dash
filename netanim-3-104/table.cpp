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

#include "table.h"
namespace netanim {

Table::Table ()
{
}

void
Table::setHeaderList (QStringList headerList)
{
  setColumnCount (headerList.size ());
  QTableWidget::setHorizontalHeaderLabels (headerList);
}

void
Table::addRow (QStringList rowContents, bool autoAdjust)
{
  uint32_t rows = rowCount ();
  setRowCount (rows + 1);
  uint32_t column = 0;
  foreach (QString st, rowContents)
    {
      QTableWidgetItem * item = new QTableWidgetItem (st);
      setItem (rows, column++, item);
    }
  if (autoAdjust)
    {
      resizeRowsToContents();
      resizeColumnsToContents ();
    }
}


void
Table::removeAllRows ()
{
  for (int i = 0; i < rowCount (); ++i)
    {
      removeRow (i);
    }
  setRowCount (0);
}


void
Table::adjust ()
{
  resizeRowsToContents();
  resizeColumnsToContents ();
}


}
