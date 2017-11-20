#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <database.h>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <checkboxdelegate.h>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QAbstractItemView>
#include <combobutton.h>


/*
 * 函数名称：MainWindow::paintEvent
 * 函数参数：QPaintEvent *e
 * 函数返回值：void
 * 函数功能：窗口的绘图事件，主要绘制窗口的横向的分割线
 *
 */
void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter;
    painter.begin(this);
    painter.setPen(QColor(190, 190, 190));
    painter.drawLine(10, 64, 490, 64);
    painter.drawLine(10, 157, 490, 157);
    painter.drawLine(10, 253, 490, 253);
    painter.end();
}


/*
 * 函数名称：MainWindow::MainWindow
 * 函数参数：QWidget *parent
 * 函数返回值：无
 * 函数功能：构造函数。界面的创建以及信号与槽的连接
 *
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fpw(new ForkPlanWidget),
    upw(new UsingPlanWidget),
    cdd(new ChangeDatabaseDialog),
    db(new database)
{
    ui->setupUi(this);
    this->setFixedSize(850,790);
    ui->tableView->close();
    ui->PartInformationBtn->setEnabled(false);

    cdd->setWindowModality(Qt::ApplicationModal);
    cdd->show();
    QObject::connect(this,SIGNAL(sendPartnerId(int,int)),upw,SLOT(onSendPartnerId(int,int)));
    QObject::connect(upw,SIGNAL(widgetOpen()),this,SLOT(show()));
    QObject::connect(cdd,SIGNAL(sendMessage(QString,QString,QString,QString)),this,SLOT(onSendMessage(QString,QString,QString,QString)));
    QObject::connect(upw,SIGNAL(sendUpdatePosId(QVector<int>)),this,SLOT(onSendUpdatePosId(QVector<int>)));
}


/*
 * 函数名称：MainWindow::onSendMessage
 * 函数参数：QString server, QString databaseName, QString uid, QString pwd
 * 函数返回值：void
 * 函数功能：数据库信息界面确认按钮按下时触发的主界面的槽函数，用于设置数据库，进行sqlserver的连接
 *
 */
void MainWindow::onSendMessage(QString server, QString databaseName, QString uid, QString pwd)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    db->ChangeDatabase(server,databaseName,uid,pwd);
    cdd->hide();
}

/*
 * 函数名称：MainWindow::~MainWindow
 * 函数参数：无
 * 函数返回值：无
 * 函数功能：析构函数
 *
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete fpw;
    delete upw;
}


/*
 * 函数名称：MainWindow::closeEvent
 * 函数参数：QCloseEvent *e
 * 函数返回值：void
 * 函数功能：关闭事件。按下关闭按钮时程序相应的操作
 *
 */
void MainWindow::closeEvent(QCloseEvent *e)
{
    fpw->close();
    upw->close();
    e->accept();
}


/*
 * 函数名称：MainWindow::on_CheckPlanBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：查询计划按钮的点击事件的槽函数。按下按钮删除List的按钮失效，防止错误操作，通过操作数据库，将相应的数据从数据库中
 *         显示到tablewidget中。界面上有些查询选项，根据选项可以更精确的找到想要的数据，显示结束后连接界面被修改的响应槽函数
 *         ，用于双击修改时，界面数据与数据库数据的同步
 *
 */
