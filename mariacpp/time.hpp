/****************************************************************************
  Copyright (C) 2015 Karol Roslaniec <mariacpp@roslaniec.net>
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not see <http://www.gnu.org/licenses>
  or write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*****************************************************************************/
#ifndef MARIACPP_TIME_HPP
#define MARIACPP_TIME_HPP

#include <mysql.h>
#include <iosfwd>
#include <string>
#include <chrono>

#if __cplusplus <= 201703L
using days = std::chrono::duration<uint32_t, std::ratio<86400>>;
using months = std::chrono::duration<uint32_t, std::ratio<2629746>>;
using years = std::chrono::duration<uint32_t, std::ratio<31556952>>;
#endif

namespace MariaCpp {
    using namespace std::chrono;

    struct Time : public MYSQL_TIME {
        typedef unsigned year_t;
        typedef unsigned month_t;
        typedef unsigned day_t;
        typedef unsigned hour_t;
        typedef unsigned minute_t;
        typedef unsigned second_t;

        Time(const MYSQL_TIME& time) : MYSQL_TIME(time) {}

        Time(const std::string& str);

        static Time none();

        static Time date(year_t year, month_t month, day_t day);

    	static Time date(year_month_day ymd);

        static Time time(hour_t hour, minute_t minute, second_t second);

        static Time datetime(year_t year, month_t month, day_t day, hour_t hour, minute_t minute, second_t second);

        template<typename T, typename U>
        static Time datetime(time_point<T, U> tp) {
        	auto datePoint = floor<days>(tp);
        	year_month_day ymd{datePoint};

        	MYSQL_TIME mysqlTime{};
        	mysqlTime.year  = static_cast<int>(ymd.year());
        	mysqlTime.month = static_cast<unsigned int>(ymd.month());
        	mysqlTime.day   = static_cast<unsigned int>(ymd.day());

        	auto timeOfDay = tp - datePoint;
        	hh_mm_ss todComponents{timeOfDay};

        	mysqlTime.hour   = todComponents.hours().count();
        	mysqlTime.minute = todComponents.minutes().count();
        	mysqlTime.second = todComponents.seconds().count();

        	auto fractional = timeOfDay - todComponents.to_duration();
        	mysqlTime.second_part = static_cast<unsigned int>(
				std::chrono::duration_cast<milliseconds>(fractional).count()
			);

        	return mysqlTime;
        }

	    time_t to_time_t() {
		    std::tm tm{};
		    tm.tm_year = this->year - 1900;
		    tm.tm_mon = this->month - 1;
		    tm.tm_mday = this->day;
		    tm.tm_hour = this->hour;
		    tm.tm_min = this->minute;
		    tm.tm_sec = this->second;
		    return std::mktime(&tm);
	    }

	    template<typename T=system_clock>
	    time_point<T> to_time_point() {
		    return T::from_time_t(to_time_t());
	    }

	    year_month_day to_ymd() const {
	        return {std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)};
        }

	    void print(std::ostream& os) const;
    };

    inline std::ostream& operator<<(std::ostream& os, const Time& time) {
        return time.print(os), os;
    }
}
#endif
