/*
 * CeguiTranslator.cpp
 *
 *  Created on: 2013.06.13.
 *      Author: Baarnus
 */

#include "CeguiTranslator.hpp"
#include "../../game/settings/GameSettings.hpp"

void translateCeguiWindow(CEGUI::Window* ceguiWindow, bool isRecursive)
{
	// If null return
	if (!ceguiWindow)
		return;
	// Translate
	CEGUI::String text = ceguiWindow->getText();
	if (text.length() > 0 && text[0] == '@')
	{
		std::string code = ceguiStringToUtf8(text.substr(1, text.length() - 1));
		ceguiWindow->setText(utf8ToCeguiString(GameSettingsSingleton.getLanguage().textForCode(code)));
	}
	// Recursive for all children
	if (isRecursive)
		for (size_t i = 0; i < ceguiWindow->getChildCount(); ++i)
			translateCeguiWindow(ceguiWindow->getChildAtIdx(i));
}
