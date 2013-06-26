/*
 * Field.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "Field.h"

namespace AstrOWar {

Field::Field(int x, int y, int z) {
	posX = x;
	posY = y;
	posZ = z;
	isDisruptive = false;
	hajo = nullptr;
}

Field::~Field() {
}

void Field::setShip(Ship *s) {
	hajo = s;
}

std::string Field::toString() {
	return (hajo == nullptr) ? "0" : "1";
}

Ship* Field::getHajo() {
	return hajo;
}

void Field::setHajo(Ship* hajo) {
	this->hajo = hajo;
}

bool Field::IsDisruptive() {
	return isDisruptive;
}

void Field::setDisruptive(bool isDisruptive) {
	this->isDisruptive = isDisruptive;
}

int Field::getPosX() const {
	return posX;
}

int Field::getPosY() const {
	return posY;
}

int Field::getPosZ() const {
	return posZ;
}

bool Field::fire() {
	isDisruptive = true;
	return hajo != nullptr;
}

void Field::resetShip() {
	hajo = nullptr;
}

} /* namespace AstrOWar */
