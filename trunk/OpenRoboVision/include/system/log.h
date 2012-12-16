//
// OpenRoboVision
//
// logger routines
//
// robocraft.ru
//

#ifndef _ORV_LOG_H_
#define _ORV_LOG_H_

#include <string>

// enable write data to file
#define LOG_ENABLE_WRITE		1
#define LOG_DEFAULT_FILE_NAME	"log.txt"
#define LOG_DEFAULT_BUFFER_NAME	"buffer"

namespace orv
{
	namespace log
	{
		// get string with date
		std::string get_date_string(bool underline=true);

		// write into log-file
		void write(const char* message, const char* filename=LOG_DEFAULT_FILE_NAME);
		void write(const std::string message, 
							const char* filename=LOG_DEFAULT_FILE_NAME);
		void write(const std::string message, 
							const std::string filename=std::string(LOG_DEFAULT_FILE_NAME));
		void write_buffer(const char* buf,
							const int buf_size, 
							const char* buf_name=LOG_DEFAULT_BUFFER_NAME, 
							const char* filename=LOG_DEFAULT_FILE_NAME);

		// запись строчки в файл
		void write_string(const std::string message, 
							const char* filename=LOG_DEFAULT_FILE_NAME);
		void write_string(const std::string message, 
							const std::string filename=std::string(LOG_DEFAULT_FILE_NAME));

	}; // namespace log
}; //namespace orv

#endif //#ifndef _ORV_LOG_H_
