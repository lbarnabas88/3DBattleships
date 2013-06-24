/*
 * OisFramework.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "OisFramework.hpp"
// Utils
#include "../../tools/utils.hpp"
// STD
#include <algorithm>

//*** OIS FRAMEWORK INIT HELPER
OisFrameworkInitHelper::~OisFrameworkInitHelper()
{
}

bool OisFrameworkInitHelper::isInputExlusive()
{
#ifndef NDEBUG
	return false;
#else
	return true;
#endif
}

//*** OIS FRAMEWORK ***/
OisFramework::OisFramework() :
		mInputManager(NULL), mKeyboard(NULL), mMouse(NULL)
{
}

OisFramework::~OisFramework()
{
	shutdown();
}

// Init
void OisFramework::init(OisFrameworkInitHelper* initHelper)
{
	// If initHelper is nil, stop
	if (!initHelper)
		return;
	// Shut down the previous init
	shutdown();
	// Param list
	OIS::ParamList pl;
	// Set window handler
	pl.insert(std::make_pair(std::string("WINDOW"), utils::t2str(initHelper->getWindowHnd())));
	// If the input need to be non-exclusive add some settings
	if (!initHelper->isInputExlusive())
	{
#if defined OIS_WIN32_PLATFORM
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	}
	// Create the input system
	mInputManager = OIS::InputManager::createInputSystem(pl);
	// Create Keyboard
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	if (mKeyboard)
		mKeyboard->setEventCallback(this);
	// Create Mouse
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
	if (mMouse)
		mMouse->setEventCallback(this);
	// Create joysticks
	for (int i = 0; i < mInputManager->getNumberOfDevices(OIS::OISJoyStick); ++i)
	{
		OIS::JoyStick* joystick = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
		if (joystick)
		{
			joystick->setEventCallback(this);
			mJoySticks.push_back(joystick);
		}
	}
}

// Shut down
void OisFramework::shutdown()
{
	if (mInputManager)
	{
		// Destroy the keyboard
		if (mKeyboard)
		{
			mInputManager->destroyInputObject(mKeyboard);
			mKeyboard = NULL;
		}
		// Destroy the mouse
		if (mMouse)
		{
			mInputManager->destroyInputObject(mMouse);
			mMouse = NULL;
		}
		// Destroy all joystricks
		for (auto joystick : mJoySticks)
			mInputManager->destroyInputObject(joystick);
		mJoySticks.clear();
		// Destroy input manager
		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = NULL;
	}
}

// Capture
void OisFramework::capture()
{
	if (mInputManager)
	{
		mKeyboard->capture();
		mMouse->capture();
		for (auto joystick : mJoySticks)
			joystick->capture();
	}
}

//*** Getters ***/
OIS::Keyboard* OisFramework::getKeyboard()
{
	return mKeyboard;
}
OIS::Mouse* OisFramework::getMouse()
{
	return mMouse;
}
OIS::JoyStick* OisFramework::getJoystick(size_t i)
{
	if (i < getNumOfJoysticks())
		return mJoySticks[i];
	return NULL;
}
size_t OisFramework::getNumOfJoysticks()
{
	return mJoySticks.size();
}

//*** Listeners ***/
// KeyListener
bool OisFramework::keyPressed(const OIS::KeyEvent &arg)
{
	for (auto keyListener : mKeyListeners)
		keyListener->keyPressed(arg);
	return true;
}
bool OisFramework::keyReleased(const OIS::KeyEvent &arg)
{
	for (auto keyListener : mKeyListeners)
		keyListener->keyReleased(arg);
	return true;
}

// MouseListener
bool OisFramework::mouseMoved(const OIS::MouseEvent &arg)
{
	for (auto mouseListener : mMouseListeners)
		mouseListener->mouseMoved(arg);
	return true;
}
bool OisFramework::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	for (auto mouseListener : mMouseListeners)
		mouseListener->mousePressed(arg, id);
	return true;
}
bool OisFramework::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	for (auto mouseListener : mMouseListeners)
		mouseListener->mouseReleased(arg, id);
	return true;
}
// JoyStrickListener
bool OisFramework::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->buttonPressed(arg, button);
	return true;
}
bool OisFramework::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->buttonReleased(arg, button);
	return true;
}
bool OisFramework::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->axisMoved(arg, axis);
	return true;
}
bool OisFramework::sliderMoved(const OIS::JoyStickEvent &arg, int index)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->sliderMoved(arg, index);
	return true;
}
bool OisFramework::povMoved(const OIS::JoyStickEvent &arg, int index)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->povMoved(arg, index);
	return true;
}
bool OisFramework::vector3Moved(const OIS::JoyStickEvent &arg, int index)
{
	for (auto joyStickListener : mJoyStickListeners)
		joyStickListener->vector3Moved(arg, index);
	return true;
}
// Broadcast to listeners
// Add listener
void OisFramework::addKeyListener(OIS::KeyListener* keyListener)
{
	if (!keyListener)
		return;
	if (std::find(mKeyListeners.begin(), mKeyListeners.end(), keyListener) != mKeyListeners.end())
		return;
	mKeyListeners.push_back(keyListener);
}
void OisFramework::addMouseListener(OIS::MouseListener* mouseListener)
{
	if (!mouseListener)
		return;
	if (std::find(mMouseListeners.begin(), mMouseListeners.end(), mouseListener) != mMouseListeners.end())
		return;
	mMouseListeners.push_back(mouseListener);
}
void OisFramework::addJoyStickListener(OIS::JoyStickListener* joystickListener)
{
	if (!joystickListener)
		return;
	if (std::find(mJoyStickListeners.begin(), mJoyStickListeners.end(), joystickListener) != mJoyStickListeners.end())
		return;
	mJoyStickListeners.push_back(joystickListener);
}
// Remove listener
void OisFramework::removeKeyListener(OIS::KeyListener* keyListener)
{
	auto findIt = std::find(mKeyListeners.begin(), mKeyListeners.end(), keyListener);
	if (findIt != mKeyListeners.end())
		mKeyListeners.erase(findIt);
}
void OisFramework::removeMouseListener(OIS::MouseListener* mouseListener)
{
	auto findIt = std::find(mMouseListeners.begin(), mMouseListeners.end(), mouseListener);
	if (findIt != mMouseListeners.end())
		mMouseListeners.erase(findIt);
}
void OisFramework::removeJoyStickListener(OIS::JoyStickListener* joystickListener)
{
	auto findIt = std::find(mJoyStickListeners.begin(), mJoyStickListeners.end(), joystickListener);
	if (findIt != mJoyStickListeners.end())
		mJoyStickListeners.erase(findIt);
}
// Remove all listener
void OisFramework::removeAllListeners()
{
	removeAllKeyListeners();
	removeAllMouseListeners();
	removeAllJoyStickListeners();
}
void OisFramework::removeAllKeyListeners()
{
	mKeyListeners.clear();
}
void OisFramework::removeAllMouseListeners()
{
	mMouseListeners.clear();
}
void OisFramework::removeAllJoyStickListeners()
{
	mJoyStickListeners.clear();
}

// Singleton
OisFramework& OisFramework::getSingleton()
{
	static OisFramework instance;
	return instance;
}

OisFramework& OisFrameworkSingleton = OisFramework::getSingleton();
