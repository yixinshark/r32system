//
// Created by zyz on 23-7-18.
//

#ifndef R32SYSTEM_SINGLECMD_H
#define R32SYSTEM_SINGLECMD_H

#include "basecmd.h"
/*!
 * \brief 单一指令类
 */
class SingleCmd : public BaseCmd
{
public:
    SingleCmd();
    ~SingleCmd() override;

public:
    // 初始化命令
    void initCmd() override;
    // 等待时间之后执行命令
    int waitSecs() override { return waitSecs; }
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
    // 接收命令回执
    void recvCmdAckData(int senderType, quint8 cmd) override;

public:
    void setWaitSecs(int secs) { waitSecs = secs; }
    void setCmdCode(int code) { cmdCode = code; }

private:
    int waitSecs = 0 ; // 等待时间,单位秒
    int cmdCode;       // 命令码
    QByteArray m_sendData;
};

#endif //R32SYSTEM_SINGLECMD_H
