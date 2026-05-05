#include "coursestatswidget.h"
#include "../../models/datamanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

CourseStatsWidget::CourseStatsWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F8F6F4;");
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(12,12,12,12);
    root->setSpacing(8);

    QLabel *title = new QLabel("课程统计");
    title->setStyleSheet("font-weight:700;font-size:16px;color:#222;");
    root->addWidget(title);

    QFrame *card = new QFrame;
    card->setStyleSheet("QFrame{background:white;border-radius:14px;padding:12px;}");
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->addWidget(new QLabel("任务完成率: --"));
    cl->addWidget(new QLabel("平均提前时间: --"));
    cl->addWidget(new QLabel("总任务数: --"));
    root->addWidget(card);
}

void CourseStatsWidget::loadCourse(const Course &course)
{
    // This will be filled to compute per-course stats from DataManager
}
