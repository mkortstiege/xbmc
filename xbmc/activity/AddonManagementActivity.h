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

#include "activity/AddonActivity.h"

class CAddonManagementActivity : public CAddonActivity
{
public:
  CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description);
  CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details);
  CAddonManagementActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details, const CVariant& executionLabel);
  CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description);
  CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details);
  CAddonManagementActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details, const CVariant& executionLabel);
  virtual ~CAddonManagementActivity() { }

  virtual const char* GetType() const { return "AddonManagementActivity"; }
  virtual std::string GetExecutionLabel() const;

  virtual bool CanExecute() const { return m_addon != NULL; }
  virtual bool Execute() const;
};
