#include "changedatabasedialog.h"
#include "ui_changedatabasedialog.h"

#include <database.h>
#include <QMessageBox>


/*
 * 函数名称：ChangeDatabaseDialog::ChangeDatabaseDialog
 * 函数参数：QWidget *parent
 * 函数返回值：无
 * 函数功能：构造函数，构造修改数据库界面
 *
 */
ChangeDatabaseDialog::ChangeDatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeDatabaseDialog)
{
    ui->setupUi(this);
    this->setFixedSize(420,320);
    this->setWindowTitle("确认数据库");
    ui->DatabaseAddressLe->setText("115.159.110.221,9528");
    ui->DatabaseNameLe->setText("Freemud_PosUpgrade");
    ui->UIDLe->setText("waimaitest");
    ui->PWDLe->setText("waimaitest@freemud");
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
}


/*
 * 函数名称：ChangeDatabaseDialog::~ChangeDatabaseDialog
 * 函数参数：无
 * 函数返回值：无
 * 函数功能：析构函数
 *
 */
ChangeDatabaseDialog::~ChangeDatabaseDialog()
{
    delete ui;
}


/*
 * 函数名称：ChangeDatabaseDialog::on_ConfirmBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：确认按钮按下时触发的槽函数，将行编辑器里面的文本以信号的方式发送到主界面，用于设置数据库连接
 *
 */
void ChangeDatabaseDialog::on_ConfirmBtn_clicked()
{
    QString server = ui->DatabaseAddressLe->text();
    QString databaseName = ui->DatabaseNameLe->text();
    QString uid = ui->UIDLe->text();
    QString pwd = ui->PWDLe->text();
    if(server == NULL || databaseName == NULL || uid == NULL || pwd == NULL)
    {
        QMessageBox::warning(NULL, "警告", "请输入数据库信息", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    emit sendMessage(server,databaseName,uid,pwd);
}


/*
 * 函数名称：ChangeDatabaseDialog::on_CancelBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：取消按钮按下时触发的槽函数，将当前界面隐藏
 *
 */
void ChangeDatabaseDialog::on_CancelBtn_clicked()
{
    this->hide();
}





