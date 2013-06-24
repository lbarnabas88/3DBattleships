/*
 * GameCoordinator.cpp
 *
 *  Created on: 2013.06.14.
 *      Author: Baarnus
 */

#include "GameCoordinator.hpp"
// Framework
#include "../../../graphics/Ois/OisFramework.hpp"

#include <iostream>
using namespace std;

GameCoordinatorListener::~GameCoordinatorListener()
{
}

GameCoordinator::GameCoordinator() :
		mControlProvider(NULL), mListener(NULL), mCameraNode(NULL)
{
	OisFrameworkSingleton.addKeyListener(this);
	// Set player nums
	for (int i = 0; i < 2; ++i)
		mShipControllers[i].setPlayer(i);
}

GameCoordinator::~GameCoordinator()
{
	OisFrameworkSingleton.removeKeyListener(this);
}

void GameCoordinator::connectToGrids(Grid3D* gridA, Grid3D* gridB)
{
	mGrids[0] = gridA;
	mGrids[1] = gridB;
	for (auto& grid : mGrids)
	{
		assert(grid);
		grid->setGridListener(this);
	}
}

void GameCoordinator::connectToShipLists(CEGUI::MultiColumnList* shipListA, CEGUI::MultiColumnList* shipListB, ShipListControllerDataProvider* provider)
{
	if (shipListA)
	{
		mShipControllers[0].setColumnList(shipListA);
		mShipControllers[0].setDataProvider(provider);
		mShipControllers[0].buildForSet();
	}
	if (shipListB)
	{
		mShipControllers[1].setColumnList(shipListB);
		mShipControllers[1].setDataProvider(provider);
		mShipControllers[1].buildForSet();
	}
}

void GameCoordinator::connectToCameraNode(Ogre::SceneNode* cameraNode)
{
	mCameraNode = cameraNode;
}

// Grid listener
void GameCoordinator::onSelect(Grid3D* grid, size_t x, size_t y, size_t z)
{
	if (mControlProvider->getGamePhase() == GameControlProvider::GP_SET)
	{
		cout << "Ship to " << grid->getNode()->getName() << ": [" << x << "," << y << "," << z << "]" << endl;
		auto ship = mControlProvider->createShip(grid, { x, y, z });
		if (ship)
		{
		}
		else
		{
			cout << "Nincs több ilyen hajó!" << endl;
		}
	}
	else if (mControlProvider->getGamePhase() == GameControlProvider::GP_BATTLE)
	{
		cout << "Fire on " << grid->getNode()->getName() << ": [" << x << "," << y << "," << z << "]" << endl;
	}
}

void GameCoordinator::onNodeSearch(Ogre::SceneNode* foundNode)
{
	//
	auto ship = mControlProvider->getShipForNode(foundNode);
	if (ship)
	{
		cout << "Found ship: " << ship->getNode()->getName() << endl;
		// Set as selected
		mControlProvider->selectShip(ship);
	}
}

// Convert direction with the camera
std::vector<int> GameCoordinator::convertDirection(std::vector<int> direction)
{
	std::vector<int> converted(3);

	auto cam_ori = mCameraNode->getOrientation();

	vector<Ogre::Vector3> locals = { cam_ori.xAxis(), cam_ori.yAxis(), cam_ori.zAxis() };
	vector<Ogre::Vector3> globals = { Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z, -Ogre::Vector3::UNIT_X, -Ogre::Vector3::UNIT_Y, -Ogre::Vector3::UNIT_Z };
	vector<std::string> axis_names = { "X", "Y", "Z", "-X", "-Y", "-Z" };
	cout << "Nearest axes for locals: ";
	for (size_t i = 0; i < locals.size(); ++i)
	{
		size_t nearest_index = 0;
		for (size_t j = 0; j < globals.size(); ++j)
		{
			if (locals[i].angleBetween(globals[j]) < locals[i].angleBetween(globals[nearest_index]))
				nearest_index = j;
		}
		cout << "Local#" << axis_names[i] << " -> Global#" << axis_names[nearest_index] << " ";
		// Converting table
		converted[nearest_index % 3] = (nearest_index >= 3 ? -1 : 1) * direction[i];
	}
	cout << endl;

	cout << "Dir: " << direction[0] << ";" << direction[1] << ";" << direction[2] << ", Conv: " << converted[0] << ";" << converted[1] << ";" << converted[2] << endl;

	return converted;
}

