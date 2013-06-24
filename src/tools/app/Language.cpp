/*
 * Language.cpp
 *
 *  Created on: 2013.01.23.
 *      Author: Baarnus
 */

#include "Language.hpp"

#include "../utils.hpp"
#include "../csv.h"

namespace app
{

class LanguageCsvParseDelegate: public csv::ParseDelegate
{
public:
	LanguageCsvParseDelegate(std::map<std::string, std::string> &p_dictionary, std::string p_langCode,
			std::list<std::pair<std::string, std::string> > &p_languages) :
			m_dictionary(p_dictionary), m_languages(p_languages), m_langCode(p_langCode), m_state(Header), m_langColumn(0)
	{
	}
	// Run once at the begining of the process
	void onStartDocument(std::string p_title)
	{
	}
	// Run on every line start
	void onStartLine(int p_lineNumber)
	{
		if (p_lineNumber == 0)
			m_state = Header;
		else if (p_lineNumber > 0)
			m_state = CodeLine;
	}
	// Run on every cell
	void onReadField(std::string p_field, int p_fieldNumber)
	{
		if (m_state == Header)
		{
			if (p_fieldNumber > 0)
			{
				m_langCodes.push_back(p_field);
				if (p_field == m_langCode)
					m_langColumn = p_fieldNumber;
			}
		}
		else if (m_state == LangName)
		{
			m_langNames.push_back(p_field);
		}
		else if (m_state == CodeLine)
		{
			if (p_fieldNumber == 0)
			{
				if (p_field == "main.localname")
				{
					m_state = LangName;
				}
				else
				{
					m_code = p_field;
				}
			}
			else
			{
				if (p_fieldNumber == m_langColumn)
				{
					m_dictionary[m_code] = p_field;
				}
			}
		}
	}
	// Run on every line end
	void onEndLine(int p_lineNumber)
	{
	}
	// Run once at the end of the process
	void onEndDocument(std::string p_title)
	{
		// Lang codes and names to languages
		for (auto i = m_langCodes.begin(), j = m_langNames.begin(); i != m_langCodes.end() && j != m_langNames.end(); ++i, ++j)
		{
			m_languages.push_back(std::pair<std::string, std::string>(*i, *j));
		}
	}
private:
	// INPUT/OUTPUT
	// Dictionary reference
	std::map<std::string, std::string> &m_dictionary;
	// Language list reference
	std::list<std::pair<std::string, std::string> > &m_languages;
	// The language to load
	std::string m_langCode;
	// HELPERS
	// States
	enum State
	{
		Header, LangName, CodeLine
	};
	State m_state;
	// The code of the line
	std::string m_code;
	std::string m_text;
	int m_langColumn;
	// Temporary stores for lang codes and names
	std::list<std::string> m_langCodes;
	std::list<std::string> m_langNames;
};

// Load texts from a csv file
bool Language::loadFromFile(std::string p_filename)
{
	return csv::parseFile(p_filename, csv::ParseDelegate::Ptr(new LanguageCsvParseDelegate(m_dictionary, m_actLangCode, m_languages)), '\t');
}

// Load texts from a csv formatted string
bool Language::loadFromString(std::string p_content)
{
	return csv::parseString(p_content, csv::ParseDelegate::Ptr(new LanguageCsvParseDelegate(m_dictionary, m_actLangCode, m_languages)), '\t');
}

// Get a text to a code
std::string Language::textForCode(std::string p_code) const
{
	auto fintIt = m_dictionary.find(p_code);
	if (fintIt != m_dictionary.end())
		return fintIt->second;
	return "<!" + p_code + "!>";
}

// Get a wtext to a code
std::wstring Language::wtextForCode(std::string p_code) const
{
	return utils::utf8ToWString(textForCode(p_code));
}

// Get lists of languages
const std::list<std::pair<std::string, std::string> > Language::getLanguageList() const
{
	return m_languages;
}

// Check if there is translation for lang code
bool Language::isTranslationExtist(std::string code) const
{
	for (auto lang : m_languages)
		if (lang.first == code)
			return true;
	return false;
}

std::string Language::getLangName(std::string code) const
{
	for (auto lang : m_languages)
		if (lang.first == code)
			return lang.second;
	return "";
}

std::string Language::getActLangCode() const
{
	return m_actLangCode;
}

void Language::setLangCode(const std::string actLangCode)
{
	m_actLangCode = actLangCode;
}

} /* namespace app */
