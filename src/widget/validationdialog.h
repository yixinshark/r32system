//
// Created by yixin on 2023/7/27.
//

#ifndef R32SYSTEM_VALIDATIONDIALOG_H
#define R32SYSTEM_VALIDATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>

class ValidationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ValidationDialog(QWidget *parent = nullptr);
    ~ValidationDialog() override;

private:
    void initUI();
    void onAccepted();

private:
    QString m_username;
    QString m_password;
    QLineEdit *m_passwordLineEdit;
};

#endif //R32SYSTEM_VALIDATIONDIALOG_H
