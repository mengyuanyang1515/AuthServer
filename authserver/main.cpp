#include <QCoreApplication>
#include "auth_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    AuthManager server;
    server.Initialize();
    
    return a.exec();
}
