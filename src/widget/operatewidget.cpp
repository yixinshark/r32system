//
// Created by zyz on 23-5-12.
//

#include "operatewidget.h"

#include "handler32data.h"
#include "handlemcudata.h"
#include "handleanalyserdata.h"
#include "connectwidget.h"
#include "controlcmdflow.h"
#include "recorddata.h"

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDateTime>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QMessageBox>

OperateWidget::OperateWidget(QWidget *parent)
    : QWidget(parent)
    , m_r32ConnectWidget(new ConnectWidget("R32传感器", this))
    , m_r32AnaConnectWidget(new ConnectWidget("分析仪", this))
    , m_mcuConnectWidget(new ConnectWidget("单片机", this))
    , m_startBtn(new QPushButton(tr("开始"), this))
    , m_optMsgLabel(new QTextEdit(this))
    , m_tableWidget(new QTableWidget(this))
    , m_controlCmdFlow(new ControlCmdFlow(this))
    , m_timer(new QTimer(this))
{
    // 初始化界面
    initUI();
    // 单片机数据处理
    auto *handlerMcuData = new HandleMcuData(m_mcuConnectWidget->serialPortCom(), this);
    // r32数据处理
    auto *handler32Data = new Handler32data(m_r32ConnectWidget->serialPortCom(), this);
    // 分析仪数据处理
    auto *handlerAnalyserData = new HandleAnalyserData(m_r32AnaConnectWidget->serialPortCom(), this);

    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &OperateWidget::updateTableWidget);

    // 设置数据发送、处理
    m_controlCmdFlow->setMcuDataHandler(handlerMcuData);
    m_controlCmdFlow->setR32DataHandler(handler32Data);
    m_controlCmdFlow->setR32AnaDataHandler(handlerAnalyserData);

    connect(handlerMcuData, &HandleMcuData::frameReceived,
            m_controlCmdFlow, &ControlCmdFlow::recvAck);
    connect(handler32Data, &Handler32data::frameReceived,
            m_controlCmdFlow, &ControlCmdFlow::recvAck);
    connect(handlerAnalyserData, &HandleAnalyserData::frameReceived,
            m_controlCmdFlow, &ControlCmdFlow::recvAck);

    // 开始执行
    connect(m_startBtn, &QPushButton::clicked,this, &OperateWidget::startBtnClicked);
    // 命令执行提示信息
    connect(m_controlCmdFlow, &ControlCmdFlow::cmdexecuted,
            this, &OperateWidget::showOperateMsg);

    connect(m_controlCmdFlow, &ControlCmdFlow::exceuteOvered, this, [this](){
        showOperateMsg("-----任务执行完成-----");
        m_startBtn->setText("开始");
        m_started = false;

        // 1s后停止更新表格
        QTimer::singleShot(1000, this, [this]{
            if (m_timer->isActive()) {
                m_timer->stop();
            }
        });

        // 提示执行完成
        QMessageBox::information(nullptr, tr("提示"), tr("任务执行完成,请确保数据保存!"), QMessageBox::Ok);
    });
}

OperateWidget::~OperateWidget()
{

}

void OperateWidget::initUI()
{
    // 添加一个groupbox
    auto *groupBox = new QGroupBox(tr("基本参数"), this);

    auto *groupBox1 = new QGroupBox(this);
    groupBox1->setContentsMargins(0, 0, 0, 0);
    groupBox1->setLayout(initSettingUI());

    // 标定点位个数：1-3；检测点位个数：1-5
    auto *groupBox2 = new QGroupBox(this);
    groupBox2->setContentsMargins(0, 0, 0, 0);
    groupBox2->setLayout(initCalibrationUI());

    auto *hLayout2 = new QHBoxLayout();
    hLayout2->setSpacing(20);
    hLayout2->addWidget(m_r32AnaConnectWidget);
    hLayout2->addWidget(m_r32ConnectWidget);
    hLayout2->addWidget(m_mcuConnectWidget);

    // 多行显示操作内容提示
    m_optMsgLabel->setReadOnly(true);
    m_optMsgLabel->setMaximumHeight(80);

    auto *vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    vLayout->setContentsMargins(10, 20, 10, 5);
    vLayout->addWidget(groupBox1);
    vLayout->addWidget(groupBox2);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(m_optMsgLabel);
    groupBox->setLayout(vLayout);

    // 报表groupbox
    auto *groupBox3 = new QGroupBox(tr("报表"), this);
    initTableWidget();
    auto *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(10, 20, 10, 10);
    vBoxLayout->addWidget(m_tableWidget);
    groupBox3->setLayout(vBoxLayout);

    auto *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(initBtnsUI());
    mainLayout->addWidget(groupBox3);
    //mainLayout->addWidget(m_msgLabel);
    setLayout(mainLayout);
}

