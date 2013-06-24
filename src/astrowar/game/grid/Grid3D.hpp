/*
 * Grid3D.hpp
 *
 *  Created on: 2013.04.16.
 *      Author: baarnus
 */

#ifndef GRID3D_HPP_
#define GRID3D_HPP_

/** Includes **/
// STD
#include <vector>
// OGRE
#include <OGRE/Ogre.h>
// OIS
#include <OIS/OIS.h>

class Grid3D;

class Grid3DListener
{
public:
	// Virtual destructor
	virtual ~Grid3DListener();
	// Listener functions
	virtual void onSelect(Grid3D* grid, size_t x, size_t y, size_t z) = 0;
	virtual void onNodeSearch(Ogre::SceneNode* foundNode) = 0;
};

class Grid3D: public OIS::KeyListener, public OIS::MouseListener, public Ogre::RaySceneQueryListener
{
public:
	Grid3D(Ogre::SceneManager* scene_manager, Ogre::Camera* camera, std::vector<size_t> dimensions, Ogre::ColourValue colour = Ogre::ColourValue::White);
	virtual ~Grid3D();

	Ogre::SceneNode* getNode();
	void handleInput(OIS::Keyboard* keyboard, OIS::Mouse* mouse);
	void update(Ogre::Real dt);

	Ogre::Vector3 coords2position(std::vector<size_t> coords);
	std::vector<size_t> position2coords(Ogre::Vector3 position);

	// Marker movers
	void markerMoveTo(std::vector<size_t> coords);
	void markerMove(std::vector<int> coords);
	void markerStepTo(size_t direction, size_t place);
	void markerStep(size_t direction, int numberOfSteps);
	// Keyboard listeners
	virtual bool keyPressed(const OIS::KeyEvent& keyEvent);
	virtual bool keyReleased(const OIS::KeyEvent& keyEvent);
	// Mouse listeners
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	// Ray listener
	virtual bool queryResult(Ogre::MovableObject* obj, Ogre::Real distance);
	virtual bool queryResult(Ogre::SceneQuery::WorldFragment* fragment, Ogre::Real distance);
	// Do select
	void processSelected();
	// Get set listener
	const Grid3DListener* getGridListener() const;
	void setGridListener(Grid3DListener* gridListener);
	// Marker visible
	void setMarkerVisible(const bool visible);
	void showMarker();
	void hideMarker();
	// Activate
	bool isActive() const;
	void setActive(bool isActive);
	void activate();
	void deactivate();
	// Getter of dimensions
	std::vector<size_t> getDimensions() const;
protected:
	// Reference to the scene manager and camera
	Ogre::SceneManager* mSceneManager;
	Ogre::Camera* mCamera;
	// Grid handlers
	Ogre::ManualObject* mGridShape;
	Ogre::SceneNode* mSceneNode;
	// Size and offset of the grid
	std::vector<size_t> mDimensions;
	Ogre::Vector3 mOffset;
	// Marker handlers
	Ogre::Entity* mMarkEntity;
	Ogre::SceneNode* mMarkNode;
	std::vector<size_t> mMarkerCoords;
	// Ray scene query
	Ogre::Ray mMouseRay;
	Ogre::RaySceneQuery* mRayScnQuery;
	bool isRightButton;
	// Listener
	Grid3DListener* mGridListener;
	// Active
	bool mIsActive;
};

#endif /* GRID3D_HPP_ */
