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
#include <mariacpp/resultset.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/mariadb_error.hpp>
#include <cstdlib>
#include <algorithm>

namespace MariaCpp {

    MYSQL_ROW ResultSet::fetch_row() {
        _lengths = nullptr;
        _row = mysql_fetch_row(_res);
        if (!_row && _conn.errorno()) _conn.throw_exception();
        return _row;
    }

    std::string ResultSet::getString(unsigned col) const {
        assert_col(col);
        const char* data = _row[col];
        if (!data) return std::string();
        unsigned long len = length(col);
        return std::string(data, len);
    }

    int32_t ResultSet::getInt(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        if (fetch_field_direct(col)->flags & UNSIGNED_FLAG)
            return strtoul(_row[col], NULL, 10);
        return strtol(_row[col], NULL, 10);
    }

    int64_t ResultSet::getInt64(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        if (fetch_field_direct(col)->flags & UNSIGNED_FLAG)
            return strtoull(_row[col], NULL, 10);
        return strtoll(_row[col], NULL, 10);
    }

    uint32_t ResultSet::getUInt(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        if (fetch_field_direct(col)->flags & UNSIGNED_FLAG)
            return strtoul(_row[col], NULL, 10);
        return strtol(_row[col], NULL, 10);
    }

    uint64_t ResultSet::getUInt64(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        if (fetch_field_direct(col)->flags & UNSIGNED_FLAG)
            return strtoull(_row[col], NULL, 10);
        return strtoll(_row[col], NULL, 10);
    }

    float ResultSet::getFloat(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        return strtof(_row[col], NULL);
    }

    double ResultSet::getDouble(unsigned col) const {
        assert_col(col);
        if (isNull(col)) return 0;
        return strtod(_row[col], NULL);
    }

#ifdef MARIADB_VERSION_ID

    int ResultSet::async_status() const {
        return _conn.async_status();
    }

    void ResultSet::free_result_start() {
        assert(!_conn._async_status);
        _conn._async_status = mysql_free_result_start(_res);
        if (!_conn._async_status) _res = nullptr;
    }

    void ResultSet::free_result_cont(int status) {
        assert(_conn._async_status);
        _conn._async_status = mysql_free_result_cont(_res, status);
        if (!_conn._async_status) _res = nullptr;
    }

    MYSQL_ROW ResultSet::fetch_row_start() {
        assert(!_conn._async_status);
        _lengths = nullptr;
        _conn._async_status = mysql_fetch_row_start(&_row, _res);
        if (_conn._async_status) return nullptr;
        if (!_row && _conn.errorno()) _conn.throw_exception();
        return _row;
    }

    MYSQL_ROW ResultSet::fetch_row_cont(int status) {
        assert(_conn._async_status);
        _conn._async_status = mysql_fetch_row_cont(&_row, _res, status);
        if (_conn._async_status) return nullptr;
        if (!_row && _conn.errorno()) _conn.throw_exception();
        return _row;
    }

    void ResultSet::fetchFieldNames() {
        _col_names.resize(_res->field_count);
        auto* fields = fetch_fields();
        for (int i = 0; i < _res->field_count; ++i) {
            _col_names[i] = std::string(fields[i].name, fields[i].name_length);
        }
    }

    std::string ResultSet::getString(const std::string& col) const {
        return getString(getFieldIndexByName(col));
    }

    int32_t ResultSet::getInt(const std::string& col) const {
        return getInt(getFieldIndexByName(col));
    }

    int64_t ResultSet::getInt64(const std::string& col) const {
        return getInt64(getFieldIndexByName(col));
    }

    uint32_t ResultSet::getUInt(const std::string& col) const {
        return getUInt(getFieldIndexByName(col));
    }

    uint64_t ResultSet::getUInt64(const std::string& col) const {
        return getUInt64(getFieldIndexByName(col));
    }

    float ResultSet::getFloat(const std::string& col) const {
        return getFloat(getFieldIndexByName(col));
    }

    double ResultSet::getDouble(const std::string& col) const {
        return getDouble(getFieldIndexByName(col));
    }

    static bool lc_equals(const std::string& a, const std::string& b) {
        return a.size() == b.size() && std::ranges::equal(a, b,
                  [](const char c, const char d) {
                      return tolower(c) == tolower(d);
                  });
    }

    int ResultSet::getFieldIndexByName(const std::string& name) const {
        for (int i = 0; i < _col_names.size(); ++i) {
            if (lc_equals(name, _col_names[i]))
                return i;
        }
        throw mariadb_error("unknown column name \"" + name + "\", did you forget to call \"fetchFieldNames()\"?");
    }
#endif /* MARIADB_VERSION_ID */
}
