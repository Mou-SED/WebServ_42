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

TEST_CASE("check syntax", "[parcing]") {
	std::ifstream file("tests/test.conf");
	REQUIRE(check_syntax(file) == "server { server_name aymane; autoindex on; ");
	REQUIRE_NOTHROW(check_syntax(file));
	file.close();
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