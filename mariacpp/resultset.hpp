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
#ifndef MARIACPP_RESULTSET_HPP
#define MARIACPP_RESULTSET_HPP

#include <mysql.h>
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>

namespace MariaCpp {

    class Connection;

    class ResultSet {
    public:
	    typedef unsigned int idx_t;

        ResultSet(Connection& conn, MYSQL_RES* res, bool fetch_names = false) : _conn(conn), _res(res), _row(), _lengths() {
            if (fetch_names)
                fetchFieldNames();
        }

        ~ResultSet() { if (_res) mysql_free_result(_res); }

        void data_seek(my_ulonglong offset) { return mysql_data_seek(_res, offset); }

        bool eof() const { return mysql_eof(_res); }

        MYSQL_FIELD* fetch_field() const { return mysql_fetch_field(_res); }

        MYSQL_FIELD* fetch_field_direct(idx_t col) const {
            assert_col(col);
            return mysql_fetch_field_direct(_res, col);
        }

        MYSQL_FIELD* fetch_fields() const { return mysql_fetch_fields(_res); }

        // Better use length(col)
        unsigned long* fetch_lengths() const { return _lengths ? _lengths : _lengths = mysql_fetch_lengths(_res); }

        MYSQL_ROW fetch_row();

        MYSQL_FIELD_OFFSET field_seek(MYSQL_FIELD_OFFSET offset) { return mysql_field_seek(_res, offset); }

        MYSQL_FIELD_OFFSET field_tell() { return mysql_field_tell(_res); }

        void free_result() {
            mysql_free_result(_res);
            _res = NULL;
        }

        bool next() { return fetch_row(); }

        unsigned int num_fields() const { return mysql_num_fields(_res); }

        my_ulonglong num_rows() const { return mysql_num_rows(_res); }

#   if 80000 <= LIBMYSQL_VERSION_ID
        enum enum_resultset_metadata result_metadata(MYSQL_RES *result)
            { return mysql_result_metadata(_res); }
#   endif

        MYSQL_ROW_OFFSET row_seek(MYSQL_ROW_OFFSET offset) { return mysql_row_seek(_res, offset); }

        MYSQL_ROW_OFFSET row_tell() { return mysql_row_tell(_res); }

        // getRaw() might return NULL, iff column is NULL
        const char* getRaw(idx_t col) const {
            assert_col(col);
            return _row[col];
        }

        bool isNull(idx_t col) const {
            assert_col(col);
            return !_row[col];
        }

        std::string getString(idx_t col) const;

        std::string getBinary(idx_t col) const { return getString(col); }

        int32_t getInt(idx_t col) const;

        uint32_t getUInt(idx_t col) const;

        int64_t getInt64(idx_t col) const;

        uint64_t getUInt64(idx_t col) const;

        bool getBoolean(idx_t col) const { return getInt(col); }

        float getFloat(idx_t col) const;

		double getDouble(idx_t col) const;

        void fetchFieldNames();

		bool isNull(const std::string& col) const;

        std::string getString(const std::string& col) const;

        std::string getBinary(const std::string& col) const { return getString(col); }

        int32_t getInt(const std::string& col) const;

        uint32_t getUInt(const std::string& col) const;

        int64_t getInt64(const std::string& col) const;

        uint64_t getUInt64(const std::string& col) const;

        bool getBoolean(const std::string& col) const { return getInt(col); }

        float getFloat(const std::string& col) const;

        double getDouble(const std::string& col) const;

		idx_t length(idx_t col) const {
            assert_col(col);
            return fetch_lengths() ? _lengths[col] : 0;
        }

#   ifdef MARIADB_VERSION_ID

        int async_status() const;

        void free_result_start();

        void free_result_cont(int status);

        MYSQL_ROW fetch_row_start();

        MYSQL_ROW fetch_row_cont(int status);

        bool next_start() { return fetch_row_start(); }

        bool next_cont(int status) { return fetch_row_cont(status); }

#   endif /* MARIADB_VERSION_ID */

    private:
        inline void assert_col(idx_t col) const;

        // Noncopyable
        ResultSet(const ResultSet&);

        void operator=(ResultSet&);

        Connection& _conn;
        MYSQL_RES* _res;
        MYSQL_ROW _row; // _row == _res->current_row
        mutable unsigned long* _lengths; // == fetch_lengths()
        std::vector<std::string> _col_names;

        int getFieldIndexByName(const std::string& name) const;
    };

    void ResultSet::assert_col(idx_t col) const {
        assert(col < _res->field_count);
    }
}
#endif
