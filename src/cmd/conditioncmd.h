//
// Created by yixin on 2023/7/19.
//

#ifndef R32SYSTEM_CONDITIONCMD_H
#define R32SYSTEM_CONDITIONCMD_H

#include "basecmd.h"

/*!
 * \class 条件等待命令
 */
class ConditionCmd : public BaseCmd
{
public:
    ConditionCmd();
    ~ConditionCmd() override;

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

    void setEnable(bool enable) { m_enable = enable; }
    void setWaitSecs(int secs) { m_waitSecs = secs; }
    void setPrecision(float precision) { m_precision = precision; }
    void setCondition(int condition) { m_targetConcentration = condition; }
    void setCmdCode(int cmdCode) { m_cmdCode = cmdCode; }
    int cmdCode() const { return m_cmdCode; }
    // 命令发送者(发送者类型)
    void setSender(HandleDataBase *sender) { m_sender = sender; }

private:
    bool m_enable = true;
    int m_waitSecs = 0;
    // 精度
    float m_precision = 0.0;
    // 目标浓度
    int m_targetConcentration = 0;
    bool m_overed = false;
    int m_cmdCode = 0;
    HandleDataBase *m_sender;
};


#endif //R32SYSTEM_CONDITIONCMD_H
