/*
 * Grid3D.cpp
 *
 *  Created on: 2013.04.16.
 *      Author: baarnus
 */

/** INCLUDES **/
#include "Grid3D.hpp"
// STD
#include <string>
#include <iostream>
#include <math.h>
// GRAPP
#include "../../../graphics/Ogre/OgreFramework.hpp"
#include "../../../graphics/Ois/OisFramework.hpp"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using namespace std;
using namespace Ogre;

//**************************/
//******* GRID 3D Listener **********/
//**************************/

Grid3DListener::~Grid3DListener()
{
}

//**************************/
//******* GRID 3D **********/
//**************************/
char sId = 'A';

void createGridOneDirection(Ogre::ManualObject* shape, Ogre::Vector3 a, Ogre::Vector3 b, Ogre::Vector3 c, size_t na, size_t nb, Ogre::Vector3 offset,
		Ogre::ColourValue colour)
{
	// Assert if shape is NULL
	assert(shape != NULL);
	// If not, add lines
	for (size_t i = 0; i <= na; ++i)
		for (size_t j = 0; j <= nb; ++j)
		{
			Ogre::Vector3 start = offset + i * a + j * b;
			shape->position(start);
			shape->colour(colour);
			shape->position(start + c);
			shape->colour(colour);
		}
}

Grid3D::Grid3D(Ogre::SceneManager* scene_manager, Ogre::Camera* camera, std::vector<size_t> dimensions, Ogre::ColourValue colour) :
		mSceneManager(scene_manager), mCamera(camera), mDimensions(dimensions), mGridListener(NULL)
{
	// Assert if scene manager doesn't exist
	assert(scene_manager != NULL);
	// Assert if there are not enought value for dimensions
	assert(dimensions.size() >= 3);
	// Create the manual object for grid
	mGridShape = mSceneManager->createManualObject(string("Grid") + sId);
	// Create the grid
	mOffset = Ogre::Vector3::NEGATIVE_UNIT_X * mDimensions[0] / 2.0 + Ogre::Vector3::NEGATIVE_UNIT_Y * mDimensions[1] / 2.0
			+ Ogre::Vector3::NEGATIVE_UNIT_Z * mDimensions[2] / 2.0;
	mGridShape->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	{
		createGridOneDirection(mGridShape, Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_Y, mDimensions[2] * Ogre::Vector3::UNIT_Z, mDimensions[0], mDimensions[1],
				mOffset, colour);
		createGridOneDirection(mGridShape, Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_Z, mDimensions[1] * Ogre::Vector3::UNIT_Y, mDimensions[0], mDimensions[2],
				mOffset, colour);
		createGridOneDirection(mGridShape, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z, mDimensions[0] * Ogre::Vector3::UNIT_X, mDimensions[1], mDimensions[2],
				mOffset, colour);
	}
	mGridShape->end();

	// Create the scene node
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(string("GridNode") + sId);
	mSceneNode->attachObject(mGridShape);

	// Create the marker
	mMarkEntity = mSceneManager->createEntity(string("MarkEntity") + sId, "Torpedo.mesh");
	mMarkNode = mSceneNode->createChildSceneNode(string("MarkNode") + sId);
	mMarkNode->attachObject(mMarkEntity);
	// Marker to 0,0,0
	mMarkerCoords.resize(3);
	for (auto& c : mMarkerCoords)
		c = 0;
	mMarkNode->setPosition(coords2position(mMarkerCoords));
	// Add to listeners
	OisFrameworkSingleton.addKeyListener(this);
	OisFrameworkSingleton.addMouseListener(this);
	// Create ray
	mRayScnQuery = mSceneManager->createRayQuery(Ogre::Ray());
	// Next ID
	++sId;
}

Grid3D::~Grid3D()
{
	OisFrameworkSingleton.removeKeyListener(this);
	OisFrameworkSingleton.removeMouseListener(this);
	mSceneManager->destroyManualObject(mGridShape);
	mSceneManager->destroyQuery(mRayScnQuery);
}

Ogre::SceneNode* Grid3D::getNode()
{
	return mSceneNode;
}

void Grid3D::handleInput(OIS::Keyboard* keyboard, OIS::Mouse* mouse)
{
}

void Grid3D::update(Ogre::Real dt)
{
	// Rotating the marker
	mMarkNode->rotate(Ogre::Vector3::UNIT_Y, Radian(1 * dt));
}

Ogre::Vector3 Grid3D::coords2position(std::vector<size_t> coords)
{
	assert(coords.size() >= 3);
	return mOffset + coords[0] * Vector3::UNIT_X + coords[1] * Vector3::UNIT_Y + coords[2] * Vector3::UNIT_Z + Vector3(0.5f, 0.5f, 0.5f);
}

