#include "coursestatswidget.h"
#include "../../models/datamanager.h"
#include "../../models/task.h"
#include "../../ui/theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QDebug>

CourseStatsWidget::CourseStatsWidget(QWidget *parent)
    : QWidget(parent)
    , currentCourseName("")
{
    setStyleSheet(QString("background:%1;").arg(Theme::BACKGROUND));
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(16);

    QHBoxLayout *header = new QHBoxLayout;
    header->setSpacing(10);
    QLabel *icon = new QLabel("📊");
    icon->setStyleSheet("font-size:20px;");
    header->addWidget(icon);

    QLabel *title = new QLabel("课程统计");
    title->setStyleSheet(QString(
        "font-size:18px;"
        "font-weight:600;"
        "color:%1;"
    ).arg(Theme::TEXT_PRIMARY));
    header->addWidget(title);
    header->addStretch();
    root->addLayout(header);

    QFrame *mainCard = new QFrame;
    mainCard->setStyleSheet(QString(
        "QFrame{"
        "background:%1;"
        "border-radius:20px;"
        "padding:20px;"
        "}"
    ).arg(Theme::CARD_BG));
    QVBoxLayout *mainCardLayout = new QVBoxLayout(mainCard);
    mainCardLayout->setSpacing(16);

    QHBoxLayout *completionRow = new QHBoxLayout;
    completionRow->setSpacing(12);

    QFrame *percentCircle = new QFrame;
    percentCircle->setFixedSize(80, 80);
    percentCircle->setStyleSheet(QString(
        "background:qconicalgradient(angle:0,stop:0 %1,stop:1 %2);"
        "border-radius:40px;"
    ).arg(Theme::PRIMARY).arg(Theme::PRIMARY_LIGHT));
    QVBoxLayout *circleLayout = new QVBoxLayout(percentCircle);
    circleLayout->setContentsMargins(4, 4, 4, 4);

    QFrame *innerCircle = new QFrame;
    innerCircle->setStyleSheet("background:white;border-radius:36px;");
    QVBoxLayout *innerLayout = new QVBoxLayout(innerCircle);
    innerLayout->setContentsMargins(0, 0, 0, 0);

    completionPercentLabel = new QLabel("0%");
    completionPercentLabel->setStyleSheet(QString(
        "font-size:20px;"
        "font-weight:700;"
        "color:%1;"
    ).arg(Theme::PRIMARY));
    completionPercentLabel->setAlignment(Qt::AlignCenter);
    innerLayout->addWidget(completionPercentLabel, 1, Qt::AlignCenter);
    circleLayout->addWidget(innerCircle);
    completionRow->addWidget(percentCircle);

    QVBoxLayout *completionInfo = new QVBoxLayout;
    completionInfo->setSpacing(4);

    QLabel *compTitle = new QLabel("完成进度");
    compTitle->setStyleSheet(QString(
        "font-size:14px;"
        "font-weight:600;"
        "color:%1;"
    ).arg(Theme::TEXT_PRIMARY));
    completionInfo->addWidget(compTitle);

    completionLabel = new QLabel("0 / 0 任务", this);
    completionLabel->setStyleSheet(QString(
        "font-size:13px;"
        "color:%1;"
    ).arg(Theme::TEXT_TERTIARY));
    completionInfo->addWidget(completionLabel);

    completionRow->addLayout(completionInfo);
    completionRow->addStretch();
    mainCardLayout->addLayout(completionRow);

    progressBar = new QProgressBar;
    progressBar->setFixedHeight(10);
    progressBar->setStyleSheet(QString(
        "QProgressBar{"
        "background:%1;"
        "border:none;"
        "border-radius:5px;"
        "}"
        "QProgressBar::chunk{"
        "background:%2;"
        "border-radius:5px;"
        "}"
    ).arg(Theme::BORDER).arg(Theme::PRIMARY));
    mainCardLayout->addWidget(progressBar);

    root->addWidget(mainCard);

    QFrame *statsCard = new QFrame;
    statsCard->setStyleSheet(QString(
        "QFrame{"
        "background:%1;"
        "border-radius:20px;"
        "padding:20px;"
        "}"
    ).arg(Theme::CARD_BG));
    QGridLayout *statsGrid = new QGridLayout(statsCard);
    statsGrid->setSpacing(16);

    QFrame *stat1 = createStatItem("✅ 已完成", "0", Theme::SUCCESS);
    completedTasksLabel = stat1->findChild<QLabel*>("value");
    statsGrid->addWidget(stat1, 0, 0);

    QFrame *stat2 = createStatItem("⏳ 待完成", "0", Theme::WARNING);
    pendingTasksLabel = stat2->findChild<QLabel*>("value");
    statsGrid->addWidget(stat2, 0, 1);

    QFrame *stat3 = createStatItem("⚠️ 逾期", "0", Theme::DANGER);
    overdueTasksLabel = stat3->findChild<QLabel*>("value");
    statsGrid->addWidget(stat3, 1, 0);

    QFrame *stat4 = createStatItem("⏰ 提前完成", "0天", Theme::PRIMARY);
    avgTimeLabel = stat4->findChild<QLabel*>("value");
    statsGrid->addWidget(stat4, 1, 1);

    QFrame *stat5 = createStatItem("🎯 按时完成", "0%", Theme::SUCCESS);
    onTimeRateLabel = stat5->findChild<QLabel*>("value");
    statsGrid->addWidget(stat5, 2, 0, 1, 2);

    root->addWidget(statsCard);
    root->addStretch();

    connect(&DataManager::instance(), &DataManager::tasksChanged, this, &CourseStatsWidget::refreshStats);
}

