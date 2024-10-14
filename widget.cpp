#include "widget.h"
#include "./ui_widget.h"

widget::widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);
    connect(ui->button_BrowseSource, SIGNAL(clicked(bool)), this, SLOT(on_button_BrowseSource_clicked()));
        connect(ui->button_BrowseBackup, SIGNAL(clicked(bool)), this, SLOT(on_button_BrowseBackup_clicked()));
        connect(ui->button_Backup, SIGNAL(clicked(bool)), this, SLOT(on_button_Backup_clicked()));
        connect(ui->button_Restore, SIGNAL(clicked(bool)), this, SLOT(widget::on_button_Restore_clicked()));
}

widget::~widget()
{
    delete ui;
}


void widget::on_button_BrowseSource_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择源目录"), ui->lineEdit_Source->text());
    if (!dir.isEmpty()) {
        ui->lineEdit_Source->setText(dir);
    }
}

void widget::on_button_BrowseBackup_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择备份目录"), ui->lineEdit_Backup->text());
    if (!dir.isEmpty()) {
        ui->lineEdit_Backup->setText(dir);
    }
}

void widget::on_button_Backup_clicked()
{
    QString sourceDir = ui->lineEdit_Source->text();
    QString backupDir = ui->lineEdit_Backup->text();
    if (sourceDir.isEmpty() || backupDir.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请选择源目录和备份目录！"));
        return;
    }
    // 在这里调用备份函数
    if (!QDir().mkpath(backupDir)) {
            QMessageBox::critical(this, tr("错误"), tr("无法创建备份目录：%1").arg(backupDir));
            return;
        }

        if (backupDirectory(sourceDir, backupDir)) {
            QMessageBox::information(this, tr("成功"), tr("备份成功！"));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("备份失败！"));
        }

    qDebug() << "备份源目录：" << sourceDir << "到备份目录：" << backupDir;
}

void widget::on_button_Restore_clicked()
{
    QString backupDir = ui->lineEdit_Backup->text();
    QString restoreDir = ui->lineEdit_Source->text();
    if (backupDir.isEmpty() || restoreDir.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请选择备份目录和恢复目录！"));
        return;
    }
    // 在这里调用恢复函数
    if (!QDir().mkpath(restoreDir)) {
            QMessageBox::critical(this, tr("错误"), tr("无法创建恢复目录：%1").arg(restoreDir));
            return;
        }

        if (restoreDirectory(backupDir, restoreDir)) {
            QMessageBox::information(this, tr("成功"), tr("恢复成功！"));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("恢复失败！"));
        }
    qDebug() << "从备份目录：" << backupDir << "恢复到：" << restoreDir;
}
bool widget::backupDirectory(const QString &sourceDir, const QString &backupDir)
{
    QDir source(sourceDir);
    QDir backup(backupDir);

    if (!source.exists() || !backup.exists()) {
        return false;
    }

    QStringList files = source.entryList(QDir::Files | QDir::NoSymLinks);
    foreach (QString file, files) {
        QString sourceFile = sourceDir + "/" + file;
        QString backupFile = backupDir + "/" + file;
        if (!QFile::copy(sourceFile, backupFile)) {
            return false;
        }
    }

    QStringList dirs = source.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QString dir, dirs) {
        QString sourceSubDir = sourceDir + "/" + dir;
        QString backupSubDir = backupDir + "/" + dir;
        if (!QDir().mkpath(backupSubDir)) {
            return false;
        }
        if (!backupDirectory(sourceSubDir, backupSubDir)) {
            return false;
        }
    }

    return true;
}

bool widget::restoreDirectory(const QString &backupDir, const QString &restoreDir)
{
    QDir backup(backupDir);
    QDir restore(restoreDir);

    if (!backup.exists() || !restore.exists()) {
        return false;
    }

    QStringList files = backup.entryList(QDir::Files | QDir::NoSymLinks);
    foreach (QString file, files) {
        QString backupFile = backupDir + "/" + file;
        QString restoreFile = restoreDir + "/" + file;
        if (!QFile::copy(backupFile, restoreFile)) {
            return false;
        }
    }

    QStringList dirs = backup.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QString dir, dirs) {
        QString backupSubDir = backupDir + "/" + dir;
        QString restoreSubDir = restoreDir + "/" + dir;
        if (!QDir().mkpath(restoreSubDir)) {
            return false;
        }
        if (!restoreDirectory(backupSubDir, restoreSubDir)) {
            return false;
        }
    }

    return true;
}
