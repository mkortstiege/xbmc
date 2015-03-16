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

#include "AddonActivity.h"

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, const CVariant& description)
  : CUniqueActivity(addon->Name(), description, addon->Icon()),
    m_addon(addon)
{ }

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details)
  : CUniqueActivity(addon->Name(), description, addon->Icon(), details),
  m_addon(addon)
{ }

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, const CVariant& description, const CVariant& details, const CVariant& executionLabel)
  : CUniqueActivity(addon->Name(), description, addon->Icon(), details, executionLabel),
  m_addon(addon)
{ }

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description)
  : CUniqueActivity(addon->Name(), description, addon->Icon(), level),
  m_addon(addon)
{ }

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details)
  : CUniqueActivity(addon->Name(), description, addon->Icon(), details, level),
  m_addon(addon)
{ }

CAddonActivity::CAddonActivity(ADDON::AddonPtr addon, ActivityLevel level, const CVariant& description, const CVariant& details, const CVariant& executionLabel)
  : CUniqueActivity(addon->Name(), description, addon->Icon(), details, executionLabel, level),
  m_addon(addon)
{ }
