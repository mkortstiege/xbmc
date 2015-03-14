#pragma once
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

#include "FileItem.h"
#include "activity/IActivity.h"
#include "filesystem/IDirectory.h"

#define PROPERTY_ACTIVITY_IDENTIFIER  "Activity.ID"
#define PROPERTY_ACTIVITY_LEVEL       "Activity.Level"
#define PROPERTY_ACTIVITY_DESCRIPTION "Activity.Description"

namespace XFILE
{
  class CActivitiesDirectory : public IDirectory
  {
  public:
    CActivitiesDirectory() { }
    virtual ~CActivitiesDirectory() { }

    // implementations of IDirectory
    virtual bool GetDirectory(const CURL& url, CFileItemList& items);
    virtual bool Create(const CURL& url) { return true; }
    virtual bool Exists(const CURL& url) { return true; }
    virtual bool AllowAll() const { return true; }

    static CFileItemPtr ActivityToFileItem(const ActivityPtr& activity);
  };
}
