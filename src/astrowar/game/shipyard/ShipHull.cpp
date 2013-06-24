/*
 * ShipHull.cpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#include "ShipHull.hpp"

Ogre::String ShipHull::shipColorToMaterialName(ShipColor sc)
{
	switch (sc)
	{
	case SCLR_RED:
		return "HullR";
	case SCLR_GREEN:
		return "HullG";
	case SCLR_BLUE:
		return "HullB";
	case SCLR_CYAN:
		return "HullC";
	case SCLR_MAGENTA:
		return "HullM";
	case SCLR_YELLOW:
		return "HullY";
	case SCLR_WHITE:
		return "HullW";
	case SCLR_BLACK:
		return "HullK";
	default:
		return "Hull";
	}
}

#include <iostream>
using namespace std;

std::vector<Ogre::Quaternion> generateOrientations()
{
	std::vector<Ogre::Quaternion> orientations;
	// Orientations
	size_t numOfO = 0;
	for (float angle1 = 0.0f; angle1 < 360.0f; angle1 += 90.0f)
	{
		for (float angle2 = 0.0f; angle2 < 360.0f; angle2 += 90.0f)
		{
			cout << "GenO#" << numOfO++ << ": " << angle1 << " " << angle2 << endl;
			Ogre::Quaternion q = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle1)), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle2)), Ogre::Vector3::UNIT_Z);
			orientations.push_back(q);
		}
	}
	for (float angle1 = 90.0f; angle1 >= -90.0f; angle1 -= 180.0f)
	{
		for (float angle2 = 0.0f; angle2 < 360.0f; angle2 += 90.0f)
		{
			cout << "GenO#" << numOfO++ << ": " << angle1 << " " << angle2 << endl;
			Ogre::Quaternion q = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle1)), Ogre::Vector3::UNIT_X) * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle2)), Ogre::Vector3::UNIT_Z);
			orientations.push_back(q);
		}
	}
	return orientations;
}

std::vector<Ogre::Quaternion> ShipHull::sOrientations = generateOrientations();

ShipHull::~ShipHull()
{
}

ShipHull::ShipHull() :
		mSceneNode(NULL), mEntity(NULL), mGrid(NULL), mColor(SCLR_DEFAULT), mOrientationIndex(0)
{
}

// Get accessors
Ogre::SceneNode* ShipHull::getNode()
{
	return mSceneNode;
}

Ogre::Entity* ShipHull::getEntity()
{
	return mEntity;
}

Grid3D* ShipHull::getGrid()
{
	return mGrid;
}

// Color the ship
ShipHull::ShipColor ShipHull::getColor() const
{
	return mColor;
}

void ShipHull::setColor(ShipColor color)
{
	mColor = color;
	if (mEntity)
		mEntity->getSubEntity(0)->setMaterialName(shipColorToMaterialName(color));
}

// Orientation setter, getter
size_t ShipHull::getOrientationIndex() const
{
	return mOrientationIndex;
}

void ShipHull::setOrientationIndex(const size_t orientation)
{
	if (orientation >= sOrientations.size())
		return;
	mOrientationIndex = orientation;
	mSceneNode->setOrientation(sOrientations[mOrientationIndex]);
}

Ogre::Quaternion ShipHull::getOrientation() const
{
	return Ogre::Quaternion();
}