void MainWindow::on_CheckPlanBtn_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    ui->DeleteListBtn->setEnabled(false);
    if(!ui->DeletePlanBtn->isEnabled())
    {
        ui->DeletePlanBtn->setEnabled(true);
    }
    if(!ui->EnablePlanBtn->isEnabled())
    {
        ui->EnablePlanBtn->setEnabled(true);
    }
    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
    int flag = 0;
    int partnerId = 0;
    partnerId = ui->PlanNameLe->text().toInt();

    int planState = 0;
    QString text = ui->comboBox->currentText();
    if(text == "启用")
    {
        planState = 1;
    }
    else if(text == "禁用")
    {
        planState = 0;
    }
    else if(text == "全部")
    {
        flag = 1;
    }
    else
    {
        QMessageBox::warning(NULL, "警告", "未选择计划状态", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }


    if(flag == 0)
    {
        QSqlQueryModel *model = new QSqlQueryModel;
        if(partnerId != 0)
        {
            ui->SQLTe->setText("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                              "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                              " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                              "where PartnerId = " + QString::number(partnerId) + " "
                              "and PlanState = " + QString::number(planState));

            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PartnerId = %1 "
                                        "and PlanState = %2").arg(partnerId).arg(planState));
        }
        else
        {
            ui->SQLTe->setText("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                              "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                              " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                              "where PlanState = " + QString::number(planState));
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PlanState = %1 ").arg(planState));

        }
        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("计划编号")<<tr("商户号")<<tr("计划名称")<<tr("计划描述")
                                                   <<tr("计划开始时间")<<tr("计划结束时间")<<tr("文件地址")<<tr("计划状态"));

        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);

                if(j == 4 || j == 5)
                {
                    QString date = data.toString().section('T',0,0);
                    QString time = data.toString().section('T',1,1);
                    QString date_time = date + " " + time;
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
                }
                else if(j == 7)
                {
                    ComboButton *combox = new ComboButton(); // 下拉选择框控件
                    if(data.toString() == "启用")
                    {
                        combox->addItem("启用");
                        combox->addItem("禁用");
                    }
                    if(data.toString() == "禁用")
                    {
                        combox->addItem("禁用");
                        combox->addItem("启用");
                    }
                    ui->tableWidget->setCellWidget(i, 7, (QWidget*)combox);

                    QObject::connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_currentIndexChanged(QString)));
                }
                else
                {
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
                }
            }
        }

        /*设置单列不可编辑*/
        int tmp = 0;
        for(tmp = 0;tmp < rowCount;tmp++)
        {
            ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        }

        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        QSqlQueryModel *model = new QSqlQueryModel;
        if(partnerId != 0)
        {
            ui->SQLTe->setText("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                              "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                              " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                              "where PartnerId = " + QString::number(partnerId));
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PartnerId = %1").arg(partnerId));

        }
        else
        {
            ui->SQLTe->setText("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                              "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                              " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] ");
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) as PlanState "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "));
        }
        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("计划编号")<<tr("商户号")<<tr("计划名称")<<tr("计划描述")
                                                   <<tr("计划开始时间")<<tr("计划结束时间")<<tr("文件地址")<<tr("计划状态"));
        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);
                if(j == 4 || j == 5)
                {
                    QString date = data.toString().section('T',0,0);
                    QString time = data.toString().section('T',1,1);
                    QString date_time = date + " " + time;
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
                }
                else if(j == 7)
                {
                    ComboButton *combox = new ComboButton(); // 下拉选择框控件
                    if(data.toString() == "启用")
                    {
                        combox->addItem("启用");
                        combox->addItem("禁用");
                    }
                    if(data.toString() == "禁用")
                    {
                        combox->addItem("禁用");
                        combox->addItem("启用");
                    }

                    ui->tableWidget->setCellWidget(i, 7, (QWidget*)combox);;

                    QObject::connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_currentIndexChanged(QString)));
                }
                else
                {
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
                }
            }
        }

        /*设置单列不可编辑*/
        int tmp = 0;
        for(tmp = 0;tmp < rowCount;tmp++)
        {
            ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        }

        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    }
    QObject::connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
}


/*
 * 函数名称：MainWindow::on_NewPlanBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：新增计划按钮按下响应的槽函数，打开新增计划页面
 *
 */
void MainWindow::on_NewPlanBtn_clicked()
{
    fpw->show();
    fpw->setWindowTitle(QString("fork升级计划新增"));
}


/*
 * 函数名称：MainWindow::on_EnablePlanBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：启用计划按钮按下响应的槽函数，打开启用计划页面，并将选择要启用计划的partnerId和planId传到启用计划界面
 *
 */
