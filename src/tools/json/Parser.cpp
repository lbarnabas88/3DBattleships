/*
 * Parser.cpp
 *
 *  Created on: 2013.06.22.
 *      Author: quiga
 */

#include "Parser.h"
#include <iostream>

namespace JSON {

Parser::Parser() {
	root = nullptr;
}

Parser::~Parser() {
}

/**
 *	Visszatérési értékek:
 *		0, ha sikeres
 *		1, ha a json fájl nem megfelelő
 *		2, ha a json file nemtalálható, vagy sikertelen a beolvasás
 */
int Parser::parseShip(std::string path, AstrOWar::Ship &h) {
	root = json_load_file(path.c_str(), 0, &error);

	if (!root)
		return 2;

	json_t *name, *x, *y, *z, *mesh, *size, *minta, *type, *max;

	name = json_object_get(root, "name");
	if (!json_is_string(name))
		return 1;

	type = json_object_get(root, "type");
	if (!json_is_integer(type))
		return 1;

	max = json_object_get(root, "max");
	if (!json_is_integer(max))
		return 1;

	size = json_object_get(root, "size");
	if (!json_is_object(size))
		return 1;

	x = json_object_get(size, "x");
	if (!json_is_integer(x))
		return 1;

	y = json_object_get(size, "y");
	if (!json_is_integer(y))
		return 1;

	z = json_object_get(size, "z");
	if (!json_is_integer(z))
		return 1;

	minta = json_object_get(root, "structure");
	if (!json_is_array(minta))
		return 1;

	mesh = json_object_get(root, "mesh");
	if (!json_is_string(mesh))
		return 1;

	h.setName(json_string_value(name));
	h.setX(json_integer_value(x));
	h.setY(json_integer_value(y));
	h.setZ(json_integer_value(z));
	h.setMesh(json_string_value(mesh));
	h.setType(json_integer_value(type));
	h.setMax(json_integer_value(max));

	int _x = 0, _z = 0;
	_x = json_integer_value(x);
	_z = json_integer_value(z);

	std::vector<std::vector<int> > v;
	v.resize(_x, std::vector<int>(_z, 0));

	for (int i = 0; i < _x * _z; i++) {
		int __x = i % _x;
		int __z = (i - __x) / _x;
		int __y = json_integer_value(json_array_get(minta, i));
		v[__x][__z] = __y;
	}

	h.setStructure(v);

	json_decref(root);
	return 0;
}

int Parser::parse(std::string path, std::vector<AstrOWar::Ship> *ships) {
	int code = 0;
	json_t *Root = json_load_file(path.c_str(), 0, &error);
	if (!Root)
		return 2;
	json_t* lista = json_object_get(Root, "ships");

	for (unsigned int i = 0; i < json_array_size(lista) && code == 0; i++) {
		AstrOWar::Ship s;
		json_t *info = json_object_get(json_array_get(lista, i), "info");
		if (!json_is_string(info))
			return 1;
		code = parseShip("data/ships/" + std::string(json_string_value(info)),
				s);
		ships->push_back(s);
	}
	json_decref(lista);
	json_decref(Root);
	return code;
}

AstrOWar::Message Parser::decode(std::string json) {
	AstrOWar::Message msg;

	json_t* Root = json_loads(json.c_str(), 0, &error);
	if (Root) {
		json_t *msgType, *x, *y, *z, *pos, *id, *refId;
		id = json_object_get(Root, "id");
		refId = json_object_get(Root, "refId");
		msgType = json_object_get(Root, "msgType");
		pos = json_object_get(Root, "pos");
		x = json_array_get(pos, 0);
		y = json_array_get(pos, 1);
		z = json_array_get(pos, 2);

		if (json_is_integer(msgType) && json_is_array(pos)
				&& json_is_integer(id) && json_is_integer(refId)
				&& json_is_integer(x) && json_is_integer(y)
				&& json_is_integer(z)) {

			int _id = json_integer_value(id);
			int _refId = json_integer_value(refId);
			int _msgType = json_integer_value(msgType);
			int _x = json_integer_value(x);
			int _y = json_integer_value(y);
			int _z = json_integer_value(z);

			msg.init(_msgType, _x, _y, _z, _id, _refId);
		}
	}
	return msg;
}

std::string Parser::encode(AstrOWar::Message msg) {
	json_t *Root, *pos;

	Root = json_object();
	pos = json_array();

	json_array_append(pos, json_integer(msg.getPosX()));
	json_array_append(pos, json_integer(msg.getPosY()));
	json_array_append(pos, json_integer(msg.getPosZ()));

	json_object_set_new(Root, "id", json_integer(msg.getId()));
	json_object_set_new(Root, "refId", json_integer(msg.getRefId()));
	json_object_set_new(Root, "msgType", json_integer(msg.getMsgType()));
	json_object_set_new(Root, "pos", pos);

	return std::string(json_dumps(Root, 0));
}

Parser& Parser::getSingleton() {
	static Parser instance;
	return instance;
}

Parser& JSONSingleton = Parser::getSingleton();
} /* namespace JSON */
