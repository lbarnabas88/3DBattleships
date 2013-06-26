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
#include "../model/models/GameModelListener.h"
#include "../model/elements/Ship.h"

class GameControl: public GameControlProvider, public ShipListControllerDataProvider, public AstrOWar::GameModelListener
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
	virtual void fireTorpedo(std::vector<size_t> coords);
	// Control Provider Ships
	virtual ShipHull* createShip(Grid3D* grid, std::vector<size_t> coords, std::string type = "");
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
	// Game Model Listener
	/*
	 * lövés esetén x,y,z koordináták, true ha sikeres, false ha nem
	 */
	virtual void onFireEvent(int x, int y, int z, bool damaged, bool sunken, AstrOWar::Ship*);
	/*
	 * találat esetén: x,y,z koordináták, és true ha sikeres, false ha nem
	 */
	virtual void onHitEvent(int x, int y, int z, bool damaged, bool sunken);
	/*
	 * játékos halála esetén, true ha én, false ha az ellenfél
	 */
	virtual void onDeadEvent(bool won);
	/*
	 * játékos kilépése esetén
	 */
	virtual void onExitEvent();
	/*
	 * hiba esetén, hibakóddal
	 */
	virtual void onErrorEvent(int error);
	/*
	 * a hálózat állapotát adja meg
	 */
	virtual void onNetworkEvent(bool success);
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
	void onBattleEnd(int winnerPlayer);
	void onShot(FireResult fireResult);
	bool mPrevReady;
	// Game Phase
	GameControlProvider::GamePhase mPhase;
	// Ship numbers
	std::vector<size_t> mShipNumbers;
	std::vector<size_t> mInitShipNumbers;
	void setPlayer(int player);
	int mPlayer;
	// Ships
	struct ShipPair
	{
		int iD;
		ShipHull* hull;
	};
	std::vector<ShipPair> mShips;
	ShipHull* getShipForId(int id);
	int getIdForShip(ShipHull* hull);
	void addShip(int id, ShipHull* hull);
};

#endif /* GAMECONTROL_HPP_ */
