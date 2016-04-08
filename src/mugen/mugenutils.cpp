#include "mugenutils.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::istream & mugen::_getline(std::istream & __is, std::string & __str)
{
	std::istream & _is = std::getline(__is, __str);
	// Cut the line at the comments: find the ; character
	bool ignored = false;
	for (unsigned int index = 0; index < __str.size(); index++) {
		if (ignored) {
			if (__str[index] == '\"')
				ignored = false;
			continue;
		}
		// else
		switch (__str[index]) {
		case '"':
			ignored = true;
			break;
		case ';': // do not process comments
			__str = __str.substr(0, index);
			index = __str.size();
			break;
		}
	}
	return _is;
}

const std::regex mugen::MugenTextFile::regexSectionHeader("^[ \t]*\\[[ \t]*([^\\]]+?)[ \t]*\\][ \t\r]*$");
const std::regex mugen::MugenTextFile::regexKeyValue("^[ \t]*([^=]+?)[ \t]*=[ \t]*([^\r]+?)[ \t\r]*$");
const std::regex mugen::MugenTextFile::regexKeyQuotedValue("^[ \t]*([^=]+?)[ \t]*=[ \t]*\"([^\r\"]+?)\"[ \t\r]*$");

mugen::MugenTextFile::MugenTextFile(std::string path): m_path(path), m_inputstream(path)
{
	m_section = "";
}

mugen::MugenTextFile::~MugenTextFile()
{
	if (m_inputstream.is_open())
		m_inputstream.close();
}

const std::string & mugen::MugenTextFile::section() const
{
	return m_section;
}

const bool mugen::MugenTextFile::newSection() const
{
	return m_newSection;
}

const std::string mugen::MugenTextFile::nextLine()
{
	m_newSection = false;
	std::string s;
	mugen::_getline(m_inputstream, s);
	std::smatch sm;
	if (std::regex_match(s, sm, regexSectionHeader)) {
		m_section = sm[1];
		m_newSection = true;
	}
	return s;
}

mugen::MugenTextFile::operator bool() const
{
	return (bool) m_inputstream;
}

const mugen::MugenTextKeyValue mugen::MugenTextFile::nextValue()
{
	std::string line;
	m_newSection = false;
	while (mugen::_getline(m_inputstream, line)) {
		std::smatch sm;
		if (std::regex_match(line, sm, regexSectionHeader)) {
			m_section = sm[1];
			m_newSection = true;
		}
		else if (std::regex_match(line, sm, regexKeyQuotedValue)) {
			return MugenTextKeyValue(sm[1], sm[2]);
		}
		else if (std::regex_match(line, sm, regexKeyValue)) {
			return MugenTextKeyValue(sm[1], sm[2]);
		}
	}
	return MugenTextKeyValue();
}

mugen::MugenTextKeyValue::MugenTextKeyValue(): isEmpty(true)
{
}

mugen::MugenTextKeyValue mugen::MugenTextKeyValue::read(const std::string & stringToRead)
{
	std::smatch sm;
	if (std::regex_match(stringToRead, sm, MugenTextFile::regexKeyQuotedValue)) {
		return MugenTextKeyValue(sm[1], sm[2]);
	}
	else if (std::regex_match(stringToRead, sm, MugenTextFile::regexKeyValue)) {
		return MugenTextKeyValue(sm[1], sm[2]);
	}
	return MugenTextKeyValue();
}

mugen::MugenTextKeyValue::MugenTextKeyValue(std::string key, std::string value): k(key), v(value)
{
}

mugen::MugenTextKeyValue::MugenTextKeyValue(const mugen::MugenTextKeyValue & kvpair): k(kvpair.name()), v(kvpair.value())
{
}

mugen::MugenTextKeyValue::MugenTextKeyValue(mugen::MugenTextKeyValue && kvpair)
{
	std::swap(isEmpty, kvpair.isEmpty);
	if (!isEmpty) {
		std::swap(k, kvpair.k);
		std::swap(v, kvpair.v);
	}
}

mugen::MugenTextKeyValue & mugen::MugenTextKeyValue::operator=(mugen::MugenTextKeyValue && kvpair)
{
	std::swap(isEmpty, kvpair.isEmpty);
	if (!isEmpty) {
		std::swap(k, kvpair.k);
		std::swap(v, kvpair.v);
	}
	return *this;
}

mugen::MugenTextKeyValue & mugen::MugenTextKeyValue::operator=(const mugen::MugenTextKeyValue & kvpair)
{
	isEmpty = ! (bool) kvpair;
	if (!isEmpty) {
		k = kvpair.name();
		v = kvpair.value();
	}
	return *this;
}

mugen::MugenTextKeyValue::operator bool() const
{
	return !isEmpty;
}

const std::string & mugen::MugenTextKeyValue::name() const
{
	return k;
}

const std::string & mugen::MugenTextKeyValue::value() const
{
	return v;
}
