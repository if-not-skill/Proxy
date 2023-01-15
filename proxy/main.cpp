#include <QtCore/QCoreApplication>

#include "ProxyServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);	

	ProxyServer proxy;

    return a.exec();
}
