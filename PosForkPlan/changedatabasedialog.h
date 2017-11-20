#ifndef CHANGEDATABASEDIALOG_H
#define CHANGEDATABASEDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ChangeDatabaseDialog;
}

class ChangeDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeDatabaseDialog(QWidget *parent = 0);
    ~ChangeDatabaseDialog();

private slots:
    void on_ConfirmBtn_clicked();

    void on_CancelBtn_clicked();

signals:
    void sendMessage(QString,QString,QString,QString);

private:
    Ui::ChangeDatabaseDialog *ui;
};

#endif // CHANGEDATABASEDIALOG_H
