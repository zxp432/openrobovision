//
// OpenRoboVision
//
// parser for configuration files
//
// robocraft.ru
//

// configuration example:
//
//# comment
// key1 = value1
// key2 = value2
// key3 = value3

#ifndef _ORV_CONFIG_PARSER_H_
#define _ORV_CONFIG_PARSER_H_

#include <string>
#include <vector>

// for store configuration data
typedef struct parser_config_item {
	std::string key;
	std::string value;
} parser_config_item;

class ConfigParser
{
public:
	ConfigParser();
	ConfigParser(const char* file_name);
	~ConfigParser();

	// parse configuration file 
	// and put pairs into values (vector of parser_config_item)
	int parse_file(const char* file_name);

	// routines for get values by key
	std::string get_string(const std::string key);
	int get_int(const std::string key);
	float get_float(const std::string key);

	// remove blankes
	static std::string trim(std::string str);
	// remove blankes from left
	static std::string triml(std::string str);
	// remove blankes from right
	static std::string trimr(std::string str);

protected:
	std::string config_file_name;
	std::vector< parser_config_item > values;
};

#endif //#ifndef _ORV_CONFIG_PARSER_H_
