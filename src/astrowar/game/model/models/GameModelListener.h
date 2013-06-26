/*
 * GameModelListener.h
 *
 *  Created on: 2013.06.26.
 *      Author: quiga
 */

#ifndef GAMEMODELLISTENER_H_
#define GAMEMODELLISTENER_H_

namespace AstrOWar {

class GameModelListener {
public:
	virtual ~GameModelListener();

	/*
	 * lövés esetén x,y,z koordináták, true ha sikeres, false ha nem
	 */
	virtual void onFireEvent(int, int, int, bool, bool)=0;
	/*
	 * találat esetén: x,y,z koordináták, és true ha sikeres, false ha nem
	 */
	virtual void onHitEvent(int, int, int, bool, bool)=0;
	/*
	 * játékos halála esetén, true ha én, false ha az ellenfél
	 */
	virtual void onDeadEvent(bool)=0;
	/*
	 * játékos kilépése esetén
	 */
	virtual void onExitEvent()=0;
	/*
	 * hiba esetén, hibakóddal
	 */
	virtual void onErrorEvent(int)=0;

};

} /* namespace AstrOWar */
#endif /* GAMEMODELLISTENER_H_ */
