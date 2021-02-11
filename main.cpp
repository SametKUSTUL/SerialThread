#include <QCoreApplication>
#include <QSerialPort>
#include <initializer_list>
#include <signal.h>
#include <unistd.h>
#include "device.h"

void catchUnixSignals(std::initializer_list<int> quitSignals) {
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        printf("\nquit the application by signal(%d).\n", sig);
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}

void QCoreApplication::quit(){
    qDebug()<<"PROGRAM BEKLENMEDIK BIR BICIMDE KAPATILDI!";
    qInfo()<<"MAİN THREAD QUIT"<<QString::number((quint64)QThread::currentThread(), 16);
    QThread::currentThread()->deleteLater();
    QCoreApplication::exit(0);

}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP,SIGPWR});
    QSerialPort *serial=new QSerialPort(&a);
    serial->setPortName("/dev/ttyRS1");
    qInfo()<<"MAİN THREAD"<<QString::number((quint64)QThread::currentThread(), 16);
    serial->open(QIODevice::ReadWrite);
    if(serial->isOpen()){
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setBaudRate(QSerialPort::Baud9600);
        device *dev=new device(serial);
        dev->threadStart();

    }

    return a.exec();
}
