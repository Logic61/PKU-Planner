#include "courseheaderwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

CourseHeaderWidget::CourseHeaderWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(12,12,12,12);
    root->setSpacing(8);

    titleLabel = new QLabel("课程名");
    titleLabel->setStyleSheet("font-size:18px; font-weight:700; color:#222;");
    subtitleLabel = new QLabel("教师 · 教室 · 周次");
    subtitleLabel->setStyleSheet("color:#666; font-size:12px;");

    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(titleLabel, 1);

    addTaskBtn = new QPushButton("+ 任务");
    editBtn = new QPushButton("编辑");
    folderBtn = new QPushButton("资料");

    for (QPushButton* b : {addTaskBtn, editBtn, folderBtn}) {
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet("border-radius:8px; padding:6px 10px; background:#FAFAFA; border:1px solid #E8E8E8;");
    }

    top->addWidget(addTaskBtn, 0, Qt::AlignRight);
    top->addWidget(editBtn, 0, Qt::AlignRight);
    top->addWidget(folderBtn, 0, Qt::AlignRight);

    QPushButton* closeBtn = new QPushButton("✕", this);
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("QPushButton{background:#FAFAFA;border:1px solid #E8E8E8;border-radius:6px;font-size:14px;color:#999;}"
                            "QPushButton:hover{background:#F0F0F0;color:#666;}");
    top->addWidget(closeBtn, 0, Qt::AlignRight);

    root->addLayout(top);
    root->addWidget(subtitleLabel);

    connect(addTaskBtn, &QPushButton::clicked, this, &CourseHeaderWidget::addTaskClicked);
    connect(editBtn, &QPushButton::clicked, this, &CourseHeaderWidget::editClicked);
    connect(folderBtn, &QPushButton::clicked, this, &CourseHeaderWidget::folderClicked);
    connect(closeBtn, &QPushButton::clicked, this, &CourseHeaderWidget::closeClicked);
}

void CourseHeaderWidget::setCourse(const Course& course)
{
    titleLabel->setText(course.name);
    subtitleLabel->setText(QString("%1 · %2 · 第%3周").arg(course.teacher).arg(course.location).arg(course.startPeriod));
}
