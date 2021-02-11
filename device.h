#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QDebug>
#include <QDateTime>

class device : public QObject
{
    Q_OBJECT
public:
    explicit device(QSerialPort *serialPort,QObject *parent = nullptr);

    void threadStart();

signals:

private:
    bool threadTerminate=false;

    QSerialPort *serialPort;
    QThread threadDevice;

    // Communication Functions
    void sendPoll();
    quint16 writeSerialPort(QByteArray data);
    QByteArray readSerialPort();


public slots:

private slots:
    void threadProcess();
    void deleteLater();
};

#endif // DEVICE_H
