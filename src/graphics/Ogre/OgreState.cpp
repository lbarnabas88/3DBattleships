/*
 * OgreState.cpp
 *
 *  Created on: 2013.04.20.
 *      Author: baarnus
 */

#include "OgreState.hpp"
#include "OgreFramework.hpp"
#include "../../tools/utils.hpp"

OgreState::OgreState(Ogre::String name) :
		mName(name)
{
	mSceneManager = OgreFrameworkSingleton.getRoot()->createSceneManager("DefaultSceneManager", mName + "SceneManager");
}

OgreState::~OgreState()
{
	OgreFrameworkSingleton.getRoot()->destroySceneManager(mSceneManager);
}

void OgreState::onActivate()
{
	OgreFrameworkSingleton.getRoot()->addFrameListener(this);
}

void OgreState::onDeactivate()
{
	OgreFrameworkSingleton.getRoot()->removeFrameListener(this);
	mSceneManager->destroyAllCameras();
	OgreFrameworkSingleton.getRenderWindow()->removeAllViewports();
	mViews.clear();
}

void OgreState::addCameraAndViewport(Ogre::Camera* camera, Ogre::Viewport* viewport)
{
	mViews.push_back(std::make_pair(camera, viewport));
}

void OgreState::createCameraAndViewport(int zOrder, float left, float top, float width, float height)
{
	static int sViewNumber = 0;
	Ogre::Camera* camera = mSceneManager->createCamera(mName + "Camera" + utils::t2str<int>(sViewNumber));
	Ogre::Viewport* viewport = OgreFrameworkSingleton.getRenderWindow()->addViewport(camera, zOrder, left, top, width, height);
	camera->setAspectRatio((Ogre::Real) viewport->getActualWidth() / (Ogre::Real) viewport->getActualHeight());
	addCameraAndViewport(camera, viewport);
	++sViewNumber;
}

size_t OgreState::getNumOfViews() const
{
	return mViews.size();
}

Ogre::Camera* OgreState::getCamera(size_t i)
{
	return mViews[i].first;
}

Ogre::Viewport* OgreState::getViewport(size_t i)
{
	return mViews[i].second;
}

void OgreState::onRun()
{
	// Message pump for correct runing
	Ogre::WindowEventUtilities::messagePump();
	// If window is closed
	if (OgreFrameworkSingleton.getRenderWindow()->isClosed())
	{
		// Pop all the state to exit the application
		stop();
		// No need to continue the Running
		return;
	}
	// Capture the inputs
	OisFrameworkSingleton.capture();
	// Render one frame of Ogre and pop all state if fail
	if (!OgreFrameworkSingleton.getRoot()->renderOneFrame())
		stop();
}
