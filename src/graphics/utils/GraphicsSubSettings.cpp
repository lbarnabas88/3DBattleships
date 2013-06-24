/*
 * GraphicsSubSettings.cpp
 *
 *  Created on: 2013.06.11.
 *      Author: Baarnus
 */

#include "GraphicsSubSettings.hpp"
// Tools
#include "../../tools/utils.hpp"
#include "../utils/resolution.hpp"
// Boost
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

namespace grapp
{

GraphicsSubSettings::GraphicsSubSettings()
{
}

GraphicsSubSettings::~GraphicsSubSettings()
{
}

// Resolution
GraphicsSubSettings::ResolutionType GraphicsSubSettings::getResolution() const
{
	string resString = getOption<std::string>("gui.resolution");
	if (!utils::isResolution(resString))
		resString = getDefault<std::string>("gui.resolution");
	return utils::str2res(resString);
}

void GraphicsSubSettings::setResolution(const ResolutionType resolution)
{
	setResolution(resolution.first, resolution.second);
}

void GraphicsSubSettings::setResolution(int width, int height)
{
	setOption<string>("gui.resolution", utils::res2str(width, height));
}

// Fullscreen
bool GraphicsSubSettings::isFullscreen() const
{
	return getOption<bool>("gui.fullscreen");
}
void GraphicsSubSettings::setFullscreen(const bool fullscreen)
{
	setOption<bool>("gui.fullscreen", fullscreen);
}
// Init
void GraphicsSubSettings::onInit()
{
	addOption<string>("gui.resolution", "Resolution of the graphics surface.", "800x600");
	addOption<bool>("gui.fullscreen", "Is the graphics surface fullscreen?");
}

} /* namespace grapp */
