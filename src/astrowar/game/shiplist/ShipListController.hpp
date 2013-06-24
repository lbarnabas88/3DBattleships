/*
 * ShipListController.hpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#ifndef SHIPLISTCONTROLLER_HPP_
#define SHIPLISTCONTROLLER_HPP_

// CEGUI
#include <CEGUI/CEGUI.h>

class ShipListController;

class ShipListControllerDataProvider
{
public:
	virtual ~ShipListControllerDataProvider();
	// Data
	virtual size_t numOfShipType(int player) = 0;
	virtual CEGUI::String getShipTypeName(int player, size_t i) = 0;
	virtual size_t getShipTypeCount(int player, size_t i) = 0;
	virtual void onSelectionChange(int player, unsigned selection) = 0;
};

class ShipListController
{
public:
	// Create/Destroy
	ShipListController();
	virtual ~ShipListController();
	// Setter / Getter
	CEGUI::MultiColumnList* getColumnList() const;
	void setColumnList(CEGUI::MultiColumnList* columnList);
	ShipListControllerDataProvider* getDataProvider() const;
	void setDataProvider(ShipListControllerDataProvider* dataProvider);
	// Build
	void buildForSet();
	void buildForBattle();
	// Clear
	void clear();
	// Upate
	void update();
	// Set/Get player
	int getPlayer() const;
	void setPlayer(const int player);
	// CEGUI listeners
	bool onSelectionChanged(const CEGUI::EventArgs& args);
private:
	// Controllable element
	CEGUI::MultiColumnList* mColumnList;
	// Data provider
	ShipListControllerDataProvider* mDataProvider;
	// Player
	int mPlayer;
};

#endif /* SHIPLISTCONTROLLER_HPP_ */
