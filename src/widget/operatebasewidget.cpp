//
// Created by zyz on 23-5-13.
//

#include "operatebasewidget.h"
#include "statuswidget.h"

#include "handledatabase.h"
#include "serialportcom.h"

#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSerialPortInfo>

OperateBaseWidget::OperateBaseWidget(HandleDataBase *handleData, QWidget *parent)
    : QWidget(parent)
    , m_connectBtn(new QPushButton("连接", this))
    , m_paramSettingsBtn(new QPushButton("参数设置", this))
    , m_cntStatusWidget(new StatusWidget(this))
    , m_portComboBox(new QComboBox(this))
    , m_serialPortCom(new SerialPortCom(this))
    , m_handleData(handleData)
{
    m_connectBtn->setObjectName("connect");

    connect(m_serialPortCom, &SerialPortCom::dataReceived,
            m_handleData, &HandleDataBase::processReceivedData);
    connect(m_serialPortCom, &SerialPortCom::serialPortClosed, this, [this]{
        m_connectBtn->setObjectName("connect");
        m_connectBtn->setText("连接");
        m_connectBtn->setEnabled(true);
        m_cntStatusWidget->setSelected(false);
        Q_EMIT serialPortClosed();
    });

    connect(m_handleData, &HandleDataBase::recvedFrameData, this, [this](const QByteArray &data){
        operateMsg("接收到数据:" + data.toHex());
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
             operateMsg(m_portComboBox->currentText() + " 连接断开");
        }

        // 1s后使能
        QTimer::singleShot(1000, this, [this]{
            m_connectBtn->setEnabled(true);
        });
    });

    connect(m_handleData, &HandleDataBase::frameReceived, this, &OperateBaseWidget::recvAckData);
}

OperateBaseWidget::~OperateBaseWidget()
{

}

bool OperateBaseWidget::connectSerialPort()
{
    bool ret = m_serialPortCom->openSerialPort(m_portComboBox->currentText(), m_serialPortParams.baudRate,
                                               m_serialPortParams.dataBits, m_serialPortParams.parity, m_serialPortParams.stopBits);
    QString msg = ret ? "连接成功" : "连接失败";
    msg += " serialPort:" + m_portComboBox->currentText() + " 波特率:" + QString::number(m_serialPortParams.baudRate) + " 检验和:" +
            m_serialPortParams.parity + " 数据位:" + QString::number(m_serialPortParams.dataBits) + " 停止位:" + m_serialPortParams.stopBits;

    operateMsg(msg);

   return ret;
}

void OperateBaseWidget::operateMsg(const QString &msg)
{
    Q_EMIT operatedMsg(msg);
}

void OperateBaseWidget::sendCmdData(quint8 cmd, const QVariantMap &info)
{
    QByteArray data = m_handleData->getSendData(cmd, info);
    qInfo() << "send cmd: " << cmd << " data:" << data;

    QString msg = QString("%1发送数据:").arg(m_serialPortCom->isSerialPortOpen() ? " " : "串口未打开,未");
    operateMsg(msg + data.toHex());

    m_serialPortCom->sendData(data);
}

void OperateBaseWidget::initUI()
{
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
    vLayout->setContentsMargins(10, 25, 10, 10);
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
        } else {
            qWarning() << "cancel serialPort param settings!";
        }

        m_paramSettingsBtn->setEnabled(true);
    });
}

bool OperateBaseWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress && watched == m_portComboBox) {
        updateAvailablePorts();
    }

    return QObject::eventFilter(watched, event);
}

void OperateBaseWidget::updateAvailablePorts() const
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

    // for test
#ifdef QT_DEBUG
    if (!ports.contains("/dev/pts/0"))
        ports.append("/dev/pts/0");

    if (!ports.contains("/dev/pts/1"))
        ports.append("/dev/pts/1");

    if (!ports.contains("/dev/pts/2"))
        ports.append("/dev/pts/2");

    if (!ports.contains("/dev/pts/3"))
        ports.append("/dev/pts/3");

    if (!ports.contains("/dev/pts/4"))
        ports.append("/dev/pts/4");

    if (!ports.contains("/dev/pts/5"))
        ports.append("/dev/pts/5");

    if (!ports.contains("/dev/pts/6"))
        ports.append("/dev/pts/6");
#endif

    m_portComboBox->addItems(ports);
}
