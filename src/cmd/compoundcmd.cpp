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

}

void CompoundCmd::initCmd()
{

}

int CompoundCmd::waitSecs()
{
    return 0;
}

QString CompoundCmd::cmdInfo()
{
    if (m_curCmd)
        return m_curCmd->cmdInfo();

    return QString("标定浓度中");
}

void CompoundCmd::execute()
{
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