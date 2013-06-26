/*
 * GameModel.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "GameModel.h"

namespace AstrOWar {

int GameModel::idCounter = 0;
int GameModel::shipIdCounter = 0;

GameModel::GameModel() :
		fireHandler(nullptr), hitHandler(nullptr), deadHandler(nullptr), exitHandler(
				nullptr), errorHandler(nullptr) {
	youtNext = false;
	gr = nullptr;
	pModel = new PhysicsModel(this);
	nModel = new NetworkModel(this);
}

GameModel::~GameModel() {
}

void GameModel::init(graphics *g) {
	gr = g;
	JSON::JSONSingleton.parse("config/astrowar.json", &kollekcio);
	nModel->registerMessageEventHandler(&GameModel::messageEventHandler);
}

/*
 *  n = 10
 *	add Fighter, Destroyer, Battleship, Carrier to physicsModel
 */
void GameModel::createTeszt() {
	pModel->init(10);
	addShipToModel("Fighter", 0, 3, 0);
	addShipToModel("Destroyer", 1, 6, 0);
	addShipToModel("Battleship", 2, 0, 0);
	addShipToModel("Carrier", 3, 0, 0);
	pModel->toString();
}

void GameModel::setSocket(sf::TcpSocket *mSocket, bool _iss){
	nModel->setSocket(mSocket, _iss);
	youtNext = _iss;
}

Pair<int> GameModel::addShipToModel(int type, int x, int y, int z){
	for (Ship s : kollekcio) {
		if (s.getType() == type)
			return pModel->addShip(s.clone(GameModel::shipIdCounter++), x, y, z);
	}
	return Pair<int>(3, -1);
}

Pair<int> GameModel::addShipToModel(std::string type, int x, int y, int z) {
	for (Ship s : kollekcio) {
		if (s.getName() == type)
			return pModel->addShip(s.clone(GameModel::shipIdCounter++), x, y, z);
	}
	return Pair<int>(3, -1);
}

Pair<int> GameModel::editShip(int id, int x, int y, int z){
	Ship* hajo = nullptr;
	for(Ship* s : pModel->getShips()){
		if(s->getId() == id) hajo = s;
	}
	if(hajo == nullptr) return Pair<int>(3, -1);
	return pModel->editShip(hajo, x, y, z);
}

int GameModel::deleteShip(int id){
	Ship* hajo = nullptr;
	for(Ship* s : pModel->getShips()){
		if(s->getId() == id) hajo = s;
	}
	if(hajo==nullptr) return 1;
	hajo->resetField();
	delete hajo;
	return 0;
}

void GameModel::sendMessageOnNetwork(Message msg) {
	lista[msg.getId()] = msg;
	if (msg.getId() > GameModel::idCounter)
		GameModel::idCounter = msg.getId();
	nModel->sendMessage(JSON::JSONSingleton.encode(msg));
}

void GameModel::messageEventHandlerHELLO(Message &m) {
	if (!nModel->isServer()) {
		sendMessageOnNetwork(
				Message().init(HELLO, GameModel::getId(), m.getId()));
	}
}
void GameModel::messageEventHandlerEXIT(Message &m) {
	//INFO ellenfél kilépett
	if (exitHandler != nullptr)
		(gr->*exitHandler)();
}
void GameModel::messageEventHandlerOK(Message &m) {
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == IMDIED) {
			//INFO utasítások lekezelése
			if (deadHandler != nullptr) {
				(gr->*deadHandler)(true);
			}
		}
	}
}
void GameModel::messageEventHandlerBAD(Message &m) {
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());
		//INFO utasítások elvetése
		if (errorHandler != nullptr)
			(gr->*errorHandler)(m.getMsgType());
	}
}
void GameModel::messageEventHandlerFIRE(Message &m) {
	//INFO	ő volt e soron
	if (!isYourNext()) {
		//INFO valid e az utasítás
		if (m.validate(pModel->getDimension())) {
			//INFO utasítás végrehajtása
			if (pModel->fire(m)) {
				//INFO eredmény alapján FIREOK
				sendMessageOnNetwork(
						Message().init(FIREOK, GameModel::getId(), m.getId()));
				if (pModel->checkShip(m)) {
					if (hitHandler != nullptr)	// sucess
						(gr->*hitHandler)(m.getPosX(), m.getPosY(), m.getPosZ(),
								true, true);
					pModel->check();
				} else {
					if (hitHandler != nullptr) // csak sérülés
						(gr->*hitHandler)(m.getPosX(), m.getPosY(), m.getPosZ(),
								true, false);
				}

				if (pModel->idead()) {
					sendMessageOnNetwork(
							Message().init(IMDIED, GameModel::getId(), 0));
				}

			} else {
				//INFO eredmény alapján FIREBAD
				sendMessageOnNetwork(
						Message().init(FIREBAD, GameModel::getId(), m.getId()));
				if (hitHandler != nullptr)
					(gr->*hitHandler)(m.getPosX(), m.getPosY(), m.getPosZ(),
							false, false);
			}
			youtNext = true;
			return;
		}
	}
	//INFO eredmény alapján BAD
	sendMessageOnNetwork(Message().init(BAD, m.getId() + 1, m.getId()));
}
void GameModel::messageEventHandlerIMDIED(Message &m) {
	//INFO az ellenfél az összes hajóját elvesztette
	sendMessageOnNetwork(Message().init(OK, GameModel::getId(), m.getId()));
	if (deadHandler != nullptr) {
		(gr->*deadHandler)(false);
	}
}
void GameModel::messageEventHandlerFIREOK(Message &m) {
	//INFO találat, garfikai kijelzés
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == FIRE) {
			pModel->fire(m);
			youtNext = false;
			if (fireHandler != nullptr) {
				(gr->*fireHandler)(m.getPosX(), m.getPosY(), m.getPosZ(), true,
						false);
			}
		}
	}
}
void GameModel::messageEventHandlerFIRESUCESS(Message &m) {
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == FIRE) {
			pModel->fire(m);
			youtNext = false;
			if (fireHandler != nullptr) {
				(gr->*fireHandler)(m.getPosX(), m.getPosY(), m.getPosZ(), true,
						true);
			}
		}
	}
}
void GameModel::messageEventHandlerFIREBAD(Message &m) {
	//INFO célt tévesztett, garfikai kijelzés
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == FIRE) {
			pModel->fire(m);
			youtNext = false;
			if (fireHandler != nullptr) {
				(gr->*fireHandler)(m.getPosX(), m.getPosY(), m.getPosZ(), false,
						false);
			}
		}
	}
}

