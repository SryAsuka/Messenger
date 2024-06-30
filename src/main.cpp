#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "messenger.h"
#include"formchat.h"
#include<QDebug>
#include<QApplication>

int main(int argc, char *argv[])
{


    // QGuiApplication app(argc, argv);//初始化应用程序
    QApplication app(argc, argv);
    Messenger *mess = new Messenger(nullptr);//创建messenger实例
    // FormChat *formChat=new FormChat(nullptr);


    QQmlApplicationEngine engine;//设置qml引擎
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("mess", mess);
    // engine.rootContext()->setContextProperty("formChat", mess->formChat);
    // 假设mess是一个有效的QObject指针，且formChat是mess的一个成员
    engine.rootContext()->setContextProperty("formChat", &mess->formChat);
    engine.loadFromModule("Messenger", "Main");

    return app.exec();//运行应用程序
}
