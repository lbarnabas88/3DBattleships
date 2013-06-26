/*
 * NetworkModel.cpp
 *
 *  Created on: 2013.06.23.
 *      Author: quiga
 */

#include "NetworkModel.h"
#include "../../../../tools/echoes.hpp"

namespace AstrOWar {

NetworkModel::NetworkModel(GameModel* g) :
		getMessage(nullptr) {
	_isServer = false;
	gm = g;
	init();
}
NetworkModel::~NetworkModel() {
}
void NetworkModel::setSocket(TcpSocket *_mSocket, bool _iss) {
	mSocket = _mSocket;
	mSocketConnected = true;
	_isServer = _iss;
	reciverThread.reset(new sf::Thread(&NetworkModel::run, this));
	reciverThread->launch();
}
bool NetworkModel::isServer() {
	return _isServer;
}
void NetworkModel::init() {
	mSocketConnected = false;
	mThreadShouldEnd = false;
}
bool NetworkModel::isEnableConnection() {
	return mSocketConnected;
}
void NetworkModel::sendMessage(std::string msg) {
	if (mSocketConnected) {
		sf::Packet packetSend;
		packetSend << msg;
		mSocket->send(packetSend);
	}
}
void NetworkModel::run() {
	echo("run start");
	while (!mThreadShouldEnd) {
		std::string msg;
		sf::Packet packetReceive;
		mSocket->receive(packetReceive);
		if (packetReceive >> msg) {
			if (!msg.empty()) {
				if (getMessage != nullptr) {
					(gm->*getMessage)(msg);
				}
			}
		}
	}
}
void NetworkModel::registerMessageEventHandler(
		void (GameModel::*cb)(std::string)) {
	getMessage = cb;
}

} /* namespace AstrOWar */
