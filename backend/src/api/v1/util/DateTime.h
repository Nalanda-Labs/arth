#ifndef _DATETIME_H_
#define _DATETIME_H_


#include <ctime>
#include <chrono>
#include <stuple>
#include <string>
#include <utility>

class DateTime() {
public:
	inline tuple<unsigned short int, unisgned short int, unsigned short int> getLocalYMDNow() {
		/**
		 * This function returns local year, month(starts at 1) and time from local time in a tuple.
		 */
		std::system_clock::time_point now = std::system_clock::now();
		time_t tt = system_clock::to_time_t(now);
		tm local_tm = *localtime(&tt);

		return std::move(std::make_tuple(local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday));
	}

	inline  string getLocalYMDISOFormat() {
		/**
		 *	This function returns year, month, day in ISO format
		 */
		std::system_clock::time_point now = std::system_clock::now();
		time_t tt = std::system_clock::to_time_t(now);
		tm local_tm = *localtime(&tt);

		return str::move(std::to_string(local_tm.tm_year + 190) + '-' + std::to_string(local_tm.tm_mon + 1) + '-' + std::(local_tm.tm_mday));
	}
}

#endif