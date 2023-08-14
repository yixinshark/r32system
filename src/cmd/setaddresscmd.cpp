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
    info += " 设置地址:" + QString::number(RecordData::instance()->getcurrentChanel());

    return info;
}

void SetAddressCmd::execute()
{
    if (m_sender) {
        QByteArray data = getSendBaseData();
        // 为了区分设置地址和标定浓度，给发送的数据加上一个标志位
        data[6] = 0x01;
        m_sender->sendCmdData(data);

        m_sentCount++;
    }
}

bool SetAddressCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        if(!m_executeSuccess) {
            m_errInfo = QString("设置地址:%1命令0x%2,执行失败").arg(RecordData::instance()->getcurrentChanel()).arg(QString::number(m_cmdCode,16));
        } else {
            m_errInfo = QString("设置地址:%1命令0x%2,执行成功").arg(RecordData::instance()->getcurrentChanel()).arg(QString::number(m_cmdCode,16));
        }
        m_sentCount = 0;
        // 组合命令中复用了该命令，所以需要重置
        m_executeSuccess = false;
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
        RecordData::instance()->setModuleAddress(RecordData::instance()->getcurrentChanel());
    } else {
        m_errInfo = QString("获取错误ACK:0x%1").arg(QString::number(cmd,16));
    }
}

QByteArray SetAddressCmd::getSendBaseData()
{
    QByteArray data;

    data.append( 0x65);
    data.append( (char)0x00);
    data.append(static_cast<char>(m_cmdCode));

    data.append(static_cast<char>(RecordData::instance()->getcurrentChanel()));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));

    return data;
}
