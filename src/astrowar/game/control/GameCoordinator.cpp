/*
 * GameCoordinator.cpp
 *
 *  Created on: 2013.06.14.
 *      Author: Baarnus
 */

#include "GameCoordinator.hpp"
// Framework
#include "../../../graphics/Ois/OisFramework.hpp"
// Game Settings
#include "../../../game/settings/GameSettings.hpp"
// CEGUI
#include "../../../graphics/Cegui/CeguiTranslator.hpp"

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
		mShipControllers[0].build();
	}
	if (shipListB)
	{
		mShipControllers[1].setColumnList(shipListB);
		mShipControllers[1].setDataProvider(provider);
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
		mControlProvider->createShip(grid, { x, y, z });
	}
	else if (mControlProvider->getGamePhase() == GameControlProvider::GP_BATTLE)
	{
		mControlProvider->fireTorpedo( { x, y, z });
	}
}

void GameCoordinator::onNodeSearch(Ogre::SceneNode* foundNode)
{
	if (mControlProvider->getGamePhase() != GameControlProvider::GP_SET)
		return;
	auto ship = mControlProvider->getShipForNode(foundNode);
	if (ship)
	{
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
	vector<Ogre::Vector3> globals = { Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z, -Ogre::Vector3::UNIT_X, -Ogre::Vector3::UNIT_Y,
										-Ogre::Vector3::UNIT_Z };
	vector<std::string> axis_names = { "X", "Y", "Z", "-X", "-Y", "-Z" };
	for (size_t i = 0; i < locals.size(); ++i)
	{
		size_t nearest_index = 0;
		for (size_t j = 0; j < globals.size(); ++j)
		{
			if (locals[i].angleBetween(globals[j]) < locals[i].angleBetween(globals[nearest_index]))
				nearest_index = j;
		}
		// Converting
		converted[nearest_index % 3] = (nearest_index >= 3 ? -1 : 1) * direction[i];
	}
	return converted;
}

// Key listener
bool GameCoordinator::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_NUMPADENTER && mControlProvider->getGamePhase() != GameControlProvider::GP_END && mControlProvider->getActivePlayer() == 0)
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
	fireEventOnActiveGrid();
	return true;
}

bool GameCoordinator::handleReadyButton(const CEGUI::EventArgs& arg)
{
	mControlProvider->setDone();
	return true;
}

bool GameCoordinator::handleFireButton(const CEGUI::EventArgs& arg)
{
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
	if (mControlProvider->getGamePhase() == GameControlProvider::GP_SET)
		mGrids[0]->processSelected();
	else if (mControlProvider->getGamePhase() == GameControlProvider::GP_BATTLE)
	{
		if (mControlProvider->getActivePlayer() == 0)
		{
			mGrids[1]->processSelected();
		}
	}
}

void GameCoordinator::processFireResult(GameControlProvider::FireResult fireResult)
{
	if (!fireResult.isValid)
		return;
	// Marker
	if (fireResult.isDamaged)
		mGrids[fireResult.playerOfGrid]->setMarkerAt(Grid3D::MT_RED, fireResult.coords);
	else
		mGrids[fireResult.playerOfGrid]->setMarkerAt(Grid3D::MT_WHITE, fireResult.coords);
	// Sinked ship
	if (fireResult.isSink)
	{
		ShipHull* ship = fireResult.sunkenShip;
		if (!ship)
			ship = mControlProvider->createShip(mGrids[fireResult.playerOfGrid], fireResult.ship.coords, fireResult.ship.type);
		assert(ship != NULL);
		ship->setColor(ShipHull::SCLR_RED);
		if (!fireResult.sunkenShip)
		{
			ship->setOrientationIndex(fireResult.ship.orientation);
		}
		// Update ship lists
		for (auto shipListController : mShipControllers)
			shipListController.update();
	}
}

// On Game Events
void GameCoordinator::onPlayerChange(int playerFrom, int playerTo)
{
	if (mControlProvider->getGamePhase() == GameControlProvider::GP_BATTLE)
	{
		mGrids[1 - playerFrom]->deactivate();
		mGrids[1 - playerTo]->activate();
		mGrids[0]->setMarkerVisible(false);

		CEGUI::Window* control_panel = CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Control/Battle");
		control_panel->setEnabled(playerTo == 0);
	}
}

void GameCoordinator::onSetReady()
{
	cout << "asdf" << endl;
	// Ready Button
	CEGUI::PushButton* readyButton = static_cast<CEGUI::PushButton*>(CEGUI::System::getSingleton().getGUISheet()->getChildRecursive(
			"Game/Control/Set/ReadyButton"));
	cout << "asdf2" << endl;
	if (readyButton)
		readyButton->setEnabled(true);
	cout << "asdf3" << endl;
}

void GameCoordinator::onSetCancel()
{
	// Ready Button
	CEGUI::PushButton* readyButton = static_cast<CEGUI::PushButton*>(CEGUI::System::getSingleton().getGUISheet()->getChildRecursive(
			"Game/Control/Set/ReadyButton"));
	if (readyButton)
		readyButton->setEnabled(false);
}

void GameCoordinator::onShipCreated()
{
	if (mControlProvider->getGamePhase() == GameControlProvider::GP_SET)
		if (mControlProvider)
			mShipControllers[mControlProvider->getActivePlayer()].update();
}

void GameCoordinator::onBattleStart()
{
	// Grids
	mGrids[0]->deactivate();
	mGrids[0]->setMarkerVisible(false);
	mGrids[1]->activate();
	// GUI
	CEGUI::Window* set_panel = CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Control/Set");
	CEGUI::Window* battle_panel = CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Control/Battle");
	set_panel->setVisible(false);
	battle_panel->setVisible(true);
	// Ship List Controllers
	for (auto& shipListController : mShipControllers)
		shipListController.build();
}

void GameCoordinator::onBattleEnd(int winnerPlayer)
{
	CEGUI::System::getSingleton().getGUISheet()->deactivate();
	CEGUI::Window* menu = CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/Menu");
	CEGUI::Window* end_text = CEGUI::System::getSingleton().getGUISheet()->getChildRecursive("Game/End");
	menu->activate();
	end_text->setVisible(true);
	if (winnerPlayer == 0)
		end_text->setText(utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.won")));
	else if (winnerPlayer == 1)
		end_text->setText(utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.lost")));
	else
		end_text->setText(utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.quit")));
}

void GameCoordinator::onShot(GameControlProvider::FireResult fireResult)
{
	processFireResult(fireResult);
}
