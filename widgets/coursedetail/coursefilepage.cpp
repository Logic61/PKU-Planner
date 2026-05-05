#include "coursefilepage.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>
#include <functional>
#include <algorithm>

namespace {
QString timeAgo(const QDateTime& time)
{
    const qint64 seconds = time.secsTo(QDateTime::currentDateTime());
    if (seconds < 60) {
        return "刚刚";
    }
    if (seconds < 3600) {
        return QString::number(seconds / 60) + " 分钟前修改";
    }
    if (seconds < 86400) {
        return QString::number(seconds / 3600) + " 小时前修改";
    }
    return time.toString("yyyy-MM-dd hh:mm");
}

QString noteTemplate()
{
    return "# 今日课程笔记\n\n## 内容\n\n## 作业\n\n## 疑问\n";
}
}

CourseFilePage::CourseFilePage(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F8F6F4;");

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    QLabel* title = new QLabel("课程资料", this);
    title->setStyleSheet("font-size:18px;font-weight:700;color:#222;");
    root->addWidget(title);

    QFrame* bindCard = new QFrame(this);
    bindCard->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");
    QVBoxLayout* bindLayout = new QVBoxLayout(bindCard);
    bindLayout->setContentsMargins(18, 16, 18, 16);
    bindLayout->setSpacing(10);

    QLabel* bindTitle = new QLabel("当前绑定目录", bindCard);
    bindTitle->setStyleSheet("font-size:14px;font-weight:700;color:#222;");
    pathLabel = new QLabel("未绑定目录", bindCard);
    pathLabel->setWordWrap(true);
    pathLabel->setStyleSheet("color:#4B3A35;font-size:13px;");

    QHBoxLayout* bindBtnLayout = new QHBoxLayout;
    bindBtnLayout->setSpacing(8);
    bindFolderBtn = new QPushButton("更换目录", bindCard);
    openFolderBtn = new QPushButton("打开目录", bindCard);
    for (QPushButton* button : {bindFolderBtn, openFolderBtn}) {
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet(
            "QPushButton{background:#FBECEE;color:#8B1E2D;border:1px solid #F0D6DA;border-radius:12px;padding:8px 14px;font-weight:700;}"
            "QPushButton:hover{border:1px solid #8B1E2D;}"
        );
    }
    bindBtnLayout->addWidget(bindFolderBtn);
    bindBtnLayout->addWidget(openFolderBtn);
    bindBtnLayout->addStretch();

    bindLayout->addWidget(bindTitle);
    bindLayout->addWidget(pathLabel);
    bindLayout->addLayout(bindBtnLayout);
    root->addWidget(bindCard);

    QFrame* quickCard = new QFrame(this);
    quickCard->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");
    QVBoxLayout* quickLayout = new QVBoxLayout(quickCard);
    quickLayout->setContentsMargins(18, 16, 18, 16);
    quickLayout->setSpacing(10);

    QLabel* quickTitle = new QLabel("快速操作", quickCard);
    quickTitle->setStyleSheet("font-size:14px;font-weight:700;color:#222;");
    QHBoxLayout* actionLayout = new QHBoxLayout;
    actionLayout->setSpacing(8);
    newNoteBtn = new QPushButton("+ 新建笔记", quickCard);
    recordBtn = new QPushButton("开始录音", quickCard);
    for (QPushButton* button : {newNoteBtn, recordBtn}) {
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet(
            "QPushButton{background:#FBECEE;color:#8B1E2D;border:1px solid #F0D6DA;border-radius:12px;padding:8px 14px;font-weight:700;}"
            "QPushButton:hover{border:1px solid #8B1E2D;}"
        );
    }
    actionLayout->addWidget(newNoteBtn);
    actionLayout->addWidget(recordBtn);
    actionLayout->addStretch();
    quickLayout->addWidget(quickTitle);
    quickLayout->addLayout(actionLayout);
    root->addWidget(quickCard);

    QFrame* recentCard = new QFrame(this);
    recentCard->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");
    QVBoxLayout* recentLayout = new QVBoxLayout(recentCard);
    recentLayout->setContentsMargins(18, 16, 18, 16);
    recentLayout->setSpacing(10);

    QLabel* recentTitle = new QLabel("最近文件", recentCard);
    recentTitle->setStyleSheet("font-size:14px;font-weight:700;color:#222;");
    fileList = new QListWidget(recentCard);
    fileList->setStyleSheet(R"(
        QListWidget {
            border: none;
            background: transparent;
        }
        QListWidget::item {
            padding: 10px 12px;
            border-radius: 12px;
            background: #FCFBFA;
            margin-bottom: 8px;
        }
        QListWidget::item:hover {
            background: #FBECEE;
        }
    )");
    fileList->setMinimumHeight(180);

    recentLayout->addWidget(recentTitle);
    recentLayout->addWidget(fileList);
    root->addWidget(recentCard, 1);

    connect(bindFolderBtn, &QPushButton::clicked, this, &CourseFilePage::bindFolder);
    connect(openFolderBtn, &QPushButton::clicked, this, &CourseFilePage::openFolder);
    connect(newNoteBtn, &QPushButton::clicked, this, &CourseFilePage::createNote);
    connect(recordBtn, &QPushButton::clicked, this, &CourseFilePage::showRecordingPlaceholder);
    connect(fileList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        const QString path = item->data(Qt::UserRole).toString();
        if (!path.isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    });
}

