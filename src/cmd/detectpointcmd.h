//
// Created by yixin on 2023/8/10.
//

#ifndef R32SYSTEM_DETECTPOINTCMD_H
#define R32SYSTEM_DETECTPOINTCMD_H

#include "basecmd.h"

/*!
 * @brief 检测浓度指令
 */
class DetectPointCmd : public BaseCmd
{
public:
    DetectPointCmd();
    ~DetectPointCmd() override;

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

    void setDetectPoint(int point) { m_point = point; }
    void setSender(HandleDataBase *sender);
    void setCmdCode(int cmdCode) { m_cmdCode = cmdCode; }

private:
    bool m_executeSuccess = false;
    int m_point = 0;
    int m_sentCount = 0;
    int m_cmdCode = 0;
    QString m_errInfo;

    QByteArray m_sendData;
    HandleDataBase *m_sender;
};


#endif //R32SYSTEM_DETECTPOINTCMD_H
