//
// Created by yixin on 2023/7/18.
//

#ifndef R32SYSTEM_CONTROLCMDFLOW_H
#define R32SYSTEM_CONTROLCMDFLOW_H

#include <QObject>

class BaseCmd;
class HandleDataBase;
class ControlCmdFlow : public QObject
{
    Q_OBJECT
public:
    explicit ControlCmdFlow(QObject *parent = nullptr);
    ~ControlCmdFlow() override;

    void initCmdFlow();
    void start();

public:
    void setR32AnaDataHandler(HandleDataBase *handler);
    void setMcuDataHandler(HandleDataBase *handler);
    void setR32DataHandler(HandleDataBase *handler);

private:
    BaseCmd *initSwitchSensorChannel(int channel);
    BaseCmd *initGasPointCalibration(int point, quint16 gasConcentration);


private:
    HandleDataBase *m_r32AnaDataHandler = nullptr;
    HandleDataBase *m_mcuDataHandler = nullptr;
    HandleDataBase *m_r32DataHandler = nullptr;

private:
    // 标定命令控制流
    QVector<BaseCmd *> m_calibrationCmdFlow;
    QVector<BaseCmd *> m_detectCmdFlow;
};

#endif //R32SYSTEM_CONTROLCMDFLOW_H
