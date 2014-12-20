#pragma once
/*
 *      Copyright (C) 2014 Team XBMC
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

#include <map>
#include <string>
#include <vector>

#include "activity/IActivity.h"
#include "settings/lib/ISettingCallback.h"
#include "threads/CriticalSection.h"

#define NOTIFICATION_DISPLAY_TIME 5000
#define NOTIFICATION_MESSAGE_TIME 1000

typedef std::vector<ActivityPtr> Activities;

class CActivityLog : public ISettingCallback
{
public:
  virtual ~CActivityLog();

  static CActivityLog& GetInstance();

  Activities Get() const;
  Activities Get(ActivityLevel level, bool includeHigherLevels = false) const;
  ActivityPtr Get(const std::string& activityIdentifier) const;

  void Add(ActivityPtr activity);
  void Add(ActivityPtr activity, bool withNotification, bool withSound = true);
  void AddWithNotification(ActivityPtr activity, unsigned int displayTime = NOTIFICATION_DISPLAY_TIME, unsigned int messageTime = NOTIFICATION_MESSAGE_TIME, bool withSound = true);
  void AddWithNotification(ActivityPtr activity, bool withSound);
  void Remove(ActivityPtr activity);
  void Remove(const std::string& activityIdentifier);
  void Clear();
  void Clear(ActivityLevel level, bool includeHigherLevels = false);

  bool Execute(const std::string& activityIdentifier);

protected:
  CActivityLog();
  CActivityLog(const CActivityLog&);
  CActivityLog const& operator=(CActivityLog const&);

  // implementation of ISettingCallback
  virtual void OnSettingAction(const CSetting *setting);

private:
  void SendMessage(const ActivityPtr activity, int message);

  typedef std::map<std::string, ActivityPtr> ActivitiesMap;
  Activities m_activities;
  ActivitiesMap m_activitiesMap;
  CCriticalSection m_critical;
};
