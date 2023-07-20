//
// Created by yixin on 2023/7/20.
//

#ifndef R32SYSTEM_CHANNELCMD_H
#define R32SYSTEM_CHANNELCMD_H

#include "basecmd.h"
#include "handledatabase.h"

class ChannelCmd : public BaseCmd
{
public:
    ChannelCmd();
    ~ChannelCmd() override;

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

    void setFromChannel(int channel) { m_fromChannel = channel; }
    //void setToChannel(int channel) { m_toChannel = channel; }
    void setSender(HandleDataBase *sender) { m_sender = sender; }
    void setCmdCode(int cmdCode) { m_cmdCode = cmdCode; }

private:
    QByteArray getSendData() const;
    void addCheckSum(QByteArray &data) const;

private:
    int m_fromChannel = 0;
    //int m_toChannel = 0;
    int m_currentChannel = 0;
    bool m_executeSuccess = false;
    int m_sentCount = 0;
    int m_cmdCode = 0;
    QString m_errInfo;

    HandleDataBase *m_sender;
    QByteArray m_sendData;
};


#endif //R32SYSTEM_CHANNELCMD_H
