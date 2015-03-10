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

#include "VideoDimensions.h"
#include <algorithm>

std::vector<SResolution> CVideoDimensions::resolutions =
{
  { "480", "SD", { { 0, 720 }, { 0, 480 } } },
  { "576", "SD", { { 0, 720 }, { 480, 576 } } },
  { "540", "SD", { { 720, 960 }, { 540, 544 } } },
  { "720", "HD", { { 960, 1280 }, { 540, 720 } } },
  // Full-SBS 3D can be 3840x1080
  { "1080", "HD", { { 1280, 3840 }, { 540, 1080 } } },
  // Full-SBS 4K 3D could be 8192x2160
  { "4K", "HD", { { 2048, 8192 }, { 1080, 2160 } } }
};

CVideoDimensions::CVideoDimensions(int width, int height) :
m_width(width),
m_height(height)
{
  // Find the resolution that matches these dimensions
  auto it = std::find_if(
    resolutions.cbegin(),
    resolutions.cend(),
    [this](const SResolution &resolution)
  {
    auto width = resolution.boundaries.width;
    auto height = resolution.boundaries.height;

    return m_width > width.min && m_width <= width.max &&
      m_height > height.min && m_height <= height.max;
  });

  if (it != resolutions.cend())
    m_resolution = *it;
  else
    m_resolution = SResolution();
}

std::string CVideoDimensions::GetQuality() const
{
  return m_resolution.quality;
}

std::string CVideoDimensions::GetFidelity() const
{
  return m_resolution.fidelity;
}

SResolutionBoundaries CVideoDimensions::GetBoundaries() const
{
  return m_resolution.boundaries;
}

SResolutionBoundaries CVideoDimensions::GetQualityBoundaries(const std::string &quality)
{
  auto it = std::find_if(
    resolutions.cbegin(),
    resolutions.cend(),
    [&quality](const SResolution &resolution)
  {
    return resolution.quality == quality;
  });

  if (it != resolutions.cend())
    return it->boundaries;

  return SResolutionBoundaries();
}

SResolutionBoundaries CVideoDimensions::GetFidelityBoundaries(const std::string &fidelity)
{
  SResolutionBoundaries boundaries;

  // Find all resolutions with the specified fidelity
  auto it = std::find_if(
    resolutions.cbegin(),
    resolutions.cend(),
    [&fidelity](const SResolution &resolution)
  {
    return resolution.fidelity == fidelity;
  });

  // Create a new boundary based on the fidelity's upper and lower bound
  while (it != resolutions.cend())
  {
    boundaries.width.min = std::min(boundaries.width.min, it->boundaries.width.min);
    boundaries.width.max = std::max(boundaries.width.max, it->boundaries.width.max);
    boundaries.height.min = std::min(boundaries.height.min, it->boundaries.height.min);
    boundaries.height.max = std::max(boundaries.height.max, it->boundaries.height.max);
  }

  return boundaries;
}

const std::vector<SResolution>& CVideoDimensions::GetResolutionData()
{
  return resolutions;
}
