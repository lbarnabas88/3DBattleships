/*
 * GameControlProvider.hpp
 *
 *  Created on: 2013.06.17.
 *      Author: Baarnus
 */

#ifndef GAMECONTROLPROVIDER_HPP_
#define GAMECONTROLPROVIDER_HPP_

// STD
#include <vector>
// GAME
#include "../shipyard/ShipHull.hpp"
#include "../grid/Grid3D.hpp"

class GameControlProvider
{
public:
	enum GamePhase
	{
		GP_SET, GP_BATTLE
	};
	virtual ~GameControlProvider();
	// State
	virtual GamePhase getGamePhase() = 0;
	virtual bool isSetReady() = 0;
	virtual int getActivePlayer() = 0;
	virtual void setDone() = 0;
	// Ship management
	virtual ShipHull* createShip(Grid3D* grid, std::vector<size_t> coords) = 0;
	virtual ShipHull* getShipForNode(Ogre::SceneNode* sceneNode) = 0;
	// Move a ship
	virtual void moveShipTo(ShipHull* ship, std::vector<size_t> coords) = 0;
	virtual void moveShipBy(ShipHull* ship, std::vector<int> coords) = 0;
	virtual void rotateShipTo(ShipHull* ship, size_t orientation_index) = 0;
	virtual void rotateShipNext(ShipHull* ship) = 0;
	virtual void rotateShipPrev(ShipHull* ship) = 0;
	// Selected ship
	virtual bool isExistsSelectedShip() = 0;
	virtual void selectShip(ShipHull* ship) = 0;
	virtual void moveSelectedShipTo(std::vector<size_t> coords) = 0;
	virtual void moveSelectedShipBy(std::vector<int> coords) = 0;
	virtual void rotateSelectedShipTo(size_t orientation_index) = 0;
	virtual void rotateSelectedShipNext() = 0;
	virtual void rotateSelectedShipPrev() = 0;
};

#endif /* GAMECONTROLPROVIDER_HPP_ */
