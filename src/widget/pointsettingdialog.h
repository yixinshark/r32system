//
// Created by yixin on 2023/7/27.
//

#ifndef R32SYSTEM_POINTSETTINGDIALOG_H
#define R32SYSTEM_POINTSETTINGDIALOG_H

#include <QDialog>

class PointSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PointSettingDialog(QWidget *parent = nullptr);
    ~PointSettingDialog() override;

    QVector<int> getCalibrationValues();
    QVector<int> getDetectValues();

private:
    void initUI();
    void onAccepted();

private:
    QVector<int> m_calibrationValues;
    QVector<int> m_detectionValues;
};

#endif //R32SYSTEM_POINTSETTINGDIALOG_H
