//
// OpenRoboVision
//
// logger routines
//
// robocraft.ru
//

#include "orv/system/log.h"
#include "orv/system/times.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

std::string orv::log::get_date_string(bool underline)
{
	std::stringstream out;

	orv::time::Time st;
	orv::time::get_current(&st);

	if(underline)
	{
		out<< std::setfill('0') << st.Year <<"_"<<std::setw(2)<< st.Month<<"_"
			<<std::setw(2)<< st.Day<<"_"<<std::setw(2)<< st.Hour<<"_" 
			<<std::setw(2)<< st.Minute<<"_" <<std::setw(2)<< st.Second<<"_"
			<<std::setw(3)<< st.Milliseconds;
	}
	else
	{
		out<< std::setfill('0') << st.Year <<":"<<std::setw(2)<< st.Month<<":"
			<<std::setw(2)<< st.Day<<" "<<std::setw(2)<< st.Hour<<":" 
			<<std::setw(2)<< st.Minute<<":" <<std::setw(2)<< st.Second<<":"
			<<std::setw(3)<< st.Milliseconds;
	}

	return out.str();
}

void orv::log::write(const char* message, const char* filename)
{
#if !LOG_ENABLE_WRITE
	return;
#endif //#if !LOG_ENABLE_WRITE

	std::ofstream fs(filename, std::ios::app);
	if(fs.is_open())
	{
		std::stringstream out;
		out<<"["<< get_date_string(false) <<"] "<<message<<"\n";
		fs << out.str();
		fs.close();
	}
}

void orv::log::write(const std::string message, const char* filename)
{
	write(message.c_str(), filename);
}

void orv::log::write(const std::string message, const std::string filename)
{
	write(message.c_str(), filename.c_str());
}

void orv::log::write_buffer(const char* buf, const int buf_size, 
							const char* buf_name, const char* filename)
{
#if !LOG_ENABLE_WRITE
	return;
#endif //#if !LOG_ENABLE_WRITE

	if(!buf || buf_size<=0 || !buf_name)
		return;

	std::stringstream ss; 
	ss<<buf_name<<" ("<<buf_size<<")\n";
	for(int i =0; i<buf_size; i++)
		ss<<(unsigned int)((unsigned char)buf[i])<<" ";

	ss<<"\n";
	write(ss.str().c_str(), filename);
}

void orv::log::write_string(const std::string message, const char* filename)
{
#if !LOG_ENABLE_WRITE
	return;
#endif //#if !LOG_ENABLE_WRITE

	std::ofstream fs(filename, std::ios::app);
	if(fs.is_open())
	{
		fs<< message <<"\n";
		fs.close();
	}
}

void orv::log::write_string(const std::string message, const std::string filename)
{
	write_string(message, filename.c_str());
}
