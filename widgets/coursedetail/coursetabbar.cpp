#include "coursetabbar.h"
#include <QHBoxLayout>
#include <QPushButton>

CourseTabBar::CourseTabBar(QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setContentsMargins(8,8,8,8);
    l->setSpacing(6);

    infoBtn = new QPushButton("基本信息");
    taskBtn = new QPushButton("待办事项");
    fileBtn = new QPushButton("课程资料");
    statsBtn = new QPushButton("统计分析");

    for (QPushButton* b : {infoBtn, taskBtn, fileBtn}) {
        b->setCursor(Qt::PointingHandCursor);
        b->setCheckable(true);
        b->setStyleSheet("QPushButton{background:white;border:1px solid #E8E8E8;border-radius:8px;padding:6px 10px;} QPushButton:checked{background:#8B1E2D;color:white;}");
    }

    infoBtn->setChecked(true);

    l->addWidget(infoBtn);
    l->addWidget(taskBtn);
    l->addWidget(fileBtn);
    l->addWidget(statsBtn);

    connect(infoBtn, &QPushButton::clicked, this, [this]() { infoBtn->setChecked(true); taskBtn->setChecked(false); fileBtn->setChecked(false); emit tabChanged(0); });
    connect(taskBtn, &QPushButton::clicked, this, [this]() { infoBtn->setChecked(false); taskBtn->setChecked(true); fileBtn->setChecked(false); emit tabChanged(1); });
    connect(fileBtn, &QPushButton::clicked, this, [this]() { infoBtn->setChecked(false); taskBtn->setChecked(false); fileBtn->setChecked(true); statsBtn->setChecked(false); emit tabChanged(2); });
    connect(statsBtn, &QPushButton::clicked, this, [this]() { infoBtn->setChecked(false); taskBtn->setChecked(false); fileBtn->setChecked(false); statsBtn->setChecked(true); emit tabChanged(3); });
}

void CourseTabBar::setCurrentTab(int index)
{
    infoBtn->setChecked(index == 0);
    taskBtn->setChecked(index == 1);
    fileBtn->setChecked(index == 2);
    statsBtn->setChecked(index == 3);
}