// Key listener
bool GameCoordinator::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_NUMPADENTER)
		fireEventOnActiveGrid();

	if (mControlProvider->getGamePhase() == GameControlProvider::GP_SET)
	{
		if (arg.key == OIS::KC_R || arg.key == OIS::KC_HOME || arg.key == OIS::KC_NUMPAD7)
			mControlProvider->rotateSelectedShipNext();
		if (arg.key == OIS::KC_F || arg.key == OIS::KC_END || arg.key == OIS::KC_NUMPAD1)
			mControlProvider->rotateSelectedShipPrev();
		if (arg.key == OIS::KC_A || arg.key == OIS::KC_LEFT || arg.key == OIS::KC_NUMPAD4)
			mControlProvider->moveSelectedShipBy(convertDirection( { -1, 0, 0 }));
		if (arg.key == OIS::KC_D || arg.key == OIS::KC_RIGHT || arg.key == OIS::KC_NUMPAD6)
			mControlProvider->moveSelectedShipBy(convertDirection( { 1, 0, 0 }));
		if (arg.key == OIS::KC_S || arg.key == OIS::KC_DOWN || arg.key == OIS::KC_NUMPAD2)
			mControlProvider->moveSelectedShipBy(convertDirection( { 0, -1, 0 }));
		if (arg.key == OIS::KC_W || arg.key == OIS::KC_UP || arg.key == OIS::KC_NUMPAD8)
			mControlProvider->moveSelectedShipBy(convertDirection( { 0, 1, 0 }));
		if (arg.key == OIS::KC_Q || arg.key == OIS::KC_PGDOWN || arg.key == OIS::KC_NUMPAD3)
			mControlProvider->moveSelectedShipBy(convertDirection( { 0, 0, 1 }));
		if (arg.key == OIS::KC_E || arg.key == OIS::KC_PGUP || arg.key == OIS::KC_NUMPAD9)
			mControlProvider->moveSelectedShipBy(convertDirection( { 0, 0, -1 }));
	}

	return true;
}

bool GameCoordinator::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

// CEGUI handler
bool GameCoordinator::handleSetButton(const CEGUI::EventArgs& arg)
{
	cout << "Set Button" << endl;
	fireEventOnActiveGrid();
	return true;
}

bool GameCoordinator::handleReadyButton(const CEGUI::EventArgs& arg)
{
	cout << "Ready Button" << endl;
	return true;
}

bool GameCoordinator::handleFireButton(const CEGUI::EventArgs& arg)
{
	cout << "Fire Button" << endl;
	fireEventOnActiveGrid();
	return true;
}

// Control provider
GameControlProvider* GameCoordinator::getControlProvider() const
{
	return mControlProvider;
}

void GameCoordinator::setControlProvider(GameControlProvider* controlProvider)
{
	mControlProvider = controlProvider;
}

// Listener
GameCoordinatorListener* GameCoordinator::getListener() const
{
	return mListener;
}

void GameCoordinator::setListener(GameCoordinatorListener* listener)
{
	mListener = listener;
}

void GameCoordinator::fireEventOnActiveGrid()
{
	int grid_num = 0;
	for (auto grid : mGrids)
		if (grid && mControlProvider->getActivePlayer() == grid_num++)
			grid->processSelected();
}

// On Game Events
void GameCoordinator::onPlayerChange(int playerFrom, int playerTo)
{
	mGrids[playerFrom]->getNode()->setVisible(false);
	mGrids[playerTo]->getNode()->setVisible(true);
}

void GameCoordinator::onSetReady()
{
	cout << "Set Ready" << endl;
	// Ready Button
	CEGUI::PushButton* readyButton = static_cast<CEGUI::PushButton*>(CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Control/Set/ReadyButton"));
	if (readyButton)
		readyButton->setEnabled(true);
}

void GameCoordinator::onSetCancel()
{
	cout << "Set Cancel" << endl;
	// Ready Button
	CEGUI::PushButton* readyButton = static_cast<CEGUI::PushButton*>(CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Control/Set/ReadyButton"));
	if (readyButton)
		readyButton->setEnabled(false);
}

void GameCoordinator::onShipCreated()
{
	if (mControlProvider)
		mShipControllers[mControlProvider->getActivePlayer()].update();
}
