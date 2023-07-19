//
// Created by yixin on 2023/7/18.
//

#ifndef R32SYSTEM_CONTROLCMDFLOW_H
#define R32SYSTEM_CONTROLCMDFLOW_H

#include <QObject>
#include <QTimer>

class BaseCmd;
class RecordData;
class HandleDataBase;

class ControlCmdFlow : public QObject
{
    Q_OBJECT
public:
    explicit ControlCmdFlow(QObject *parent = nullptr);
    ~ControlCmdFlow() override;

signals:
    void cmdexecuted(const QString &info);

public:
    void start();
    void recvAck(char cmd, const QVariantMap &info);

public:
    void setR32AnaDataHandler(HandleDataBase *handler);
    void setMcuDataHandler(HandleDataBase *handler);
    void setR32DataHandler(HandleDataBase *handler);
    void initCmdFlow();

private:
    void timerTimeout();
    void executeCmdFlow();

private:
    // 选择通道
    BaseCmd *initSwitchChannel(int channel);
    // 读取传感器地址
    BaseCmd *initReadSensorAddress();
    // 读取固件版本号
    BaseCmd *initReadFirmwareVersion();
    // 等待3分钟
    BaseCmd *initWait3Minutes();
    // 打开电磁阀
    BaseCmd *initOperateValve();
    // 操作风扇
    BaseCmd *initOperateFan();
    // 浓度5000点等待
    BaseCmd *initWait5000Concentration();
    // 关闭电磁阀
    BaseCmd *initCloseValve();
    // 关闭风扇
    BaseCmd *initCloseFan();
    // 读取传感器浓度
    BaseCmd *initMarkConcentration(int point, int concentration);
    // 读取电阻值
    BaseCmd *initReadResistance(int concentration);

    // 标定传感器浓度
    BaseCmd *initGasPointCalibration(int point, int concentration);

private:
    HandleDataBase *m_r32AnaDataHandler = nullptr;
    HandleDataBase *m_mcuDataHandler = nullptr;
    HandleDataBase *m_r32DataHandler = nullptr;

private:
    QTimer *m_timer;
    RecordData *m_recordData;

private:
    // 标定命令控制流
    QList<BaseCmd *> m_calibrationCmdFlow;
    QList<BaseCmd *> m_detectCmdFlow;
};

#endif //R32SYSTEM_CONTROLCMDFLOW_H
