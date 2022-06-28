<!-- -*- mode: markdown -*-  -->

MariaCpp is C++ library that lets you connect to the MariaDB Server
(or MySQL Server).


Major features of MariaCpp
--------------------------

*   __LGPL__ (with static link exception) license

*   thin C++ wrapper around _MariaDB Connector/C_ (C-API);
    _thin_ means that C++ objects have none (or minimal) internal state,
    and you can possibly mix C++ code with native C-API

*   this fork requires C++20 (if you need older C++ standards then use the original)

*   it takes benefits of 2 major C++ paradigms: __RAII__ and __exceptions__

*   supports most C-API features, including __prepared statements__

*   supports __multithreading__ (multiple connections to DB)

*   supports __non-blocking__ (a.k.a. __Asynchronous__) API

*   no other dependencies (e.g. no Boost dependency)



Example
-------


```C++
#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/mariadb_error.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace MariaCpp;

const char *uri = "tcp://localhost:3306/test";
const char *user = "test";
const char *passwd = "";

int main() {
	scoped_library_init maria_lib_init;

	try {
		Connection conn;
		conn.connect(Uri(uri), user, passwd);

		conn.query("CREATE TEMPORARY TABLE test "
		           "(i INT, s CHAR(15), d DATETIME)");
		std::unique_ptr<PreparedStatement> stmt(
		  conn.prepare("INSERT INTO test (i,s,d) values(?,?,?)"));

		assert(3 == stmt->param_count());
		stmt->setInt(0, 1);
		stmt->setString(1, "string-1");
		stmt->setDateTime(2, Time("2016-03-23 02:41"));
		stmt->execute();

		stmt->setInt(0, 2);
		stmt->setNull(1);
		stmt->setDateTime(2, Time::datetime(2015, 02, 21, 12, 45, 51));
		stmt->execute();

		stmt.reset(conn.prepare("SELECT i, s, d FROM test ORDER BY i"));
		stmt->execute();
		while (stmt->fetch()) {
			std::cout << "i = " << stmt->getInt(0);
			std::cout << ", i from column name = " << stmt->getInt("i");
			std::cout << ", s = ";
			if (stmt->isNull(1)) std::cout << "NULL";
			else std::cout << stmt->getString(1);
			std::cout << ", d = " << stmt->getTime(2);
			std::cout << std::endl;
		}
		conn.close();
	} catch (mariadb_error &e) {
		std::cerr << e << std::endl;
		return 1;
	}
	return 0;
}
```

    i = 1, s = string-1, d = 2016-03-23 02:41:00
    i = 2, s = NULL, d = 2015-02-21 12:45:51

FAQ
---

**Q**: There exists already MySQL Connector/C++. What's the benefit of MariaCpp?

**A**: The primary difference is a license: MySQL Connector/C++ is __GPL__.
   MariaCpp is licensed as LGPL (__Lesser GPL__) with static link exception.
   Other difference is that _MySQL Connector/C++_ is based on JDBC 4.0 API,
   while MariaCpp API is based on  _MariaDB Connector/C_.
   Nevertheless, migration from MySQL Connector/C++ might be surprisingly easy.

* * *

**Q**: Can I use _MariaCpp_ with _MySQL Connector/C_ as underlying library
   instead of _MariaDB Connector/C_?

**A**: Yes, you can. But please notice that _MySQL Connector/C_ is GPL licensed.
   As result, your code must be GPL licensed as well (or other FLOSS license).
   Sometimes it's not desirable.

* * *

**Q**: Why _MariaCpp_ is licensed as LGPL?

**A**: MariaCpp is licensed in the same spirit as _MariaDB Connector/C_.

* * *

**Q**: What are the differences in this fork?

**A**: This fork is not meant to replace or generally improve upon the original.<br />
It just contains all the QoL changes that makes working with the library much more comfortable for me and LGPL requires open source and stuff.<br />
<br />
The most notable differences are:
- Result access via column name.<br />
   This incurs a small overhead as the column names have to be stored for each query.<br />
   The `PreparedStatement` always stores column names automatically now because they are already contained in the query result anyway so the only penalty is a single allocation and copy for each name.<br />
   The `ResultSet` pays a single branch by default. The `store_result` and `use_result` methods of `ResultSet` now have an optional bool to also fetch column names and enable access via name.
- Uniform result accessors method naming.<br />
   The method names of `PreparedStatement` and `ResultSet` match now which allows for generic code in simple cases.
- Support for retrieving floats.<br />
   Why this hell is this missing in the original?
- Support setting `std::time_point` directly into a datetime field because the boilerplate for this is just painful.
- Retry all queries on deadlock error.<br />
   This is probably the most use case specific change, but it would've been a pain to implement around the library in my use case. Create another fork or use the original if you don't like it. :P
- Only C++20 support and replaced some platform dependent stuff with newer std.
- Should work with msvc and clang out of the box (maybe? Promises like that are scary.)

* * *
