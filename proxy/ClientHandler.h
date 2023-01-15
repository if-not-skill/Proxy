#pragma once

#include <QObject>


class QTcpSocket;

struct DetailsRequestBodyDomainName;
struct DetailsRequestBodyIPv4;
struct DetailsRequestBodyIPv6;


class ClientHandler : public QObject
{
	Q_OBJECT


public:
	ClientHandler(QObject *parent, QTcpSocket * link);
	~ClientHandler() override;

private:
	void connectToServerDomainName(const QByteArray& buffer);
	void connectToServerIpV4(const QByteArray& buffer);
	void connectToServerIpV6(const QByteArray& buffer);

private slots:
	void readClient();
	void readServer();

	void clientIdentifierMessage();
	void clientDetailsMessage();

	void disconnectedClient();
	void disconnectedServer();

	void serverSocketConnected();

private:
	QTcpSocket* clientLink = nullptr;
	QTcpSocket* serverLink = nullptr;
};
