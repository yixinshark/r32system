//
// Created by zyz on 23-6-1.
//

#include "handledatabase.h"
#include "serialportcom.h"

HandleDataBase::HandleDataBase(SerialPortCom *serialPortCom, QObject *parent)
    : QObject(parent)
    , m_serialPortCom(serialPortCom)
{
    connect(m_serialPortCom, &SerialPortCom::dataReceived, this, &HandleDataBase::processReceivedData);
}

HandleDataBase::~HandleDataBase() {

}

QByteArray HandleDataBase::getSendData(char cmd, const QVariantMap &info)
{
    QByteArray data;
    addContent(cmd, info, data);

    return data;
}

void HandleDataBase::addCheckSum(QByteArray &data)
{
    quint16 checksum = 0;
    for (char i : data)
        checksum += static_cast<quint8>(i);

    data.append(static_cast<char>((checksum >> 8) & 0xFF));
    data.append(static_cast<char>(checksum & 0xFF));
}

void HandleDataBase::sendCmdData(const QByteArray &data)
{
    if (!m_serialPortCom)
        return;

    m_serialPortCom->sendData(data);
}
