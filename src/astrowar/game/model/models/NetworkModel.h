/*
 * NetworkModel.h
 *
 *  Created on: 2013.06.23.
 *      Author: quiga
 */

#ifndef NetworkModel_H_
#define NetworkModel_H_

#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include <memory>
#include <iostream>
#include <vector>

using namespace sf;

namespace AstrOWar {

class GameModel;

class NetworkModel {
public:
	NetworkModel(GameModel *g);
	virtual ~NetworkModel();

	bool isEnableConnection() {
		return mSocketConnected;
	}

	void sendMessage(std::string msg);
	void registerMessageEventHandler(void (GameModel::*cb)(std::string));
	bool isServer();
	void setSocket(TcpSocket *_mSocket, bool _iss);

private:
	void init();
	void run();

	void listenForClient();
	void connectToServer();
	std::string reciveMessage();
	GameModel* gm;
	std::shared_ptr<sf::Thread> reciverThread;

	bool mThreadShouldEnd;
	bool mSocketConnected;
	bool _isServer;
	sf::TcpSocket *mSocket;
	void (GameModel::*getMessage)(std::string);

};
} /* namespace AstrOWar */

#endif /* NetworkModel_H_ */
