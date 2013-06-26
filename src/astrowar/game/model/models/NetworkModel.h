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
private:
	GameModel* gm;

	bool mThreadShouldEnd;
	bool mSocketConnected;
	bool _isServer;
	sf::TcpSocket *mSocket;

	void (GameModel::*getMessage)(std::string);
	std::shared_ptr<sf::Thread> reciverThread;

protected:
	void init();
	void run();

	void listenForClient();
	void connectToServer();
	std::string reciveMessage();

public:
	NetworkModel(GameModel *g);
	virtual ~NetworkModel();

	bool isServer();
	bool isEnableConnection();

	void sendMessage(std::string msg);
	void setSocket(TcpSocket *_mSocket, bool _iss);
	void registerMessageEventHandler(void (GameModel::*cb)(std::string));
};
} /* namespace AstrOWar */

#endif /* NetworkModel_H_ */
