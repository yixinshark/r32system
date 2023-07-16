//
// Created by zyz on 23-5-13.
//

#ifndef R32_SERIALPORTSETTINGSWIDGET_H
#define R32_SERIALPORTSETTINGSWIDGET_H

#include <QDialog>
#include <QSerialPort>

struct SerialPortParams {
    QString portName;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
};

class QLabel;
class QComboBox;
class SerialPortSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SerialPortSettingsDialog(QWidget *parent = nullptr);
    ~SerialPortSettingsDialog() override;

    void setPortName(const QString &portName);
    SerialPortParams serialPortParams() const { return m_serialPortParams; }

private:
    void initVerticalUI();
    void getSerialPortParams();
    void styleForWindows();

private:
    QString m_portName;
    QLabel *m_titleLabel;
    QComboBox *m_baudRateComboBox;
    QComboBox *m_parityComboBox;
    QComboBox *m_dataBitsComboBox;
    QComboBox *m_stopBitsComboBox;

    SerialPortParams m_serialPortParams;
};

#endif //R32_SERIALPORTSETTINGSWIDGET_H
