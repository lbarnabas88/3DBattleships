/*
 * Shipyard.hpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#ifndef SHIPYARD_HPP_
#define SHIPYARD_HPP_

// STD
#include <map>
// OGRE
#include <OGRE/Ogre.h>
// Game
#include "ShipHull.hpp"
#include "../grid/Grid3D.hpp"

class Shipyard
{
public:
	// Ship type struct
	struct ShipTypeDescription
	{
		// File Path for mesh
		Ogre::String meshFilePath;
	};
	// Create & Destroy
	Shipyard(Ogre::SceneManager* sceneManager);
	virtual ~Shipyard();
	// Ship
	void registerShipType(std::string shipTypeName, ShipTypeDescription shipTypeDescription);
	ShipHull* createShip(std::string shipTypeName, Grid3D* grid);
	void destroyShip(ShipHull* ship);
	size_t getNumberOfShipTypes() const;
	std::string getNameOfShipType(size_t i) const;
	// Ship get
	ShipHull* getShip(Ogre::SceneNode* sceneNode);
private:
	std::vector<std::pair<std::string, ShipTypeDescription> > mShipTypes;
	std::vector<ShipHull*> mShips;
	Ogre::SceneManager* mSceneManager;
};

#endif /* SHIPYARD_HPP_ */
