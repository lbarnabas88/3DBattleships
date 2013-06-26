/*
 * AOWMainMenuState.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#include "AOWMainMenuState.hpp"
// AOW
#include "../game/GameState.hpp"
// CEGUI
#include "../../graphics/Cegui/CeguiTranslator.hpp"

#include <iostream>
using namespace std;

AOWMainMenuState::AOWMainMenuState() :
		OgreState("AstrOWarMainMenuState"), mThreadShouldEnd(false), mIsServer(false), mSocketConnected(false), mShouldStartGame(false)
{
	// Skybox
	mSceneManager->setSkyBox(true, "Astrowar/SpaceSkyBox");
}

AOWMainMenuState::~AOWMainMenuState()
{
}

bool AOWMainMenuState::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// mSocket!
	if (mShouldStartGame)
	{
		mShouldStartGame = false;
		pushState(app::State::Ptr(new GameState(mSocketConnected ? &mSocket : NULL, mIsServer)));
	}
	return true;
}

void AOWMainMenuState::onActivate()
{
	// Camera & Viewport
	createCameraAndViewport(0);
	Ogre::Camera* mCamera = getCamera(0);
	mCamera->setPosition(0, 0, 2);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(0.2);
	mCamera->setAutoAspectRatio(true);
	auto viewport = getViewport(0);
	viewport->setBackgroundColour(Ogre::ColourValue(0.1, 0.4, 0.7));

	OgreState::onActivate();
	mShouldStartGame = false;
	// CEGUI System singleton
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	// Init main menu
	guiSys.executeScriptFile("init_main_menu.lua");
	// Translate windgets text
	translateCeguiWindow(guiSys.getGUISheet());
	// Exit Button
	CEGUI::PushButton* exitButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/Menu/ExitButton"));
	if (exitButton)
		exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::exitButtonHandler, this));
	// Server Start Button
	CEGUI::PushButton* serverStartButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ServerPanel/StartButton"));
	if (serverStartButton)
		serverStartButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::serverStartHandler, this));
	// Server Cancel Button
	CEGUI::PushButton* serverCancelButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ServerWaiting/CancelButton"));
	if (serverCancelButton)
		serverCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::cancelButtonHandler, this));
	// Client Start Button
	CEGUI::PushButton* clientStartButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ClientPanel/StartButton"));
	if (clientStartButton)
		clientStartButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::clientStartHandler, this));
	// Client Cancel Button
	CEGUI::PushButton* clientCancelButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ClientWaiting/CancelButton"));
	if (clientCancelButton)
		clientCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::cancelButtonHandler, this));
	// One Player Button
	CEGUI::PushButton* onePlayerButton = static_cast<CEGUI::PushButton*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/Menu/SingleplayerButton"));
	if (onePlayerButton)
		onePlayerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AOWMainMenuState::onePlayerButtonHandler, this));
	// Add as frame listener
	OgreFrameworkSingleton.getRoot()->addFrameListener(this);
}

void AOWMainMenuState::onDeactivate()
{
	// Thread stop
	mThreadShouldEnd = true;
	// CEGUI System singleton
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	// Shut down main menu
	guiSys.executeScriptFile("shutdown_main_menu.lua");
	OgreState::onDeactivate();
}

bool AOWMainMenuState::onePlayerButtonHandler(const CEGUI::EventArgs& e)
{
	mShouldStartGame = true;
	mSocketConnected = false;
	return true;
}

bool AOWMainMenuState::exitButtonHandler(const CEGUI::EventArgs& e)
{
	stop();
	return true;
}

bool AOWMainMenuState::serverStartHandler(const CEGUI::EventArgs& e)
{
	// Get data widget
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	CEGUI::Spinner* portSpinner = static_cast<CEGUI::Spinner*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ServerPanel/Port"));
	// Get data
	if (portSpinner)
	{
		// Start a server listener
		startAsServer(portSpinner->getCurrentValue());
	}
	return true;
}

bool AOWMainMenuState::clientStartHandler(const CEGUI::EventArgs& e)
{
	// Get data widgets
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
	CEGUI::Editbox* addressBox = static_cast<CEGUI::Editbox*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ClientPanel/ServerUrl"));
	CEGUI::Spinner* portSpinner = static_cast<CEGUI::Spinner*>(guiSys.getGUISheet()->getChildRecursive("MainMenu/ClientPanel/Port"));
	if (addressBox && portSpinner)
	{
		// Connent to the server
		startAsClient(addressBox->getText().c_str(), portSpinner->getCurrentValue());
	}
	return true;
}

bool AOWMainMenuState::cancelButtonHandler(const CEGUI::EventArgs& e)
{
	mThreadShouldEnd = true;
	return true;
}

void AOWMainMenuState::startAsServer(unsigned short port)
{
	mNetworkThread.reset(new sf::Thread(&AOWMainMenuState::listenForClient, this));
	mAddress.second = port;
	mNetworkThread->launch();
	mIsServer = true;
}

void AOWMainMenuState::startAsClient(std::string address, unsigned short port)
{
	mNetworkThread.reset(new sf::Thread(&AOWMainMenuState::connectToServer, this));
	mAddress.first = address;
	mAddress.second = port;
	mNetworkThread->launch();
	mIsServer = false;
}

void AOWMainMenuState::listenForClient()
{
	std::cout << "Server starts on port " << mAddress.second << std::endl;
	sf::TcpListener listener;
	listener.setBlocking(false);
	listener.listen(mAddress.second);

	mThreadShouldEnd = false;
	while (!mThreadShouldEnd)
	{
		if (listener.accept(mSocket) == sf::Socket::Done)
		{
			std::cout << "Incoming client: " << mSocket.getRemoteAddress() << std::endl;
			mShouldStartGame = true;
			mSocketConnected = true;
			mThreadShouldEnd = true;
		}
	}
	std::cout << "Server Listen Ended" << std::endl;
}

void AOWMainMenuState::connectToServer()
{
	std::cout << "Connenting to " << mAddress.first << " on port " << mAddress.second << std::endl;

	mThreadShouldEnd = false;
	while (!mThreadShouldEnd)
	{
		sf::Socket::Status status = mSocket.connect(mAddress.first, mAddress.second);
		if (status == sf::Socket::Done)
		{
			std::cout << "Connected to server: " << mSocket.getRemoteAddress() << std::endl;
			mShouldStartGame = true;
			mSocketConnected = true;
			mThreadShouldEnd = true;
		}
	}
	std::cout << "Connenction Ended" << std::endl;
}
