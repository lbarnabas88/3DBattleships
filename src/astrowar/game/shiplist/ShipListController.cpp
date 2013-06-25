/*
 * ShipListController.cpp
 *
 *  Created on: 2013.06.15.
 *      Author: Baarnus
 */

#include "ShipListController.hpp"
// Settings
#include "../../../game/settings/GameSettings.hpp"
// CEGUI
#include "../../../graphics/Cegui/CeguiTranslator.hpp"
// ECHO
#include "../../../tools/echoes.hpp"

//*****************************************************
//*** Ship List Controller Data Provider
//*****************************************************

ShipListControllerDataProvider::~ShipListControllerDataProvider()
{
}

//*****************************************************
//*** Ship List Controller
//*****************************************************

ShipListController::ShipListController() :
		mColumnList(NULL), mDataProvider(NULL), mPlayer(0)
{
}

ShipListController::~ShipListController()
{
}

CEGUI::MultiColumnList* ShipListController::getColumnList() const
{
	return mColumnList;
}

void ShipListController::setColumnList(CEGUI::MultiColumnList* columnList)
{
	mColumnList = columnList;
	if (mColumnList)
	{
		mColumnList->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, CEGUI::Event::Subscriber(&ShipListController::onSelectionChanged, this));
	}
}

ShipListControllerDataProvider* ShipListController::getDataProvider() const
{
	return mDataProvider;
}

void ShipListController::setDataProvider(ShipListControllerDataProvider* dataProvider)
{
	mDataProvider = dataProvider;
}

// Build
void ShipListController::build()
{
	// If nil do nothing
	if (!mColumnList)
		return;
	// Clear
	clear();
	// Add columns
	mColumnList->addColumn(mDataProvider->getShipColumnName(), 0, CEGUI::UDim(0.7, 0));
	mColumnList->addColumn(mDataProvider->getQuantityColumnName(), 1, CEGUI::UDim(0.2, 0));
	// Set selection mode
	mColumnList->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	// Add cells
	if (!mDataProvider)
		return;
	for (size_t i = 0; i < mDataProvider->numOfShipType(mPlayer); ++i)
	{
		// Add row
		mColumnList->addRow(i);
		// Create cell for name
		CEGUI::ListboxTextItem* item;
		item = new CEGUI::ListboxTextItem(mDataProvider->getShipTypeName(mPlayer, i), i);
		if (mDataProvider->needSelection())
			item->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		mColumnList->setItem(item, 0, i);
		// Select 1st
		if (i == 0)
			mColumnList->setItemSelectState(item, true);
		// Create cell for count
		item = new CEGUI::ListboxTextItem(utf8ToCeguiString(utils::t2str(mDataProvider->getShipTypeCount(mPlayer, i))), i);
		if (mDataProvider->needSelection())
			item->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		mColumnList->setItem(item, 1, i);
		// Select 1st
		if (i == 0)
			mColumnList->setItemSelectState(item, true);
	}
	// Set not sortable
	mColumnList->setUserSortControlEnabled(false);
}
// Clear
void ShipListController::clear()
{
	// If nil do nothing
	if (!mColumnList)
		return;
	// Echo
	echof("Columns: %d, Rows: %d", mColumnList->getRowCount(), mColumnList->getRowCount());
	// Clear all column
	while (mColumnList->getRowCount() > 0)
		mColumnList->removeRow(0);
	while (mColumnList->getColumnCount() > 0)
		mColumnList->removeColumn(0);
	mColumnList->resetList();
}

// Upate
void ShipListController::update()
{
	if (!mColumnList || !mDataProvider)
		return;
	echof("Rows: %d", mColumnList->getRowCount());
	for (unsigned i = 0; i < mColumnList->getRowCount(); ++i)
	{
		auto row = mColumnList->getItemAtGridReference(CEGUI::MCLGridRef(i, 1));
		echof("RowId: %d, ColumnId: %d, Row: %p", i, 1, row);
		if (row)
			row->setText(utf8ToCeguiString(utils::t2str(mDataProvider->getShipTypeCount(mPlayer, i))));
	}
	mColumnList->invalidate(true);
}

// Set/Get player
int ShipListController::getPlayer() const
{
	return mPlayer;
}

void ShipListController::setPlayer(const int player)
{
	mPlayer = player;
}

// CEGUI listeners
bool ShipListController::onSelectionChanged(const CEGUI::EventArgs& args)
{
	if (mDataProvider && mColumnList)
	{
		mDataProvider->onSelectionChange(mPlayer, mColumnList->getFirstSelectedItem()->getID());
	}
	return true;
}
