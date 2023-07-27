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
    // 完成任务
    void exceuteOvered();

public:
    void start();
    void stop();
    void recvAck(char cmd, const QVariantMap &info);

    void setCalibrationValues(const QVector<int> &values) { m_calibrationValues = values; }
    void setDetectionValues(const QVector<int> &values) { m_detectionValues = values; }
    void setFromChannel(int channel) { m_fromChannel = channel; }
    void setTotalChannel(int channel) { m_totalChannel = channel; }
    void setCalibrationMode(bool mode) { m_calibrationMode = mode; }
    void setDetectMode(bool mode) { m_detectMode = mode; }

public:
    void setR32AnaDataHandler(HandleDataBase *handler);
    void setMcuDataHandler(HandleDataBase *handler);
    void setR32DataHandler(HandleDataBase *handler);

private:
    void initCalibrationCmdFlow();
    void initDetectCmdFlow();
    void timerTimeout();
    void executeCmdFlow();

private:
    // 轮询上电检测
    BaseCmd *initPowerOnDetect();
    // 选择通道
    BaseCmd *initSwitchChannel(int channel);
    // 读取传感器地址
    BaseCmd *initReadSensorAddress();
    // 读取固件版本号
    BaseCmd *initReadFirmwareVersion();
    // 等待3分钟
    BaseCmd *initWait3Minutes();
    // 操作电磁阀
    BaseCmd *initOperateValve(bool open1, bool open2, bool open3, bool open4);
    // 操作风扇
    BaseCmd *initOperateFan(bool open1, bool open2, bool open3, bool open4, int waitSecs = 0);
    // 浓度点等待
    BaseCmd *initWaitConcentration(int point);
    // 关闭电磁阀
    BaseCmd *initCloseValve();
    // 关闭风扇
    BaseCmd *initCloseFan();
    // 读取传感器浓度
    BaseCmd *initMarkConcentration(int point);
    // 读取电阻值
    BaseCmd *initReadResistance(int concentration);

    // 标定传感器浓度
    BaseCmd *initGasPointCalibration(int point, int concentration);
    // 关闭分析仪获取数据
    BaseCmd *initCloseR32AnaGetGasData();
    // 标定完成
    BaseCmd *initCalibrationOver();

    // 获取气体浓度值
    BaseCmd *initGetGasConcentration(int concentration);


private:
    HandleDataBase *m_r32AnaDataHandler = nullptr;
    HandleDataBase *m_mcuDataHandler = nullptr;
    HandleDataBase *m_r32DataHandler = nullptr;

private:
    QTimer *m_timer;
    RecordData *m_recordData;

    bool m_isStarted = false;
    int m_fromChannel = 0;
    int m_totalChannel = 0;
    // 标定模式
    bool m_calibrationMode = false;
    // 检测模式
    bool m_detectMode = false;

private:
    // 命令控制流
    QList<BaseCmd *> m_controlCmdFlow;

    QVector<int> m_calibrationValues;
    QVector<int> m_detectionValues;
};

#endif //R32SYSTEM_CONTROLCMDFLOW_H
