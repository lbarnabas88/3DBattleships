/*
 * Shipyard.cpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#include "Shipyard.hpp"
// Utils
#include "../../../tools/utils.hpp"
// OGRE
#include "../../../graphics/Ogre/OgreFramework.hpp"

Shipyard::Shipyard(Ogre::SceneManager* sceneManager) :
		mSceneManager(sceneManager)
{
	assert(mSceneManager);
}

Shipyard::~Shipyard()
{
	while (mShips.size() > 0)
		destroyShip(mShips[0]);
}

// Ship
void Shipyard::registerShipType(std::string shipTypeName, ShipTypeDescription shipTypeDescription)
{
	mShipTypes.push_back(std::make_pair(shipTypeName, shipTypeDescription));
}

ShipHull* Shipyard::createShip(std::string shipTypeName, Grid3D* grid)
{
	// Ship number
	static int shipNumber = 0;
	// New ship hull
	ShipHull* newShip = new ShipHull;
	// Get definition
	auto shipTypeIt = mShipTypes.begin();
	for (; shipTypeIt != mShipTypes.end(); ++shipTypeIt)
		if (shipTypeIt->first == shipTypeName)
			break;
	if (shipTypeIt == mShipTypes.end())
		return NULL;
	auto description = shipTypeIt->second;
	// Create newShip stuff
	newShip->mSceneNode = grid->getNode()->createChildSceneNode("Ship" + utils::t2str(shipNumber++));
	newShip->mEntity = mSceneManager->createEntity(description.meshFilePath);
	newShip->mSceneNode->attachObject(newShip->mEntity);
	newShip->mGrid = grid;
	// Add new ship to the collection
	mShips.push_back(newShip);
	// Return the new ship
	return newShip;
}

void Shipyard::destroyShip(ShipHull* ship)
{
	if (!ship)
		return;
	mSceneManager->destroyEntity(ship->mEntity);
	ship->mSceneNode->getParentSceneNode()->removeChild(ship->mSceneNode);
	auto fintIt = std::find(mShips.begin(), mShips.end(), ship);
	if (fintIt != mShips.end())
		mShips.erase(fintIt);
}

size_t Shipyard::getNumberOfShipTypes() const
{
	return mShipTypes.size();
}

std::string Shipyard::getNameOfShipType(size_t i) const
{
	if (i >= mShipTypes.size())
		return "";
	return mShipTypes[i].first;
}

ShipHull* Shipyard::getShip(Ogre::SceneNode* sceneNode)
{
	for (auto ship : mShips)
		if (ship->getNode() == sceneNode)
			return ship;
	return NULL;
}
