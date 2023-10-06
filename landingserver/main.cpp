#include <QCoreApplication>
#include "landing_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    LandingManager landingManager;
    landingManager.Initialize();
    
    return a.exec();
}
