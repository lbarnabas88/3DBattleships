/*
 * NetworkModel.cpp
 *
 *  Created on: 2013.06.23.
 *      Author: quiga
 */

#include "NetworkModel.h"

namespace AstrOWar {

NetworkModel::NetworkModel(GameModel* g) : getMessage(nullptr) {
	_isServer = false;
	gm = g;
	init();
}
NetworkModel::~NetworkModel() {
}

void NetworkModel::setSocket(TcpSocket *_mSocket, bool _iss){
	mSocket = _mSocket;
	mSocketConnected = true;
	_isServer = _iss;
	reciverThread.reset(new sf::Thread(&NetworkModel::run, this));
	reciverThread->launch();
}

bool NetworkModel::isServer(){
	return _isServer;
}

void NetworkModel::init() {
	mSocketConnected = false;
	mThreadShouldEnd = false;
}
/*
void NetworkModel::startAsServer(unsigned short port) {
	_isServer = true;
	mNetworkModelThread.reset(new sf::Thread(&NetworkModel::listenForClient, this));
	mAddress.second = port;
	mNetworkModelThread->launch();
}
*/
/*
void NetworkModel::listenForClient() {
	std::cout << "SERVER: " << "Server starts on port " << mAddress.second
			<< std::endl;
	sf::TcpListener listener;

	listener.setBlocking(false);
	listener.listen(mAddress.second);

	mThreadShouldEnd = false;
	while (!mThreadShouldEnd) {
		if (listener.accept(mSocket) == sf::Socket::Done) {
			std::cout << "SERVER: " << "Incoming client: "
					<< mSocket.getRemoteAddress() << std::endl;
			mShouldStartGame = true;
			mSocketConnected = true;
			mThreadShouldEnd = true;
		}
	}
	std::cout << "SERVER: " << "Server Listen Ended" << std::endl;
}

void NetworkModel::connectToServer() {
	std::cout << "CLIENT: " << "Connenting to " << mAddress.first << " on port "
			<< mAddress.second << std::endl;

	mThreadShouldEnd = false;
	while (!mThreadShouldEnd) {
		std::cout << "CONNECT " << std::endl;
		sf::Socket::Status status = mSocket.connect(mAddress.first,
				mAddress.second);
		if (status == sf::Socket::Done) {
			std::cout << "CLIENT: " << "Connected to server: "
					<< mSocket.getRemoteAddress() << std::endl;
			mShouldStartGame = true;
			mSocketConnected = true;
			mThreadShouldEnd = true;
			isListenReciver = true;
			//reciverThread.launch();
		}
	}
	std::cout << "Connenction Ended" << std::endl;

	while (isListenReciver) {
		std::string msg;
		sf::Packet packetReceive;
		mSocket.receive(packetReceive);
		if (packetReceive >> msg) {
			if (!msg.empty()) {
				if (getMessage != nullptr){
					(gm->*getMessage)(msg);
				}
			}
		}
	}
	std::cout << "Client Thread Ended" << std::endl;
}
*/
void NetworkModel::sendMessage(std::string msg) {
	if (mSocketConnected) {
		sf::Packet packetSend;
		packetSend << msg;
		mSocket->send(packetSend);
	}
}

void NetworkModel::run() {
	std::cout << "run start" << std::endl;

	while (!mThreadShouldEnd) {
			std::string msg;
			sf::Packet packetReceive;
			mSocket->receive(packetReceive);
			if (packetReceive >> msg) {
				if (!msg.empty()) {
					if (getMessage != nullptr){
						(gm->*getMessage)(msg);
					}
				}
			}
		}
}

void NetworkModel::registerMessageEventHandler(void (GameModel::*cb)(std::string)) {
	getMessage = cb;
}

} /* namespace AstrOWar */
