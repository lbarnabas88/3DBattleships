/*
 * Parser.h
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#ifndef PARSER_H_
#define PARSER_H_
#include "jansson.h"
#include "../../astrowar/game/model/elements/Ship.h"
#include "../../astrowar/game/model/elements/Message.h"
#include <string>
#include <vector>


namespace JSON {

class Parser {
private:
	json_error_t error;
	json_t *root;
protected:
	Parser();
	virtual ~Parser();
	int parseShip(std::string path, AstrOWar::Ship &h);
public:
	int parse(std::string path, std::vector<AstrOWar::Ship> *ships);
	AstrOWar::Message decode(std::string json);
	std::string encode(AstrOWar::Message msg);
	static Parser& getSingleton();
};
extern Parser& JSONSingleton;
} /* namespace JSON */
#endif /* PARSER_H_ */
