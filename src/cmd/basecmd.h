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
    BaseCmd();
    virtual ~BaseCmd();

public:
//    int waitSecs = 0 ; // 等待时间
//    int cmdCode;       // 命令码

//    enum SenderType {
//        SENDER_NONE = 0,         // 无
//        SENDER_R32,              // R32
//        SENDER_R32_ANALYSER,     // R32分析仪
//        SENDER_MCU               // MCU单片机
//    };
//
//    enum CmdType {
//        CMD_TYPE_SINGLE,         // 单个指令
//        CMD_TYPE_MULTI           // 多个指令
//    };

public:
    // 初始化命令
    void initCmd() = 0;
    // 等待时间之后执行命令
    int waitSecs() = 0;
    // 命令信息
    virtual QString cmdInfo() = 0;
    // 执行命令
    virtual void execute() = 0;
    // 命令是否执行完毕,判断标准是命令执行并且收到回执，或者命令执行超时
    virtual bool exeOvered() = 0;
    // 命令是否执行成功
    virtual bool exeSuccess() = 0;
    // 命令执行错误信息
    virtual QString exeErrInfo() = 0;
    void setExeErrInfo(const QString &errInfo) { m_errInfo = errInfo; }
    // 接收命令回执
    virtual void recvCmdAckData(int senderType, quint8 cmd) = 0;
    // 命令发送者(发送者类型)
    void setSender(HandleDataBase *sender) { m_sender = sender; }

protected:
//    CmdType m_cmdType;
    HandleDataBase *m_sender;
    QString m_errInfo;

//protected:
//    // 命令发送者
//    QMap<SenderType, HandleDataBase *> m_cmdSenderMap;
};

#endif //R32SYSTEM_BASECMD_H

