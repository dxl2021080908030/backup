#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class widget; }
QT_END_NAMESPACE

class widget : public QWidget
{
    Q_OBJECT

public:
    widget(QWidget *parent = nullptr);
    ~widget();

private slots:
    void on_button_BrowseSource_clicked();
    void on_button_BrowseBackup_clicked();
    void on_button_Backup_clicked();
    void on_button_Restore_clicked();

private:
    Ui::widget *ui;
    bool backupDirectory(const QString &sourceDir, const QString &backupDir);
    bool restoreDirectory(const QString &backupDir, const QString &restoreDir);


};
#endif // WIDGET_H
