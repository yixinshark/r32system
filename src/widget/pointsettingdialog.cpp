//
// Created by yixin on 2023/7/27.
//

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QRegularExpression>

#include "pointsettingdialog.h"

PointSettingDialog::PointSettingDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("参数自定义设置"));
    initUI();
}

PointSettingDialog::~PointSettingDialog()
{

}

QVector<int> PointSettingDialog::getCalibrationValues()
{
    return m_calibrationValues;
}

QVector<int> PointSettingDialog::getDetectValues()
{
    return m_detectionValues;
}

void PointSettingDialog::initUI()
{
    auto *leftRightLayout = new QHBoxLayout;

    auto *calibrationGroupBox = new QGroupBox(tr("标定点"));
    auto *calibrationLayout = new QVBoxLayout;
    calibrationLayout->setContentsMargins(10, 20, 10, 10);
    for (int i = 1; i <= 5; ++i)
    {
        auto *label = new QLabel(tr("标定点%1").arg(i));
        auto *lineEdit = new QLineEdit;
        lineEdit->setObjectName(QString("calibrationLineEdit%1").arg(i));
        calibrationLayout->addWidget(label);
        calibrationLayout->addWidget(lineEdit);
    }
    calibrationGroupBox->setLayout(calibrationLayout);

    auto *detectionGroupBox = new QGroupBox(tr("检测点"));
    auto *detectionLayout = new QVBoxLayout;
    detectionLayout->setContentsMargins(10, 20, 10, 10);
    for (int i = 1; i <= 5; ++i)
    {
        auto *label = new QLabel(tr("检测点%1").arg(i));
        auto *lineEdit = new QLineEdit;
        lineEdit->setObjectName(QString("detectionLineEdit%1").arg(i));
        detectionLayout->addWidget(label);
        detectionLayout->addWidget(lineEdit);
    }
    detectionGroupBox->setLayout(detectionLayout);

    leftRightLayout->addWidget(calibrationGroupBox);
    leftRightLayout->addWidget(detectionGroupBox);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PointSettingDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    auto *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(leftRightLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void PointSettingDialog::onAccepted()
{
    QList<QLineEdit *> calibrationLineEdits = findChildren<QLineEdit *>(QRegularExpression("^calibrationLineEdit\\d$"));
    QList<QLineEdit *> detectionLineEdits = findChildren<QLineEdit *>(QRegularExpression("^detectionLineEdit\\d$"));

    for (QLineEdit *lineEdit : calibrationLineEdits) {
        bool ok;
        int value = lineEdit->text().toInt(&ok);
        if (ok && value >= 0) {
            m_calibrationValues.append(value);
        } else {
            qDebug() << "Invalid input for calibration point.";
            break;
        }
    }

    for (QLineEdit *lineEdit : detectionLineEdits) {
        bool ok;
        int value = lineEdit->text().toInt(&ok);
        if (ok && value >= 0) {
            m_detectionValues.append(value);
        } else {
            qDebug() << "Invalid input for detection point.";
            break;
        }
    }

    // 对输入的数据进行从大到小排序
    std::sort(m_calibrationValues.begin(), m_calibrationValues.end(), std::greater<int>());
    std::sort(m_detectionValues.begin(), m_detectionValues.end(), std::greater<int>());

    accept();
}
