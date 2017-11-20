#include "usingplanwidget.h"
#include "ui_usingplanwidget.h"

#include <database.h>
#include <QString>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <checkboxdelegate.h>
#include <QtGui>
#include <QStandardItemModel>
#include <QTableView>
#include <QTableWidget>
#include <QVector>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>


/*
 * 函数名称：UsingPlanWidget::paintEvent
 * 函数参数：QPaintEvent *e
 * 函数返回值：void
 * 函数功能：界面的绘图事件，主要绘制界面的横向分割线
 *
 */
void UsingPlanWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter;
    painter.begin(this);
    painter.setPen(QColor(190, 190, 190));
    painter.drawLine(10, 60, 320, 60);
    painter.drawLine(10, 125, 320, 125);
    painter.drawLine(10, 190, 790, 190);
    painter.end();
}


/*
 * 函数名称：UsingPlanWidget::UsingPlanWidge
 * 函数参数：QWidget *parent
 * 函数返回值：无
 * 函数功能：构造函数，并设置界面中主要控件的属性
 *
 */
UsingPlanWidget::UsingPlanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsingPlanWidget)
{
    ui->setupUi(this);
    this->setFixedSize(820,680);
    ui->tableView->hide();
    ui->PosVerLe->setInputMask("0.0.0.00");
    ui->PosHeartBeatLe->setInputMask("0000-00-00");
    ui->PosVerLe->setFocus();

}


/*
 * 函数名称：UsingPlanWidget::~UsingPlanWidge
 * 函数参数：无
 * 函数返回值：无
 * 函数功能：析构函数
 *
 */
UsingPlanWidget::~UsingPlanWidget()
{
    delete ui;
}


/*
 * 函数名称：UsingPlanWidget::onSendPartnerId
 * 函数参数：int data1,int data2
 * 函数返回值：void
 * 函数功能：接受从主界面传过来参数的槽函数，将partnerId和planId接受起来
 *
 */
void UsingPlanWidget::onSendPartnerId(int data1,int data2)
{
    partnerId = data1;
    QString str  = tr("%1").arg(data1);
    ui->PosLable->setText(str + "商户的pos如下：");
    planId = data2;

}


/*
 * 函数名称：UsingPlanWidget::on_pushButton_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：查询按钮按下时触发的槽函数，将该partnerId下所有的pos都展示出来
 *
 */
void UsingPlanWidget::on_pushButton_clicked()
{
    QString posVer = ui->PosVerLe->text();
    QString posHeartBeat = ui->PosHeartBeatLe->text();

    QSqlQueryModel *model = new QSqlQueryModel;

    if(posVer == "..." && posHeartBeat == "--")
    {
        ui->textEdit->setText("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                              " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                              " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                              " ON tp.StoreID = ts.ID"
                              " WHERE PartnerId = " + QString::number(partnerId));
        model->setQuery(QObject::tr("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                                    " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                                    " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                                    " ON tp.StoreID = ts.ID"
                                    " WHERE PartnerId = %1").arg(partnerId));

        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount + 1);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("选择")<<tr("PosID")<<tr("PosNO")
                                                   <<tr("商户ID")<<tr("更新版本")<<tr("最后一次心跳时间")<<tr("PartnerId")<<tr("商户编号"));
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
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(date_time));
                }
                else
                {
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(data.toString()));
                }
            }
        }

        CheckBoxDelegate *ck = new CheckBoxDelegate();
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setItemDelegate(ck);
    }
    else if(posVer == "..." && posHeartBeat != "--")
    {
        ui->textEdit->setText("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                              " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                              " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                              " ON tp.StoreID = ts.ID"
                              " WHERE PartnerId = " + QString::number(partnerId) + " "
                              " AND tp.LastHeartbeatTime > " + posHeartBeat);
        model->setQuery(QObject::tr("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                                    " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                                    " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                                    " ON tp.StoreID = ts.ID"
                                    " WHERE PartnerId = %1 "
                                    " AND tp.LastHeartbeatTime > '%2' ").arg(partnerId).arg(posHeartBeat));

        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount + 1);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("选择")<<tr("PosID")<<tr("Pos编号")<<tr("商户ID")
                                                   <<tr("更新版本")<<tr("最后一次心跳时间")<<tr("PartnerId")<<tr("商户编号"));
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
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(date_time));
                }
                else
                {
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(data.toString()));
                }
            }
        }

        CheckBoxDelegate *ck = new CheckBoxDelegate();
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setItemDelegate(ck);
    }
    else if(posVer != "..." && posHeartBeat == "--")
    {
        ui->textEdit->setText("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                              " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                              " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                              " ON tp.StoreID = ts.ID"
                              " WHERE PartnerId = " + QString::number(partnerId) + " "
                              " AND tp.UpdateVer = " + posVer);
        model->setQuery(QObject::tr("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                                    " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                                    " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                                    " ON tp.StoreID = ts.ID"
                                    " WHERE PartnerId = %1 "
                                    " AND tp.UpdateVer = '%2'").arg(partnerId).arg(posVer));

        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount + 1);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("选择")<<tr("PosID")<<tr("Pos编号")<<tr("商户ID")
                                                   <<tr("更新版本")<<tr("最后一次心跳时间")<<tr("PartnerId")<<tr("商户编号"));
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
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(date_time));
                }
                else
                {
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(data.toString()));
                }
            }
        }

        CheckBoxDelegate *ck = new CheckBoxDelegate();
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setItemDelegate(ck);
    }
    else
    {
        ui->textEdit->setText("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                              " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                              " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                              " ON tp.StoreID = ts.ID"
                              " WHERE PartnerId = " + QString::number(partnerId) + " "
                              " AND tp.UpdateVer = " + posVer + " "
                              " AND tp.LastHeartbeatTime > " + posHeartBeat);
        model->setQuery(QObject::tr("SELECT tp.ID,tp.PosNO,tp.StoreID,tp.UpdateVer,tp.LastHeartbeatTime,ts.PartnerId,ts.StoreID"
                                    " FROM [Freemud_PosUpgrade].[dbo].[tPos] AS tp(NOLOCK)"
                                    " LEFT JOIN [Freemud_PosUpgrade].[dbo].[tStore] AS ts(NOLOCK)"
                                    " ON tp.StoreID = ts.ID"
                                    " WHERE PartnerId = %1 "
                                    " AND tp.UpdateVer = '%2'"
                                    " AND tp.LastHeartbeatTime > '%3' ").arg(partnerId).arg(posVer).arg(posHeartBeat));

        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        ui->tableWidget->setRowCount(rowCount);
        ui->tableWidget->setColumnCount(columnCount + 1);
        ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("选择")<<tr("PosID")<<tr("Pos编号")<<tr("商户ID")
                                                   <<tr("更新版本")<<tr("最后一次心跳时间")<<tr("PartnerId")<<tr("商户编号"));
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
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(date_time));
                }
                else
                {
                    ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(data.toString()));
                }
            }
        }

        CheckBoxDelegate *ck = new CheckBoxDelegate();
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->tableWidget->setItemDelegate(ck);
    }

}


