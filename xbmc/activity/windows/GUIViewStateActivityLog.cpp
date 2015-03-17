/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include "GUIViewStateActivityLog.h"
#include "FileItem.h"
#include "filesystem/File.h"
#include "guilib/GraphicContext.h"
#include "guilib/WindowIDs.h"
#include "utils/StringUtils.h"
#include "view/ViewState.h"

CGUIViewStateActivityLog::CGUIViewStateActivityLog(const CFileItemList& items) : CGUIViewState(items)
{
  AddSortMethod(SortByDate, 552, LABEL_MASKS("%L", "%d", "%L", "%d"));  // Label, Date | Label, Date

  SetSortMethod(SortByDate);
  SetViewAsControl(DEFAULT_VIEW_AUTO);

  SetSortOrder(SortOrderDescending);
  LoadViewState(items.GetPath(), WINDOW_ACTIVITY_LOG);
}

void CGUIViewStateActivityLog::SaveViewState()
{
  SaveViewToDb(m_items.GetPath(), WINDOW_ACTIVITY_LOG);
}

std::string CGUIViewStateActivityLog::GetExtensions()
{
  return "";
}

VECSOURCES& CGUIViewStateActivityLog::GetSources()
{
  m_sources.clear();

  return CGUIViewState::GetSources();
}

