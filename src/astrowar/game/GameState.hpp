/*
 * GameState.hpp
 *
 *  Created on: 2013.04.24.
 *      Author: baarnus
 */

#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

/** Include **/
// Tools
#include "../../graphics/Ogre/OgreState.hpp"
// Game
#include "grid/Grid3D.hpp"
#include "control/GameCoordinator.hpp"
#include "control/GameControlProvider.hpp"
#include "control/GameControl.hpp"
#include "shipyard/Shipyard.hpp"
// Network
#include <SFML/Network.hpp>
// CEGUI
#include <CEGUI/CEGUI.h>
// STD
#include <vector>

class GameState: public OgreState, public OIS::MouseListener, public OIS::KeyListener
{
public:
	GameState(sf::Socket* connectedPlayerSocket = NULL);
	virtual ~GameState();
protected:
	// State
	void onActivate();
	void onDeactivate();
	// FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// Mouse Listener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	// Key listener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	// CEGUI handlers
	bool backButtonHandler(const CEGUI::EventArgs& arg);
	// Torpedo
	std::shared_ptr<Grid3D> gridA, gridB;
	void toggleGrids();
	// Camera
	Ogre::Camera* mCamera;
	Ogre::SceneNode* mCameraNode;
	Ogre::Vector3 mCameraDestination;
	bool mIsCameraMoving;
	// Players
	sf::Socket* mNetPlayerSocket;
	// Game control
	GameControl* mControl;
	// Game coordinator
	GameCoordinator* mCoordinator;
};

#endif /* GAMESTATE_HPP_ */
