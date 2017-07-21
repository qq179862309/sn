#pragma once
#include <map>

namespace SN
{
#define UsingSTL std::map
	using Section = UsingSTL<std::string, std::string>;
	using iniFormat = UsingSTL<std::string, Section>;

	class SNAPI CIni
	{
	public:
		CIni();
		void setIni(CIni& ini);
	protected:
		~CIni();

		void resetValues();
		bool findSection(const std::string& section) const;

		iniFormat values;

		CIni(const CIni&) = delete;
		CIni(const CIni*) = delete;
		CIni& operator=(const CIni&) = delete;
	};

	class SNAPI CReaderIni : public CIni
	{
	public:
		using CIni::CIni;

		bool readIniFile(const std::string& filename);

		template<class T>
		T get(const std::string& section, const std::string& key, const T& default_value) const;

		const Section* const getSection(const std::string& section) const;
	private:
		template<class T>
		T parseValue(const std::string& value, const T& default_value) const;
		void parse(std::ifstream& s);
	};

	class SNAPI CWriterIni : public CIni
	{
	public:
		using CIni::CIni;

		bool writeIniFile(const std::string& filename);

		bool setSection(const std::string& section);
		bool removeSection(const std::string& section);
		bool remove(const std::string& section, const std::string& key);

		template<class T = std::string>
		bool set(const std::string& section, const std::string& key, const T& value);

		bool sectionSpaceEnabled = true;
	};

	template<class T>
	inline T CReaderIni::get(const std::string & section, const std::string & key, const T& default_value) const
	{
		auto sec = this->getSection(section);

		if (sec != nullptr && this->findSection(section))

			if (sec->find(key) != sec->end())
				return parseValue<T>(sec->at(key), default_value);
		return default_value;
	}

	template<class T>
	inline T CReaderIni::parseValue(const std::string& value, const T& default_value) const
	{
		T ret;
		std::stringstream ss(value);
		if (ss >> ret)
			return std::move(ret);
		else
			return default_value;
	}

	template<class T>
	inline bool CWriterIni::set(const std::string & section, const std::string & key, const T & value)
	{
		std::string ret;
		std::stringstream ss;
		ss << value;
		if (ss >> ret) {
			this->setSection(section);
			this->values[section].insert(std::make_pair(key, ret));
			return true;
		}
		else
			return false;
	}
}
