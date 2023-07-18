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
    return QString();
}

void CompoundCmd::execute()
{

}

bool CompoundCmd::exeOvered()
{
    return false;
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

}

void CompoundCmd::recvAckTimeout()
{

}
