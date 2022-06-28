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
#define _CRT_SECURE_NO_WARNINGS
#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/mariadb_error.hpp>
#include <mariacpp/uri.hpp>
#include <cstdlib>
#include <iostream>
#include <condition_variable>
#include <future>
#include <chrono>
#include <vector>

using namespace std::literals;

std::atomic<bool> run_threads = false;

std::mutex mutex;
std::condition_variable cond;
int counter = 0;

struct thread_result {
	unsigned thread_index = -1;
	unsigned long conn_id = 0;
	std::string conn_stats{};
	bool failed = true;
};

thread_result thread_start(char const* uri, char const* user, char const* passwd, unsigned thread_i) {
	thread_result res {.thread_index = thread_i};
    // It's very important to call thread_init() method
    MariaCpp::scoped_thread_init maria_thread;

    try {
        MariaCpp::Connection conn;
        // Connect to DB using MySQL Connector/C++ style host URI.
        // You can also use alternative connect() method with
        // the same arguments as C-API.
        conn.connect(MariaCpp::Uri(uri), user, passwd);

        std::clog << thread_i << ". Connection status: SUCCESS" << std::endl;
	    res.conn_id = conn.thread_id();
	    // Wait on condition for other threads to connect...
	    {
			std::unique_lock ul{mutex};
			++counter;
		}
	    run_threads.wait(false);

        // Each thread has connection do DB.
        // You can put here whatever queries you want

		res.conn_stats = conn.stat();
        conn.close(); // optional
	    res.failed = false;
    }
	catch (MariaCpp::mariadb_error& e) {
        std::cerr << e << std::endl;
    }
    catch (std::exception& e) {
	    std::cerr << e.what() << std::endl;
	}
	return res;
}

int main() {
    // Initialize MariaDB/MySQL library
    // Alternatively call methods:
    //   MariaCpp::library_init()
    //   ....
    //   MariaCpp::library_end()
    // In multithreaded MariaDB environment you MUST call library_init()
    // method from main thread before creating other threads!
    MariaCpp::scoped_library_init maria_lib_init;
    if (MariaCpp::thread_safe())
        std::clog << "MariaDB library is thread safe: OK" << std::endl;
    else
        std::cerr << "Oops... MariaDB library is NOT thread safe!" << std::endl;

    char const* uri = std::getenv("TEST_DB_URI");
    char const* user = std::getenv("TEST_DB_USER");
    char const* passwd = std::getenv("TEST_DB_PASSWD");
    if (!uri) uri = "tcp://localhost:3306/test";
    if (!user) user = "test";
    if (!passwd) passwd = "";
    std::clog << "DB uri: " << uri << std::endl;
    std::clog << "DB user: " << user << std::endl;
    std::clog << "DB passwd: " << passwd << std::endl;

    unsigned const num_threads = 5;
	std::vector<std::future<thread_result>> futures;

    // Create futures
    for (unsigned tnum = 0; tnum < num_threads; tnum++) {
	    futures.emplace_back(std::async(thread_start, uri, user, passwd, tnum));
    }

    // Wait for threads to enter condition
	{
		std::unique_lock ul{mutex};
		if (!cond.wait_for(ul, 3s, [] { return counter < num_threads; })) {
			std::cerr << "TIMEOUT" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

    // trigger (start) all threads
    run_threads = true;
	run_threads.notify_all();

    // Wait for futures and print results
	int err_count = 0;
	auto fut = futures.begin();
	while (!futures.empty()) {
		if (fut->wait_for(10ms) != std::future_status::ready) {
			if (++fut == futures.end())
				fut = futures.begin();
			continue;
		}
		auto res = fut->get();
		std::clog << res.thread_index << ". Finished res=" << !res.failed << std::endl;
		std::clog << "   conn id: " << res.conn_id << std::endl;
		std::clog << "   stats: " << res.conn_stats << std::endl;
		err_count += res.failed;
		fut = futures.erase(fut);
	}

    return err_count;
}
