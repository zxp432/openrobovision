//
// OpenRoboVision
//
// simple parser for configuration files
//
// robocraft.ru
//

#include "orv/system/config_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

ConfigParser::ConfigParser()
{
}

ConfigParser::ConfigParser(const char* file_name)
{
	if(file_name)
	{
		config_file_name = file_name;
		parse_file( config_file_name.c_str() );
	}
}

ConfigParser::~ConfigParser()
{
	values.clear();
}

// parse configuration file 
// and put pairs into values (vector of parser_config_item)
int ConfigParser::parse_file(const char* file_name)
{
	if(!file_name)
		return -1;

	config_file_name = file_name;

	std::ifstream file;
	file.open(config_file_name.c_str());

	if (!file || file.is_open() != true)
	{
		printf("[!][ConfigParser] Error:  Cannot open file %s !\n", config_file_name.c_str());
		return -1;
	}

	std::string data;

	// all strings of file
	while (!file.eof())
	{
		parser_config_item item;

		// get line
		std::getline(file, data);

		// if blank
		if(data=="") 
			continue;

		// if comment
		if (data.substr(0, 1) == "#")
			continue;

		// get key
		size_t found = data.find('=');
		if(found>1000) // blank line = 4294967295
			continue;

		item.key = data.substr(0, (found-1));
		item.key = trim(item.key);

		item.value = data.substr((found+1));
		item.value = trim(item.value);

		values.push_back(item);
	}

	file.close();

	return values.size();
}

std::string ConfigParser::get_string(const std::string key)
{
	int length = values.size();
	if (length > 0)
	{
		for (int i = 0; i < length; i++)
		{
			if (key == values[i].key)
				return values[i].value;
		}
	}
	printf("[!][ConfigParser] Warning: cant find key: %s \n", key.c_str());
	return "";
}

int ConfigParser::get_int(const std::string key)
{
	int length = values.size();
	if (length > 0)
	{
		for (int i = 0; i < length; i++)
		{
			if (key == values[i].key)
				return atoi( values[i].value.c_str() );
		}
	}
	printf("[!][ConfigParser] Warning: cant find key: %s \n", key.c_str());
	return 0;
}

float ConfigParser::get_float(const std::string key)
{
	int length = values.size();
	if (length > 0)
	{
		for (int i = 0; i < length; i++)
		{
			if (key == values[i].key)
				return static_cast<float>( atof(values[i].value.c_str()) );
		}
	}
	printf("[!][ConfigParser] Warning: cant find key: %s \n", key.c_str());
	return 0.f;
}

// remove blankes
std::string ConfigParser::trim(std::string str)
{
	return triml(trimr(str));
}

// remove blankes from left
std::string ConfigParser::triml(std::string str)
{
	return str.erase(0, str.find_first_not_of(' '));
}

// remove blankes from right
std::string ConfigParser::trimr(std::string str)
{
	//return str.erase(str.find_last_not_of(' ')+1);
	std::string s = str;

	// remove comment
	size_t found = s.find('#');
	if( found < s.length() )
		s = s.substr(0, (found-1));

	s.erase(s.find_last_not_of(' ')+1);

	// remove \r \n
	if (!s.empty() && s[s.size() - 1] == '\n')
		s.erase(s.size() - 1);
	if (!s.empty() && s[s.size() - 1] == '\r')
		s.erase(s.size() - 1);

	return s;
}
