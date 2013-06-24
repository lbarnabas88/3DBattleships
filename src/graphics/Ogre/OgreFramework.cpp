/*
 * OgreFramework.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "OgreFramework.hpp"
// Tools
#include "../utils/resolution.hpp"
// STD
#include <iostream>
using namespace std;

/**********************************
 * Ogre Framework Init Helper
 *********************************/
OgreFrameworkInitHelper::~OgreFrameworkInitHelper()
{
}
//*** CONFIG ***/
Ogre::String OgreFrameworkInitHelper::getPluginsCfg()
{
#ifndef NDEBUG
	return "plugins_d.cfg";
#else
	return "plugins.cfg";
#endif
}
Ogre::String OgreFrameworkInitHelper::getResourcesCfg()
{
#ifndef NDEBUG
	return "resources_d.cfg";
#else
	return "resources.cfg";
#endif
}
//*** LOG ***/
Ogre::String OgreFrameworkInitHelper::getOgreLog()
{
#ifndef NDEBUG
	return "ogre_d.log";
#else
	return "ogre.log";
#endif
}
//*** GRAPHICS METRICS ***/
std::pair<int, int> OgreFrameworkInitHelper::getResolution()
{
	return std::make_pair(800, 600);
}
bool OgreFrameworkInitHelper::isFullscreen()
{
	return false;
}
Ogre::String OgreFrameworkInitHelper::getWindowTitle()
{
#ifndef NDEBUG
	return "OGRE Rendering Window [DEBUG]";
#else
	return "OGRE Rendering Window";
#endif
}
/**********************************
 * Ogre Framework
 *********************************/
OgreFramework::OgreFramework() :
		mRoot(NULL), mRenderSystem(NULL), mRenderWindow(NULL)
{
}

OgreFramework::~OgreFramework()
{
	shutdown();
}

void OgreFramework::init(OgreFrameworkInitHelper* initHelper)
{
	//*** INIT HELPER CREATE ***/
	// The helper is null?
	bool isHelperNull = initHelper == NULL;
	// In the case of nullhelper create default
	if (isHelperNull)
		initHelper = new OgreFrameworkInitHelper;
	//*** INIT OGRE ***/
	// Create root
	mRoot = new Ogre::Root(initHelper->getPluginsCfg(), "", initHelper->getOgreLog());
	// Config file handler
	Ogre::ConfigFile cf;
	// Load config file
	cf.load(initHelper->getResourcesCfg());
	// Process all the resources sections
	for (auto secIt = cf.getSectionIterator(); secIt.hasMoreElements();)
	{
		// Section name
		Ogre::String sectionName = secIt.peekNextKey();
		// Section resources
		Ogre::ConfigFile::SettingsMultiMap *resources = secIt.getNext();
		// For all resource add to the ResourceGroupManager
		for (auto res : *resources)
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(res.second, res.first, sectionName);
	}
	// Get OpenGL rendering subsystem
	mRenderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	// Set rendersystem to the root
	mRoot->setRenderSystem(mRenderSystem);
	// Init root without creating window
	mRoot->initialise(false);
	// Setting for the render window
	Ogre::NameValuePairList settings;
	// To the main monitor
	settings["monitorIndex"] = "0";
	// Create the render window
	auto resolution = initHelper->getResolution();
	mRenderWindow = mRoot->createRenderWindow(initHelper->getWindowTitle(), resolution.first, resolution.second, initHelper->isFullscreen(), &settings);
	// Set Default Multimaos number for textures
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	// Init all resoruce group
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//*** INIT HELPER DESTROY ***/
	// In the case of nullhelper destroy default
	if (isHelperNull)
		delete initHelper;
}

// Shut down
void OgreFramework::shutdown()
{
	if (mRoot)
	{
		delete mRoot;
		mRoot = NULL;
	}
}

// Get objects
Ogre::Root* OgreFramework::getRoot()
{
	return mRoot;
}
Ogre::RenderSystem* OgreFramework::getRenderSystem()
{
	return mRenderSystem;
}
Ogre::RenderWindow* OgreFramework::getRenderWindow()
{
	return mRenderWindow;
}

void OgreFramework::toggleFullscreen()
{
	bool fullscreen = mRenderWindow->isFullScreen();
	if (fullscreen)
	{
	}
	else
	{
	}
	mRenderWindow->setFullscreen(!fullscreen, mRenderWindow->getWidth(), mRenderWindow->getHeight());
}

OgreFramework& OgreFramework::getSingleton()
{
	static OgreFramework instance;
	return instance;
}

OgreFramework& OgreFrameworkSingleton = OgreFramework::getSingleton();
