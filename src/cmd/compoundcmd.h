//
// Created by zyz on 23-7-18.
//

#ifndef R32SYSTEM_COMPOUNDCMD_H
#define R32SYSTEM_COMPOUNDCMD_H

#include "basecmd.h"

#include <QList>

/*!
 * \brief 复合指令类, 由多个单一指令组成。所有单一指令执行完毕后，复合指令执行完毕
 */
class CompoundCmd : public BaseCmd
{
public:
    CompoundCmd();
    ~CompoundCmd() override;

public:
    // 初始化命令
    void initCmd() override;
    // 等待时间之后执行命令
    int waitSecs() override;
    // 命令信息
    QString cmdInfo() override;
    // 执行命令
    void execute() override;
    // 命令是否执行完毕,判断标准是命令执行并且收到回执，或者命令执行超时
    bool exeOvered() override;
    // 命令是否执行成功
    bool exeSuccess() override;
    // 命令执行错误信息
    QString exeErrInfo() override;
    // 命令执行超时
    void recvAckTimeout() override;
    // 接收命令回执
    void recvCmdAckData(quint8 cmd) override;

private:
    QList<BaseCmd *> m_cmdList;
};

#endif //R32SYSTEM_COMPOUNDCMD_H
