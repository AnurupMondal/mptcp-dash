/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015 Christian Kreuzberger and Daniel Posch, Alpen-Adria-University
 * Klagenfurt
 *
 * This file is part of amus-ndnSIM, based on ndnSIM. See AUTHORS for complete list of
 * authors and contributors.
 *
 * amus-ndnSIM and ndnSIM are free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * amus-ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * amus-ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "adaptation-logic-guided.h"
#include "multimedia-player.h"


namespace dash
{
namespace player
{

ENSURE_ADAPTATION_LOGIC_INITIALIZED(GuidedAdaptationLogic)

ISegmentURL*
GuidedAdaptationLogic::GetNextSegment(unsigned int *requested_segment_number, const dash::mpd::IRepresentation **usedRepresentation, bool *hasDownloadedAllSegments)
{
  //TODO: read from file
  double last_download_speed = this->m_multimediaPlayer->GetLastDownloadBitRate();

  if(currentSegmentNumber < getTotalSegments ())
    *hasDownloadedAllSegments = false;
  else
  {
    *hasDownloadedAllSegments = true;
    return NULL; // everything downloaded
  }


  const IRepresentation* useRep = NULL;

  double highest_bitrate = 0.0;

  RepresentationsMap::iterator it;

  //find out the request id
  const IRepresentation* rep=m_availableRepresentations->begin()->second;
  std::string uri=rep->GetSegmentList()->GetSegmentURLs().at(0)->GetMediaURI();
  std::string range=rep->GetSegmentList()->GetSegmentURLs().at(0)->GetMediaRange();

  // for (auto& keyValue : *(this->m_availableRepresentations))
  for ( it = m_availableRepresentations->begin(); it != m_availableRepresentations->end(); it++)
  {
    std::cout << it->first << " " << uri << " " << range << " " << it->second->GetBandwidth() << std::endl;
    const IRepresentation* rep = it->second;
    if (rep->GetBandwidth() < last_download_speed)
    {
      if (rep->GetBandwidth() > highest_bitrate)
      {
        useRep = rep;
        highest_bitrate = rep->GetBandwidth();
      }
    }
  }
  exit(0);
  if (useRep == NULL)
    useRep = GetLowestRepresentation();

  //IRepresentation* rep = (this->m_availableRepresentations->begin()->second);
  *usedRepresentation = useRep;
  *requested_segment_number = currentSegmentNumber;
  *hasDownloadedAllSegments = false;
  return useRep->GetSegmentList()->GetSegmentURLs().at(currentSegmentNumber++);
}
}

}
