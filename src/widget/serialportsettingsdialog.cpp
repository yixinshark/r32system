//
// Created by zyz on 23-5-13.
//

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include <QComboBox>
#include <QListView>
#include <QPushButton>

#include "serialportsettingsdialog.h"

SerialPortSettingsDialog::SerialPortSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_titleLabel(new QLabel(this))
    , m_baudRateComboBox(new QComboBox(this))
    , m_parityComboBox(new QComboBox(this))
    , m_dataBitsComboBox(new QComboBox(this))
    , m_stopBitsComboBox(new QComboBox(this))
{
#ifdef Q_OS_WIN
    styleForWindows();
#endif
    initVerticalUI();
}

SerialPortSettingsDialog::~SerialPortSettingsDialog()
{

}

void SerialPortSettingsDialog::styleForWindows()
{
    m_dataBitsComboBox->setView(new QListView(this));
    m_dataBitsComboBox->setMinimumHeight(25);

    m_baudRateComboBox->setView(new QListView(this));
    m_baudRateComboBox->setMinimumHeight(25);

    m_parityComboBox->setView(new QListView(this));
    m_parityComboBox->setMinimumHeight(25);

    m_stopBitsComboBox->setView(new QListView(this));
    m_stopBitsComboBox->setMinimumHeight(25);
}

void SerialPortSettingsDialog::initVerticalUI()
{
    setWindowTitle("串口参数设置");
    setMinimumWidth(300);
    auto *mainLayout = new QVBoxLayout();

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setText(m_portName);
    mainLayout->addWidget(m_titleLabel);

    auto *hLayout2 = new QHBoxLayout();
    // 波特率选择
    auto *baudRateLabel = new QLabel("波特率:", this);
    m_baudRateComboBox->addItem("2400");
    m_baudRateComboBox->addItem("9600");
    m_baudRateComboBox->addItem("115200");
    m_baudRateComboBox->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding);
    hLayout2->addWidget(baudRateLabel);
    hLayout2->addWidget(m_baudRateComboBox);
    mainLayout->addLayout(hLayout2);

    auto *hLayout3 = new QHBoxLayout();
    // 校验位选择
    auto *parityLabel = new QLabel("校验位:");
    m_parityComboBox->addItem("无");
    m_parityComboBox->addItem("偶校验");
    m_parityComboBox->addItem("奇校验");
    m_parityComboBox->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    hLayout3->addWidget(parityLabel);
    hLayout3->addWidget(m_parityComboBox);
    mainLayout->addLayout(hLayout3);

    auto *hLayout4 = new QHBoxLayout();
    // 数据位选择
    auto *dataBitsLabel = new QLabel("数据位:", this);
    m_dataBitsComboBox->addItem("8");
    m_dataBitsComboBox->addItem("7");
    m_dataBitsComboBox->addItem("6");
    m_dataBitsComboBox->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding);
    hLayout4->addWidget(dataBitsLabel);
    hLayout4->addWidget(m_dataBitsComboBox);
    mainLayout->addLayout(hLayout4);

    auto *hLayout5 = new QHBoxLayout();
    // 停止位选择
    auto *stopBitsLabel = new QLabel("停止位:", this);
    m_stopBitsComboBox->addItem("1");
    m_stopBitsComboBox->addItem("1.5");
    m_stopBitsComboBox->addItem("2");
    m_stopBitsComboBox->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding);
    hLayout5->addWidget(stopBitsLabel);
    hLayout5->addWidget(m_stopBitsComboBox);
    mainLayout->addLayout(hLayout5);

    auto *sureButton = new QPushButton("确定", this);
    connect(sureButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::getSerialPortParams);
    connect(sureButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::accept);

    auto *cancelButton = new QPushButton("取消", this);
    connect(cancelButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::reject);

    auto *hLayout6 = new QHBoxLayout();
    hLayout6->addStretch();
    hLayout6->addWidget(sureButton);
    hLayout6->addWidget(cancelButton);

    mainLayout->addLayout(hLayout6);
    setLayout(mainLayout);
}

void SerialPortSettingsDialog::getSerialPortParams()
{
    m_serialPortParams.portName = m_portName;
    m_serialPortParams.baudRate = m_baudRateComboBox->currentText().toInt();

    QSerialPort::DataBits sDataBits = QSerialPort::UnknownDataBits;
    int dataBits =m_dataBitsComboBox->currentText().toInt();
    switch (dataBits) {
        case 8:
            sDataBits = QSerialPort::Data8;
            break;
        case 7:
            sDataBits = QSerialPort::Data7;
            break;
        case 6:
            sDataBits = QSerialPort::Data6;
            break;
        case 5:
            sDataBits = QSerialPort::Data5;
            break;
        default:
            break;
    }
    m_serialPortParams.dataBits = sDataBits;

    QString parity = m_parityComboBox->currentText();
    QSerialPort::Parity sParity = QSerialPort::UnknownParity;
    if (parity == "偶校验") {
        sParity = QSerialPort::EvenParity;
    } else if (parity == "奇校验") {
        sParity = QSerialPort::OddParity;
    } else if (parity == "无") {
        sParity = QSerialPort::NoParity;
    }
    m_serialPortParams.parity = sParity;

    QString stopBits =m_stopBitsComboBox->currentText();
    QSerialPort::StopBits sStopBits = QSerialPort::UnknownStopBits;
    if (stopBits == "1") {
        sStopBits = QSerialPort::OneStop;
    } else if (stopBits == "2") {
        sStopBits = QSerialPort::TwoStop;
    }else if (stopBits == "1.5") {
        sStopBits = QSerialPort::OneAndHalfStop;
    }
    m_serialPortParams.stopBits = sStopBits;
}

void SerialPortSettingsDialog::setPortName(const QString &portName)
{
    m_portName = portName;
    m_titleLabel->setText(m_portName);
}
