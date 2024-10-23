#include "widget.h"
#include "./ui_widget.h"

widget::widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);
    connect(ui->button_BrowseSource, SIGNAL(released(bool)), this, SLOT(on_button_BrowseSource_clicked()));
        connect(ui->button_BrowseBackup, SIGNAL(released(bool)), this, SLOT(on_button_BrowseBackup_clicked()));
        connect(ui->button_Backup, SIGNAL(released(bool)), this, SLOT(on_button_Backup_clicked()));
        connect(ui->button_BrowseRestoreFile,SIGNAL(released(bool)),this,SLOT(on_button_BrowseRestoreFile_clicked()));
        connect(ui->button_Restore, SIGNAL(released(bool)), this, SLOT(on_button_Restore_clicked()));
}

widget::~widget()
{
    delete ui;
}


void widget::on_button_BrowseSource_clicked()
{
    QFileDialog dialog(this, tr("选择备份数据"), ui->lineEdit_Source->text(), tr("所有文件 (*)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 将文件路径转换为字符串，并在LineEdit中显示
            ui->lineEdit_Source->setText(files.join(";"));
        }
    };
}
void widget::on_button_BrowseBackup_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("选择备份目录"), ui->lineEdit_Backup->text());
    if (!path.isEmpty()) {
        ui->lineEdit_Backup->setText(path);
    }
}
void widget::on_button_Backup_clicked()
{
    QString sourcePathsStr = ui->lineEdit_Source->text();
    QString backupPath = ui->lineEdit_Backup->text();

    if (sourcePathsStr.isEmpty() || backupPath.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请选择源文件和备份目录！"));
        return;
    }

    if (!QDir().mkpath(backupPath)) {
        QMessageBox::critical(this, tr("错误"), tr("无法创建备份目录：%1").arg(backupPath));
        return;
    }

    QStringList sourcePaths = sourcePathsStr.split(";");
    bool success = true;

    foreach (const QString &sourcePath, sourcePaths) {
        QFileInfo fileInfo(sourcePath);
        if (fileInfo.isFile()) {
            // 处理单个文件
            QString backupFile = backupPath + "/" + fileInfo.fileName();
            if (!QFile::copy(sourcePath, backupFile)) {
                success = false;
                break;
            }
        } else {
            QMessageBox::warning(this, tr("警告"), tr("跳过非文件项：%1").arg(sourcePath));
        }
    }

    if (success) {
        QMessageBox::information(this, tr("成功"), tr("备份成功！"));
    } else {
        QMessageBox::critical(this, tr("错误"), tr("备份失败！"));
    }
}
void widget::on_button_BrowseRestoreFile_clicked()
{
    // 使用QFileDialog来选择一个或多个文件
    QFileDialog dialog(this, tr("选择要恢复的文件"), ui->lineEdit_Backup->text(), tr("所有文件 (*)"));
    dialog.setFileMode(QFileDialog::ExistingFiles); // 设置为选择一个或多个文件
    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 将文件路径转换为字符串，并在LineEdit中显示
            ui->lineEdit_Backup->setText(files.join(";"));
        }
    };
}
void widget::on_button_Restore_clicked()
{
    QString backupPath = ui->lineEdit_Backup->text();
        QString restorePath = ui->lineEdit_Source->text();
        if (backupPath.isEmpty() || restorePath.isEmpty()) {
            QMessageBox::information(this, tr("提示"), tr("请选择备份文件和恢复位置！"));
            return;
        }

        QFileInfo fileInfo(backupPath);
        if (fileInfo.isFile()) {
            // 处理单个文件
            QString restoreFile = restorePath + "/" + fileInfo.fileName();
            if (QFile::copy(backupPath, restoreFile)) {
                QMessageBox::information(this, tr("成功"), tr("恢复成功！"));
            } else {
                QMessageBox::critical(this, tr("错误"), tr("恢复失败！"));
            }
        } else {
            QMessageBox::warning(this, tr("警告"), tr("备份路径不是一个有效的文件：%1").arg(backupPath));
        }
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
