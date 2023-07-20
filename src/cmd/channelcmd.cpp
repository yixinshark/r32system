//
// Created by yixin on 2023/7/20.
//

#include "channelcmd.h"
#include "handledatabase.h"

ChannelCmd::ChannelCmd()
{

}

ChannelCmd::~ChannelCmd()
{

}

void ChannelCmd::initCmd()
{

}

int ChannelCmd::waitSecs()
{
    return 0;
}

QString ChannelCmd::cmdInfo()
{
    QString info;
    if (m_sender)
        info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 通道:" + QString::number(m_currentChannel);
    info += " 内容:" + m_sendData.toHex();

    return info;
}

void ChannelCmd::execute()
{
    if (m_currentChannel == 0) {
        m_currentChannel = m_fromChannel;
    } else {
        m_sendData[2] = static_cast<char>(m_currentChannel);
    }

    if (m_sender) {
        m_sendData = getSendData();
        m_sender->sendCmdData(m_sendData);

        m_currentChannel++;
        m_sentCount++;
    }
}

bool ChannelCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        return true;
    } else {
        return false;
    }
}

bool ChannelCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString ChannelCmd::exeErrInfo()
{
    return m_errInfo;
}

void ChannelCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令%2,执行超时").arg(m_sender->senderName()).arg(m_cmdCode);
}

void ChannelCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        m_executeSuccess = true;
    } else {
        m_errInfo = QString("获取错误ACK:%1").arg(cmd);
    }
}

QByteArray ChannelCmd::getSendData() const
{
    QByteArray data;

    data.append( 0x61);
    data.append(static_cast<char>(m_cmdCode));

    data.append(static_cast<char>(m_currentChannel));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    addCheckSum(data);

    return data;
}

void ChannelCmd::addCheckSum(QByteArray &data) const
{
    quint16 checksum = 0;
    for (char i : data)
        checksum += static_cast<quint8>(i);

    data.append(static_cast<char>((checksum >> 8) & 0xFF));
    data.append(static_cast<char>(checksum & 0xFF));
}