/*
 * 函数名称：UsingPlanWidget::on_StartBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：启用按钮按下时触发的槽函数，根据选中的pos将加入到启用计划的列表当中，并将pos的updatestate修改为待更新
 *
 */
void UsingPlanWidget::on_StartBtn_clicked()
{
    UpdatePosId.clear();
    if(!ui->tableView->verticalHeader()->count())
    {
        QMessageBox::warning(NULL, "警告", "无查询结果，请查询后再启用", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    QVector<int> rowNum;
    int i = 0;
    int row = ui->tableView->model()->rowCount();
    for(i = 0;i < row;i++)
    {
        QModelIndex index = ui->tableWidget->model()->index(i,0);
        QVariant data = ui->tableWidget->model()->data(index);
        if(data.toString() == "true")
        {
            rowNum.push_back(i);
        }
    }
    if(rowNum.count() == 0)
    {
        QMessageBox::warning(NULL, "警告", "请选择你要启用计划的pos", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    ui->textEdit->setText("");

    QVector<int>::iterator it;    
    for(it = rowNum.begin();it != rowNum.end();it++)
    {
        QModelIndex itemIndex = ui->tableWidget->model()->index(*it,1);
        QVariant itemData = ui->tableWidget->model()->data(itemIndex);
        int posId = itemData.toInt();
        UpdatePosId.push_back(posId);

        ui->textEdit->setText("insert into [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                              "(PosId,PlanId,UpdateState,CreateDate) "
                              "values (" + QString::number(posId) + "," + QString::number(planId) + "," + QString::number(0) + "," + "GETDATE());");
        QSqlQuery query;
        bool res = query.exec(QObject::tr("insert into [Freemud_PosUpgrade].[dbo].[tUpdateIPList] "
                                          "(PosId,PlanId,UpdateState,CreateDate) "
                                          "values (%1,%2,%3,GETDATE());")
                              .arg(posId).arg(planId).arg(0));
        if(!res)
        {
            qDebug()<<query.lastError();
            QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }

    QSqlQuery query1;
    bool ret = query1.exec(QObject::tr("update  [Freemud_PosUpgrade].[dbo].[tUpdatePlan] set PlanState = 1 where "
                                       "ID = %1").arg(planId));
    if(!ret)
    {
        qDebug()<<query1.lastError();
        QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    QMessageBox::information(NULL, "信息", "计划分配成功", QMessageBox::Yes, QMessageBox::Yes);
    if(ui->AllChooseBtn->text() == "取消")
    {
        ui->AllChooseBtn->setText("全选");
    }

//    this->hide();
    emit sendUpdatePosId(UpdatePosId);
    ui->PosVerLe->clear();
    ui->PosHeartBeatLe->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
}


/*
 * 函数名称：UsingPlanWidget::closeEventd
 * 函数参数：QCloseEvent *e
 * 函数返回值：void
 * 函数功能：关闭事件，将界面中的一些空间的属性修改为初始状态
 *
 */
void UsingPlanWidget::closeEvent(QCloseEvent *e)
{
    ui->PosVerLe->clear();
    ui->PosHeartBeatLe->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    if(ui->AllChooseBtn->text() == "取消")
    {
        ui->AllChooseBtn->setText("全选");
    }
    e->accept();
}


/*
 * 函数名称：UsingPlanWidget::on_AllChooseBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：全选按钮按下时触发的槽函数，将显示的所有的pos都置为选中状态，按钮的文本修改为取消，点击取消按钮后取消所有选中状态
 *
 */
void UsingPlanWidget::on_AllChooseBtn_clicked()
{
    QString data = "true";
    int row = ui->tableWidget->rowCount();
    if(row == 0)
    {
        QMessageBox::warning(NULL, "警告", "无查询结果，请查询", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    int i = 0;
    QString text = ui->AllChooseBtn->text();
    if(text == "全选")
    {
        for(i = 0;i < row;i++)
        {
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(data));
        }
        ui->AllChooseBtn->setText("取消");
    }
    else
    {
        for(i = 0;i < row;i++)
        {
            ui->tableWidget->setItem(i,0,new QTableWidgetItem("false"));
        }
        ui->AllChooseBtn->setText("全选");
    }

}