void MainWindow::on_EnablePlanBtn_clicked()
{
    if(!ui->tableView->verticalHeader()->count())
    {
        QMessageBox::warning(NULL, "警告", "无查询结果，请查询后再启用计划", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    bool focus = ui->tableWidget->isItemSelected(ui->tableWidget->currentItem()); // 判断是否选中一行
    if(focus == false)
    {
        QMessageBox::warning(NULL, "警告", "请选择要启用的计划", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    int row = ui->tableWidget->currentItem()->row();

    QAbstractItemModel *model = ui->tableWidget->model();
    QModelIndex index1 = model->index(row,1);
    QModelIndex index2 = model->index(row,0);

    QVariant data1 = model->data(index1);
    QVariant data2 = model->data(index2);

    upw->setWindowTitle(QString("启用计划"));
    upw->show();
    emit sendPartnerId(data1.toInt(),data2.toInt());
}


/*
 * 函数名称：MainWindow::slotCellChanged
 * 函数参数：int row, int column
 * 函数返回值：void
 * 函数功能：界面数据被修改后触发的槽函数，用于将界面数据和数据库数据保持同步，防止数据的差异。
 *         通过表格的列数来区分是plan表还是list表，保证修改数据的准确性
 *
 */
void MainWindow::slotCellChanged(int row, int column)
{
    int flag = ui->tableWidget->columnCount();

    if(flag == 8)
    {
        if(column == 0)
        {
            QMessageBox::warning(NULL, "警告", "ID不可修改", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        int id = ui->tableWidget->item(row,0)->text().toInt();
        int partnerId = ui->tableWidget->item(row,1)->text().toInt();
        QString planName = ui->tableWidget->item(row,2)->text();
        QString planDesc = ui->tableWidget->item(row,3)->text();
        QString planStartTime1 = ui->tableWidget->item(row,4)->text();
        QString planStartTime = planStartTime1.section(' ',0,0);
        QString planEndTime1 = ui->tableWidget->item(row,5)->text();
        QString planEndTime = planEndTime1.section(' ',0,0);
        QString fileUrl = ui->tableWidget->item(row,6)->text();

        QSqlQuery query;
        bool res = query.exec(QObject::tr("update [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                          "set PartnerId = %1,PlanName = '%2',PlanDesc = '%3',PlanStartTime = '%4',PlanEndTime = '%5',FileUrl = '%6' "
                                          "where ID = %7 ").arg(partnerId).arg(planName).arg(planDesc).arg(planStartTime).arg(planEndTime)
                              .arg(fileUrl).arg(id));
        if(!res)
        {
            qDebug()<<query.lastError();
            QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        ui->SQLTe->setText("update [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                          "set PartnerId = " + QString::number(partnerId) + ",PlanName = " + planName + ",PlanDesc = " + planDesc + ",PlanStartTime = "
                          + planStartTime + ",PlanEndTime = " + planEndTime + ",FileUrl = " + fileUrl + " "
                          "where ID =  " + QString::number(id));
    }
    else if(flag == 6)
    {
        if(column == 0)
        {
            QMessageBox::warning(NULL, "警告", "ID不可修改", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        int id = ui->tableWidget->item(row,0)->text().toInt();
        int posid = ui->tableWidget->item(row,2)->text().toInt();
        int planid = ui->tableWidget->item(row,3)->text().toInt();

        QSqlQuery query;
        bool res = query.exec(QObject::tr("UPDATE [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                                          "SET PosId = %1,PlanId = %2  WHERE ID = %3 ")
                              .arg(posid).arg(planid).arg(id));
        if(!res)
        {
            qDebug()<<query.lastError();
            QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        ui->SQLTe->setText("UPDATE [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                           "SET PosId = " + QString::number(posid) + ",PlanId = " + QString::number(planid) + " WHERE ID = " +QString::number(id));
    }
    else
    {
        QMessageBox::warning(NULL, "警告", "删除错误", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

}


/*
 * 函数名称：MainWindow::on_DeletePlanBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：删除计划按钮按下所触发的槽函数。根据选中记录的主键，删除在数据库中的数据，并将界面数据实时更新
 *
 */
void MainWindow::on_DeletePlanBtn_clicked()
{
    bool focus = ui->tableWidget->isItemSelected(ui->tableWidget->currentItem()); // 判断是否选中一行
    if(focus == false)
    {
        QMessageBox::warning(NULL, "警告", "请选择要删除的计划", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    int row = ui->tableWidget->currentItem()->row();
    QAbstractItemModel *model = ui->tableWidget->model();
    QModelIndex index = model->index(row,0);
    QVariant data = model->data(index);
    int id = data.toInt();

    QSqlQuery query;
    bool res = query.exec(QObject::tr("DELETE FROM [Freemud_PosUpgrade].[dbo].[tUpdatePlan] WHERE ID = %1 ").arg(id));
    if(!res)
    {
        qDebug()<<query.lastError();
        QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    ui->SQLTe->setText("DELETE FROM [Freemud_PosUpgrade].[dbo].[tUpdatePlan] WHERE ID = " + QString::number(id));

    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
    int flag = 0;
    int partnerId = 0;
    partnerId = ui->PlanNameLe->text().toInt();

    int planState = 0;
    QString text = ui->comboBox->currentText();
    if(text == "启用")
    {
        planState = 1;
    }
    else if(text == "禁用")
    {
        planState = 0;
    }
    else if(text == "全部")
    {
        flag = 1;
    }
    else
    {
        QMessageBox::warning(NULL, "警告", "未选择任务状态", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(flag == 0)
    {
        QSqlQueryModel *model = new QSqlQueryModel;
        if(partnerId != 0)
        {
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PartnerId = %1 "
                                   "and PlanState = %2").arg(partnerId).arg(planState));
        }
        else
        {
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PlanState = %1 ").arg(planState));
        }

        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("计划编号")<<tr("商户号")<<tr("计划名称")<<tr("计划描述")
                                                   <<tr("计划开始时间")<<tr("计划结束时间")<<tr("文件地址")<<tr("计划状态"));
        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);
                if(j == 4 || j == 5)
                {
                    QString date = data.toString().section('T',0,0);
                    QString time = data.toString().section('T',1,1);
                    QString date_time = date + " " + time;
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
                }
                else if(j == 7)
                {
                    ComboButton *combox = new ComboButton(); // 下拉选择框控件
                    if(data.toString() == "启用")
                    {
                        combox->addItem("启用");
                        combox->addItem("禁用");
                    }
                    if(data.toString() == "禁用")
                    {
                        combox->addItem("禁用");
                        combox->addItem("启用");
                    }
                    ui->tableWidget->setCellWidget(i, 7, (QWidget*)combox);

                    QObject::connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_currentIndexChanged(QString)));
                }
                else
                {
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
                }
            }
        }

        /*设置单列不可编辑*/
        int tmp = 0;
        for(tmp = 0;tmp < rowCount;tmp++)
        {
            ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        }

        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    }
    else
    {
        QSqlQueryModel *model = new QSqlQueryModel;
        if(partnerId != 0)
        {
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                        "where PartnerId = %1").arg(partnerId));
        }
        else
        {
            model->setQuery(QObject::tr("select ID,PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,"
                                        "(case  PlanState when 0 then '禁用' when 1 then '启用' else '错误' end) "
                                        " from [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "));
        }
        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("计划编号")<<tr("商户号")<<tr("计划名称")<<tr("计划描述")
                                                   <<tr("计划开始时间")<<tr("计划结束时间")<<tr("文件地址")<<tr("计划状态"));
        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);
                if(j == 4 || j == 5)
                {
                    QString date = data.toString().section('T',0,0);
                    QString time = data.toString().section('T',1,1);
                    QString date_time = date + " " + time;
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
                }
                else if(j == 7)
                {
                    ComboButton *combox = new ComboButton(); // 下拉选择框控件
                    if(data.toString() == "启用")
                    {
                        combox->addItem("启用");
                        combox->addItem("禁用");
                    }
                    if(data.toString() == "禁用")
                    {
                        combox->addItem("禁用");
                        combox->addItem("启用");
                    }
                    ui->tableWidget->setCellWidget(i, 7, (QWidget*)combox);

                    QObject::connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_currentIndexChanged(QString)));
                }
                else
                {
                    ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
                }
            }
        }

        /*设置单列不可编辑*/
        int tmp = 0;
        for(tmp = 0;tmp < rowCount;tmp++)
        {
            ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        }

        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    }
        QObject::connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
}


/*
 * 函数名称：MainWindow::on_CheckListBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：查询list按钮按下时触发的槽函数，将数据库中list表中的数据展示到表格中
 *
 */
void MainWindow::on_CheckListBtn_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    ui->DeletePlanBtn->setEnabled(false);
    ui->EnablePlanBtn->setEnabled(false);
    if(!ui->DeleteListBtn->isEnabled())
    {
        ui->DeleteListBtn->setEnabled(true);
    }
    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
    int planid = ui->PlanIdLe->text().toInt();
    QString planstate = ui->UpdateStateCB->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;
    if(planid == 0 && planstate == "--请选择--")
    {
        ui->SQLTe->setText("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                           "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END) AS UpdateState"
                           ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK)"
                           "ON tu.PosId = tp.ID");

        model->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                    "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                    ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                    "ON tu.PosId = tp.ID"));

    }
    else if(planid != 0 && planstate == "--请选择--")
    {
        ui->SQLTe->setText("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                           "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END) AS UpdateState"
                           ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                           "ON tu.PosId = tp.ID "
                           "WHERE tu.PlanId = " + QString::number(planid));
        model->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                    "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                    ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                    "ON tu.PosId = tp.ID "
                                    "WHERE tu.PlanId = %1 ").arg(planid));

    }
    else if(planid == 0 && planstate != "--请选择--")
    {
        int state = -2;
        if(planstate == "待更新")
        {
            state = 0;
        }
        else if(planstate == "正在更新")
        {
            state = 1;
        }
        else if(planstate == "已下发")
        {
            state = 2;
        }
        else
        {
            state = -1;
        }

        ui->SQLTe->setText("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                           "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END) AS UpdateState"
                           ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                           "ON tu.PosId = tp.ID "
                           "WHERE tu.UpdateState = " + QString::number(state));
        model->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                    "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                    ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                    "ON tu.PosId = tp.ID "
                                    "WHERE tu.UpdateState = %1 ").arg(state));

    }
    else
    {
        int state1 = -2;
        if(planstate == "待更新")
        {
            state1 = 0;
        }
        else if(planstate == "正在更新")
        {
            state1 = 1;
        }
        else if(planstate == "已下发")
        {
            state1 = 2;
        }
        else
        {
            state1 = -1;
        }
        ui->SQLTe->setText("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                           "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END) AS UpdateState"
                           ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                           "ON tu.PosId = tp.ID "
                           "WHERE tu.PlanId = " + QString::number(planid) + " "
                           "AND tu.UpdateState = " + QString::number(state1));
        model->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                    "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                    ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                    "ON tu.PosId = tp.ID "
                                    "WHERE tu.PlanId = %1 "
                                    "AND tu.UpdateState = %2 ").arg(planid).arg(state1));

    }

    ui->tableView->setModel(model);

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("序号")<<tr("Pos编号")<<tr("PosId")<<tr("计划编号")<<tr("更新状态")<<tr("创建日期"));
    int i = 0,j = 0;
    for(i = 0;i < rowCount;i++)
    {
        for(j = 0;j < columnCount;j++)
        {
            QModelIndex index = ui->tableView->model()->index(i,j);
            QVariant data = ui->tableView->model()->data(index);
            if(j == 5)
            {
                QString date = data.toString().section('T',0,0);
                QString time = data.toString().section('T',1,1);
                QString date_time = date + " " + time;
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
            }
            else if(j == 4)
            {
                ComboButton *combox = new ComboButton(); // 下拉选择框控件
                if(data.toString() == "正在更新")
                {
                    combox->addItem("正在更新");
                    combox->addItem("待更新");
                    combox->addItem("已下发");
                    combox->addItem("更新失败");
                }
                else if(data.toString() == "待更新")
                {
                    combox->addItem("待更新");
                    combox->addItem("正在更新");
                    combox->addItem("已下发");
                    combox->addItem("更新失败");
                }
                else if(data.toString() == "已下发")
                {
                    combox->addItem("已下发");
                    combox->addItem("待更新");
                    combox->addItem("正在更新");
                    combox->addItem("更新失败");
                }
                else if(data.toString() == "更新失败")
                {
                    combox->addItem("更新失败");
                    combox->addItem("正在更新");
                    combox->addItem("已下发");
                    combox->addItem("待更新");
                }
                ui->tableWidget->setCellWidget(i, 4, (QWidget*)combox);

                QObject::connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_currentIndexChanged(QString)));
            }
            else
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }
    }

    /*设置单列不可编辑*/
    int tmp = 0;
    for(tmp = 0;tmp < rowCount;tmp++)
    {
        ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        ui->tableWidget->item(tmp,1)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        ui->tableWidget->item(tmp,5)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    QObject::connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
}


