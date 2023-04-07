#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "webserv.hpp"

TEST_CASE("file is open", "[webserv]") {
	REQUIRE(webserv("tests/nginx.conf") == true);
	REQUIRE(webserv("tests/test.conf") == false);
}

TEST_CASE("ip address", "[parcing]") {
	REQUIRE(ipAddress("172.16.13.1") == true);
	REQUIRE(ipAddress("0.0.0.0") == true);
	REQUIRE(ipAddress("255.255.255.255") == true);
	REQUIRE(ipAddress("1...1") == false);
	REQUIRE(ipAddress("-1.61.13.5") == false);
	REQUIRE(ipAddress("256.61.13.5") == false);
	REQUIRE(ipAddress("1.a1.13.5") == false);
}
