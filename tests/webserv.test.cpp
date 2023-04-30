#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "webserv.hpp"

TEST_CASE("ip address", "[parcing]") {
	REQUIRE(ipAddress("127.0.0.1") == true);
	REQUIRE(ipAddress("0.0.0.0") == true);
	REQUIRE(ipAddress("255.255.255.255") == true);
	REQUIRE(ipAddress("1...1") == false);
	REQUIRE(ipAddress("-1.61.13.5") == false);
	REQUIRE(ipAddress("256.61.13.5") == false);
	REQUIRE(ipAddress("1.a1.13.5") == false);
}

TEST_CASE("[A] Check synatx part", "[parcing]") {

	REQUIRE_THROWS(error_page_check("300 4d00 500;"));
	REQUIRE_NOTHROW(error_page_check("300 400 500;"));
	REQUIRE_NOTHROW(error_page_check("300 400 500"));
	REQUIRE_NOTHROW(client_max_body_size_check("1000;"));
	REQUIRE_NOTHROW(client_max_body_size_check("1000M;"));
	REQUIRE_THROWS(client_max_body_size_check("100G;"));
	REQUIRE_NOTHROW(autoindex_check("on;"));
	REQUIRE_NOTHROW(autoindex_check("off;"));
	REQUIRE_THROWS(autoindex_check("On;"));
}

TEST_CASE("check listen", "[parcing]")
{
	REQUIRE_THROWS(listen_check("-80;", 1));
	REQUIRE_NOTHROW(listen_check("80", 1));
	REQUIRE_THROWS(listen_check("8.0", 1));
	REQUIRE_NOTHROW(listen_check("127.0.0.1:80", 1));
	REQUIRE_THROWS(listen_check("127.0.0.:80", 1));
	REQUIRE_THROWS(listen_check("127.0.0.1::80", 1));
}