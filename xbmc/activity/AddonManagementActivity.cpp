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

#include "AddonManagementActivity.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "guilib/WindowIDs.h"
#include "utils/URIUtils.h"

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description)
  : CAddonActivity(addon, description)
{ }

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details)
  : CAddonActivity(addon, description, details)
{ }

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details, const CVariant& executionLabel)
  : CAddonActivity(addon, description, details, executionLabel)
{ }

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description)
  : CAddonActivity(addon, level, description)
{ }

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details)
  : CAddonActivity(addon, level, description, details)
{ }

CAddonManagementActivity::CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details, const CVariant& executionLabel)
  : CAddonActivity(addon, level, description, details, executionLabel)
{ }

std::string CAddonManagementActivity::GetExecutionLabel() const
{
  std::string executionLabel = CAddonActivity::GetExecutionLabel();
  if (!executionLabel.empty())
    return executionLabel;

  return g_localizeStrings.Get(24087);
}

bool CAddonManagementActivity::Execute() const
{
  if (!CanExecute())
    return false;

  std::vector<std::string> params;
  params.push_back(URIUtils::AddFileToFolder("addons://", m_addon->ID()));
  params.push_back("return");
  g_windowManager.ActivateWindow(WINDOW_ADDON_BROWSER, params);
  return true;
}
