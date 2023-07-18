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
    int waitSecs() override { return m_waitSecs; }
    // 设置发送数据
    void setSendData(const QByteArray &data) override;
    // 命令信息
    QString cmdInfo() override;
    // 执行命令
    void execute() override;
    // 命令是否执行完毕,判断标准是命令执行并且收到回执，或者命令执行超时
    bool exeOvered() override;
    // 命令是否执行成功
    bool exeSuccess() override;
    // 命令执行超时
    void recvAckTimeout() override;
    // 命令执行错误信息
    QString exeErrInfo() override;
    // 接收命令回执
    void recvCmdAckData(quint8 cmdCode) override;
    // 执行错误信息
    void setExeErrInfo(const QString &errInfo) { m_errInfo = errInfo; }
    // 命令发送者(发送者类型)
    void setSender(HandleDataBase *sender) { m_sender = sender; }

public:
    void setWaitSecs(int secs) { m_waitSecs = secs; }
    void setCmdCode(int code) { m_cmdCode = code; }

private:
    int m_waitSecs = 0 ;    // 等待时间,单位秒
    int m_cmdCode = 0;      // 命令码
    int m_sentCount = 0;    // 已发送次数
    bool m_executeSuccess = false;
    HandleDataBase *m_sender;
    QString m_errInfo;
    QByteArray m_sendData;
};

#endif //R32SYSTEM_SINGLECMD_H
