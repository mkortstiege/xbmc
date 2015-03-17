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

#include "activity/UniqueActivity.h"

class CNotificationActivity : public CUniqueActivity
{
public:
  CNotificationActivity(const CVariant& label, const CVariant& description, ActivityLevel level = ActivityLevelInformation)
    : CUniqueActivity(label, description, level)
  { }
  CNotificationActivity(const CVariant& label, const CVariant& description, const std::string& icon, ActivityLevel level = ActivityLevelInformation)
    : CUniqueActivity(label, description, icon, level)
  { }
  CNotificationActivity(const CVariant& label, const CVariant& description, const std::string& icon, const CVariant& details, ActivityLevel level = ActivityLevelInformation)
    : CUniqueActivity(label, description, icon, details, level)
  { }
  CNotificationActivity(const CVariant& label, const CVariant& description, const std::string& icon, const CVariant& details, const CVariant& executionLabel, ActivityLevel level = ActivityLevelInformation)
    : CUniqueActivity(label, description, icon, details, executionLabel, level)
  { }
  virtual ~CNotificationActivity() { }

  virtual const char* GetType() const { return "NotificationActivity"; }

  virtual bool CanExecute() const { return false; }
  virtual bool Execute() const { return true; }
};
