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
#include "../../../tools/json/Parser.h"
#include "GameModelListener.h"

using namespace std;

namespace AstrOWar {

// csak egy mintaosztály, nem létezik

class GameModel {
private:
	bool youtNext;
	GameModelListener *gml;
	std::vector<AstrOWar::Ship> kollekcio; // a hajók listája, mindegyikből 1 példány
	map<int, Message> lista;					// elküldött üzeneteket tárolja
	PhysicsModel *pModel;									// fizikai model
	NetworkModel *nModel;									// hálózati model

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
	void init(GameModelListener *g);
	void createTeszt();

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
