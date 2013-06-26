/*
 * GameControl.cpp
 *
 *  Created on: 2013.06.17.
 *      Author: Baarnus
 */

#include "GameControl.hpp"
// CEGUI
#include "../../../graphics/Cegui/CeguiTranslator.hpp"
// GAME
#include "../../../game/settings/GameSettings.hpp"
// ECHO
#include "../../../tools/echoes.hpp"

#include "../model/elements/Ship.h"
#include "../model/models/GameModel.h"

//****************************************
//*** Game Control
//****************************************

GameControl::GameControl(Ogre::SceneManager* sceneManager) :
		mShipyard(sceneManager), mSelectedShip(NULL), mListener(NULL), mPrevReady(false), mPhase(GP_SET), mPlayer(0)
{
	// Init shipyard
	/*	mShipyard.registerShipType("Destroyer", { "Destroyer.mesh" });
	 mShipyard.registerShipType("Cruiser", { "Cruiser.mesh" });
	 mShipyard.registerShipType("Battleship", { "Battleship.mesh" });
	 mShipyard.registerShipType("Station", { "Station.mesh" });
	 mShipyard.registerShipType("Carrier", { "Carrier.mesh" });
	 */

	for (auto i : AstrOWar::GameModelSingleton.getCollection())
	{
		mShipyard.registerShipType(i.getName(), { i.getMesh() });
		mInitShipNumbers.push_back(i.getMax());
	}
	mShipNumbers = mInitShipNumbers;

	mListSelections[0] = 0;
	mListSelections[1] = 0;
}

GameControl::~GameControl()
{
	AstrOWar::GameModelSingleton.exit();
}

// Control Provider
GameControlProvider::GamePhase GameControl::getGamePhase()
{
	return mPhase;
}

bool GameControl::isSetReady()
{
	// All ship set
	size_t sum = 0;
	for (auto num : mShipNumbers)
		sum += num;
	if (sum != 0)
		return false;
	for (auto& ship : mShips)
		if (isShipValid(ship.hull))
			return false;
	return true;
}

int GameControl::getActivePlayer()
{
	return mPlayer;
}

void GameControl::setDone()
{
	selectShip(NULL);
	onBattleStart();
}

void GameControl::fireTorpedo(std::vector<size_t> coords)
{
	// FIRE
	AstrOWar::GameModelSingleton.fire(coords[0], coords[1], coords[2]);
}

