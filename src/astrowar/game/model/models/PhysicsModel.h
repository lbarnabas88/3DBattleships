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

#include <iostream>
using namespace std;

namespace AstrOWar {

class GameModel;

class PhysicsModel {
protected:
	std::vector<std::vector<std::vector<Field*> > > cubeMy;
	std::vector<std::vector<std::vector<Field*> > > cubeFoe;
	std::vector<Ship*> myShips;
	std::vector<Ship*> foeShips;
	std::vector<Ship*> myShipsAll;
	GameModel* gm;
	void destroy();
public:
	PhysicsModel(GameModel *g);
	virtual ~PhysicsModel();
	void init(int n);
	Pair<int> addShip(Ship* s, int _x, int _y, int _z);
	Pair<int> editShip(Ship* s, int _x, int _y, int _z);
	void addBomb(int _x, int _y, int _z);
	void toString();
	size_t getDimension();
	bool fire(Message &m);
	bool idead();
	bool check();
	bool checkShip(Message &m);
	std::vector<Ship*> getShips(){return myShips;}
	int getShipWithPosition(int x, int y, int z);
	Ship* getShipObjectWithPosition(int x, int y, int z);
	bool isValidShip(Ship* s);
	void addShipToFoe(Ship* s, int _x, int _y, int _z);
};

} /* namespace AstrOWar */
#endif /* PHYSICSMODEL_H_ */
