/*
 * CsvParser.cpp
 *
 *  Created on: 2012.12.02.
 *      Author: Baarnus
 */

#include "Parser.hpp"

#include <fstream>
#include <iterator>

using namespace std;

namespace csv
{

ParseDelegate::~ParseDelegate()
{
}

// Parser class
class Parser
{
public:
	// Constructor: only the static functions can create a parser
	Parser(string docName, ParseDelegate::Ptr pDelegate, char delimiter) :
			m_delegate(pDelegate), m_state(Read), m_delimiter(delimiter), m_needNewLine(true), m_lineNumber(0), m_fieldNumber(0), m_docName(docName)
	{
	}
	// Perse with iterators
	template<typename Iter>
	bool parse(Iter iBegin, Iter iEend)
	{
		if (!m_delegate)
			return false;

		m_delegate->onStartDocument(m_docName);

		for (auto it = iBegin; it != iEend; ++it)
			parseChar(*it);

		if (m_actField.length() > 0)
		{
			sendActField();
			sendEndLine();
		}

		m_delegate->onEndDocument(m_docName);

		return true;
	}
private:
	// States for the state machine
	enum ParserState
	{
		Read, Infield, Backslash, InfieldBackslash
	};
	// Delegate pointer
	ParseDelegate::Ptr m_delegate;
	// The current state
	ParserState m_state;
	// The delimiter
	char m_delimiter;
	// Need new line at the next char
	bool m_needNewLine;
	// The actual cell content
	std::string m_actField;
	// Position of the cell in the table
	int m_lineNumber, m_fieldNumber;
	// Name of the parsed document
	string m_docName;

	// Helper functions
	// Sender functions
	void sendStartLine()
	{
		m_delegate->onStartLine(m_lineNumber);
		m_needNewLine = false;
		m_fieldNumber = 0;
	}
	void sendActField()
	{
		m_delegate->onReadField(m_actField, m_fieldNumber);
		m_fieldNumber++;
		m_actField.clear();
	}
	void sendEndLine()
	{
		m_delegate->onEndLine(m_lineNumber);
		m_lineNumber++;
		m_needNewLine = true;
	}

	// State handler fuctions
	void handleStateRead(const char &c)
	{
		if (c == m_delimiter)
		{
			if (m_needNewLine)
				sendStartLine();
			sendActField();
		}
		else if (c == '\n')
		{
			if (!m_needNewLine)
			{
				sendActField();
				sendEndLine();
			}
		}
		else if (c == '\"')
		{
			m_state = Infield;
		}
		else if (c == '\\')
		{
			m_state = Backslash;
		}
		else
		{
			if (m_needNewLine)
				sendStartLine();
			m_actField += c;
		}
	}
	void handleStateInfield(const char &c)
	{
		if (c == '"')
		{
			m_state = Read;
		}
		else if (c == '\\')
		{
			m_state = InfieldBackslash;
		}
		else
		{
			m_actField += c;
		}
	}
	void handleStateBackslash(const char &c)
	{
		if (c == '"')
		{
			m_actField += c;
			m_state = Read;
		}
		else
		{
			m_actField += '\\';
			m_actField += c;
			m_state = Read;
		}
	}
	void handleStateInfieldBackslash(const char &c)
	{
		if (c == '"')
		{
			m_actField += c;
			m_state = Infield;
		}
		else
		{
			m_actField += '\\';
			m_actField += c;
			m_state = Infield;
		}
	}

	// State separator function
	void parseChar(const char &c)
	{
		switch (m_state)
		{
		case Read:
			handleStateRead(c);
			break;
		case Infield:
			handleStateInfield(c);
			break;
		case Backslash:
			handleStateBackslash(c);
			break;
		case InfieldBackslash:
			handleStateInfieldBackslash(c);
			break;
		default:
			break;
		}
	}
};

bool parseFile(std::string filename, ParseDelegate::Ptr pDelegate, char delimiter)
{
	ifstream fin(filename.c_str());
	if (!fin.is_open())
		return false;
	fin.unsetf(ios::skipws);

	Parser parser(filename, pDelegate, delimiter);
	bool result = parser.parse(istream_iterator<char>(fin), istream_iterator<char>());

	fin.close();

	return result;
}
bool parseString(std::string content, ParseDelegate::Ptr pDelegate, char delimiter)
{
	Parser parser("-STRING-", pDelegate, delimiter);
	return parser.parse(content.begin(), content.end());
}

}       // namespace csv
