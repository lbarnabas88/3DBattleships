/*
 * GameModel.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "GameModel.h"
#include "../../../../tools/echoes.hpp"

namespace AstrOWar {

int GameModel::idCounter = 0;
int GameModel::shipIdCounter = 1;

GameModel::GameModel() {
	youtNext = false;
	gml = nullptr;
	pModel = new PhysicsModel(this);
	nModel = new NetworkModel(this);
}

GameModel::~GameModel() {
}
int GameModel::getId() {
	return GameModel::idCounter + 1;
}

void GameModel::init() {
	JSON::JSONSingleton.parse("config/astrowar.json", &kollekcio);
	nModel->registerMessageEventHandler(&GameModel::messageEventHandler);
}

void GameModel::setListener(GameModelListener *g) {
	gml = g;
}

void GameModel::setSocket(sf::TcpSocket *mSocket, bool _iss) {
	nModel->setSocket(mSocket, _iss);
	youtNext = _iss;
}

Pair<int> GameModel::addShipToModel(int type, int x, int y, int z) {
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

bool GameModel::isValidShip(int id) {
	pModel->dump();
	Ship* hajo = nullptr;
	for (Ship* s : pModel->getShips()) {
		if (s->getId() == id)
			hajo = s;
	}
	if (hajo == nullptr)
		return false;
	return pModel->isValidShip(hajo);
}

Pair<int> GameModel::editShip(int id, int x, int y, int z) {
	Ship* hajo = nullptr;
	for (Ship* s : pModel->getShips()) {
		if (s->getId() == id)
			hajo = s;
	}
	if (hajo == nullptr)
		return Pair<int>(3, -1);
	return pModel->editShip(hajo, x, y, z);
}

int GameModel::deleteShip(int id) {
	Ship* hajo = nullptr;
	for (Ship* s : pModel->getShips()) {
		if (s->getId() == id)
			hajo = s;
	}
	if (hajo == nullptr)
		return 1;
	hajo->resetField();
	delete hajo;
	return 0;
}

int GameModel::getShipWithPosition(int x, int y, int z) {
	return pModel->getShipWithPosition(x, y, z);
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
		if (gml != nullptr)
			gml->onNetworkEvent(true);
	} else
		sendMessageOnNetwork(Message().init(OK, GameModel::getId(), m.getId()));
}
void GameModel::messageEventHandlerEXIT(Message &m) {
	//INFO ellenfél kilépett
	if (gml != nullptr)
		gml->onExitEvent();
}
void GameModel::messageEventHandlerOK(Message &m) {
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == IMDIED) {
			//INFO utasítások lekezelése
			if (gml != nullptr)
				gml->onDeadEvent(true);
		} else if (oldM.getMsgType() == HELLO) {
			if (gml != nullptr)
				gml->onNetworkEvent(true);
		}
	}
}
void GameModel::messageEventHandlerBAD(Message &m) {
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());
		//INFO utasítások elvetése
		if (gml != nullptr)
			gml->onErrorEvent(m.getMsgType());
	}
}
void GameModel::messageEventHandlerFIRE(Message &m) {
	//INFO	ő volt e soron
	if (!isYourNext()) {
		cout << "fire init" << endl;
		//INFO valid e az utasítás
		if (m.validate(pModel->getDimension())) {
			cout << "fire validate" << endl;
			//INFO utasítás végrehajtása
			if (pModel->fire(m)) {
				cout << "fire fire" << endl;
				//INFO eredmény alapján FIREOK
				if (pModel->checkShip(m)) {
					Ship* s = pModel->getShipObjectWithPosition(m.getPosX(),
							m.getPosY(), m.getPosZ());
					sendMessageOnNetwork(
							Message().init(FIRESUCESS, s->getPx(), s->getPy(),
									s->getPz(), GameModel::getId(), m.getId(),
									s->getType()));
					if (gml != nullptr)	// sucess
						gml->onHitEvent(m.getPosX(), m.getPosY(), m.getPosZ(),
								true, true);
					pModel->check();
				} else {
					sendMessageOnNetwork(
							Message().init(FIREOK, GameModel::getId(),
									m.getId()));
					if (gml != nullptr) // csak sérülés
						gml->onHitEvent(m.getPosX(), m.getPosY(), m.getPosZ(),
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
				if (gml != nullptr)
					gml->onHitEvent(m.getPosX(), m.getPosY(), m.getPosZ(),
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
	if (gml != nullptr)
		gml->onDeadEvent(false);

}
void GameModel::messageEventHandlerFIREOK(Message &m) {
	//INFO találat, garfikai kijelzés
	if (lista.count(m.getRefId()) > 0) {
		Message oldM = lista[m.getRefId()];
		lista.erase(m.getRefId());

		if (oldM.getMsgType() == FIRE) {
			pModel->fire(m);
			youtNext = false;
			if (gml != nullptr) {
				gml->onFireEvent(oldM.getPosX(), oldM.getPosY(), oldM.getPosZ(),
						true, false, nullptr);
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
			Ship *t = nullptr;
			for (Ship s : kollekcio) {
				echo(utils::t2str(s.getType()) + " *** " + utils::t2str(m.getType()));
				if (s.getType() == m.getType())
					t = s.clone(GameModel::shipIdCounter++);
			}
			if (t != nullptr)
				pModel->addShipToFoe(t, m.getPosX(), m.getPosY(), m.getPosZ());
			echo("suc");
			if (gml != nullptr) {
				echo("suc nagyon");
				gml->onFireEvent(oldM.getPosX(), oldM.getPosY(), oldM.getPosZ(),
						true, true, t);
			}
		}
		echo("wtf?");
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
			if (gml != nullptr) {
				gml->onFireEvent(oldM.getPosX(), oldM.getPosY(), oldM.getPosZ(),
						false, false, nullptr);
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
			cout << "HELLO" << endl;
			messageEventHandlerHELLO(m);
			break;
		case OK:
			cout << "OK" << endl;
			messageEventHandlerOK(m);
			break;
		case BAD:
			cout << "BAD" << endl;
			messageEventHandlerBAD(m);
			break;
		case FIRE:
			cout << "FIRE" << endl;
			messageEventHandlerFIRE(m);
			break;
		case IMDIED:
			cout << "IMDIED" << endl;
			messageEventHandlerIMDIED(m);
			break;
		case FIREOK:
			cout << "FIREOK" << endl;
			messageEventHandlerFIREOK(m);
			break;
		case FIREBAD:
			cout << "FIREBAD" << endl;
			messageEventHandlerFIREBAD(m);
			break;
		case FIRESUCESS:
			cout << "FIRESUCESS" << endl;
			messageEventHandlerFIRESUCESS(m);
			break;
		case EXIT:
			cout << "EXIT" << endl;
			messageEventHandlerEXIT(m);
			break;
		}
	}
}

void GameModel::start() {
	sendMessageOnNetwork(Message().init(HELLO, GameModel::getId(), 0));
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
	pModel->dump();
	sendMessageOnNetwork(Message().init(EXIT, GameModel::getId(), 0));
}

void GameModel::reset(int i) {
	pModel->init(i);
}

bool GameModel::isEnableConnection() {
	return nModel->isEnableConnection();
}

vector<Ship> GameModel::getCollection() {
	return kollekcio;
}

GameModel& GameModel::getSingleton() {
	static GameModel instance;
	return instance;
}

GameModel& GameModelSingleton = GameModel::getSingleton();

} /* namespace AstrOWar */
