/*
 * GameModel.h
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#ifndef GAMEMODEL_H_
#define GAMEMODEL_H_
#include "PhysicsModel.h"
#include "NetworkModel.h"
#include "SFML/Network.hpp"
#include <vector>
#include "../elements/Ship.h"
#include "../elements/Pair.h"
#include "../elements/Message.h"
#include "../../../../tools/json/Parser.h"

using namespace std;

namespace AstrOWar {

class graphics;
// csak egy mintaosztály, nem létezik

class GameModel {
private:
	bool youtNext;
	graphics *gr;
	std::vector<AstrOWar::Ship> kollekcio; // a hajók listája, mindegyikből 1 példány
	map<int, Message> lista;					// elküldött üzeneteket tárolja
	PhysicsModel *pModel;									// fizikai model
	NetworkModel *nModel;									// hálózati model

	void (graphics::*fireHandler)(int, int, int, bool, bool);	// lövés esetén értesítés
	void (graphics::*hitHandler)(int, int, int, bool, bool);// találat esetén értesítés
	void (graphics::*deadHandler)(bool);			// halál esetén értesítés
	void (graphics::*exitHandler)();				// kilépés esetén értesítés
	void (graphics::*errorHandler)(int);	// hiba esetén értesítés, hibakóddal
	static int idCounter;
	static int getId() {
		return GameModel::idCounter + 1;
	}
	static int shipIdCounter;
protected:
	GameModel();
	virtual ~GameModel();

	void messageEventHandler(std::string encodedString);
	void messageEventHandlerHELLO(Message &m);
	void messageEventHandlerEXIT(Message &m);
	void messageEventHandlerOK(Message &m);
	void messageEventHandlerBAD(Message &m);
	void messageEventHandlerFIRE(Message &m);
	void messageEventHandlerIMDIED(Message &m);
	void messageEventHandlerFIREOK(Message &m);
	void messageEventHandlerFIREBAD(Message &m);
	void messageEventHandlerFIRESUCESS(Message &m);

	void sendMessageOnNetwork(Message msg);

	void _fire(Message &m);

public:

	static GameModel& getSingleton();
	void init(graphics *g);
	void createTeszt();


	/*
	 * lövés esetén x,y,z koordináták, true ha sikeres, false ha nem
	 */
	void registerFireEventHandler(void (graphics::*fire)(int, int, int, bool, bool));
	/*
	 * találat esetén: x,y,z koordináták, és true ha sikeres, false ha nem
	 */
	void registerHitEventHandler(void (graphics::*hit)(int, int, int, bool, bool));
	/*
	 * játékos halála esetén, true ha én, false ha az ellenfél
	 */
	void registerDeadEventHandler(void (graphics::*dead)(bool));
	/*
	 * játékos kilépése esetén
	 */
	void registerExitEventHandler(void (graphics::*exit)());
	/*
	 * hiba esetén, hibakóddal
	 */
	void registerErrorEventHandler(void (graphics::*error)(int));

	void setSocket(sf::TcpSocket *mSocket, bool _iss);

	bool isYourNext();
	void fire(int x, int y, int z);
	Pair<int> addShipToModel(int type, int x, int y, int z);
	Pair<int> addShipToModel(std::string type, int x, int y, int z);
	Pair<int> editShip(int id, int x, int y, int z);
	int deleteShip(int id);
	void exit();
	void reset(int i);
	bool isEnableConnection();
	void start();
};

extern GameModel& GameModelSingleton;

} /* namespace AstrOWar */
#endif /* GAMEMODEL_H_ */
