//
// Created by zyz on 23-5-12.
//

#include "operatewidget.h"
#include "r32analyticalwidget.h"
#include "mcuoperatewidget.h"
#include "operater32widget.h"

#include "handler32data.h"
#include "handlemcudata.h"
#include "handleanalyserdata.h"

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
    , m_tableWidget(new QTableWidget(this))
    , m_msgLabel(new QLabel(this))
{
    auto *handlerMcuData = new HandleMcuData(this);
    auto *handler32Data = new Handler32data(this);
    auto *handlerAnalyserData = new HandleAnalyserData(this);

    m_operateR32Widget = new OperateR32Widget(handler32Data,"R32传感器", this);
    m_r32AnalyticalWidget = new R32AnalyticalWidget(handlerAnalyserData, "分析仪", this);
    m_mcuOperateWidget = new McuOperateWidget(handlerMcuData, "单片机", this);

    initUI();

    connect(m_operateR32Widget, &OperateR32Widget::operatedMsg, this, &OperateWidget::showMsg);
    connect(m_r32AnalyticalWidget, &R32AnalyticalWidget::operatedMsg, this, &OperateWidget::showMsg);
    connect(m_mcuOperateWidget, &McuOperateWidget::operatedMsg, this, &OperateWidget::showMsg);
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
    toLineEdit->setText("64");

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
    hLayout2->addWidget(m_r32AnalyticalWidget);
    hLayout2->addWidget(m_operateR32Widget);
    hLayout2->addWidget(m_mcuOperateWidget);

    // 多行显示操作内容提示
    auto *optMsgLabel = new QTextEdit(this);
    optMsgLabel->setReadOnly(true);
    optMsgLabel->setMaximumHeight(100);

    auto *vLayout = new QVBoxLayout();
    vLayout->setSpacing(15);
    vLayout->setContentsMargins(10, 20, 10, 10);
    vLayout->addWidget(groupBox1);
    vLayout->addWidget(groupBox2);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(optMsgLabel);
    groupBox->setLayout(vLayout);

    // 功能按钮：开始，数据保存，清空数据
    auto *startBtn = new QPushButton(tr("开始"), this);
    auto *saveBtn = new QPushButton(tr("数据保存"), this);
    auto *clearBtn = new QPushButton(tr("清空数据"), this);
    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(clearBtn);

    // 报表groupbox
    auto *groupBox3 = new QGroupBox(tr("报表"), this);
    m_tableWidget->setColumnCount(10);
    m_tableWidget->setRowCount(10);
    m_tableWidget->setMinimumHeight(300);

    auto *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(10, 25, 10, 0);
    vBoxLayout->addWidget(m_tableWidget);
    groupBox3->setLayout(vBoxLayout);

    auto *mainLayout = new QVBoxLayout();;
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(groupBox3);
    mainLayout->addWidget(m_msgLabel);
    setLayout(mainLayout);
}

void OperateWidget::showMsg(const QString &msg)
{
    m_msgLabel->clear();
    m_msgLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + msg);
}
