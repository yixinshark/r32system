//
// Created by yixin on 2023/7/21.
//

#ifndef R32SYSTEM_CALCCRCMD_H
#define R32SYSTEM_CALCCRCMD_H

#include "basecmd.h"

/*!
 * @brief 标定浓度指令
 */
class CalCcrCmd : public BaseCmd
{
    public:
    CalCcrCmd();
    ~CalCcrCmd() override;

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

    void setR32Point(int point) { m_r32Point = point; }
    void setCalCcrPoint(int point) { m_point = point; }
    void setSender(HandleDataBase *sender) { m_sender = sender; }
    void setCmdCode(int cmdCode) { m_cmdCode = cmdCode; }

private:
    bool m_executeSuccess = false;
    int m_point = 0;
    int m_r32Point = 0;
    int m_sentCount = 0;
    int m_cmdCode = 0;
    QString m_errInfo;

    HandleDataBase *m_sender;
};


#endif //R32SYSTEM_CALCCRCMD_H
