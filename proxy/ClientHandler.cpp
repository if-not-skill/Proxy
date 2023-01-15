#include "ClientHandler.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QtEndian>

#include "Library/StructLibrary.h"
#include "Library/EnumLibrary.h"


#define SOCKS5_VER 0x05
#define IP_V6_SIZE 32


ClientHandler::ClientHandler(QObject *parent, QTcpSocket * link)
	: QObject(parent)
	, clientLink(link)
{
	clientLink->setParent(this);
	connect(clientLink, &QTcpSocket::readyRead, this, &ClientHandler::clientIdentifierMessage);
	connect(clientLink, &QTcpSocket::disconnected, this, &ClientHandler::disconnectedClient);

	qDebug() << "client connected";
}

ClientHandler::~ClientHandler()
{	
	qDebug() << "client disconnected";

	if(serverLink)
	{
		serverLink->close();
		serverLink->deleteLater();
	}
}

void ClientHandler::connectToServerDomainName(const QByteArray& buffer)
{
	const auto request = (DetailsRequestBodyDomainName*)(buffer.data() + sizeof(DetailsRequestHeader));
	const int domainSize = (int)request->address[0];
	const QString hostName = QString(request->address).trimmed();

	unsigned short port;
	memcpy(&port, (buffer.data() + sizeof(DetailsRequestHeader) + domainSize + 1), 2);

	port = qToBigEndian(port);

	serverLink->connectToHost(hostName, port);
}

void ClientHandler::connectToServerIpV4(const QByteArray& buffer)
{
	auto request = (DetailsRequestBodyIPv4*)(buffer.data() + sizeof(DetailsRequestHeader));

	QHostAddress hostAddr;
	const unsigned short port = qToBigEndian(request->port);

	auto address = request->address;
	address = qToBigEndian(address);	

	hostAddr.setAddress(address);

	serverLink->connectToHost(hostAddr, port);
}

void ClientHandler::connectToServerIpV6(const QByteArray& buffer)
{
	const auto request = (DetailsRequestBodyIPv6*)(buffer.data() + sizeof(DetailsRequestHeader));

	QHostAddress hostAddr;

	quint8* address = request->address;
	// address = qToBigEndian(address);

	hostAddr.setAddress(address);

	unsigned short port;
	memcpy(&port, (buffer.data() + sizeof(DetailsRequestHeader) + IP_V6_SIZE), 2);

	port = qToBigEndian(port);

	serverLink->connectToHost(hostAddr, port);
}

void ClientHandler::readClient()
{
	serverLink->write(clientLink->readAll());
}

void ClientHandler::readServer()
{
	clientLink->write(serverLink->readAll());
}

void ClientHandler::clientIdentifierMessage()
{
	QByteArray buffer = clientLink->readAll();
	const auto identifierRequest = (IdentifierRequest*)(buffer.data());

	if(identifierRequest->version != SOCKS5_VER)
	{
		qDebug() << "not correct version";
		clientLink->disconnectFromHost();
		return;	
	}

	const quint8 numMethods = identifierRequest->numberMethods;
	identifierRequest->methods[numMethods];

	bool isNoAuthentication = false;
	for(int methodId = 0; methodId < numMethods; ++methodId)
	{
		if(identifierRequest->methods[methodId] == AuthenticationMethod::NO_AUTHENTICATION_REQUIRED)
		{
			isNoAuthentication = true;
			break;
		}
	}

	if (!isNoAuthentication)
	{
		qDebug() << "not realize this authentication method";
		clientLink->disconnectFromHost();
		return;
	}

	MethodMessage methodMessage{};
	methodMessage.version = SOCKS5_VER;
	methodMessage.method = AuthenticationMethod::NO_AUTHENTICATION_REQUIRED;

	clientLink->write((char*)&methodMessage, sizeof(methodMessage));

	disconnect(clientLink, &QTcpSocket::readyRead, this, &ClientHandler::clientIdentifierMessage);
	connect(clientLink, &QTcpSocket::readyRead, this, &ClientHandler::clientDetailsMessage);
}

void ClientHandler::clientDetailsMessage()
{
	QByteArray buffer = clientLink->readAll();
	const auto detailsRequestHead = (DetailsRequestHeader*)buffer.data();

	if (detailsRequestHead->version != SOCKS5_VER)
	{
		qDebug() << "not correct version";
		clientLink->disconnectFromHost();
		return;
	}

	{
		DetailsMessage answer;
		answer.version = SOCKS5_VER;
		answer.connectionType = ConnectStatus::SUCCESS;
		answer.host = 0;
		answer.port = 0;
		answer.addressType = TypeAddress::IP_V4;

		clientLink->write((char*)(&answer), sizeof(answer));
	}

	disconnect(clientLink, &QTcpSocket::readyRead, this, &ClientHandler::clientDetailsMessage);
	connect(clientLink, &QTcpSocket::readyRead, this, &ClientHandler::readClient);

	serverLink = new QTcpSocket(this);
	connect(serverLink, &QTcpSocket::readyRead, this, &ClientHandler::readServer);
	connect(serverLink, &QTcpSocket::disconnected, this, &ClientHandler::disconnectedServer);
	connect(serverLink, &QTcpSocket::connected, this, &ClientHandler::serverSocketConnected);
	// connect(serverLink, &QTcpSocket::error, this, &ClientHandler::);

	switch (detailsRequestHead->addressType) 
	{ 
	case TypeAddress::IP_V4:
		connectToServerIpV4(buffer);
		break;
	case TypeAddress::DOMAINNAME:
		connectToServerDomainName(buffer);
		break;
	case TypeAddress::IP_V6:
		connectToServerIpV6(buffer);
		break;
	}
}

void ClientHandler::disconnectedClient()
{
	if (!serverLink) return;

	serverLink->close();

	deleteLater();
}

void ClientHandler::disconnectedServer()
{
	clientLink->close();

	deleteLater();
}

void ClientHandler::serverSocketConnected()
{
	qDebug() << "connected to server";
}
