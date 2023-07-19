//
// Created by yixin on 2023/7/19.
//

#include "conditioncmd.h"
#include "handledatabase.h"

ConditionCmd::ConditionCmd()
{

}

ConditionCmd::~ConditionCmd()
{

}

void ConditionCmd::initCmd()
{

}

int ConditionCmd::waitSecs()
{
    return 0;
}

QString ConditionCmd::cmdInfo()
{
    return QString("获取当前浓度，目标浓度：%1").arg(m_targetConcentration);
}

void ConditionCmd::execute()
{
    if (m_sender) {
        m_sender->sendCmdData(m_sendData);
    }
}

bool ConditionCmd::exeOvered()
{
    return m_overed;
}

bool ConditionCmd::exeSuccess()
{
    return false;
}

QString ConditionCmd::exeErrInfo()
{
    return QString();
}

void ConditionCmd::recvAckTimeout()
{

}

void ConditionCmd::recvCmdAckData(quint8 cmd)
{

}

void ConditionCmd::setCurrentConcentration(int concentration)
{
    if (concentration >= m_targetConcentration) {
        m_overed = true;
    }
}