void OperateWidget::initTableWidget()
{
    QStringList header;
    header << "传感器ID" << "判定结果" << "ON/OFF" << "软件版本" << "标定点1" << "标定点2" << "标定点3" << "标定状态"
           << "标定的零点阻值R0" << "1000PPM的阻值" << "5000PPM的阻值" << "参数P" << "参数P1" << "参数P2" << "温度" << "湿度";

    m_tableWidget->setColumnCount(header.count());
    m_tableWidget->setRowCount(m_totalChannel);
    m_tableWidget->setMinimumHeight(400);
    // 表格不能编辑
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_tableWidget->setHorizontalHeaderLabels(header);
}

void OperateWidget::showOperateMsg(const QString &msg)
{
    m_optMsgLabel->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + msg);

#if 0
    if (m_msgList.count() == 3) {
        m_msgList.removeFirst();
    }

    m_msgList.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + msg);
    m_optMsgLabel->clear();
    for (const auto &str : m_msgList) {
        m_optMsgLabel->append(str);
    }
#endif
}

QLayout *OperateWidget::initSettingUI()
{
    auto *label = new QLabel(tr("工作模式："), this);
    auto *checkBox = new QCheckBox(tr("标定"), this);
    auto *checkBox2 = new QCheckBox(tr("测试"), this);
    connect(checkBox, &QCheckBox::stateChanged, this,[this](int state){
        m_controlCmdFlow->setCalibrationMode(state == Qt::Checked);
    });
    connect(checkBox2, &QCheckBox::stateChanged, this,[this](int state){
        m_controlCmdFlow->setDetectMode(state == Qt::Checked);
    });

    // 测试个数
    auto *spinBox = new QSpinBox(this);
    spinBox->setMinimumHeight(30);
    spinBox->setRange(1, 100);
    spinBox->setSingleStep(1);
    spinBox->setValue(64);  // 起始默认值
    spinBox->setPrefix("测试个数: ");
    spinBox->setWrapping(true);

    // 显示从第几个开始
    auto *fromComboBox = new QComboBox(this);
    fromComboBox->setMinimumHeight(32);
    for (int i = 1; i <= 100; ++i) {
        fromComboBox->addItem(QString::number(i));
    }
    auto *label2 = new QLabel(tr("-"), this);
    auto *toLineEdit = new QLineEdit(this);
    toLineEdit->setMaximumWidth(50);
    toLineEdit->setReadOnly(true);
    toLineEdit->setText("64"); // 默认值

    connect(fromComboBox, &QComboBox::currentTextChanged,
            this, [this, fromComboBox, spinBox, toLineEdit]{
        int from = fromComboBox->currentText().toInt();
        int to = from + spinBox->value() - 1;
        toLineEdit->setText(QString::number(to));
        m_fromChannel = from;
        m_controlCmdFlow->setFromChannel(from);
    });
    connect(spinBox, &QSpinBox::textChanged, this,
            [this, fromComboBox, spinBox, toLineEdit]{
        int from = fromComboBox->currentText().toInt();
        int to = from + spinBox->value() - 1;
        toLineEdit->setText(QString::number(to));
        m_totalChannel = spinBox->value();
        m_controlCmdFlow->setTotalChannel(m_totalChannel);
    });

    // TSI值；分析仪浓度数
    auto *tsiLabel = new QLabel(tr("TSI值："), this);
    auto *tsiLineEdit = new QLineEdit(this);
    tsiLineEdit->setReadOnly(true);

    auto *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(10, 0, 0, 0);
    hLayout->addWidget(label);
    hLayout->addWidget(checkBox);
    hLayout->addWidget(checkBox2);
    hLayout->addStretch(1);
    hLayout->addWidget(spinBox);
    hLayout->addWidget(fromComboBox);
    hLayout->addWidget(label2);
    hLayout->addWidget(toLineEdit);
    hLayout->addStretch(1);
    hLayout->addWidget(tsiLabel);
    hLayout->addWidget(tsiLineEdit);
    hLayout->addStretch(2);

    return hLayout;
}

QLayout *OperateWidget::initCalibrationUI()
{
    QFont font;
    font.setPixelSize(16);
    auto *label3 = new QLabel(tr("标定点位个数：(1-0,2-1000,3-5000)"), this);
    label3->setFont(font);
    label3->setMinimumHeight(30);
    auto *label4 = new QLabel(tr("检测点位个数：(1-0,2-1000,3-2000,4-3000,5-5000)"), this);
    label4->setFont(font);
    label4->setMinimumHeight(30);
    auto *settingBtn = new QPushButton(tr("设置"), this);

    auto *hLayout = new QHBoxLayout();
    hLayout->setSpacing(26);
    hLayout->setContentsMargins(10, 0, 0, 0);
    hLayout->addWidget(label3);
    hLayout->addWidget(label4);
    hLayout->addWidget(settingBtn);
    hLayout->addStretch();

    return hLayout;
}

