/*
 *      Copyright (C) 2015 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */


#include "ActivitiesDirectory.h"
#include "URL.h"
#include "activity/ActivityLog.h"
#include "utils/StringUtils.h"

using namespace XFILE;

bool CActivitiesDirectory::GetDirectory(const CURL& url, CFileItemList &items)
{
  items.ClearProperties();
  items.SetContent("activities");

  CActivityLog& log = CActivityLog::GetInstance();
  Activities activities;

  std::string hostname = url.GetHostName();
  if (hostname.empty())
    activities = log.Get();
  else
  {
    bool includeHigherLevels = false;
    // check if we should only retrieve activities from a specific level or
    // also from all higher levels
    if (StringUtils::EndsWith(hostname, "+"))
    {
      includeHigherLevels = true;

      // remove the "+" from the end of the hostname
      hostname = hostname.substr(0, hostname.size() - 1);
    }

    ActivityLevel level = ActivityLevelFromString(hostname);

    // get the activites of the specified level(s)
    activities = log.Get(level, includeHigherLevels);
  }

  for (auto activity : activities)
    items.Add(ActivityToFileItem(activity));

  return true;
}

CFileItemPtr CActivitiesDirectory::ActivityToFileItem(const ActivityPtr& activity)
{
  if (activity == NULL)
    return CFileItemPtr();

  CFileItemPtr item(new CFileItem(activity->GetLabel()));
  item->m_dateTime = activity->GetDateTime();
  if (!activity->GetIcon().empty())
    item->SetIconImage(activity->GetIcon());

  item->SetProperty(PROPERTY_ACTIVITY_IDENTIFIER, activity->GetIdentifier());
  item->SetProperty(PROPERTY_ACTIVITY_LEVEL, ActivityLevelToString(activity->GetLevel()));
  item->SetProperty(PROPERTY_ACTIVITY_DESCRIPTION, activity->GetDescription());

  return item;
}
