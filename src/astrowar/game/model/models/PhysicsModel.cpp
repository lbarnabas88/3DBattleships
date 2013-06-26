/*
 * PhysicsModel.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "PhysicsModel.h"
using namespace std;

namespace AstrOWar {

PhysicsModel::PhysicsModel(GameModel *g) :
		gm(g) {
}

PhysicsModel::~PhysicsModel() {
	destroy();
}

/**
 *  kocka bal,alsó,elülső sarka a (0,0,0) pont
 */
void PhysicsModel::init(int n) {
	destroy();
	cubeMy.resize(n);
	cubeFoe.resize(n);
	for (int i = 0; i < n; i++) {
		cubeMy[i].resize(n);
		cubeFoe[i].resize(n);
		for (int j = 0; j < n; j++) {
			cubeMy[i][j].resize(n);
			cubeFoe[i][j].resize(n);
			for (int k = 0; k < n; k++) {
				cubeMy[i][j][k] = new Field(i, j, k);
				cubeFoe[i][j][k] = new Field(i, j, k);
			}
		}
	}
}

void PhysicsModel::destroy() {
	for (Ship* s : myShips) {
		delete s;
	}
	for (auto x : cubeFoe) {
		for (auto y : x) {
			for (Field* z : y)
				delete z;
		}
	}
	for (auto x : cubeMy) {
		for (auto y : x) {
			for (Field* z : y)
				delete z;
		}
	}
}

int PhysicsModel::getShipWithPosition(int x, int y, int z) {
	if (cubeMy[x][y][z]->getHajo() == nullptr)
		return cubeMy[x][y][z]->getHajo()->getId();
	return -1;
}

void PhysicsModel::addShipToFoe(Ship* s, int x, int y, int z) {
	int code = 0, data = -1;
	s->setPx(x);
	s->setPy(y);
	s->setPz(z);

	foeShips.push_back(s);
	vector<vector<int> > structure = s->getStructure();
	for (unsigned int i = 0; i < structure.size(); i++) {
		for (unsigned int j = 0; j < structure[i].size(); j++) {
			for (int k = 0; k < structure[i][j]; k++) {
				if ((x + i >= cubeFoe.size())
						|| (y + k >= cubeFoe[x + i].size())
						|| (z + j >= cubeFoe[x + i][y + k].size())
						|| (x + i < 0) || (y + k < 0) || (z + j < 0)) {
					code = -1;		// ha kilóg a pályáról
				} else {
					cubeFoe[x + i][y + k][z + j]->setShip(s);
					s->addField(cubeFoe[x + i][y + k][z + j]);
				}
			}
		}
	}
}

Ship* PhysicsModel::getShipObjectWithPosition(int x, int y, int z) {
	return cubeMy[x][y][z]->getHajo();
}

/**
 * hajó hozzáadása a modelhez
 * paraméterek:
 * 		Ship - hajó objektumra mutat
 * 		x, y, z - pozíció, hova kerüljön
 *
 * return: hibakód:
 * 		- 0 : ok
 * 		- 1 : már létező hajó
 * 		- 2 : pályán kívül
 */
Pair<int> PhysicsModel::addShip(Ship *s, int x, int y, int z) {
	int code = 0;
	cout << x << " " << y << " " << z << endl;
	s->setPx(x);
	s->setPy(y);
	s->setPz(z);
	if (!s->isNew()) {
		return Pair<int>(1, -1);	// ha már létező hajó
	}
	myShips.push_back(s);
	myShipsAll.push_back(s);
	vector<vector<int> > structure = s->getStructure();
	for (unsigned int i = 0; i < structure.size(); i++) {
		for (unsigned int j = 0; j < structure[i].size(); j++) {
			for (int k = 0; k < structure[i][j]; k++) {
				if ((x + i >= cubeMy.size()) || (y + k >= cubeMy[x + i].size())
						|| (z + j >= cubeMy[x + i][y + k].size()) || (x + i < 0)
						|| (y + k < 0) || (z + j < 0)) {
					code = -1;		// ha kilóg a pályáról
				} else {
					cubeMy[x + i][y + k][z + j]->setShip(s);
					s->addField(cubeMy[x + i][y + k][z + j]);
				}
			}
		}
	}
	return Pair<int>(code, s->getId());	// ok
}

