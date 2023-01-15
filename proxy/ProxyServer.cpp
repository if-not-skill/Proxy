#include "ProxyServer.h"

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include "ClientHandler.h"

#define PROXY_PORT 1022

#define SERVER_ADDR "postman-dev2"
#define SERVER_PORT 22


ProxyServer::ProxyServer(QObject* parent)
{
	server = new QTcpServer(this);

	connect(server, &QTcpServer::newConnection, this, &ProxyServer::newConnection);

	const bool success = server->listen(QHostAddress::LocalHost, PROXY_PORT);
	if (success)
	{
		qDebug() << "proxy server started";
	}
	else
	{
		qDebug() << "proxy server can't start";
	}
}


ProxyServer::~ProxyServer()
{
}

void ProxyServer::newConnection()
{
	QTcpSocket* clientSocket = server->nextPendingConnection();

	ClientHandler* handler = new ClientHandler(this, clientSocket);
}
