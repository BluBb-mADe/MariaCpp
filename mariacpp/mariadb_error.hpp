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
#ifndef MARIACPP_MARIADB_ERROR_HPP
#define MARIACPP_MARIADB_ERROR_HPP

#include <iostream>
#include <stdexcept>
#include <iosfwd>
#include <stacktrace>
#include <utility>

#include <mysql.h>

namespace MariaCpp {
    class mariadb_error : public std::runtime_error {
    public:
        mariadb_error(const char* str, unsigned number, const char* sqlstate,
					  std::stacktrace stacktrace=std::stacktrace::current())
		: std::runtime_error(str), errno_(number), sqlstate_(sqlstate), stacktrace_(std::move(stacktrace)) { }

        explicit mariadb_error(MYSQL* con) : mariadb_error(mysql_error(con), mysql_errno(con), mysql_sqlstate(con)) {}

        explicit mariadb_error(MYSQL_STMT* con) : mariadb_error(mysql_stmt_error(con), mysql_stmt_errno(con), mysql_stmt_sqlstate(con)) {}

        explicit mariadb_error(const std::string& reason, std::stacktrace stacktrace=std::stacktrace::current())
		: std::runtime_error(reason), errno_(0), stacktrace_(std::move(stacktrace)) { }

        void print(std::ostream& os) const {
            if (!errno_ && sqlstate_.empty())
                os << "MariaDB lib ERROR: " << what();
            else
                os << "MariaDB SQL ERROR " << errno_ << " (" << sqlstate_ << ") " << what();
        }

        [[nodiscard]] unsigned errorno() const { return errno_; }

		[[nodiscard]] std::stacktrace const& get_stacktrace() const { return stacktrace_; }

    private:
        const unsigned errno_;
        std::string sqlstate_;
		std::stacktrace stacktrace_;
    };

    inline std::ostream& operator<<(std::ostream& os, const mariadb_error& ex) {
        return ex.print(os), os;
    }

    class InvalidArgumentException final : public mariadb_error {
    public:
        explicit InvalidArgumentException(const std::string& reason) : mariadb_error(reason) {}
    };
}
#endif