void GameModel::messageEventHandler(std::string encodedString) {
	Message m = JSON::JSONSingleton.decode(encodedString);
	if (GameModel::idCounter < m.getId())
		idCounter = m.getId();
	if (m.validate()) {
		switch (m.getMsgType()) {
		case HELLO:
			messageEventHandlerHELLO(m);
			break;
		case OK:
			messageEventHandlerOK(m);
			break;
		case BAD:
			messageEventHandlerBAD(m);
			break;
		case FIRE:
			messageEventHandlerFIRE(m);
			break;
		case IMDIED:
			messageEventHandlerIMDIED(m);
			break;
		case FIREOK:
			messageEventHandlerFIREOK(m);
			break;
		case FIREBAD:
			messageEventHandlerFIREBAD(m);
			break;
		case FIRESUCESS:
			messageEventHandlerFIRESUCESS(m);
			break;
		case EXIT:
			messageEventHandlerEXIT(m);
			break;
		}
	}
}

void GameModel::start() {
	sendMessageOnNetwork(Message().init(HELLO, GameModel::getId(), 0));
}
/*
 * lövés esetén x,y,z koordináták, true ha sikeres, false ha nem
 */
void GameModel::registerFireEventHandler(
		void (graphics::*fire)(int, int, int, bool, bool)) {
	fireHandler = fire;
}
/*
 * találat esetén: x,y,z koordináták, és true ha sikeres, false ha nem
 */
void GameModel::registerHitEventHandler(
		void (graphics::*hit)(int, int, int, bool, bool)) {
	hitHandler = hit;
}
/*
 * játékos halála esetén, true ha én, false ha az ellenfél
 */
void GameModel::registerDeadEventHandler(void (graphics::*dead)(bool)) {
	deadHandler = dead;
}
/*
 * játékos kilépése esetén
 */
void GameModel::registerExitEventHandler(void (graphics::*exit)()) {
	exitHandler = exit;
}
/*
 * hiba esetén, hibakóddal
 */
void GameModel::registerErrorEventHandler(void (graphics::*error)(int)) {
	errorHandler = error;
}

bool GameModel::isYourNext() {
	return youtNext;
}
/*
 * lövés leadása ellenfélre
 */
void GameModel::fire(int x, int y, int z) {
	sendMessageOnNetwork(Message().init(FIRE, x, y, z, GameModel::getId(), 0));
}
/*
 * kilépés kezdeményezése
 */
void GameModel::exit() {
	sendMessageOnNetwork(Message().init(EXIT, GameModel::getId(), 0));
}

void GameModel::reset(int i) {
	pModel->init(i);
}

bool GameModel::isEnableConnection(){return nModel->isEnableConnection();}



GameModel& GameModel::getSingleton() {
	static GameModel instance;
	return instance;
}

GameModel& GameModelSingleton = GameModel::getSingleton();

} /* namespace AstrOWar */
