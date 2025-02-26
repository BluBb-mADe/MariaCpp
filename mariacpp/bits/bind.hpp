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
#ifndef MARIACPP_BIND_HPP
#define MARIACPP_BIND_HPP

#include <mysql.h>
#include <cstdint>
#include <string>

namespace MariaCpp {

    struct Time;

    class Bind {
    public:
        Bind();

        ~Bind();

        operator MYSQL_BIND();

        Bind& init(MYSQL_FIELD* field);

        unsigned long raw_length() const { return _length; }

        unsigned long data_length() const;

        bool error() const { return _error; }

        void realloc(unsigned long length);

        bool setNull();

        bool setTinyInt(int8_t value);

        bool setUTinyInt(uint8_t value);

        bool setSmallInt(int16_t value);

        bool setUSmallInt(uint16_t value);

        bool setYear(uint16_t value);

        bool setMediumInt(int32_t value);

        bool setUMediumInt(uint32_t value);

        bool setInt(int32_t value);

        bool setUInt(uint32_t value);

        bool setInt64(int64_t value);

        bool setUInt64(uint64_t value);

        bool setBoolean(bool value) { return setTinyInt(value); }

        bool setDouble(double value);

        bool setFloat(float value);

        bool setCString(const char* str);

        bool setString(std::string_view str);

        bool setBlob(const std::string& str);

        bool setDate(const Time& time) { return setDateTime(time); }

        bool setDateTime(const Time& time);

        bool setTime(const Time& time) { return setDateTime(time); }

        bool setTimeStamp(const uint64_t time) { return setUInt64(time); }

        bool isNull() const { return _null; }

        std::string getString() const;

        int32_t getInt() const;

        uint32_t getUInt() const;

        int64_t getInt64() const;

        uint64_t getUInt64() const;

        float getFloat() const;

        double getDouble() const;

        Time getDateTime() const;

    private:
        template<typename T>
        inline bool setNumeric(T value, enum_field_types type, bool is_unsigned);

        inline bool setBuffer(const char* s, size_t length, enum_field_types type);

        inline bool setTime(const Time& time, enum_field_types type);

        static inline void throw_unsupported_type() ;

        struct Buffer {
            typedef bool heap_t;

            inline void* data(const heap_t& heap) const;

            inline size_t size(const heap_t& heap) const;

            inline void* alloc(heap_t& heap, size_t size);

            inline void free(heap_t& heap);

        private:
            void* _data;
            size_t _size;
        };

        Bind(const Bind&); // non-copyable
        const Bind& operator=(const Bind&); // non-copyable

        Buffer _buffer;
        unsigned long _length; // Might be bigger than buffer after fetch()!
        enum_field_types _type;
        my_bool _null;
        my_bool _unsigned;
        my_bool _error;
        Buffer::heap_t _heap;
    };
}
#endif
