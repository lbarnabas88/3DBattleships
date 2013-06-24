/*
 * Settings.hpp
 *
 *  Created on: 2013.01.23.
 *      Author: Baarnus
 */

#ifndef SETTINGS_T_HPP_
#define SETTINGS_T_HPP_

#include <memory>
#include <map>

#include <boost/program_options.hpp>

namespace app
{

class Settings
{
public:
	class SubSettings
	{
	public:
		virtual ~SubSettings();
	protected:
		virtual void onInit() = 0;
		// Add options
		void addOption(std::string name, std::string description);
		template<typename T>
		void addOption(std::string name, std::string description);
		template<typename T>
		void addOption(std::string name, std::string description, T defaultValue);
		// Option handling
		bool isOption(std::string name) const;
		template<typename T>
		T getOption(std::string name) const;
		template<typename T>
		T getDefault(std::string name) const;
		template<typename T>
		void setOption(std::string name, T value);
	private:
		Settings* mSettings;
		friend class Settings;
	};
	// Add subsetting
	template<typename SubSettingType>
	SubSettingType* createSubSetting();
private:
	std::vector<SubSettings*> mSubSettings;
public:
	// Constructor & Destructor
	Settings();
	virtual ~Settings();
	// Loaders
	void loadFromFile(std::string filename);
	void loadFromArgs(int argc, char** argv);
	// Add options
	template<typename T>
	void addOption(std::string name, std::string description);
	template<typename T>
	void addOption(std::string name, std::string description, T p_defaultValue);
	void addOption(std::string name, std::string description);
	// Is option exist
	bool isOption(std::string name) const;
	// Get option
	template<typename T>
	T getOption(std::string name) const;
	// Get default value for option
	template<typename T>
	T getDefault(std::string name) const;
	// Set option
	template<typename T>
	void setOption(std::string name, T value);
	// Pointer type
	typedef std::shared_ptr<Settings> Ptr;
	static Ptr getMainOptions();
private:
	boost::program_options::options_description m_settingsDescription;
	boost::program_options::variables_map m_settingsMap;
	boost::program_options::variables_map m_defaults;
};

//*** SUBSETTINGS ***/
template<typename T>
void Settings::SubSettings::addOption(std::string name, std::string description)
{
	mSettings->addOption<T>(name, description);
}

template<typename T>
void Settings::SubSettings::addOption(std::string name, std::string description, T defaultValue)
{
	mSettings->addOption<T>(name, description, defaultValue);
}

template<typename T>
T Settings::SubSettings::getOption(std::string name) const
{
	return mSettings->getOption<T>(name);
}

template<typename T>
T Settings::SubSettings::getDefault(std::string name) const
{
	return mSettings->getDefault<T>(name);
}

template<typename T>
void Settings::SubSettings::setOption(std::string name, T value)
{
	mSettings->setOption<T>(name, value);
}

//*** SETTINGS ***/
template<typename T>
void Settings::addOption(std::string name, std::string description)
{
	m_settingsDescription.add_options()(name.c_str(), boost::program_options::value<T>(), description.c_str());
}

template<typename T>
void Settings::addOption(std::string name, std::string description, T p_defaultValue)
{
	m_settingsDescription.add_options()(name.c_str(), boost::program_options::value<T>()->default_value(p_defaultValue), description.c_str());
	m_defaults.insert(
			std::pair<std::string, boost::program_options::variable_value>(name, boost::program_options::variable_value(boost::any(p_defaultValue), true)));
}

template<typename SubSettingType>
SubSettingType* Settings::createSubSetting()
{
	SubSettings* newSubSetting = new SubSettingType;
	newSubSetting->mSettings = this;
	newSubSetting->onInit();
	mSubSettings.push_back(newSubSetting);
	return (SubSettingType*) newSubSetting;
}

template<typename T>
T Settings::getOption(std::string name) const       // Get the value
{
	auto findIt = m_settingsMap.find(name);
	if (findIt != m_settingsMap.end())
	{
		try
		{
			return findIt->second.as<T>();
		}
		catch (...)
		{
			findIt = m_defaults.find(name);
			if (findIt != m_defaults.end())
				return findIt->second.as<T>();
		}
	}
	return T();
}

template<typename T>
T Settings::getDefault(std::string name) const
{
	auto fintIt = m_defaults.find(name);
	if (fintIt != m_defaults.end())
		return fintIt->second.as<T>();
	return T();
}

template<typename T>
void Settings::setOption(std::string name, T value)
{
	auto setIt = m_settingsMap.find(name);
	if (setIt != m_settingsMap.end())
		setIt->second = boost::program_options::variable_value(boost::any(value), true);
	else
	{
		m_settingsMap.insert(
				std::pair<std::string, boost::program_options::variable_value>(name, boost::program_options::variable_value(boost::any(value), true)));
		addOption(name, name, value);
	}
}

} /* namespace app */
#endif /* SETTINGS_T_HPP_ */
