/*
 * Message.cpp
 *
 *  Created on: 2013.06.25.
 *      Author: quiga
 */

#include "Message.h"

namespace AstrOWar {

Message::Message() :
		msgType(0), posX(0), posY(0), posZ(0), id(0), refId(0) {
}

Message::~Message() {
}
Message Message::init(int _msgType, int _posX, int _posY, int _posZ, int _id,
		int _refId) {
	msgType = _msgType;
	posX = _posX;
	posY = _posY;
	posZ = _posZ;
	id = _id;
	refId = _refId;
	return (*this);
}

Message Message::init(int _msgType, int _id, int _refId) {
	msgType = _msgType;
	posX = posY = posZ = 0;
	id = _id;
	refId = _refId;
	return (*this);
}

int Message::getMsgType() {
	return msgType;
}

int Message::getPosX() {
	return posX;
}

int Message::getPosY() {
	return posY;
}

int Message::getPosZ() {
	return posZ;
}

int Message::getId() {
	return id;
}

int Message::getRefId() {
	return refId;
}

bool Message::validate() {
	return msgType != 0;
}
bool Message::validate(int n) {
	if (posX >= 0 && posX < n && posY >= 0 && posY < n && posZ >= 0
			&& posZ < n)
		return true;
	return false;
}

} /* namespace AstrOWar */
