/*
 * AstrOWarOgreInitHelper.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "AstrOWarInitHelper.hpp"
// Settings
#include "../../game/settings/GameSettings.hpp"
// STD
#include <assert.h>
// CEGUI
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

AstrOWarInitHelper::AstrOWarInitHelper()
{
}

AstrOWarInitHelper::~AstrOWarInitHelper()
{
}

// Configs
Ogre::String AstrOWarInitHelper::getPluginsCfg()
{
#ifndef NDEBUG
	return "config/plugins_d.cfg";
#else
	return "config/plugins.cfg";
#endif
}
Ogre::String AstrOWarInitHelper::getResourcesCfg()
{
	return "config/resources.cfg";
}
// Log
Ogre::String AstrOWarInitHelper::getOgreLog()
{
#ifndef NDEBUG
	return "config/log/ogre_d.log";
#else
	return "config/log/ogre.log";
#endif
}
// Graphics metrics
std::pair<int, int> AstrOWarInitHelper::getResolution()
{
	return GameSettingsSingleton.getGraphicsSettings().getResolution();
}
bool AstrOWarInitHelper::isFullscreen()
{
	return GameSettingsSingleton.getGraphicsSettings().isFullscreen();
}
Ogre::String AstrOWarInitHelper::getWindowTitle()
{
#ifndef NDEBUG
	return GameSettingsSingleton.getLanguage().textForCode("main.title") + " [DEBUG]";
#else
	return GameSettingsSingleton.getLanguage().textForCode("main.title");
#endif
}

// Window handler OIS
unsigned AstrOWarInitHelper::getWindowHnd()
{
	unsigned windowHnd = 0;
	OgreFrameworkSingleton.getRenderWindow()->getCustomAttribute("WINDOW", &windowHnd);
	return windowHnd;
}

// Cegui bootsrap
void AstrOWarInitHelper::bootstrapRenderer()
{
	Ogre::RenderWindow* window = OgreFrameworkSingleton.getRenderWindow();
	if (window)
		CEGUI::OgreRenderer::bootstrapSystem(*window);
}
// Cegui log
CEGUI::String AstrOWarInitHelper::getCeguiLog()
{
#ifndef NDEBUG
	return "config/log/gui_d.log";
#else
	return "config/log/gui.log";
#endif
}