QFrame* CourseStatsWidget::createStatItem(const QString& title, const QString& value, const QString& color)
{
    QFrame *card = new QFrame;
    card->setStyleSheet(QString(
        "background:%1;"
        "border-radius:16px;"
        "padding:16px;"
    ).arg(Theme::BACKGROUND));
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(6);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(QString(
        "font-size:12px;"
        "color:%1;"
    ).arg(Theme::TEXT_SECONDARY));
    layout->addWidget(titleLabel);

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("value");
    valueLabel->setStyleSheet(QString(
        "font-size:24px;"
        "font-weight:700;"
        "color:%1;"
    ).arg(color));
    layout->addWidget(valueLabel);

    return card;
}

void CourseStatsWidget::loadCourse(const Course &course)
{
    currentCourseName = course.name;
    refreshStats();
}

void CourseStatsWidget::refreshStats()
{
    if (currentCourseName.isEmpty()) {
        return;
    }

    const auto tasks = DataManager::instance().tasks();

    int totalCount = 0;
    int completedCount = 0;
    int overdueCount = 0;
    int onTimeCount = 0;
    int validEarlyCount = 0;
    double totalEarlyDays = 0;

    for (const Task &task : tasks) {
        if (task.course != currentCourseName) {
            continue;
        }

        totalCount++;

        if (task.completed) {
            completedCount++;
            if (task.completedAt.isValid()) {
                if (task.completedAt <= task.deadline) {
                    onTimeCount++;
                }
                qint64 secs = task.completedAt.secsTo(task.deadline);
                double days = secs / 86400.0;
                totalEarlyDays += days;
                validEarlyCount++;
            }
        } else if (task.isOverdue()) {
            overdueCount++;
        }
    }

    int pendingCount = totalCount - completedCount;
    int completionRate = totalCount > 0 ? (completedCount * 100 / totalCount) : 0;
    int onTimeRate = completedCount > 0 ? (onTimeCount * 100 / completedCount) : 0;
    double avgEarly = validEarlyCount > 0 ? (totalEarlyDays / validEarlyCount) : 0;

    completionPercentLabel->setText(QString("%1%").arg(completionRate));
    progressBar->setValue(completionRate);
    completionLabel->setText(QString("%1 / %2 任务").arg(completedCount).arg(totalCount));

    completedTasksLabel->setText(QString::number(completedCount));
    pendingTasksLabel->setText(QString::number(pendingCount));
    onTimeRateLabel->setText(QString("%1%").arg(onTimeRate));

    if (overdueCount > 0) {
        overdueTasksLabel->setText(QString::number(overdueCount));
    } else {
        overdueTasksLabel->setText("0");
    }

    if (validEarlyCount > 0) {
        avgTimeLabel->setText(QString("%1天").arg(qRound(avgEarly)));
    } else {
        avgTimeLabel->setText("--");
    }
}