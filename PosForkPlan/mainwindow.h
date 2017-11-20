#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <forkplanwidget.h>
#include <usingplanwidget.h>
#include <QCloseEvent>
#include <QStandardItem>
#include <QTableWidgetItem>
#include <database.h>
#include <changedatabasedialog.h>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent* e);

private slots:
    void on_CheckPlanBtn_clicked();

    void on_NewPlanBtn_clicked();

    void on_EnablePlanBtn_clicked();

    void slotCellChanged(int,int);

    void on_DeletePlanBtn_clicked();

    void on_CheckListBtn_clicked();

    void on_DeleteListBtn_clicked();

    void on_FullInformationBtn_clicked();

    void on_currentIndexChanged(QString);

    void onSendMessage(QString,QString,QString,QString);

    void on_ChangeDatabaseBtn_clicked();

    void onSendUpdatePosId(QVector<int>);

    void on_PartInformationBtn_clicked();

signals:
    void sendPartnerId(int,int);

    void tableDataChanged();


private:
    Ui::MainWindow *ui;
    ForkPlanWidget *fpw;
    UsingPlanWidget *upw;
    ChangeDatabaseDialog *cdd;
    database* db;
    QVector<int> UpdatePosId;

protected:
    virtual void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
