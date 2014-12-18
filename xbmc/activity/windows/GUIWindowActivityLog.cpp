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

#include "GUIWindowActivityLog.h"
#include "FileItem.h"
#include "GUIUserMessages.h"
#include "URL.h"
#include "activity/ActivityLog.h"
#include "filesystem/ActivitiesDirectory.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "guilib/WindowIDs.h"
#include "input/Key.h"
#include "settings/Settings.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "view/ViewStateSettings.h"

#define CONTROL_BUTTON_CLEAR          20
#define CONTROL_BUTTON_LEVEL          21
#define CONTROL_BUTTON_LEVEL_ONLY     22

using namespace std;

CGUIWindowActivityLog::CGUIWindowActivityLog()
  : CGUIMediaWindow(WINDOW_ACTIVITY_LOG, "ActivityLog.xml")
{ }

CGUIWindowActivityLog::~CGUIWindowActivityLog()
{ }

bool CGUIWindowActivityLog::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_INIT:
  {
    m_rootDir.AllowNonLocalSources(false);

    // is this the first time the window is opened?
    if (m_vecItems->GetPath() == "?" && message.GetStringParam().empty())
      m_vecItems->SetPath("");

    break;
  }

  case GUI_MSG_CLICKED:
  {
    int iControl = message.GetSenderId();

    // check if we should clear all items
    if (iControl == CONTROL_BUTTON_CLEAR)
    {
      CActivityLog::GetInstance().Clear(CViewStateSettings::Get().GetActivityLevel(), CViewStateSettings::Get().ShowHigherActivityLevels());

      // refresh the list
      Refresh(true);
      return true;
    }

    // check if we should change the level
    if (iControl == CONTROL_BUTTON_LEVEL)
    {
      // update the activity level
      CViewStateSettings::Get().CycleActivityLevel();
      CSettings::Get().Save();

      // update the listing
      Refresh();
      return true;
    }

    // check if we should change the level
    if (iControl == CONTROL_BUTTON_LEVEL_ONLY)
    {
      // update whether to show higher activity levels
      CViewStateSettings::Get().ToggleShowHigherActivityLevels();
      CSettings::Get().Save();

      // update the listing
      Refresh();
      return true;
    }

    // check if the user interacted with one of the activities
    if (m_viewControl.HasControl(iControl))
    {
      // get selected item
      int itemIndex = m_viewControl.GetSelectedItem();
      if (itemIndex < 0 || itemIndex >= m_vecItems->Size())
        break;

      CFileItemPtr item = m_vecItems->Get(itemIndex);
      int actionId = message.GetParam1();

      if (actionId == ACTION_DELETE_ITEM)
        return OnDelete(item);
    }

    break;
  }

  case GUI_MSG_NOTIFY_ALL:
  {
    CFileItemPtr item = std::dynamic_pointer_cast<CFileItem>(message.GetItem());
    if (item == NULL)
      break;

    switch (message.GetParam1())
    {
    case GUI_MSG_ACTIVITY_ADDED:
      OnActivityAdded(item);
      return true;

    case GUI_MSG_ACTIVITY_REMOVED:
      OnActivityRemoved(item);
      return true;

    default:
      break;
    }
  }

  default:
    break;
  }

  return CGUIMediaWindow::OnMessage(message);
}

bool CGUIWindowActivityLog::OnSelect(int item)
{
  if (item < 0 || item >= m_vecItems->Size())
    return false;

  return OnSelect(m_vecItems->Get(item));
}

void CGUIWindowActivityLog::GetContextButtons(int itemNumber, CContextButtons &buttons)
{
  if (itemNumber < 0 && itemNumber >= m_vecItems->Size())
    return;

  CFileItemPtr item = m_vecItems->Get(itemNumber);
  if (item == NULL)
    return;

  std::string activityIdentifier = item->GetProperty(PROPERTY_ACTIVITY_IDENTIFIER).asString();
  if (activityIdentifier.empty())
    return;

  ActivityPtr activity = CActivityLog::GetInstance().Get(activityIdentifier);
  if (activity == NULL)
    return;

  buttons.Add(CONTEXT_BUTTON_INFO, g_localizeStrings.Get(19033));
  buttons.Add(CONTEXT_BUTTON_DELETE, g_localizeStrings.Get(1210));
  if (activity->CanExecute())
    buttons.Add(CONTEXT_BUTTON_ACTIVATE, activity->GetExecutionLabel());
}

bool CGUIWindowActivityLog::OnContextButton(int itemNumber, CONTEXT_BUTTON button)
{
  if (itemNumber < 0 && itemNumber >= m_vecItems->Size())
    return false;

  CFileItemPtr item = m_vecItems->Get(itemNumber);
  if (item == NULL)
    return false;

  switch (button)
  {
  case CONTEXT_BUTTON_INFO:
    return OnSelect(item);

  case CONTEXT_BUTTON_DELETE:
    return OnDelete(item);

  case CONTEXT_BUTTON_ACTIVATE:
    return OnExecute(item);

  default:
    break;
  }

  return CGUIMediaWindow::OnContextButton(itemNumber, button);
}

