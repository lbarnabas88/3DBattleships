/*
 * GameCoordinator.hpp
 *
 *  Created on: 2013.06.14.
 *      Author: Baarnus
 */

#ifndef GAMECOORDINATOR_HPP_
#define GAMECOORDINATOR_HPP_

#include "../grid/Grid3D.hpp"
// STD
#include <queue>
// OIS
#include <OIS/OIS.h>
// CEGUI
#include <CEGUI/CEGUI.h>
// Game
#include "../shiplist/ShipListController.hpp"
#include "GameControlProvider.hpp"
#include "GameControlListener.hpp"

class GameCoordinatorListener
{
public:
	virtual ~GameCoordinatorListener();
};

class GameCoordinator: public Grid3DListener, public OIS::KeyListener, public GameControlListener, public Ogre::FrameListener
{
public:
	GameCoordinator();
	virtual ~GameCoordinator();
	// Connetor
	void connectToGrids(Grid3D* gridA, Grid3D* gridB);
	void connectToShipLists(CEGUI::MultiColumnList* shipListA, CEGUI::MultiColumnList* shipListB, ShipListControllerDataProvider* provider);
	void connectToCameraNode(Ogre::SceneNode* cameraNode);
	// Grid Listener
	void onSelect(Grid3D* grid, size_t x, size_t y, size_t z);
	void onNodeSearch(Ogre::SceneNode* foundNode);
	// Convert direction with the camera
	std::vector<int> convertDirection(std::vector<int> direction);
	// Key listener
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	// CEGUI handler
	bool handleSetButton(const CEGUI::EventArgs& arg);
	bool handleReadyButton(const CEGUI::EventArgs& arg);
	bool handleFireButton(const CEGUI::EventArgs& arg);
	// Control provider
	GameControlProvider* getControlProvider() const;
	void setControlProvider(GameControlProvider* controlProvider);
	// Listener
	GameCoordinatorListener* getListener() const;
	void setListener(GameCoordinatorListener* listener);
	// Frame Listener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
private:
	// On Game Events
	virtual void onPlayerChange(int playerFrom, int playerTo);
	virtual void onSetReady();
	virtual void onSetCancel();
	virtual void onShipCreated();
	virtual void onBattleStart();
	virtual void onBattleEnd(int winnerPlayer);
	virtual void onShot(GameControlProvider::FireResult fireResult);
	// Grids
	Grid3D* mGrids[2];
	void fireEventOnActiveGrid();
	void processFireResult(GameControlProvider::FireResult fireResult);
	// Ship Lists handlers
	ShipListController mShipControllers[2];
	// Control Provider
	GameControlProvider* mControlProvider;
	// Coordinator listener
	GameCoordinatorListener* mListener;
	// Camera Node
	Ogre::SceneNode* mCameraNode;
	// Sor
	std::queue<GameControlProvider::FireResult> mFireResults;
};

#endif /* GAMECOORDINATOR_HPP_ */
