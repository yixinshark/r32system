//
// Created by yixin on 2023/7/23.
//

#include "setaddresscmd.h"
#include "recorddata.h"
#include "handledatabase.h"
#include "r32constant.h"

#include <QVariantMap>

SetAddressCmd::SetAddressCmd()
{

}

SetAddressCmd::~SetAddressCmd()
{

}

void SetAddressCmd::initCmd()
{

}

int SetAddressCmd::waitSecs()
{
    return 0;
}

QString SetAddressCmd::cmdInfo()
{
    QString info;
    if (m_sender)
        info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 命令码:0x" + QString::number(m_cmdCode,16);
    int currentChannel = m_currentChannel == 0 ? m_fromChannel : m_currentChannel;
    info += " 设置地址:" + QString::number(m_currentChannel==0 ? m_fromChannel : m_currentChannel);
    info += " 内容:" + getSendData(currentChannel).toHex();

    return info;
}

void SetAddressCmd::execute()
{
    if (m_currentChannel == 0) {
        m_currentChannel = m_fromChannel;
    }

    if (m_sender) {
        m_sendData = getSendData(m_currentChannel);
        // 为了区分设置地址和标定浓度，给发送的数据加上一个标志位
        m_sendData[6] = 0x01;
        m_sender->sendCmdData(m_sendData);

        m_sentCount++;
    }
}

bool SetAddressCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        if(!m_executeSuccess) {
            m_errInfo = QString("设置地址:%1命令0x%2,执行失败").arg(m_currentChannel).arg(QString::number(m_cmdCode,16));
        } else {
            m_errInfo = QString("设置地址:%1命令0x%2,执行成功").arg(m_currentChannel).arg(QString::number(m_cmdCode,16));
        }
        m_currentChannel++;
        m_sentCount = 0;
        return true;
    } else {
        return false;
    }
}

bool SetAddressCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString SetAddressCmd::exeErrInfo()
{
    return m_errInfo;
}

void SetAddressCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令0x%2,执行超时").arg(m_sender->senderName()).arg(QString::number(m_cmdCode,16));
}

void SetAddressCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        m_executeSuccess = true;
        RecordData::instance()->setModuleAddress(m_currentChannel);
    } else {
        m_errInfo = QString("获取错误ACK:0x%1").arg(QString::number(cmd,16));
    }
}

QByteArray SetAddressCmd::getSendData(int channel)
{
    QVariantMap info;
    info.insert(MODULE_ADDRESS, true);
    info.insert(SET_MODULE_ADDRESS, channel);

    return m_sender->getSendData((char)m_cmdCode, info);
}
