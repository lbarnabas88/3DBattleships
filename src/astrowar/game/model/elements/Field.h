/*
 * Field.h
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#ifndef FIELD_H_
#define FIELD_H_
#include "Ship.h"

namespace AstrOWar {
class Ship;
//INFO hajó halálának jelzése
class Field {
private:
	Ship *hajo;
	bool isDisruptive;	// felrobbantott-e
	// positions
	int posX;
	int posY;
	int posZ;

public:
	Field(int x, int y, int z);
	virtual ~Field();

	void setShip(Ship *s);
	void setHajo(Ship *hajo);
	void setDisruptive(bool isDisruptive);

	Ship* getHajo();
	int getPosX() const;
	int getPosY() const;
	int getPosZ() const;

	bool IsDisruptive();
	bool fire();

	void resetShip();
	std::string toString();
};

} /* namespace AstrOWar */
#endif /* FIELD_H_ */