/*
 * 函数名称：MainWindow::on_DeleteListBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：删除list按钮按下时触发的槽函数。根据选中的记录的主键，删除数据库中相应的记录，并将删除后的结果实时展示在表格中
 *
 */
void MainWindow::on_DeleteListBtn_clicked()
{
    bool focus = ui->tableWidget->isItemSelected(ui->tableWidget->currentItem()); // 判断是否选中一行
    if(focus == false)
    {
        QMessageBox::warning(NULL, "警告", "请选择要删除的更新列表", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    int row = ui->tableWidget->currentItem()->row();
    QAbstractItemModel *model = ui->tableWidget->model();
    QModelIndex index = model->index(row,0);
    QVariant data = model->data(index);
    int id = data.toInt();

    ui->SQLTe->setText("DELETE FROM [Freemud_PosUpgrade].[dbo].[tUpdateIPList] WHERE ID = " + QString::number(id));

    QSqlQuery query;
    bool res = query.exec(QObject::tr("DELETE FROM [Freemud_PosUpgrade].[dbo].[tUpdateIPList] WHERE ID = %1 ").arg(id));
    if(!res)
    {
        qDebug()<<query.lastError();
        QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
    int planid = ui->PlanIdLe->text().toInt();
    QString planstate = ui->UpdateStateCB->currentText();
    QSqlQueryModel *model1 = new QSqlQueryModel;
    if(planid == 0 && planstate == "--请选择--")
    {
        model1->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                     "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                     ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                     "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                     "ON tu.PosId = tp.ID"));
    }
    else if(planid != 0 && planstate == "--请选择--")
    {
        model1->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                     "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                     ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                     "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                     "ON tu.PosId = tp.ID "
                                     "WHERE tu.PlanId = %1 ").arg(planid));
    }
    else if(planid == 0 && planstate != "--请选择--")
    {
        int state = -2;
        if(planstate == "待更新")
        {
            state = 0;
        }
        else if(planstate == "正在更新")
        {
            state = 1;
        }
        else if(planstate == "已下发")
        {
            state = 2;
        }
        else
        {
            state = -1;
        }
        model1->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                     "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                     ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                     "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                     "ON tu.PosId = tp.ID "
                                     "WHERE tu.UpdateState = %1 ").arg(state));
    }
    else
    {
        int state1 = -2;
        if(planstate == "待更新")
        {
            state1 = 0;
        }
        else if(planstate == "正在更新")
        {
            state1 = 1;
        }
        else if(planstate == "已下发")
        {
            state1 = 2;
        }
        else
        {
            state1 = -1;
        }
        model1->setQuery(QObject::tr("SELECT tu.ID,tp.PosNO,tu.PosId,tu.PlanId,"
                                     "(CASE tu.UpdateState WHEN 1 THEN '正在更新' WHEN 2 THEN '已下发' WHEN -1 THEN '更新失败' ELSE '待更新' END)"
                                     ",tu.CreateDate From [Freemud_PosUpgrade].[dbo].tUpdateIPList AS tu "
                                     "LEFT JOIN [Freemud_PosUpgrade].[dbo].tPos AS tp (NOLOCK) "
                                     "ON tu.PosId = tp.ID "
                                     "WHERE tu.PlanId = %1 "
                                     "AND tu.UpdateState = %2 ").arg(planid).arg(state1));
    }
    ui->tableView->setModel(model1);

    int rowCount = model1->rowCount();
    int columnCount = model1->columnCount();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<  tr("序号")<<tr("Pos编号")<<tr("PosId")<<tr("计划编号")<<tr("更新状态")<<tr("创建日期"));
    int i = 0,j = 0;
    for(i = 0;i < rowCount;i++)
    {
        for(j = 0;j < columnCount;j++)
        {
            QModelIndex index = ui->tableView->model()->index(i,j);
            QVariant data = ui->tableView->model()->data(index);
            if(j == 5)
            {
                QString date = data.toString().section('T',0,0);
                QString time = data.toString().section('T',1,1);
                QString date_time = date + " " + time;
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
            }
            else
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }
    }

    /*设置单列不可编辑*/
    int tmp = 0;
    for(tmp = 0;tmp < rowCount;tmp++)
    {
        ui->tableWidget->item(tmp,0)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        ui->tableWidget->item(tmp,1)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
        ui->tableWidget->item(tmp,5)->setFlags(ui->tableWidget->item(tmp,0)->flags() & (~Qt::ItemIsEditable));
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    QObject::connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));
}