ShipHull* GameControl::createShip(Grid3D* grid, std::vector<size_t> coords, std::string type)
{
	//
	auto player = getActivePlayer();
	auto ship_index = mListSelections[player];
	if (mShipNumbers[ship_index] == 0 && type == "")
		return NULL;
	if (type == "")
		mShipNumbers[ship_index]--;
	// Get which type need to be made
	auto ship = mShipyard.createShip(type == "" ? mShipyard.getNameOfShipType(ship_index) : type, grid);
	if (ship)
	{
		moveShipTo(ship, coords);
		onShipCreated();

		// Model
		if (getGamePhase() == GP_SET)
		{
			auto result = AstrOWar::GameModelSingleton.addShipToModel(type, coords[0], coords[1], coords[2]);
			addShip(result.resume, ship);
		}
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

bool GameControl::needSelection()
{
	return mPhase == GP_SET;
}

CEGUI::String GameControl::getShipColumnName()
{
	if (mPhase == GP_SET)
		return utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.ships.name.set"));
	else
		return utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.ships.name.battle"));
}

CEGUI::String GameControl::getQuantityColumnName()
{
	return utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode("game.ships.quantity"));
}

// Listener
void GameControl::onSelectionChange(int player, unsigned selection)
{
	if (mPhase == GP_SET)
	{
		echof("Player#%d, SelectedShipId: %d, ShipType %s", player, selection, mShipyard.getNameOfShipType(selection).c_str());
		mListSelections[player] = selection;
	}
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

/*
 * lövés esetén x,y,z koordináták, true ha sikeres, false ha nem
 */
void GameControl::onFireEvent(int x, int y, int z, bool damaged, bool sunken, AstrOWar::Ship* ship)
{
	// Set player
	setPlayer(1);
	// Set result
	FireResult result;
	result.isValid = true;
	result.playerOfGrid = 1;
	result.coords = std::vector<size_t>( { (size_t) x, (size_t) y, (size_t) z });
	result.isDamaged = damaged;
	result.isSink = sunken;
	if (result.isSink)
	{
		// TODO ship details from network
		result.ship.type = ship->getName();
		result.ship.orientation = 0;
		result.ship.coords = std::vector<size_t>( { (size_t) ship->getPx(), (size_t) ship->getPy(), (size_t) ship->getPz() });
	}
	// Call listener
	onShot(result);
}
/*
 * találat esetén: x,y,z koordináták, és true ha sikeres, false ha nem
 */
void GameControl::onHitEvent(int x, int y, int z, bool damaged, bool sunken)
{
	// Set player
	setPlayer(0);
	// Set result
	FireResult result;
	result.isValid = true;
	result.playerOfGrid = 0;
	result.coords = std::vector<size_t>( { (size_t) x, (size_t) y, (size_t) z });
	result.isDamaged = damaged;
	result.isSink = sunken;
	if (result.isSink)
	{
		int shipId = AstrOWar::GameModelSingleton.getShipWithPosition(x, y, z);
		if (shipId >= 0)
			result.sunkenShip = getShipForId(shipId);
		else
			result.sunkenShip = NULL;
		result.isSink = result.sunkenShip != NULL;
	}
	// Call listener
	onShot(result);
}
/*
 * játékos halála esetén, true ha én, false ha az ellenfél
 */
void GameControl::onDeadEvent(bool won)
{
	onBattleEnd(won ? 0 : 1);
}
/*
 * játékos kilépése esetén
 */
void GameControl::onExitEvent()
{
	onBattleEnd(-1);
}
/*
 * hiba esetén, hibakóddal
 */
void GameControl::onErrorEvent(int error)
{
}

/*
 * a hálózat állapotát adja meg
 */
void GameControl::onNetworkEvent(bool success)
{
	if (AstrOWar::GameModelSingleton.isYourNext())
		setPlayer(0);
	else
		setPlayer(1);
}

// Check if ship is on a valid position
bool GameControl::isShipValid(ShipHull* ship)
{
	return true;
//	auto id = getIdForShip(ship);
//	if (id < 0)
//		return false;
//	return AstrOWar::GameModelSingleton.isValidShip(id);
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
	bool act_ready = isSetReady();
	if (act_ready && !mPrevReady)
		onSetReady();
	if (!act_ready && mPrevReady)
		onSetCancel();
	mPrevReady = act_ready;
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

void GameControl::onBattleStart()
{
	mPhase = GP_BATTLE;

	mShipNumbers[0] = 2;

	if (mListener)
		mListener->onBattleStart();
}

void GameControl::onBattleEnd(int winnerPlayer)
{
	mPhase = GP_END;
	if (mListener)
		mListener->onBattleEnd(winnerPlayer);
}

void GameControl::onShot(FireResult fireResult)
{
	if (mListener)
		mListener->onShot(fireResult);
}

void GameControl::setPlayer(int player)
{
	mPlayer = player;
	onPlayerChange(1 - mPlayer, mPlayer);
}

ShipHull* GameControl::getShipForId(int id)
{
	for (auto& ship : mShips)
		if (ship.iD == id)
			return ship.hull;
	return NULL;
}

int GameControl::getIdForShip(ShipHull* hull)
{
	for (auto& ship : mShips)
		if (ship.hull == hull)
			return ship.iD;
	return -1;
}

void GameControl::addShip(int id, ShipHull* hull)
{
	mShips.push_back( { id, hull });
}
