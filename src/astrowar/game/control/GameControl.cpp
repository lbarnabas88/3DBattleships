/*
 * GameControl.cpp
 *
 *  Created on: 2013.06.17.
 *      Author: Baarnus
 */

#include "GameControl.hpp"
// CEGUI
#include "../../../graphics/Cegui/CeguiTranslator.hpp"

//****************************************
//*** Game Control
//****************************************

GameControl::GameControl(Ogre::SceneManager* sceneManager) :
		mShipyard(sceneManager), mSelectedShip(NULL), mListener(NULL), mPhase(GP_SET)
{
	// Init shipyard
	mShipyard.registerShipType("Destroyer", { "Destroyer.mesh" });
	mShipyard.registerShipType("Cruiser", { "Cruiser.mesh" });
	mShipyard.registerShipType("Battleship", { "Battleship.mesh" });
	mShipyard.registerShipType("Station", { "Station.mesh" });
	mShipyard.registerShipType("Carrier", { "Carrier.mesh" });

	mShipNumbers =
	{	2,2,1,1,1};
	mListSelections[0] = 0;
	mListSelections[1] = 0;
	// TODO load from game
}

GameControl::~GameControl()
{
}

// Control Provider
GameControlProvider::GamePhase GameControl::getGamePhase()
{
	return mPhase;
}

bool GameControl::isSetReady()
{
	size_t sum = 0;
	for (auto num : mShipNumbers)
		sum += num;
	// TODO from game
	return sum == 0;
}

int GameControl::getActivePlayer()
{
	return 0;
}

void GameControl::setDone()
{
	mPhase = GP_BATTLE;
}

ShipHull* GameControl::createShip(Grid3D* grid, std::vector<size_t> coords)
{
	//
	auto player = getActivePlayer();
	auto ship_index = mListSelections[player];
	if (mShipNumbers[ship_index] == 0)
		return NULL;
	mShipNumbers[ship_index]--;
	// Get which type need to be made
	auto ship = mShipyard.createShip(mShipyard.getNameOfShipType(ship_index), grid);
	if (ship)
	{
		moveShipTo(ship, coords);
		onShipCreated();
	}
	return ship;
}

ShipHull* GameControl::getShipForNode(Ogre::SceneNode* sceneNode)
{
	return mShipyard.getShip(sceneNode);
}

// Move Ship
void GameControl::moveShipTo(ShipHull* ship, std::vector<size_t> coords)
{
	// Normalize coords
	std::vector<size_t> dimensions = ship->getGrid()->getDimensions();
	for (size_t i = 0; i < 3; ++i)
	{
		while (coords[i] >= dimensions[i])
			coords[i] -= dimensions[i];
	}
	// Set coords
	ship->getNode()->setPosition(ship->getGrid()->coords2position(coords));
	// Color the ship
	checkShip(ship);
}

void GameControl::moveShipBy(ShipHull* ship, std::vector<int> coords)
{
	std::vector<size_t> new_coords = ship->getGrid()->position2coords(ship->getNode()->getPosition());
	std::vector<size_t> dimensions = ship->getGrid()->getDimensions();
	for (size_t i = 0; i < 3; ++i)
	{
		coords[i] += new_coords[i];
		while (coords[i] < 0)
			coords[i] += dimensions[i];
		new_coords[i] = coords[i];
	}
	moveShipTo(ship, new_coords);
}

void GameControl::rotateShipTo(ShipHull* ship, size_t orientation_index)
{
	while (orientation_index >= ShipHull::sOrientations.size())
		orientation_index -= ShipHull::sOrientations.size();
	ship->setOrientationIndex(orientation_index);
	// Color the ship
	checkShip(ship);
}

void GameControl::rotateShipNext(ShipHull* ship)
{
	rotateShipTo(ship, ship->getOrientationIndex() + 1);
}

void GameControl::rotateShipPrev(ShipHull* ship)
{
	int orientation_index = int(ship->getOrientationIndex()) - 1;
	while (orientation_index < 0)
		orientation_index += ShipHull::sOrientations.size();
	rotateShipTo(ship, orientation_index);
}

