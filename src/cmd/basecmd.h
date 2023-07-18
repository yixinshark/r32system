//
// Created by zyz on 23-7-18.
//

#ifndef R32SYSTEM_BASECMD_H
#define R32SYSTEM_BASECMD_H

# include <QString>

class HandleDataBase;
class BaseCmd
{
public:
    BaseCmd() { }
    virtual ~BaseCmd() { }

    enum CmdType {
        CMD_TYPE_SINGLE,         // 单个指令
        CMD_TYPE_MULTI           // 多个指令
    };

public:
    void setCmdType(CmdType type) { m_cmdType = type; }
    // 初始化命令
    virtual void initCmd() = 0;
    // 等待时间之后执行命令
    virtual int waitSecs() = 0;
    // 设置发送数据
    virtual void setSendData(const QByteArray &data) { };
    // 命令信息
    virtual QString cmdInfo() = 0;
    // 执行命令
    virtual void execute() = 0;
    // 命令是否执行完毕,判断标准是命令执行并且收到回执，或者命令执行超时
    virtual bool exeOvered() = 0;
    // 命令是否执行成功
    virtual bool exeSuccess() = 0;
    // 命令执行超时
    virtual void recvAckTimeout() = 0;
    // 命令执行错误信息
    virtual QString exeErrInfo() = 0;
    // 接收命令回执
    virtual void recvCmdAckData(quint8 cmdCode) = 0;

protected:
    CmdType m_cmdType;
};

#endif //R32SYSTEM_BASECMD_H