void CGUIWindowActivityLog::UpdateButtons()
{
  // only enable the "clear" button if there is something to clear
  CONTROL_ENABLE_ON_CONDITION(CONTROL_BUTTON_CLEAR, m_vecItems->GetObjectCount() > 0);

  ActivityLevel activityLevel = CViewStateSettings::Get().GetActivityLevel();
  // set the label of the "level" button
  SET_CONTROL_LABEL(CONTROL_BUTTON_LEVEL, StringUtils::Format(g_localizeStrings.Get(14119).c_str(), g_localizeStrings.Get(14115 + (int)activityLevel).c_str()));

  // set the label, value and enabled state of the "level only" button
  SET_CONTROL_LABEL(CONTROL_BUTTON_LEVEL_ONLY, 14120);
  SET_CONTROL_SELECTED(GetID(), CONTROL_BUTTON_LEVEL_ONLY, CViewStateSettings::Get().ShowHigherActivityLevels());
  CONTROL_ENABLE_ON_CONDITION(CONTROL_BUTTON_LEVEL_ONLY, activityLevel < ActivityLevelError);

  CGUIMediaWindow::UpdateButtons();
}

bool CGUIWindowActivityLog::GetDirectory(const std::string &strDirectory, CFileItemList &items)
{
  bool result = CGUIMediaWindow::GetDirectory(strDirectory, items);

  ActivityLevel currentLevel = CViewStateSettings::Get().GetActivityLevel();
  bool showHigherLevels = CViewStateSettings::Get().ShowHigherActivityLevels();

  CFileItemList filteredItems(items.GetPath());
  for (int i = 0; i < items.Size(); i++)
  {
    CFileItemPtr item = items.Get(i);
    if (item->IsParentFolder())
    {
      filteredItems.Add(item);
      continue;
    }

    if (!item->HasProperty(PROPERTY_ACTIVITY_LEVEL))
      continue;

    ActivityLevel level = ActivityLevelFromString(item->GetProperty(PROPERTY_ACTIVITY_LEVEL).asString());
    if (level == currentLevel ||
      (level > currentLevel && showHigherLevels))
      filteredItems.Add(item);
  }

  items.ClearItems();
  items.Append(filteredItems);

  return result;
}

std::string CGUIWindowActivityLog::GetStartFolder(const std::string &dir)
{
  if (dir.empty())
    return "activities://";

  if (URIUtils::PathStarts(dir, "activities://"))
    return dir;

  return CGUIMediaWindow::GetStartFolder(dir);
}

bool CGUIWindowActivityLog::OnSelect(CFileItemPtr item)
{
  if (item == NULL)
    return false;

  OnExecute(item);
  return true;
}

bool CGUIWindowActivityLog::OnDelete(CFileItemPtr item)
{
  if (item == NULL)
    return false;

  std::string activityIdentifier = item->GetProperty(PROPERTY_ACTIVITY_IDENTIFIER).asString();
  if (activityIdentifier.empty())
    return false;

  CActivityLog::GetInstance().Remove(activityIdentifier);
  return true;
}

bool CGUIWindowActivityLog::OnExecute(CFileItemPtr item)
{
  if (item == NULL)
    return false;

  std::string activityIdentifier = item->GetProperty(PROPERTY_ACTIVITY_IDENTIFIER).asString();
  if (activityIdentifier.empty())
    return false;

  const ActivityPtr activity = CActivityLog::GetInstance().Get(activityIdentifier);
  if (activity == NULL)
    return false;

  if (!activity->CanExecute())
    return true;

  return activity->Execute();
}

void CGUIWindowActivityLog::OnActivityAdded(CFileItemPtr item)
{
  if (!IsActive())
    return;

  Refresh(true);
}

void CGUIWindowActivityLog::OnActivityRemoved(CFileItemPtr item)
{
  if (!IsActive())
    return;

  int selectedItemIndex = -1;
  if (item != NULL)
  {
    selectedItemIndex = m_viewControl.GetSelectedItem();
    // only update the selected item index when the deleted item is focused
    if (m_vecItems->Get(selectedItemIndex)->GetProperty(PROPERTY_ACTIVITY_IDENTIFIER).asString() != item->GetProperty(PROPERTY_ACTIVITY_IDENTIFIER).asString())
      selectedItemIndex = -1;
  }

  Refresh(true);

  // update the selected item
  if (selectedItemIndex >= 0)
    m_viewControl.SetSelectedItem(selectedItemIndex);
}
