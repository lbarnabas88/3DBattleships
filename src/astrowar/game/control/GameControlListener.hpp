/*
 * GameControlListener.hpp
 *
 *  Created on: 2013.06.24.
 *      Author: Baarnus
 */

#ifndef GAMECONTROLLISTENER_HPP_
#define GAMECONTROLLISTENER_HPP_

#include "GameControlProvider.hpp"

class GameControlListener
{
public:
	virtual ~GameControlListener();
	// On Game Events
	virtual void onPlayerChange(int playerFrom, int playerTo) = 0;
	virtual void onSetReady() = 0;
	virtual void onSetCancel() = 0;
	virtual void onShipCreated() = 0;
	virtual void onBattleStart() = 0;
	virtual void onBattleEnd(int winnerPlayer) = 0;
	virtual void onEnemyShot(GameControlProvider::FireResult fireResult) = 0;
};

#endif /* GAMECONTROLLISTENER_HPP_ */