void Grid3D::markerMoveTo(std::vector<size_t> coords)
{
	assert(coords.size() >= 3);
	for (size_t i = 0; i < 3; ++i)
		mMarkerCoords[i] = (coords[i] < mDimensions[i] ? coords[i] : mDimensions[i] - 1);
	mMarkNode->setPosition(coords2position(mMarkerCoords));
}

void Grid3D::markerMove(std::vector<int> coords)
{
	assert(coords.size() >= 3);
	for (size_t i = 0; i < 3; ++i)
		markerStep(i, coords[i]);
}

void Grid3D::markerStepTo(size_t direction, size_t place)
{
	mMarkerCoords[direction] = place < mDimensions[direction] ? place : mDimensions[direction] - 1;
	mMarkNode->setPosition(coords2position(mMarkerCoords));
}

void Grid3D::markerStep(size_t direction, int numberOfSteps)
{
	if (numberOfSteps > 0)
	{
		size_t new_coord = mMarkerCoords[direction] + numberOfSteps;
		mMarkerCoords[direction] = (new_coord < mDimensions[direction] ? new_coord : mDimensions[direction] - 1);
	}
	else if (numberOfSteps < 0)
	{
		mMarkerCoords[direction] += (-numberOfSteps <= (int) mMarkerCoords[direction] ? numberOfSteps : -mMarkerCoords[direction]);
	}
	mMarkNode->setPosition(coords2position(mMarkerCoords));
}

bool Grid3D::keyPressed(const OIS::KeyEvent& keyEvent)
{
	if (keyEvent.key == OIS::KC_NUMPAD4 || keyEvent.key == OIS::KC_NUMPAD6) markerStep(0, keyEvent.key == OIS::KC_NUMPAD4 ? -1 : 1);
	if (keyEvent.key == OIS::KC_NUMPAD9 || keyEvent.key == OIS::KC_NUMPAD3) markerStep(1, keyEvent.key == OIS::KC_NUMPAD3 ? -1 : 1);
	if (keyEvent.key == OIS::KC_NUMPAD8 || keyEvent.key == OIS::KC_NUMPAD2) markerStep(2, keyEvent.key == OIS::KC_NUMPAD8 ? -1 : 1);

	return true;
}

bool Grid3D::keyReleased(const OIS::KeyEvent& keyEvent)
{
	return true;
}

bool Grid3D::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool Grid3D::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		mMouseRay = mCamera->getCameraToViewportRay(arg.state.X.abs / (float) arg.state.width, arg.state.Y.abs / (float) arg.state.height);
		mRayScnQuery->setRay(mMouseRay);
		mRayScnQuery->execute(this);
	}

	return true;
}

bool Grid3D::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool Grid3D::queryResult(Ogre::MovableObject* obj, Ogre::Real distance)
{
	if (obj->getName() == mGridShape->getName())
	{
		// Intersection of the ray and the grid
		auto vec = mMouseRay.getPoint(distance) - mSceneNode->getPosition();
		// Some normalization - all to => [-1;1]
		vec.x /= mDimensions[0] * mSceneNode->getScale().x / 2;
		vec.y /= mDimensions[1] * mSceneNode->getScale().y / 2;
		vec.z /= mDimensions[2] * mSceneNode->getScale().z / 2;
		// Convert to a vector
		std::vector<float> point = { vec.x, vec.y, vec.z };
		// Determine the clicked side
		size_t side = 0;
		for (size_t i = 0; i < point.size(); ++i)
			if (fabsf(point[i]) > fabsf(point[side])) side = i;
		// Calc the new coordinates
		for (size_t i = 0; i < point.size(); ++i)
		{
			if (i == side) continue;
			size_t coord = mDimensions[i] * (point[i] + 1) / 2;
			markerStepTo(i, coord);
		}
	}

	return true;
}

bool Grid3D::queryResult(Ogre::SceneQuery::WorldFragment* fragment, Ogre::Real distance)
{
	return true;
}

// Do select
void Grid3D::processSelected()
{
	if (mGridListener) mGridListener->onSelect(this, mMarkerCoords[0], mMarkerCoords[1], mMarkerCoords[2]);
}

// Get Set Listener
const Grid3DListener* Grid3D::getGridListener() const
{
	return mGridListener;
}
void Grid3D::setGridListener(Grid3DListener* gridListener)
{
	mGridListener = gridListener;
}

// Marker visible
void Grid3D::setMarkerVisible(const bool visible)
{
	mMarkNode->setVisible(visible);
}

void Grid3D::showMarker()
{
	setMarkerVisible(true);
}

void Grid3D::hideMarker()
{
	setMarkerVisible(false);
}
