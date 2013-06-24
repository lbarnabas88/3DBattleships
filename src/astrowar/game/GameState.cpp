/*
 * GameState.cpp
 *
 *  Created on: 2013.04.24.
 *      Author: baarnus
 */

#include "GameState.hpp"
// STD
#include <iostream>
// Frameworks
#include "../../graphics/Ois/OisFramework.hpp"
// CEGUI
#include "../../graphics/Cegui/CeguiTranslator.hpp"

using namespace std;
using namespace Ogre;
using namespace CEGUI;

GameState::GameState(sf::Socket* connectedPlayerSocket) :
		OgreState("GameState"), mCamera(NULL), mCameraNode(NULL), mNetPlayerSocket(connectedPlayerSocket), mControl(NULL), mCoordinator(NULL)
{
	// Create light
	Light* l = mSceneManager->createLight("MainLight");
	l->setPosition(1, 3, 4);
	// Skybox
	mSceneManager->setSkyBox(true, "Astrowar/SpaceSkyBox");
}

GameState::~GameState()
{
}

// Notify if game changed
void GameState::notifyOnGameChange()
{
//	if (mControlProvider->getGamePhase() == 1)
//	{
	if (mControl->getActivePlayer() == 0)
	{
		gridA->activate();
		gridB->deactivate();
	}
	else
	{
		gridA->deactivate();
		gridB->activate();
	}
//	}
}

void GameState::onActivate()
{
	// Super class
	OgreState::onActivate();
	// Camera & Viewport
	createCameraAndViewport(0);
	mCamera = getCamera(0);
	mCamera->setPosition(0, 0, 2);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(0.2);
	mCamera->setAutoAspectRatio(true);
	auto viewport = getViewport(0);
	viewport->setBackgroundColour(ColourValue(0.1, 0.4, 0.7));

	// CEGUI System singleton
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	// Init main menu
	guiSys.executeScriptFile("init_game.lua");
	// Translate windgets text
	translateCeguiWindow(guiSys.getGUISheet());

	mCameraNode = mSceneManager->getRootSceneNode()->createChildSceneNode("Camera Node");
	mCameraNode->attachObject(mCamera);
	// Create grids
	size_t dim = 8;
	std::vector<size_t> dimensions = { dim, dim, dim };
	gridA.reset(new Grid3D(mSceneManager, mCamera, dimensions, ColourValue(0.8, 0.3, 0.1, 0.5)));
	gridB.reset(new Grid3D(mSceneManager, mCamera, dimensions, ColourValue(0.3, 0.8, 0.1, 0.5)));
	size_t maxDimension = 0;
	for (auto d : dimensions)
		if (d > maxDimension)
			maxDimension = d;
	float scale = 1.0f / maxDimension;
	gridA->getNode()->setScale(scale, scale, scale);
	gridB->getNode()->setScale(scale, scale, scale);
	mCoordinator = new GameCoordinator;
	mCoordinator->connectToGrids(gridA.get(), gridB.get());
	// Create control provider
	mControl = new GameControl(mSceneManager);
	// Activation of grids
	gridA->activate();
	gridB->deactivate();
	// Add to listeners
	OisFrameworkSingleton.addMouseListener(this);
	OisFrameworkSingleton.addKeyListener(this);

	cout << "Game State Cegui Before" << endl;

	// Back Button
	CEGUI::PushButton* backButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("Game/Menu/BackButton"));
	if (backButton)
		backButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::backButtonHandler, this));
	// Set Button
	CEGUI::PushButton* setButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("Game/Control/Set/SetButton"));
	if (setButton)
		setButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameCoordinator::handleSetButton, mCoordinator));
	// Ready Button
	CEGUI::PushButton* readyButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("Game/Control/Set/ReadyButton"));
	if (readyButton)
		readyButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameCoordinator::handleReadyButton, mCoordinator));
	// Fire Button
	CEGUI::PushButton* fireButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("Game/Control/Battle/FireButton"));
	if (fireButton)
		fireButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameCoordinator::handleFireButton, mCoordinator));

	cout << "Game State Control Buttons Created" << endl;

	// Ship list
	CEGUI::MultiColumnList* shipList1 = static_cast<CEGUI::MultiColumnList*>(guiSys.getGUISheet()->getChildRecursive("Game/Player1Panel/ShipList"));
	CEGUI::MultiColumnList* shipList2 = static_cast<CEGUI::MultiColumnList*>(guiSys.getGUISheet()->getChildRecursive("Game/Player2Panel/ShipList"));
	cout << "Game State ShipLists Get: " << shipList1 << " " << shipList2 << endl;
	mCoordinator->connectToShipLists(shipList1, shipList2, mControl);
	cout << "Game State ShipLists Set" << endl;
	// Set coordinstor
	mCoordinator->setControlProvider(mControl);
	mCoordinator->setListener(this);
	mCoordinator->connectToCameraNode(mCameraNode);
	mControl->setListener(mCoordinator);

	cout << "Game State Started" << endl;
}

void GameState::onDeactivate()
{
	// Destroy Game Control
	delete mControl;
	// Destroy Game Coordinator
	delete mCoordinator;
	// Destroy
	mSceneManager->destroySceneNode(mCameraNode);
	// Destroy Grids
	gridA.reset();
	gridB.reset();
	// Remove from listeners
	OisFrameworkSingleton.removeMouseListener(this);
	OisFrameworkSingleton.removeKeyListener(this);
	// CEGUI System singleton
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	// Init main menu
	guiSys.executeScriptFile("shutdown_game.lua");
	// Super class
	OgreState::onDeactivate();
}

bool GameState::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	gridA->update(evt.timeSinceLastFrame);
	gridB->update(evt.timeSinceLastFrame);
	return true;
}

bool GameState::mouseMoved(const OIS::MouseEvent &arg)
{
	if (arg.state.buttonDown(OIS::MB_Middle))
	{
		mCameraNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(-0.005 * arg.state.X.rel));
		mCameraNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(-0.005 * arg.state.Y.rel));
	}

	return true;
}

bool GameState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool GameState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool GameState::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool GameState::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

// CEGUI handlers
bool GameState::backButtonHandler(const CEGUI::EventArgs& arg)
{
	popState();
	return true;
}
