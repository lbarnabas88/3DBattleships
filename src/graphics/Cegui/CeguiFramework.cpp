/*
 * CeguiFramework.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "CeguiFramework.hpp"
// CEGUI
#include <CEGUI/ScriptingModules/LuaScriptModule/CEGUILua.h>

//********************************
//*** CEGUI FRAMEWORK INIT HELPER
//********************************
CeguiFrameworkInitHelper::~CeguiFrameworkInitHelper()
{
}

CEGUI::String CeguiFrameworkInitHelper::getCeguiLog()
{
#ifndef NDEBUG
	return "Cegui_d.log";
#else
	return "Cegui.log";
#endif
}

//********************************
//*** CEGUI FRAMEWORK
//********************************
CeguiFramework::CeguiFramework() :
		mIsInited(false)
{
}

CeguiFramework::~CeguiFramework()
{
	shutdown();
}

// Init
void CeguiFramework::init(CeguiFrameworkInitHelper* initHelper)
{
	// If is inited not init
	if (mIsInited)
		return;
	// initHelper musn't be nil
	assert(initHelper);
	// Create default logger with the given file name
	CEGUI::DefaultLogger* defLog = new CEGUI::DefaultLogger;
	defLog->setLogFilename(initHelper->getCeguiLog());
	// Bootstrap system
	initHelper->bootstrapRenderer();
	// Set resource groups
	CEGUI::AnimationManager::setDefaultResourceGroup("Animations");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("Scripts");
	// Create Lua scripting module
	CEGUI::System::getSingleton().setScriptingModule(&CEGUI::LuaScriptModule::create());
	// Execute init script
	CEGUI::System::getSingleton().executeScriptFile("init_script.lua");
	// Set init to true
	mIsInited = true;
}
// Shut down
void CeguiFramework::shutdown()
{
	if (mIsInited)
	{
		CEGUI::System::getSingleton().destroy();
		mIsInited = false;
	}
}
// Singleton
CeguiFramework& CeguiFramework::getSingleton()
{
	static CeguiFramework instance;
	return instance;
}

CeguiFramework& CeguiFrameworkSingleton = CeguiFramework::getSingleton();
