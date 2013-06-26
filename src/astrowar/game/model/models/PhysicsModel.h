/*
 * physicsModel.h
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#ifndef PHYSICSMODEL_H_
#define PHYSICSMODEL_H_
#include <vector>
#include "../elements/Field.h"
#include "../elements/Message.h"
#include "../elements/Pair.h"
#include "../../../../tools/utils.hpp"

#include <iostream>
#include <fstream>
using namespace std;

namespace AstrOWar {

class GameModel;

class PhysicsModel {
private:
	std::vector<std::vector<std::vector<Field*> > > cubeMy;
	std::vector<std::vector<std::vector<Field*> > > cubeFoe;
	std::vector<Ship*> myShips;
	std::vector<Ship*> foeShips;
	std::vector<Ship*> myShipsAll;
	GameModel* gm;
protected:
	void destroy();
public:
	PhysicsModel(GameModel *g);
	virtual ~PhysicsModel();

	void dump();
	void toString();
	void init(int n);
	void addBomb(int _x, int _y, int _z);
	void addShipToFoe(Ship* s, int _x, int _y, int _z);

	Pair<int> addShip(Ship* s, int _x, int _y, int _z);
	Pair<int> editShip(Ship* s, int _x, int _y, int _z);

	int getShipWithPosition(int x, int y, int z);
	Ship* getShipObjectWithPosition(int x, int y, int z);
	std::vector<Ship*> getShips();
	size_t getDimension();

	bool fire(Message &m);
	bool idead();
	bool check();
	bool checkShip(Message &m);
	bool isValidShip(Ship* s);
};

} /* namespace AstrOWar */
#endif /* PHYSICSMODEL_H_ */