Pair<int> PhysicsModel::editShip(Ship* s, int x, int y, int z) {
	int code = 0;
	s->setPx(x);
	s->setPy(y);
	s->setPz(z);
	vector<vector<int> > structure = s->getStructure();
	s->resetField();
	for (unsigned int i = 0; i < structure.size(); i++) {
		for (unsigned int j = 0; j < structure[i].size(); j++) {
			for (int k = 0; k < structure[i][j]; k++) {
				if ((x + i >= cubeMy.size()) || (y + k >= cubeMy[x + i].size())
						|| (z + j >= cubeMy[x + i][y + k].size()) || (x + i < 0)
						|| (y + k < 0) || (z + j < 0)) {
					code = -1;		// ha kilóg a pályáról
				} else {
					cubeMy[x + i][y + k][z + j]->setShip(s);
					s->addField(cubeMy[x + i][y + k][z + j]);
				}
			}
		}
	}
	return Pair<int>(0, s->getId());	// ok
}

bool PhysicsModel::isValidShip(Ship* s) {
	int x = s->getPx(), y = s->getPy(), z = s->getPz();
	vector<vector<int> > structure = s->getStructure();

	for (unsigned int i = 0; i < structure.size(); i++) {
		for (unsigned int j = 0; j < structure[i].size(); j++) {
			for (int k = 0; k < structure[i][j]; k++) {
				if ((x + i >= cubeMy.size()) || (y + k >= cubeMy[x + i].size())
						|| (z + j >= cubeMy[x + i][y + k].size()) || (x + i < 0)
						|| (y + k < 0) || (z + j < 0)) {
					return false;		// ha kilóg a pályáról
				}
			}
		}
	}
	return true;
}

void PhysicsModel::addBomb(int _x, int _y, int _z) {
	cubeFoe[_x][_y][_z]->setDisruptive(true);
}

size_t PhysicsModel::getDimension() {
	return cubeMy.size();
}

bool PhysicsModel::fire(Message &m) {
	//INFO ha 4es akkor rám lőttek
	if (m.getMsgType() == FIRE) {
		cout << "*******" << endl;
		cout << m.getPosX() << " " << m.getPosY() << " " << m.getPosZ() << endl;
		cout << cubeMy[m.getPosX()][m.getPosY()][m.getPosZ()]->toString()
				<< endl;
		return cubeMy[m.getPosX()][m.getPosY()][m.getPosZ()]->fire();
	}
	//INFO ha 6os v. 7es akkor én lőttem
	else if (m.getMsgType() == FIREOK || m.getMsgType() == FIREBAD) {
		addBomb(m.getPosX(), m.getPosY(), m.getPosZ());
	}
	return false;
}

bool PhysicsModel::idead() {
	return myShips.size() == 0;
}

bool PhysicsModel::check() {
	for (int i = myShips.size() - 1; i >= 0; i--) {
		if (myShips[i]->isDead()) {
			myShips.erase(myShips.begin() + i);
		}
	}
	return false;
}

bool PhysicsModel::checkShip(Message &m) {
	return cubeMy[m.getPosX()][m.getPosY()][m.getPosZ()]->getHajo()->isDead();
}

void PhysicsModel::toString() {

	for (unsigned int i = 0; i < cubeMy.size(); i++) {
		for (unsigned int j = 0; j < cubeMy[i].size(); j++) {
			for (unsigned int k = 0; k < cubeMy[i][j].size(); k++) {
				cout << cubeMy[i][j][k]->toString() << " ";
			}
			cout << endl;
		}
		cout << "************************************************" << endl;
	}

}

} /* namespace AstrOWar */
