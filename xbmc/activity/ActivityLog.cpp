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

#include "ActivityLog.h"
#include "FileItem.h"
#include "GUIUserMessages.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "dialogs/GUIDialogSelect.h"
#include "filesystem/ActivitiesDirectory.h"
#include "guilib/GUIWindowManager.h"
#include "settings/Settings.h"
#include "threads/SingleLock.h"
#include "utils/Variant.h"

CActivityLog::CActivityLog()
  : m_activities(),
    m_activitiesMap()
{ }

CActivityLog::~CActivityLog()
{
  Clear();
}

CActivityLog& CActivityLog::GetInstance()
{
  static CActivityLog s_instance;
  return s_instance;
}

Activities CActivityLog::Get() const
{
  Activities activities;

  CSingleLock lock(m_critical);
  for (auto activity : m_activities)
    activities.push_back(activity);

  return activities;
}

Activities CActivityLog::Get(ActivityLevel level, bool includeHigherLevels /* = false */) const
{
  Activities activities;

  CSingleLock lock(m_critical);
  for (auto activity : m_activities)
  {
    if (activity->GetLevel() == level ||
       (includeHigherLevels && activity->GetLevel() > level))
      activities.push_back(activity);
  }

  return activities;
}

ActivityPtr CActivityLog::Get(const std::string& activityIdentifier) const
{
  if (activityIdentifier.empty())
    return ActivityPtr();

  CSingleLock lock(m_critical);
  auto activity = m_activitiesMap.find(activityIdentifier);
  if (activity == m_activitiesMap.end())
    return ActivityPtr();

  return activity->second;
}

void CActivityLog::Add(ActivityPtr activity)
{
  if (activity == NULL || activity->GetIdentifier().empty() ||
      !CSettings::Get().GetBool("activities.enabled") ||
     (activity->GetLevel() == ActivityLevelInformation && !CSettings::Get().GetBool("activities.enablednotifications")))
    return;

  CSingleLock lock(m_critical);
  auto itActivity = m_activitiesMap.find(activity->GetIdentifier());
  if (itActivity != m_activitiesMap.end())
    return;

  m_activities.push_back(activity);
  m_activitiesMap.insert(std::make_pair(activity->GetIdentifier(), activity));

  SendMessage(activity, GUI_MSG_ACTIVITY_ADDED);
}

void CActivityLog::Add(ActivityPtr activity, bool withNotification, bool withSound /* = true */)
{
  if (!withNotification)
    Add(activity);
  else
    AddWithNotification(activity, withSound);
}

void CActivityLog::AddWithNotification(ActivityPtr activity, unsigned int displayTime /* = NOTIFICATION_DISPLAY_TIME */ , unsigned int messageTime /* = NOTIFICATION_MESSAGE_TIME */ , bool withSound /* = true */)
{
  if (activity == NULL)
    return;

  Add(activity);

  // queue the activity as a kai toast notification
  if (!activity->GetIcon().empty())
    CGUIDialogKaiToast::QueueNotification(activity->GetIcon(), activity->GetLabel(), activity->GetDescription(), displayTime, withSound, messageTime);
  else
  {
    CGUIDialogKaiToast::eMessageType type = CGUIDialogKaiToast::Info;
    if (activity->GetLevel() == ActivityLevelWarning)
      type = CGUIDialogKaiToast::Warning;
    else if (activity->GetLevel() == ActivityLevelError)
      type = CGUIDialogKaiToast::Error;

    CGUIDialogKaiToast::QueueNotification(type, activity->GetLabel(), activity->GetDescription(), displayTime, withSound, messageTime);
  }
}

void CActivityLog::AddWithNotification(ActivityPtr activity, bool withSound)
{
  AddWithNotification(activity, NOTIFICATION_DISPLAY_TIME, NOTIFICATION_MESSAGE_TIME, withSound);
}

void CActivityLog::Remove(ActivityPtr activity)
{
  if (activity == NULL || activity->GetIdentifier().empty())
    return;

  CSingleLock lock(m_critical);
  auto itActivity = m_activitiesMap.find(activity->GetIdentifier());
  if (itActivity == m_activitiesMap.end())
    return;

  m_activitiesMap.erase(itActivity);
  for (Activities::iterator it = m_activities.begin(); it != m_activities.end(); ++it)
  {
    if (*it == activity)
    {
      m_activities.erase(it);
      break;
    }
  }

  SendMessage(activity, GUI_MSG_ACTIVITY_REMOVED);
}

void CActivityLog::Remove(const std::string& activityIdentifier)
{
  if (activityIdentifier.empty())
    return;

  CSingleLock lock(m_critical);
  auto itActivity = m_activitiesMap.find(activityIdentifier);
  if (itActivity == m_activitiesMap.end())
    return;

  ActivityPtr activity = itActivity->second;
  m_activitiesMap.erase(itActivity);
  for (Activities::iterator it = m_activities.begin(); it != m_activities.end(); ++it)
  {
    if (*it == activity)
    {
      m_activities.erase(it);
      break;
    }
  }

  SendMessage(activity, GUI_MSG_ACTIVITY_REMOVED);
}

void CActivityLog::Clear()
{
  CSingleLock lock(m_critical);
  m_activities.clear();
  m_activitiesMap.clear();
}

void CActivityLog::Clear(ActivityLevel level, bool includeHigherLevels /* = false */)
{
  Activities activitiesCopy = m_activities;
  for (auto activity : activitiesCopy)
  {
    if (activity->GetLevel() == level ||
       (includeHigherLevels && activity->GetLevel() > level))
      Remove(activity);
  }
}

bool CActivityLog::Execute(const std::string& activityIdentifier)
{
  if (activityIdentifier.empty())
    return false;

  CSingleLock lock(m_critical);
  auto itActivity = m_activitiesMap.find(activityIdentifier);
  if (itActivity == m_activitiesMap.end())
    return false;

  return itActivity->second->Execute();
}

void CActivityLog::OnSettingAction(const CSetting *setting)
{
  if (setting == NULL)
    return;

  const std::string& settingId = setting->GetId();
  if (settingId == "activities.show")
  {
    std::vector<std::string> params;
    params.push_back("activities://");
    params.push_back("return");
    g_windowManager.ActivateWindow(WINDOW_ACTIVITY_LOG, params);
  }
}

void CActivityLog::SendMessage(const ActivityPtr activity, int message)
{
  CFileItemPtr item(new CFileItem(activity->GetLabel()));
  item->SetLabel2(activity->GetDescription());
  item->SetProperty("Activity.ID", activity->GetIdentifier());

  CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, message, 0, item);
  g_windowManager.SendThreadMessage(msg);
}
