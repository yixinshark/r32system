//
// Created by yixin on 2023/7/19.
//

#ifndef R32SYSTEM_TIMERCMD_H
#define R32SYSTEM_TIMERCMD_H

#include "basecmd.h"
class QTimer;
class TimerCmd : public BaseCmd
{
public:
    TimerCmd();
    ~TimerCmd() override;

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

    void setWaitSecs(int secs) { m_waitSecs = secs; }

private:
    int m_waitSecs;
    bool m_overed = false;
};


#endif //R32SYSTEM_TIMERCMD_H
