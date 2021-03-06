#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <ctime>
#include <chrono>
#include <tuple>
#include <string>
#include <utility>

class DateTime
{
public:	
	static std::string toISOFormat(std::chrono::system_clock::time_point time_point) {
		time_t tt = std::chrono::system_clock::to_time_t(time_point);
		tm local_tm = *localtime(&tt);

		return std::move(std::to_string(local_tm.tm_year + 1900) + "-" + std::to_string(local_tm.tm_mon + 1) + "-" + std::to_string(local_tm.tm_mday) + " " + std::to_string(local_tm.tm_hour) + ":" +
						 std::to_string(local_tm.tm_min) + ":" + std::to_string(local_tm.tm_sec));
	}

	static inline std::tuple<unsigned short int, unsigned short int, unsigned short int> getLocalYMDNow()
	{
		/**
		 * This function returns local year, month(starts at 1) and time from local time in a tuple.
		 */
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		time_t tt = std::chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&tt);

		return std::move(std::make_tuple(local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday));
	}

	static inline std::string getLocalDateISOFormat()
	{
		/**
		 *	This function returns year, month, day in ISO format in current timezone.
		 */
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		time_t tt = std::chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&tt);

		return std::move(std::to_string(local_tm.tm_year + 1900) + "-" + std::to_string(local_tm.tm_mon + 1) + "-" + std::to_string(local_tm.tm_mday));
	}

	static inline std::string getLocalDateTimeISOFormat()
	{
		/**
		 *	This function returns date and time in ISO format in current timezone.
		 */
		return toISOFormat(std::chrono::system_clock::now());
	}
};

#endif