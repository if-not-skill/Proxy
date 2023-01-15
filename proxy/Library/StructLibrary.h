#pragma once

#include "EnumLibrary.h"

#pragma pack(push, 1)
struct IdentifierRequest
{
	unsigned char			version;
	unsigned char			numberMethods;
	AuthenticationMethod	methods[];
};

struct MethodMessage
{
	unsigned char			version;
	AuthenticationMethod	method;
};

struct DetailsRequestHeader
{
	unsigned char	version;
	Socks5Command	command;
	unsigned char	reserved = 0x00;
	TypeAddress		addressType;
};

struct DetailsRequestBodyDomainName
{
	char			address[1];
	unsigned short	port;
};

struct DetailsRequestBodyIPv4
{
	unsigned int	address;
	unsigned short	port;
};

struct DetailsRequestBodyIPv6
{
	unsigned char*			address;
	unsigned short	port;
};

struct DetailsMessage
{
	unsigned char	version;
	ConnectStatus	connectionType;
	unsigned char	reserve = 0x00;
	TypeAddress		addressType;
	unsigned int	host;
	unsigned short	port;
};
#pragma pack(pop)
