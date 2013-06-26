/*
 * Ship.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "Ship.h"
#include "../../../../tools/echoes.hpp"
#include "../../../../tools/utils.hpp"

namespace AstrOWar {

Ship::Ship() {
	x = y = z = id = max = px = py = pz = 0;
	type = 0;
}

Ship* Ship::_clone(Ship *s, int _id) {
	setName(s->getName());
	setX(s->getX());
	setY(s->getY());
	setZ(s->getZ());
	setMesh(s->getMesh());
	setStructure(s->getStructure());
	id = _id;
	setType(s->getType());
	return this;
}

Ship::~Ship() {
}

void Ship::setName(std::string str) {
	name = str;
}
void Ship::setId(int i) {
	id = i;
}
void Ship::setX(int i) {
	x = i;
}
void Ship::setY(int i) {
	y = i;
}
void Ship::setZ(int i) {
	z = i;
}
void Ship::setMesh(std::string str) {
	mesh = str;
}

void Ship::setStructure(vector<vector<int> > v) {
	structure = v;
}

Ship* Ship::clone(int id) {
	return (new Ship())->_clone(this, id);
}

vector<vector<int> > Ship::getStructure() {
	return structure;
}
std::string Ship::getName() {
	return name;
}
int Ship::getId() {
	return id;
}
int Ship::getX() {
	return x;
}
int Ship::getY() {
	return y;
}
int Ship::getZ() {
	return z;
}
std::string Ship::getMesh() {
	return mesh;
}
int Ship::getType() {
	return type;
}
int Ship::getMax() {
	return max;
}
void Ship::setMax(int i) {
	max = i;
}
void Ship::setType(int i) {
	type = i;
}

void Ship::toString() {
	cout << name << endl;
	for (unsigned int i = 0; i < structure.size(); i++) {
		for (unsigned int j = 0; j < structure[i].size(); j++)
			cout << structure[i][j] << " ";
		cout << endl;
	}
}

void Ship::addField(Field* f) {
	mezok.push_back(f);
}

bool Ship::isNew() {
	return mezok.size() == 0;
}

bool Ship::isDead() {
	double i = mezok.size();
	double j = 0;
	for (Field* f : mezok)
		if (f->IsDisruptive())
			j++;
	echo("Ship::IsDead() return " + ((i * _G) >= j)?"true":"false");
	echo("Ship::IsDead()::i is " + utils::t2str(i) );
	echo("Ship::IsDead()::j is " + utils::t2str(j) );
	return ((i * _G) <= j);
}

void Ship::resetField() {
	for (Field* f : mezok) {
		f->resetShip();
	}
	mezok.resize(0);
}
int Ship::getPx() {
	return px;
}

void Ship::setPx(int px) {
	this->px = px;
}

int Ship::getPy() {
	return py;
}

void Ship::setPy(int py) {
	this->py = py;
}

int Ship::getPz() {
	return pz;
}

void Ship::setPz(int pz) {
	this->pz = pz;
}
} /* namespace AstrOWar */
