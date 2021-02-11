#include "device.h"

device::device(QSerialPort *serialPort,QObject *parent) : QObject(parent)
{
    this->serialPort=serialPort;
    qInfo()<<"Cons Thread Name"<<QString::number((quint64)QThread::currentThread(), 16);


}

void device::threadStart()
{
    qInfo()<<"Device thread Started!";
    this->moveToThread(&threadDevice);
    connect(&threadDevice,SIGNAL(started()),this,SLOT(threadProcess()));
    connect(&threadDevice,SIGNAL(finished()),&threadDevice,SLOT(deleteLater()));

    this->serialPort->moveToThread(&threadDevice);
    threadDevice.start(QThread::Priority::NormalPriority);

}

void device::sendPoll()
{
    QByteArray senddata;
    senddata.clear();

    senddata[0]=0x50;
    senddata[1]=0x20;
    senddata[2]=0xFA;
    if(this->writeSerialPort(senddata)!=0){
        qInfo()<<"<<"<<this->readSerialPort().toHex();
    }

}

quint16 device::writeSerialPort(QByteArray data)
{
    if(this->serialPort->isOpen() && data.length()>0){
        this->serialPort->clear(QSerialPort::AllDirections);
        this->serialPort->write(data);
        if(this->serialPort->waitForBytesWritten(-1)){
            qInfo()<<">>"<<data.toHex();
            return data.length();
        }
    }
    return -1;

}

QByteArray device::readSerialPort()
{

    QByteArray dataReceive;
    dataReceive.clear();
    if(this->serialPort->isOpen()){
        while (this->serialPort->waitForReadyRead(500)) {
            dataReceive+=this->serialPort->readAll();
            if(dataReceive.contains(0xFA)){
                this->serialPort->clear(QSerialPort::Input);
                return dataReceive;
            }
        }
    }
    else{
        this->serialPort->open(QIODevice::ReadWrite);
    }
    return dataReceive;

}

void device::threadProcess()
{
    /*
     * Mesaj yazmak için bekleme süesini -1 yani yazana kadar yaptım
     * Mesaj alma zamanını 500 ms yaptım. Eger ki 500 ms den daha önce mesaj gelirse ve içerinde FA olursa 500 beklenmeden çıkıyor fakat bu şart sağlanmazsa 500 ms bekliyor
     * 200 tüm theread uyutuluyor. Bunu koymazsam cevap aldığım an diğer mesajı basarım, karşı cihaz bunu kabul etmeyebilir.
     */
    qInfo()<<"Thread Process  Thread Name"<<QString::number((quint64)QThread::currentThread(), 16);
    while (!threadTerminate) {
        qDebug()<<"GIR"<<QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
        this->sendPoll();
        qDebug()<<"CIK"<<QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
        QThread::msleep(200);
        qDebug()<<"************************************************************";
    }

    threadDevice.quit();

}

void device::deleteLater()
{
    qInfo()<<"Thread Terminated!"<<QString::number((quint64)QThread::currentThread(), 16);;


}
