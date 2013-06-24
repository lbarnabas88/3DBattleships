/*
 * OisOgreWindowListener.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "OisOgreWindowListener.hpp"

OisOgreWindowListener::OisOgreWindowListener() :
		mInited(false)
{
}

OisOgreWindowListener::~OisOgreWindowListener()
{
	shutdown();
}

// Init
void OisOgreWindowListener::init()
{
	if (!mInited)
	{
		Ogre::WindowEventUtilities::addWindowEventListener(OgreFrameworkSingleton.getRenderWindow(), this);
		OisFrameworkSingleton.addKeyListener(this);
		windowResized(OgreFrameworkSingleton.getRenderWindow());
		mInited = true;
	}
}

// Shut down
void OisOgreWindowListener::shutdown()
{
	if (mInited)
	{
		Ogre::WindowEventUtilities::removeWindowEventListener(OgreFrameworkSingleton.getRenderWindow(), this);
		OisFrameworkSingleton.removeKeyListener(this);
		mInited = false;
	}
}

// WindowEventListener
void OisOgreWindowListener::windowResized(Ogre::RenderWindow* rw)
{
	if (rw == OgreFrameworkSingleton.getRenderWindow())
	{
		unsigned w, h, d;
		int x, y;
		rw->getMetrics(w, h, d, x, y);

		OIS::Mouse* mouse = OisFrameworkSingleton.getMouse();
		if (mouse)
		{
			const OIS::MouseState& ms = mouse->getMouseState();
			ms.width = w;
			ms.height = h;
		}
	}
}

void OisOgreWindowListener::windowClosed(Ogre::RenderWindow* rw)
{
	if (rw == OgreFrameworkSingleton.getRenderWindow())
	{
		OisFrameworkSingleton.shutdown();
	}
}

bool OisOgreWindowListener::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_RETURN)
	{
		auto keyboard = OisFrameworkSingleton.getKeyboard();
		if (keyboard->isModifierDown(OIS::Keyboard::Alt))
			OgreFrameworkSingleton.toggleFullscreen();
	}
	return true;
}

bool OisOgreWindowListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

OisOgreWindowListener& OisOgreWindowListener::getSingleton()
{
	static OisOgreWindowListener instance;
	return instance;
}
