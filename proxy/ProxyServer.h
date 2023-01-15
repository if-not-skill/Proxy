#pragma once

#include <Qobject>

class QTcpServer;
class QTcpSocket;


class ProxyServer : public QObject
{
	Q_OBJECT
public:
	ProxyServer(QObject* parent = nullptr);
	~ProxyServer() override;

private slots:
	void newConnection();

private:
	QTcpServer* server;

};

