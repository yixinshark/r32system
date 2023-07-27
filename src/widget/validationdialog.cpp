//
// Created by yixin on 2023/7/27.
//

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include "validationdialog.h"

ValidationDialog::ValidationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("用户验证"));

    // 读取adminconfig.ini文件中的用户名和密码
    QSettings settings(":src/res/adminconfig.ini", QSettings::IniFormat);
    m_username = settings.value("/User/Username").toString();
    m_password = settings.value("/User/Password").toString();

    initUI();
}

ValidationDialog::~ValidationDialog()
{

}

void ValidationDialog::initUI()
{
    auto *layout = new QVBoxLayout;

    auto *usernameLabel = new QLabel(tr("用户名: ") + m_username);
    layout->addWidget(usernameLabel);

    m_passwordLineEdit = new QLineEdit;
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_passwordLineEdit);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ValidationDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ValidationDialog::reject);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void ValidationDialog::onAccepted()
{
    QString enteredPassword = m_passwordLineEdit->text();
    if (enteredPassword == m_password) {
        accept();
    } else {
        QMessageBox::warning(this, tr("验证失败"), tr("密码错误，请重新输入。"));
        reject();
    }
}
