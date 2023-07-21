//
// Created by zyz on 23-7-18.
//

#include "compoundcmd.h"

CompoundCmd::CompoundCmd()
{
    m_cmdType = CMD_TYPE_MULTI;
}

CompoundCmd::~CompoundCmd()
{
    if (m_cmdQueue.isEmpty())
        return;

    for (auto *cmd : m_cmdQueue)
        delete cmd;
}

void CompoundCmd::initCmd()
{

}

int CompoundCmd::waitSecs()
{
    if (m_curCmd)
        return m_curCmd->waitSecs();

    auto *cmd = m_cmdQueue.first();
    if (cmd)
        return cmd->waitSecs();

    return 0;
}

QString CompoundCmd::cmdInfo()
{
    QString info;
    info = QString("通道%1: ").arg(m_loopIndex-1);

    if (m_curCmd)
        info += m_curCmd->cmdInfo();

    return info;
}

void CompoundCmd::execute()
{
    if (m_curCmd) {
        m_curCmd->execute();
        return;
    }

    auto *cmd = m_cmdQueue.dequeue();
    if (cmd) {
        m_curCmd = cmd;
        cmd->execute();
    }

    m_cmdQueueBak.enqueue(cmd);

    if (m_cmdQueue.isEmpty()) {
        m_loopIndex++;

        if (m_loopIndex >= m_loopCount) {
            m_overed = true;
        } else {
            m_cmdQueue = m_cmdQueueBak;
            m_cmdQueueBak.clear();
        }
    }
}

bool CompoundCmd::exeOvered()
{
    return m_overed;
}

bool CompoundCmd::exeSuccess()
{
    return false;
}

QString CompoundCmd::exeErrInfo()
{
    return QString();
}

void CompoundCmd::recvCmdAckData(quint8 cmdCode)
{
    if (m_curCmd)
        m_curCmd->recvCmdAckData(cmdCode);

    if (m_curCmd->exeOvered()) {
        m_curCmd = nullptr;
        execute();
    }
}

void CompoundCmd::recvAckTimeout()
{
    if (m_curCmd)
        m_curCmd->recvAckTimeout();

    if (m_curCmd->exeOvered()) {
        m_curCmd = nullptr;
        execute();
    }
}
