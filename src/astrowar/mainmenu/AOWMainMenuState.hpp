/*
 * AOWMainMenuState.hpp
 *
 *  Created on: 2013.06.12.
 *      Author: Baarnus
 */

#ifndef AOWMAINMENUSTATE_HPP_
#define AOWMAINMENUSTATE_HPP_

#include "../../graphics/Ogre/OgreState.hpp"
// STD
#include <memory>
#include <utility>
// SFML
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
// CEGUI
#include <CEGUI/CEGUI.h>

class AOWMainMenuState: public OgreState
{
public:
	AOWMainMenuState();
	virtual ~AOWMainMenuState();
	// FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// State
	void onActivate();
	void onDeactivate();
	// Gui handlers
	bool onePlayerButtonHandler(const CEGUI::EventArgs& e);
	bool exitButtonHandler(const CEGUI::EventArgs& e);
	bool serverStartHandler(const CEGUI::EventArgs& e);
	bool clientStartHandler(const CEGUI::EventArgs& e);
	bool cancelButtonHandler(const CEGUI::EventArgs& e);
	// Network functions
	void startAsServer(unsigned short port);
	void startAsClient(std::string address, unsigned short port);
	// Thread functions
	void listenForClient();
	void connectToServer();
private:
	// Thread
	std::shared_ptr<sf::Thread> mNetworkThread;
	bool mThreadShouldEnd;
	// Network parameters
	std::pair<std::string, unsigned short> mAddress;
	sf::TcpSocket mSocket;
	bool mIsServer;
	bool mSocketConnected;
	// Game Sharting
	bool mShouldStartGame;
};

#endif /* AOWMAINMENUSTATE_HPP_ */
