//
// Created by yixin on 2023/7/18.
//

#include "connectwidget.h"
#include "statuswidget.h"
#include "serialportcom.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QEvent>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QListView>
#include "serialportconfig.h"

ConnectWidget::ConnectWidget(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_connectBtn(new QPushButton("连接", this))
    , m_paramSettingsBtn(new QPushButton("参数设置", this))
    , m_cntStatusWidget(new StatusWidget(this))
    , m_portComboBox(new QComboBox(this))
    , m_serialPortCom(new SerialPortCom(this))
    , m_title(title)
{
    initUI();

    m_connectBtn->setObjectName("connect");

    connect(m_serialPortCom, &SerialPortCom::serialPortClosed, this, [this]{
        m_connectBtn->setObjectName("connect");
        m_connectBtn->setText("连接");
        m_connectBtn->setEnabled(true);
        m_cntStatusWidget->setSelected(false);
        Q_EMIT serialPortClosed();
    });

    connect(m_connectBtn, &QPushButton::clicked, this, [this]{
        m_connectBtn->setEnabled(false);

        // 准备连接
        if (m_connectBtn->objectName() == "connect") {
            qInfo() << "----------begin connect serial prot-----------";
            if (connectSerialPort()) {
                m_cntStatusWidget->setSelected(true);
                m_connectBtn->setObjectName("disConnect");
                m_connectBtn->setText("断开");
            }
        } else if (m_connectBtn->objectName() == "disConnect") {
            m_serialPortCom->closeSerialPort();
            m_cntStatusWidget->setSelected(false);
            m_connectBtn->setObjectName("connect");
            m_connectBtn->setText("连接");
            Q_EMIT operatedMsg(m_portComboBox->currentText() + " 连接断开");
        }

        // 1s后使能
        QTimer::singleShot(1000, this, [this]{
            m_connectBtn->setEnabled(true);
        });
    });
}

ConnectWidget::~ConnectWidget()
{

}

void ConnectWidget::initUI()
{
    m_portComboBox->setView(new QListView(this));
    m_portComboBox->setMinimumHeight(30);
    auto *groupBox = new QGroupBox(m_title, this);

    auto *label = new QLabel("串口：", this);
    label->setFixedWidth(40);

    updateAvailablePorts();
    m_portComboBox->installEventFilter(this);
    auto *hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    hLayout->addWidget(m_portComboBox);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_cntStatusWidget);
    btnLayout->addWidget(m_paramSettingsBtn);
    btnLayout->addWidget(m_connectBtn);

    auto *vLayout = new QVBoxLayout;
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(10, 18, 10, 5);
    vLayout->addLayout(hLayout);
    vLayout->addLayout(btnLayout);
    groupBox->setLayout(vLayout);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);

    connect(m_paramSettingsBtn, &QPushButton::clicked, this, [this]{
        m_paramSettingsBtn->setEnabled(false);

        SerialPortSettingsDialog dialog(this);
        dialog.setPortName(m_portComboBox->currentText());
        if (dialog.exec() == QDialog::Accepted) {
            m_serialPortParams = dialog.serialPortParams();
            m_isManualDisconnect = true;
        } else {
            qWarning() << "cancel serialPort param settings!";
        }

        m_paramSettingsBtn->setEnabled(true);
    });
}

bool ConnectWidget::connectSerialPort()
{
    if (!m_isManualDisconnect) {
        // 一件链接
        m_portComboBox->setCurrentText(m_serialPortParams.portName);
    }

    bool ret = m_serialPortCom->openSerialPort(m_serialPortParams.portName, m_serialPortParams.baudRate,
                                               m_serialPortParams.dataBits, m_serialPortParams.parity, m_serialPortParams.stopBits);
    QString msg = ret ? "连接成功" : "连接失败";
    msg += " serialPort:" + m_portComboBox->currentText() + " 波特率:" + QString::number(m_serialPortParams.baudRate) + " 检验和:" +
           m_serialPortParams.parity + " 数据位:" + QString::number(m_serialPortParams.dataBits) + " 停止位:" + m_serialPortParams.stopBits;

    Q_EMIT operatedMsg(msg);

    if (ret && m_isManualDisconnect) {
        m_isManualDisconnect = false;

        QString beginGroup;
        if (m_title == "R32传感器") {
            beginGroup = "r32";
        } else if (m_title == "分析仪") {
            beginGroup = "r32Analyser";
        } else if (m_title == "单片机") {
            beginGroup = "mcu";
        }

        // 同步到配置文件
        SerialPortConfig config;
        config.saveSettings(beginGroup, m_serialPortParams);
    }

    return ret;
}

void ConnectWidget::updateAvailablePorts() const
{
    qInfo() << "available ports:" << QSerialPortInfo::availablePorts().size();

    QStringList ports;
    if (m_portComboBox->count()) {
        ports.append(m_portComboBox->currentText());

        m_portComboBox->clear();
    }

    QList<QSerialPortInfo> serials = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &info, serials) {
        if (!ports.contains(info.portName()))
            ports.append(info.portName());
    }

    m_portComboBox->addItems(ports);
}

bool ConnectWidget::eventFilter(QObject *watched, QEvent *event) {
    if(event->type() == QEvent::MouseButtonPress && watched == m_portComboBox) {
        updateAvailablePorts();
    }

    return QObject::eventFilter(watched, event);
}

bool ConnectWidget::isConnected() const
{
    return m_serialPortCom->isSerialPortOpen();
}

void ConnectWidget::setSerialPortParams(const SerialPortParams &serialPortParams)
{
    m_serialPortParams = serialPortParams;
    // 模拟connect按钮点击
    m_connectBtn->click();
}
