/*
 * ShipHull.hpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#ifndef SHIPHULL_HPP_
#define SHIPHULL_HPP_

// OGRE
#include <OGRE/Ogre.h>
// GAME
#include "../grid/Grid3D.hpp"

class ShipHull
{
public:
	// Ship colors
	enum ShipColor
	{
		SCLR_DEFAULT, SCLR_RED, SCLR_GREEN, SCLR_BLUE, SCLR_CYAN, SCLR_MAGENTA, SCLR_YELLOW, SCLR_WHITE, SCLR_BLACK
	};
	static Ogre::String shipColorToMaterialName(ShipColor sc);
	// Orientations
	static std::vector<Ogre::Quaternion> sOrientations;
	// Destroy
	virtual ~ShipHull();
	// Get accessors
	Ogre::SceneNode* getNode();
	Ogre::Entity* getEntity();
	Grid3D* getGrid();
	// Color the ship
	ShipColor getColor() const;
	void setColor(ShipColor color);
	// Orientation setter, getter
	size_t getOrientationIndex() const;
	void setOrientationIndex(const size_t orientation);
	Ogre::Quaternion getOrientation() const;
private:
	// Create
	ShipHull();
	// Ogre stuff
	Ogre::SceneNode* mSceneNode;
	Ogre::Entity* mEntity;
	// Grid the ship on
	Grid3D* mGrid;
	// Color of the ship
	ShipColor mColor;
	// Orientation index
	size_t mOrientationIndex;
	// Shipyard must see inside
	friend class Shipyard;
};

#endif /* SHIPHULL_HPP_ */
