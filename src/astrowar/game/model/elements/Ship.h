/*
 * Ship.h
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#ifndef SHIP_H_
#define SHIP_H_
#include <vector>
#include <string>
#include <iostream>
using namespace std;

#include "Field.h"

/*
 * a hajó akkor robban fel, ha a mezőinek legalább _G százalékát eltalálták.
 */

#define _G 0.4

namespace AstrOWar {
class Field;

class Ship {
protected:
	std::string name;
	int id;
	int x, y, z;		// mátrix segédek
	int px, py, pz;		// positions
	int type;
	int max;
	std::string mesh;
	std::vector<std::vector<int> > structure;
	std::vector<Field*> mezok;
	Ship* _clone(Ship *s, int _id);

public:
	Ship();
	virtual ~Ship();

	void setName(std::string str);
	void setId(int i);
	void setX(int i);
	void setY(int i);
	void setZ(int i);
	void setMax(int i);
	void setType(int i);
	void setMesh(std::string str);
	void setStructure(vector<vector<int> > v);
	void addField(Field* f);

	vector<vector<int> > getStructure();
	std::string getName();
	std::string getMesh();
	int getId();
	int getX();
	int getY();
	int getZ();
	int getMax();
	int getType();
	bool isNew();
	bool isDead();
	void resetField();
	Ship* clone(int id);
	void toString();

	int getPx();
	void setPx(int px);
	int getPy();
	void setPy(int py);
	int getPz();
	void setPz(int pz);
};

} /* namespace AstrOWar */
#endif /* SHIP_H_ */
