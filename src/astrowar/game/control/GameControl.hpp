/*
 * GameControl.hpp
 *
 *  Created on: 2013.06.17.
 *      Author: Baarnus
 */

#ifndef GAMECONTROL_HPP_
#define GAMECONTROL_HPP_

#include "../shipyard/Shipyard.hpp"
#include "GameControlProvider.hpp"
#include "../shiplist/ShipListController.hpp"
#include "GameControlListener.hpp"

class GameControl: public GameControlProvider, public ShipListControllerDataProvider
{
public:
	// Create & Destroy
	GameControl(Ogre::SceneManager* sceneManager);
	virtual ~GameControl();
	// Control Provider States
	virtual GameControlProvider::GamePhase getGamePhase();
	virtual bool isSetReady();
	virtual int getActivePlayer();
	virtual void setDone();
	virtual FireResult fireTorpedo(std::vector<size_t> coords);
	// Control Provider Ships
	virtual ShipHull* createShip(Grid3D* grid, std::vector<size_t> coords);
	ShipHull* getShipForNode(Ogre::SceneNode* sceneNode);
	// Move ship
	virtual void moveShipTo(ShipHull* ship, std::vector<size_t> coords);
	virtual void moveShipBy(ShipHull* ship, std::vector<int> coords);
	virtual void rotateShipTo(ShipHull* ship, size_t orientation_index);
	virtual void rotateShipNext(ShipHull* ship);
	virtual void rotateShipPrev(ShipHull* ship);
	// Selected ship
	virtual bool isExistsSelectedShip();
	virtual void selectShip(ShipHull* ship);
	virtual void moveSelectedShipTo(std::vector<size_t> coords);
	virtual void moveSelectedShipBy(std::vector<int> coords);
	virtual void rotateSelectedShipTo(size_t orientation_index);
	virtual void rotateSelectedShipNext();
	virtual void rotateSelectedShipPrev();
	// Data
	virtual size_t numOfShipType(int player);
	virtual CEGUI::String getShipTypeName(int player, size_t i);
	virtual size_t getShipTypeCount(int player, size_t i);
	virtual bool needSelection();
	virtual CEGUI::String getShipColumnName();
	virtual CEGUI::String getQuantityColumnName();
	// Listener
	virtual void onSelectionChange(int player, unsigned selection);
	// Self listener
	GameControlListener* getListener();
	void setListener(GameControlListener* listener);
private:
	// Check if ship is on a valid position
	bool isShipValid(ShipHull* ship);
	// Color shipt to correct color
	void checkShip(ShipHull* ship);
	void colorOnSelection(ShipHull* ship);
	// Ship source
	Shipyard mShipyard;
	// Selected ship
	ShipHull* mSelectedShip;
	// List selections
	unsigned mListSelections[2];
	// Listener
	GameControlListener* mListener;
	void onPlayerChange(int playerFrom, int playerTo);
	void onSetReady();
	void onSetCancel();
	void onShipCreated();
	void onBattleStart();
	void onBattleEnd();
	bool mPrevReady;
	// Game Phase
	GameControlProvider::GamePhase mPhase;

	// TEST
	std::vector<size_t> mShipNumbers;
	int mPlayer;
};

#endif /* GAMECONTROL_HPP_ */
