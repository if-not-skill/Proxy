#pragma once

enum class AuthenticationMethod : unsigned char
{
	NO_AUTHENTICATION_REQUIRED	= 0x00,
	GSSAPI						= 0x01,
	USERNAME_OR_PASSWORD		= 0x02,
	// IANA_ASSIGNED ,
	// RESERVED_FOR_PRIVATE_METHODS,
	NO_ACCEPTABLE_METHODS		= 0xFF
};

enum class Socks5Command : unsigned char
{
	CONNECT			= 0x01,
	BIND			= 0x02,
	UDP_ASSOCIATE	= 0x03,
};

enum class TypeAddress : unsigned char
{
	IP_V4		= 0x01,
	DOMAINNAME	= 0x03,
	IP_V6		= 0x04,
};

enum class ConnectStatus : unsigned char
{
	SUCCESS						= 0x00,
	ERROR_SOCKS_SERVER			= 0x01,
	NOT_ALLOWED_RULESET			= 0x02,
	NETWORK_UNREACHABLE			= 0x03,
	HOST_UNREACHABLE			= 0x04,
	CONNECTION_refused			= 0x05,
	TTL_EXPIRED					= 0x06,
	COMMAND_NOT_SUPPORTED		= 0x07,
	ADDRESS_TYPE_NOT_SUPPORTED	= 0x08
};