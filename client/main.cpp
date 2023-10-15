#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "loginmanager.h"
#include <QQmlContext.h>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    LoginManager loginManager;
    loginManager.Initialize();
    
    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/client/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    loginManager.setQmlEngine(&engine);
    engine.rootContext()->setContextProperty("loginManager", &loginManager);
    
    engine.load(url);  // 加载（类似于加载配置文件）
    
    return app.exec();
}