QLayout *OperateWidget::initBtnsUI()
{
    // 功能按钮：开始，数据保存，清空数据
    auto *cancelBtn = new QPushButton(tr("取消"), this);
    auto *saveBtn = new QPushButton(tr("数据保存"), this);
    auto *clearBtn = new QPushButton(tr("清空数据"), this);

    connect(cancelBtn, &QPushButton::clicked, this, [this]{
        // 询问是否取消
        if (m_started) {
            if (QMessageBox::question(this, tr("提示"), tr("是否取消当前操作？"), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes) {
                m_controlCmdFlow->stop();
                m_started = false;
            }

            m_startBtn->setText("开始");
            if (m_timer->isActive()) {
                m_timer->stop();
            }
        }
    });

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(clearBtn);

    return btnLayout;
}

void OperateWidget::startBtnClicked()
{
    if (!m_mcuConnectWidget->isConnected() || !m_r32ConnectWidget->isConnected()
        || !m_r32AnaConnectWidget->isConnected()) {
        QMessageBox::warning(this, tr("警告"), tr("请先连接串口"), QMessageBox::Ok);
        return;
    }

    m_started = true;
    m_startBtn->setText("执行中...");
    QTimer::singleShot(0, this, [this] {
        m_controlCmdFlow->start();
        m_timer->start();
    });
}

void OperateWidget::updateTableWidget()
{
    // 取出RecordData数据更新到表格中
    auto *recordData = RecordData::instance();
    if (recordData->recordDataCount() == 0) {
        return;
    }

    for (int i = m_fromChannel; i <= m_totalChannel; i++) {
        if (!recordData->hasChannel(i)) {
            continue;
        }

        // 更新表格
        const R32Info &info = recordData->getR32Info(i);
        int row = i - m_fromChannel;
        // 如果表格行不够，则添加
        if (row >= m_tableWidget->rowCount()) {
            m_tableWidget->setRowCount(row + 1);
        }

        // 传感器ID，判读表格中是否有值，有则更新，无则添加
        if (m_tableWidget->item(row, 0)) {
            m_tableWidget->item(row, 0)->setText(QString::number(info.channel));
        } else {
            m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(info.channel)));
        }
        // 判定结果
        if (m_tableWidget->item(row, 1)) {
            m_tableWidget->item(row, 1)->setText(info.valid ? "合格" : "不合格");
        } else {
            m_tableWidget->setItem(row, 1, new QTableWidgetItem(info.valid ? "合格" : "不合格"));
        }

        // on/off, 如果是off，则显示红色背景，否则显示绿色背景
        if (m_tableWidget->item(row, 2)) {
            m_tableWidget->item(row, 2)->setText(info.on ? "ON" : "OFF");
        } else {
            m_tableWidget->setItem(row, 2, new QTableWidgetItem(info.on ? "ON" : "OFF"));
        }
        m_tableWidget->item(row, 2)->setBackgroundColor(info.on ? Qt::green : Qt::red);

        // 软件版本
        if (!info.softVersion.isEmpty()) {
            if (m_tableWidget->item(row, 3)) {
                m_tableWidget->item(row, 3)->setText(info.softVersion);
            } else {
                m_tableWidget->setItem(row, 3, new QTableWidgetItem(info.softVersion));
            }
        }

        // 标定点1
        if (info.point1 > 0) {
            if (m_tableWidget->item(row, 4)) {
                m_tableWidget->item(row, 4)->setText(QString::number(info.point1));
            } else {
                m_tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(info.point1)));
            }
            m_tableWidget->item(row, 4)->setBackgroundColor(info.point1Valid ? Qt::green : Qt::red);
        }

        // 标定点2
        if (info.point2 > 0) {
            if (m_tableWidget->item(row, 5)) {
                m_tableWidget->item(row, 5)->setText(QString::number(info.point2));
            } else {
                m_tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(info.point2)));
            }
            m_tableWidget->item(row, 5)->setBackgroundColor(info.point2Valid ? Qt::green : Qt::red);
        }

        // 标定点3
        if (info.point3 > 0) {
            if (m_tableWidget->item(row, 6)) {
                m_tableWidget->item(row, 6)->setText(QString::number(info.point3));
            } else {
                m_tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(info.point3)));
            }
            m_tableWidget->item(row, 6)->setBackgroundColor(info.point3Valid ? Qt::green : Qt::red);
        }

        // 标定状态
        if (m_tableWidget->item(row, 7)) {
            m_tableWidget->item(row, 7)->setText(info.calStatus ? "成功" : "失败");
        } else {
            m_tableWidget->setItem(row, 7, new QTableWidgetItem(info.calStatus ? "成功" : "失败"));
        }
        m_tableWidget->item(row, 7)->setBackgroundColor(info.calStatus ? Qt::green : Qt::red);
    }
}
