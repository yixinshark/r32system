//
// Created by zyz on 23-5-12.
//

#ifndef R32_OPERATEWIDGET_H
#define R32_OPERATEWIDGET_H

#include <QWidget>

class QTimer;
class QLabel;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QTableWidget;
class ConnectWidget;
class ControlCmdFlow;

class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);
    ~OperateWidget() override;

private:
    void initUI();
    void initTableWidget();
    void showOperateMsg(const QString &msg);
    void startBtnClicked();

private:
    // 初始化通道设置UI
    QLayout *initSettingUI();
    // 初始化标定点位设置UI
    QLayout *initCalibrationUI();
    // 初始化功能按钮UI
    QLayout *initBtnsUI();
    // 初始化报表信息UI
    //QLayout *initReportUI();

private:
    void updateTableWidget();
    // 数据同步到数据库
    void syncDataToDB();

private:
    // r32传感器串口连接
    ConnectWidget *m_r32ConnectWidget;
    // r32分析仪串口连接
    ConnectWidget *m_r32AnaConnectWidget;
    // mcu串口连接
    ConnectWidget *m_mcuConnectWidget;

    QLineEdit *m_tsiLineEdit;

    QPushButton *m_startBtn;
    QTextEdit *m_optMsgLabel;
    QTableWidget *m_tableWidget;
    // 控制命令流程
    ControlCmdFlow *m_controlCmdFlow;
    QTimer *m_timer;

    bool m_started = false;
    int m_fromChannel = 0;
    int m_totalChannel = 0;
    QStringList m_msgList;
};

#endif //R32_OPERATEWIDGET_H
