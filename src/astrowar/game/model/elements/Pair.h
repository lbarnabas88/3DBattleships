/*
 * Pair.h
 *
 *  Created on: 2013.06.26.
 *      Author: quiga
 */

#ifndef PAIR_H_
#define PAIR_H_

namespace AstrOWar {

template<typename T>
struct Pair {
	T resume;
	int errorCode;

	Pair(int error, T res) {
		errorCode = error;
		resume = res;
	}
};
}

#endif /* PAIR_H_ */
