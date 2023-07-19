//
// Created by zyz on 23-5-12.
//

#include "operatewidget.h"

#include "handler32data.h"
#include "handlemcudata.h"
#include "handleanalyserdata.h"
#include "connectwidget.h"
#include "controlcmdflow.h"

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

OperateWidget::OperateWidget(QWidget *parent)
    : QWidget(parent)
    , m_r32ConnectWidget(new ConnectWidget("R32传感器", this))
    , m_r32AnaConnectWidget(new ConnectWidget("分析仪", this))
    , m_mcuConnectWidget(new ConnectWidget("单片机", this))
    , m_startBtn(new QPushButton(tr("开始"), this))
    , m_optMsgLabel(new QTextEdit(this))
    , m_tableWidget(new QTableWidget(this))
    //, m_msgLabel(new QLabel(this))
    , m_controlCmdFlow(new ControlCmdFlow(this))
{
    // 初始化界面
    initUI();
    // 单片机数据处理
    auto *handlerMcuData = new HandleMcuData(m_mcuConnectWidget->serialPortCom(), this);
    // r32数据处理
    auto *handler32Data = new Handler32data(m_r32ConnectWidget->serialPortCom(), this);
    // 分析仪数据处理
    auto *handlerAnalyserData = new HandleAnalyserData(m_r32AnaConnectWidget->serialPortCom(), this);

    // 设置数据发送、处理
    m_controlCmdFlow->setMcuDataHandler(handlerMcuData);
    m_controlCmdFlow->setR32DataHandler(handler32Data);
    m_controlCmdFlow->setR32AnaDataHandler(handlerAnalyserData);
    m_controlCmdFlow->initCmdFlow();

    connect(m_r32ConnectWidget, &ConnectWidget::operatedMsg, this, &OperateWidget::showMsg);
    connect(m_r32AnaConnectWidget, &ConnectWidget::operatedMsg, this, &OperateWidget::showMsg);
    connect(m_mcuConnectWidget, &ConnectWidget::operatedMsg, this, &OperateWidget::showMsg);

    // 开始执行
    connect(m_startBtn, &QPushButton::clicked,
            m_controlCmdFlow, &ControlCmdFlow::start, Qt::QueuedConnection);
    // 命令执行提示信息
    connect(m_controlCmdFlow, &ControlCmdFlow::cmdexecuted,
            this, &OperateWidget::showOperateMsg);
}

OperateWidget::~OperateWidget()
{

}

void OperateWidget::initUI()
{
    // 添加一个groupbox
    auto *groupBox = new QGroupBox(tr("基本参数"), this);

    auto *label = new QLabel(tr("工作模式："), this);
    auto *checkBox = new QCheckBox(tr("标定"), this);
    auto *checkBox2 = new QCheckBox(tr("测试"), this);

    // 测试个数
    auto *spinBox = new QSpinBox(this);
    spinBox->setMinimumHeight(30);
    spinBox->setRange(1, 300);
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

    connect(fromComboBox, &QComboBox::currentTextChanged, this, [fromComboBox, spinBox, toLineEdit]{
        int from = fromComboBox->currentText().toInt();
        int to = from + spinBox->value() - 1;
        toLineEdit->setText(QString::number(to));
    });
    connect(spinBox, &QSpinBox::textChanged, this, [fromComboBox, spinBox, toLineEdit]{
        int from = fromComboBox->currentText().toInt();
        int to = from + spinBox->value() - 1;
        toLineEdit->setText(QString::number(to));
    });

    // TSI值；分析仪浓度数
    auto *tsiLabel = new QLabel(tr("TSI值："), this);
    auto *tsiLineEdit = new QLineEdit(this);
    tsiLineEdit->setReadOnly(true);

    auto *groupBox1 = new QGroupBox(this);
    groupBox1->setContentsMargins(0, 0, 0, 0);
    auto *hLayout1 = new QHBoxLayout();
    hLayout1->setContentsMargins(10, 0, 0, 0);
    hLayout1->addWidget(label);
    hLayout1->addWidget(checkBox);
    hLayout1->addWidget(checkBox2);
    hLayout1->addStretch(1);
    hLayout1->addWidget(spinBox);
    hLayout1->addWidget(fromComboBox);
    hLayout1->addWidget(label2);
    hLayout1->addWidget(toLineEdit);
    hLayout1->addStretch(1);
    hLayout1->addWidget(tsiLabel);
    hLayout1->addWidget(tsiLineEdit);
    hLayout1->addStretch(2);
    groupBox1->setLayout(hLayout1);

    // 标定点位个数：1-3；检测点位个数：1-5
    auto *groupBox2 = new QGroupBox(this);
    groupBox2->setContentsMargins(0, 0, 0, 0);
    QFont font;
    font.setPixelSize(16);
    auto *label3 = new QLabel(tr("标定点位个数：(1-0,2-1000,3-5000)"), this);
    label3->setFont(font);
    label3->setMinimumHeight(30);
    auto *label4 = new QLabel(tr("检测点位个数：(1-0,2-1000,3-2000,4-3000,5-5000)"), this);
    label4->setFont(font);
    label4->setMinimumHeight(30);
    auto *settingBtn = new QPushButton(tr("设置"), this);
    auto *hLayout3 = new QHBoxLayout();
    hLayout3->setSpacing(26);
    hLayout3->setContentsMargins(10, 0, 0, 0);
    hLayout3->addWidget(label3);
    hLayout3->addWidget(label4);
    hLayout3->addWidget(settingBtn);
    hLayout3->addStretch();
    groupBox2->setLayout(hLayout3);

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

    // 功能按钮：开始，数据保存，清空数据
    //auto *startBtn = new QPushButton(tr("开始"), this);
    auto *saveBtn = new QPushButton(tr("数据保存"), this);
    auto *clearBtn = new QPushButton(tr("清空数据"), this);
    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(clearBtn);

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
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(groupBox3);
    //mainLayout->addWidget(m_msgLabel);
    setLayout(mainLayout);
}

void OperateWidget::initTableWidget()
{
    m_tableWidget->setColumnCount(16);
    m_tableWidget->setRowCount(10);
    m_tableWidget->setMinimumHeight(400);

    QStringList header;
    header << "传感器ID" << "判定结果" << "ON/OFF" << "软件版本" << "标定点1" << "标定点2" << "标定点3" << "标定状态"
            << "标定的零点阻值R0" << "1000PPM的阻值" << "5000PPM的阻值" << "参数P" << "参数P1" << "参数P2" << "温度" << "湿度";

    m_tableWidget->setHorizontalHeaderLabels(header);
}

void OperateWidget::showMsg(const QString &msg)
{
#if 0
    m_msgLabel->clear();
    m_msgLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + msg);
#endif
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


