#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "messenger.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Messenger *mess = new Messenger(nullptr);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("mess", mess);
    engine.loadFromModule("Messenger", "Main");

    return app.exec();
}