void CourseFilePage::loadCourse(const Course& course)
{
    currentCourse = course;
    pathLabel->setText(currentCourse.folderPath.trimmed().isEmpty() ? "未绑定目录" : currentCourse.folderPath);
    refreshFileList();
}

void CourseFilePage::refreshFileList()
{
    fileList->clear();

    const QString folderPath = currentCourse.folderPath.trimmed();
    if (folderPath.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("尚未选择课程资料目录");
        item->setFlags(Qt::NoItemFlags);
        fileList->addItem(item);
        return;
    }

    QDir dir(folderPath);
    if (!dir.exists()) {
        QListWidgetItem* item = new QListWidgetItem("目录不存在，请重新绑定");
        item->setFlags(Qt::NoItemFlags);
        fileList->addItem(item);
        return;
    }

    // 递归收集所有文件并按修改时间排序
    QList<QPair<QFileInfo, QString>> allFiles; // (QFileInfo, relative path from base dir)
    std::function<void(const QDir&, const QString&)> collectFiles = 
        [&](const QDir& baseDir, const QString& relativePath) {
        QFileInfoList entries = baseDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
        for (const QFileInfo& info : entries) {
            QString itemRelativePath = relativePath.isEmpty() ? info.fileName() : (relativePath + "/" + info.fileName());
            if (info.isFile()) {
                allFiles.append(qMakePair(info, itemRelativePath));
            } else if (info.isDir()) {
                QDir subDir(info.absoluteFilePath());
                collectFiles(subDir, itemRelativePath);
            }
        }
    };
    
    collectFiles(dir, "");
    
    // 按修改时间排序（最近的在前）
    std::sort(allFiles.begin(), allFiles.end(), 
        [](const QPair<QFileInfo, QString>& a, const QPair<QFileInfo, QString>& b) {
            return a.first.lastModified() > b.first.lastModified();
        });
    
    // 添加前10个文件到列表
    const int MAX_DISPLAY_FILES = 10;
    int count = 0;
    for (const auto& pair : allFiles) {
        if (count >= MAX_DISPLAY_FILES) break;
        const QFileInfo& info = pair.first;
        const QString& relativePath = pair.second;
        QListWidgetItem* item = new QListWidgetItem(relativePath + "\n" + timeAgo(info.lastModified()));
        item->setData(Qt::UserRole, info.absoluteFilePath());
        fileList->addItem(item);
        count++;
    }

    if (allFiles.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("暂无文件");
        item->setFlags(Qt::NoItemFlags);
        fileList->addItem(item);
    }
}

void CourseFilePage::bindFolder()
{
    const QString selectedPath = QFileDialog::getExistingDirectory(this, "选择课程资料目录", currentCourse.folderPath);
    if (selectedPath.isEmpty()) {
        return;
    }

    currentCourse.folderPath = selectedPath;
    pathLabel->setText(selectedPath);
    refreshFileList();
    emit courseUpdated(currentCourse);
}

void CourseFilePage::openFolder()
{
    const QString folderPath = currentCourse.folderPath.trimmed();
    if (folderPath.isEmpty()) {
        QMessageBox::information(this, "提示", "请先绑定目录");
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}

void CourseFilePage::createNote()
{
    if (currentCourse.folderPath.trimmed().isEmpty()) {
        QMessageBox::information(this, "提示", "请先绑定目录");
        return;
    }

    QDir dir(currentCourse.folderPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "提示", "目录不存在，请重新绑定");
        return;
    }

    const QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm") + "_note.md";
    const QString filePath = dir.absoluteFilePath(fileName);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "提示", "无法创建笔记文件");
        return;
    }

    file.write(noteTemplate().toUtf8());
    file.close();
    refreshFileList();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void CourseFilePage::showRecordingPlaceholder()
{
    QMessageBox::information(this, "开始录音", "该功能将在后续版本开放");
}