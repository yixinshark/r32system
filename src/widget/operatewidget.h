//
// Created by zyz on 23-5-12.
//

#ifndef R32_OPERATEWIDGET_H
#define R32_OPERATEWIDGET_H

#include <QWidget>

class QLabel;
class QTextEdit;
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
    // r32传感器串口连接
    ConnectWidget *m_r32ConnectWidget;
    // r32分析仪串口连接
    ConnectWidget *m_r32AnaConnectWidget;
    // mcu串口连接
    ConnectWidget *m_mcuConnectWidget;

    QPushButton *m_startBtn;
    QTextEdit *m_optMsgLabel;
    QTableWidget *m_tableWidget;
    // 控制命令流程
    ControlCmdFlow *m_controlCmdFlow;
    bool m_started = false;
    QStringList m_msgList;
};

#endif //R32_OPERATEWIDGET_H