/*
 * 函数名称：MainWindow::on_FullInformationBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：查看所有pos信息按钮按下时触发的槽函数，显示所有pos的更新信息
 *
 */
void MainWindow::on_FullInformationBtn_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    if(ui->EnablePlanBtn->isEnabled())
    {
        ui->EnablePlanBtn->setEnabled(false);
    }
    if(ui->DeletePlanBtn->isEnabled())
    {
        ui->DeletePlanBtn->setEnabled(false);
    }
    if(ui->DeleteListBtn->isEnabled())
    {
        ui->DeleteListBtn->setEnabled(false);
    }
    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));

    QString text = ui->posStateCB->currentText();
    QSqlQueryModel *model = new QSqlQueryModel;
    if(text == "全部")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE 1=1 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE 1=1 "));
    }
    else if(text == "待更新")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.Updatestate = 0 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.Updatestate = 0 "));
    }
    else if(text == "正在更新")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.Updatestate = 1 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.Updatestate = 1 "));
    }
    else if(text == "已下发")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.Updatestate = 2 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.Updatestate = 2 "));
    }
    else
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.Updatestate = -1 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.Updatestate = -1 "));
    }
    ui->tableView->setModel(model);

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("门店ID")<<tr("商户编号")<<tr("自增POS ID")<<tr("Pos编号")
                                               <<tr("最后心跳时间")<<tr("更新版本")<<tr("更新状态")<<tr("计划编号")<<tr("可否使用")<<tr("备忘录"));

    int i = 0,j = 0;
    for(i = 0;i < rowCount;i++)
    {
        for(j = 0;j < columnCount;j++)
        {
            QModelIndex index = ui->tableView->model()->index(i,j);
            QVariant data = ui->tableView->model()->data(index);
            if(j == 4)
            {
                QString date = data.toString().section('T',0,0);
                QString time = data.toString().section('T',1,1);
                QString date_time = date + " " + time;
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
            }
            else
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

}


/*
 * 函数名称：MainWindow::on_currentIndexChanged
 * 函数参数：QString data
 * 函数返回值：void
 * 函数功能：表格中combobox的文本被修改时触发的事件，修改数据库中相应的数据，进行数据的同步
 *
 */
void MainWindow::on_currentIndexChanged(QString data)
{
    int flag = ui->tableWidget->columnCount();
    if(flag == 8)
    {
        int planstate = 0;
        if(data == "启用")
        {
            planstate = 1;
        }
        ComboButton *cb = dynamic_cast<ComboButton *>(QObject::sender()); //获取信号的发送者
        QModelIndex index = ui->tableWidget->indexAt(cb->pos()); //得到当前的index
        int row = index.row();
        QSqlQuery query;
        int id = ui->tableWidget->item(row,0)->text().toInt();

        ui->SQLTe->setText("update [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                           "set PlanState = " + QString::number(planstate) + " "
                           "where ID = " + QString::number(id)) ;
        bool res = query.exec(QObject::tr("update [Freemud_PosUpgrade].[dbo].[tUpdatePlan] "
                                          "set PlanState = %1 "
                                          "where ID = %2 ").arg(planstate).arg(id));
        if(!res)
        {
            qDebug()<<query.lastError();
            QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

    }
    else if(flag == 6)
    {
        int updatestate = -2;
        if(data == "更新失败")
        {
            updatestate = -1;
        }
        else if(data == "待更新")
        {
            updatestate = 0;
        }
        else if(data == "正在更新")
        {
            updatestate = 1;
        }
        else
        {
            updatestate = 2;
        }

        ComboButton *cb = dynamic_cast<ComboButton *>(QObject::sender()); //获取信号的发送者
        QModelIndex index = ui->tableWidget->indexAt(cb->pos()); //得到当前的index
        int row = index.row();
        int id = ui->tableWidget->item(row,0)->text().toInt();

        QSqlQuery query;
        bool res = query.exec(QObject::tr("UPDATE [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                                          "SET UpdateState = %1  WHERE ID = %2 ")
                              .arg(updatestate).arg(id));
        if(!res)
        {
            qDebug()<<query.lastError();
            QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        ui->SQLTe->setText("UPDATE [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                           "SET UpdateState = " + QString::number(updatestate) + " WHERE ID = " + QString::number(id));
    }
}


/*
 * 函数名称：MainWindow::on_ChangeDatabaseBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：修改数据库按钮按下时触发的槽函数，显示修改数据库信息的界面
 *
 */
void MainWindow::on_ChangeDatabaseBtn_clicked()
{
    cdd->setWindowModality(Qt::ApplicationModal);
    cdd->show();
}


/*
 * 函数名称：MainWindow::onSendUpdatePosId
 * 函数参数：QVector<int> data
 * 函数返回值：void
 * 函数功能：启用计划界面，点击启用计划后，将本次启用计划的posid传到当前界面保存起来，并将查看本次按钮使能
 *
 */
void MainWindow::onSendUpdatePosId(QVector<int> data)
{
    UpdatePosId.swap(data);
    ui->PartInformationBtn->setEnabled(true);
}


/*
 * 函数名称：MainWindow::on_PartInformationBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：点击查看本次pos更新详情按钮时触发发的槽函数，只显示最近一次更新的pos信息
 *
 */
void MainWindow::on_PartInformationBtn_clicked()
{

    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    if(ui->EnablePlanBtn->isEnabled())
    {
        ui->EnablePlanBtn->setEnabled(false);
    }
    if(ui->DeletePlanBtn->isEnabled())
    {
        ui->DeletePlanBtn->setEnabled(false);
    }
    if(ui->DeleteListBtn->isEnabled())
    {
        ui->DeleteListBtn->setEnabled(false);
    }
    QObject::disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slotCellChanged(int,int)));

    QString sql = "";
    int size = UpdatePosId.size();
    int tmp = 0;
    QVector<int>::iterator it;
    for(it = UpdatePosId.begin();it != UpdatePosId.end();it++)
    {
        tmp++;
        sql += QString::number(*it);
        if(tmp < size)
        {
            sql += ",";
        }
    }

    QString text = ui->posStateCB->currentText();
    QSqlQueryModel *model = new QSqlQueryModel;
    if(text == "全部")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.posid in (" + sql + ")");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.posid in (%1)" ).arg(sql));

    }
    else if(text == "正在更新")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.posid in (" + sql + ") "
                           "AND tu.UpdateState = 1 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.posid in (%1) "
                                    "AND tu.UpdateState = 1 ").arg(sql));
    }
    else if(text == "已下发")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.posid in (" + sql + ") "
                           "AND tu.UpdateState = 2 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.posid in (%1) "
                                    "AND tu.UpdateState = 2 ").arg(sql));
    }
    else if(text == "更新失败")
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.posid in (" + sql + ") "
                           "AND tu.UpdateState = -1 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.posid in (%1) "
                                    "AND tu.UpdateState = -1 ").arg(sql));
    }
    else
    {
        ui->SQLTe->setText("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                           "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                           "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                           "FROM tPos AS tp (NOLOCK) "
                           "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                           "ON tp.StoreID=ts.ID "
                           "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                           "ON tp.ID=tu.posid "
                           "WHERE tu.posid in (" + sql + ") "
                           "AND tu.UpdateState = 0 ");

        model->setQuery(QObject::tr("SELECT ts.ID as StoreID,ts.StoreID as StoreNO,tp.ID as '自增POS ID', tp.PosNO,tp.LastHeartbeatTime,tp.UpdateVer,"
                                    "(case  tu.Updatestate when 1 then '正在更新' when 2 then '已下发' when -1 then '更新失败' else '待更新' end) as UpdateStatus,"
                                    "isnull(CONVERT (varchar(20), tu.planid),'未安排') as planid,tp.[Enable],tu.memo AS Memo "
                                    "FROM tPos AS tp (NOLOCK) "
                                    "LEFT JOIN dbo.tStore AS ts (NOLOCK) "
                                    "ON tp.StoreID=ts.ID "
                                    "LEFT JOIN [Freemud_PosUpgrade].[dbo].tUpdateIPList tu (NOLOCK) "
                                    "ON tp.ID=tu.posid "
                                    "WHERE tu.posid in (%1) "
                                    "AND tu.UpdateState = 0 ").arg(sql));
    }

    ui->tableView->setModel(model);

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("门店ID")<<tr("商户编号")<<tr("自增POS ID")<<tr("最后心跳时间")
                                               <<tr("Pos编号")<<tr("更新版本")<<tr("更新状态")<<tr("计划编号")<<tr("可否使用")<<tr("备忘录"));
    int i = 0,j = 0;
    for(i = 0;i < rowCount;i++)
    {
        for(j = 0;j < columnCount;j++)
        {
            QModelIndex index = ui->tableView->model()->index(i,j);
            QVariant data = ui->tableView->model()->data(index);
            if(j == 4)
            {
                QString date = data.toString().section('T',0,0);
                QString time = data.toString().section('T',1,1);
                QString date_time = date + " " + time;
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(date_time));
            }
            else
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


}