bool GameControl::isExistsSelectedShip()
{
	return mSelectedShip;
}

void GameControl::selectShip(ShipHull* ship)
{
	colorOnSelection(mSelectedShip);
	mSelectedShip = (ship == mSelectedShip ? NULL : ship);
	colorOnSelection(mSelectedShip);
}

void GameControl::moveSelectedShipTo(std::vector<size_t> coords)
{
	if (mSelectedShip)
		moveShipTo(mSelectedShip, coords);
}

void GameControl::moveSelectedShipBy(std::vector<int> coords)
{
	if (mSelectedShip)
		moveShipBy(mSelectedShip, coords);
}

void GameControl::rotateSelectedShipTo(size_t orientation_index)
{
	if (mSelectedShip)
		rotateShipTo(mSelectedShip, orientation_index);
}

void GameControl::rotateSelectedShipNext()
{
	if (mSelectedShip)
		rotateShipNext(mSelectedShip);
}

void GameControl::rotateSelectedShipPrev()
{
	if (mSelectedShip)
		rotateShipPrev(mSelectedShip);
}

// Data
size_t GameControl::numOfShipType(int player)
{
	return mShipyard.getNumberOfShipTypes();
}

CEGUI::String GameControl::getShipTypeName(int player, size_t i)
{
	return utf8ToCeguiString(mShipyard.getNameOfShipType(i));
}

size_t GameControl::getShipTypeCount(int player, size_t i)
{
	return mShipNumbers[i];
}

// Listener
void GameControl::onSelectionChange(int player, unsigned selection)
{
	std::cout << "Player#" << player << ", SelectedShipId: " << selection << ", Ship type: " << mShipyard.getNameOfShipType(selection) << std::endl;
	mListSelections[player] = selection;
}

// Self listener
GameControlListener* GameControl::getListener()
{
	return mListener;
}

void GameControl::setListener(GameControlListener* listener)
{
	mListener = listener;
}

// Check if ship is on a valid position
bool GameControl::isShipValid(ShipHull* ship)
{
	return true;
}

// Color shipt to correct color
void GameControl::checkShip(ShipHull* ship)
{
	// If ship NULL to nothing
	if (!ship)
		return;
	// Color ship
	ShipHull::ShipColor ship_color = ShipHull::SCLR_DEFAULT;
	if (isShipValid(ship))
	{
		if (mSelectedShip == ship)
			ship_color = ShipHull::SCLR_CYAN;
		else
			ship_color = ShipHull::SCLR_DEFAULT;
	}
	else
	{
		if (mSelectedShip == ship)
			ship_color = ShipHull::SCLR_MAGENTA;
		else
			ship_color = ShipHull::SCLR_RED;
	}
	ship->setColor(ship_color);
	// Check
	static bool prev_ready = false;
	bool act_ready = isSetReady();
	if (act_ready && !prev_ready)
		onSetReady();
	if (!act_ready && prev_ready)
		onSetCancel();
	prev_ready = act_ready;
}

void GameControl::colorOnSelection(ShipHull* ship)
{
	if (!ship)
		return;
	if (ship->getColor() == ShipHull::SCLR_DEFAULT)
		ship->setColor(ShipHull::SCLR_CYAN);
	else if (ship->getColor() == ShipHull::SCLR_RED)
		ship->setColor(ShipHull::SCLR_MAGENTA);
	else if (ship->getColor() == ShipHull::SCLR_CYAN)
		ship->setColor(ShipHull::SCLR_DEFAULT);
	else if (ship->getColor() == ShipHull::SCLR_MAGENTA)
		ship->setColor(ShipHull::SCLR_RED);

}

void GameControl::onPlayerChange(int playerFrom, int playerTo)
{
	if (mListener)
		mListener->onPlayerChange(playerFrom, playerTo);
}

void GameControl::onSetReady()
{
	if (mListener)
		mListener->onSetReady();
}

void GameControl::onSetCancel()
{
	if (mListener)
		mListener->onSetCancel();
}

void GameControl::onShipCreated()
{
	if (mListener)
		mListener->onShipCreated();
}
