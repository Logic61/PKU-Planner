#include "statspage.h"
#include "../models/datamanager.h"
#include "../models/task.h"
#include "../models/course.h"
#include "../ui/theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QGridLayout>
#include <QDate>
#include <QList>
#include <QMap>
#include <QVector>
#include <QScrollArea>
#include <algorithm>

namespace {
QFrame* makeCard(const QString &title, const QString &value) {
    QFrame *card = new QFrame;
    card->setStyleSheet("QFrame{background:white;border-radius:20px;padding:18px;}");
    QVBoxLayout *l = new QVBoxLayout(card);
    QLabel *t = new QLabel(title);
    t->setObjectName("title");
    t->setStyleSheet("color:#666;font-size:12px;");
    QLabel *v = new QLabel(value);
    v->setObjectName("value");
    v->setStyleSheet("font-size:20px;font-weight:700;color:#222;");
    l->addWidget(t);
    l->addWidget(v);
    return card;
}

QString dayText(int day) {
    switch (day) {
    case 1: return "周一";
    case 2: return "周二";
    case 3: return "周三";
    case 4: return "周四";
    case 5: return "周五";
    case 6: return "周六";
    case 7: return "周日";
    default: return "";
    }
}
}

StatsPage::StatsPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F7F6F4;");

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background:transparent;");
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *contentWidget = new QWidget;
    contentWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    contentWidget->setMinimumWidth(400);

    QVBoxLayout *root = new QVBoxLayout(contentWidget);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    QLabel *title = new QLabel("学习分析");
    title->setStyleSheet("font-size:18px;font-weight:700;color:#222;");
    root->addWidget(title);

    QLabel *subtitle = new QLabel("本学期效率概览");
    subtitle->setStyleSheet("color:#888;font-size:12px;margin-bottom:8px;");
    root->addWidget(subtitle);

    // 四个统计卡片
    QHBoxLayout *cards = new QHBoxLayout;
    cards->setSpacing(12);
    cardTotal = makeCard("任务总数", "0");
    cardDone = makeCard("已完成率", "0%");
    cardOnTime = makeCard("按时完成率", "0%");
    cardAvg = makeCard("平均提前时间", "0 天");
    cards->addWidget(cardTotal,1);
    cards->addWidget(cardDone,1);
    cards->addWidget(cardOnTime,1);
    cards->addWidget(cardAvg,1);
    root->addLayout(cards);

    // 课程任务数量排行（进度条）
    QFrame *rankCard = new QFrame;
    rankCard->setStyleSheet("QFrame{background:white;border-radius:20px;padding:16px;}");
    QVBoxLayout *rankLayout = new QVBoxLayout(rankCard);
    QLabel *rankTitle = new QLabel("课程任务数量排行");
    rankTitle->setStyleSheet("font-weight:700;color:#222;margin-bottom:8px;");
    rankLayout->addWidget(rankTitle);
    rankContainer = new QVBoxLayout;
    rankContainer->setSpacing(8);
    rankLayout->addLayout(rankContainer);
    root->addWidget(rankCard);

    // 热力日历（本月）
    QFrame *heatCard = new QFrame;
    heatCard->setStyleSheet("QFrame{background:white;border-radius:20px;padding:16px;}");
    QVBoxLayout *heatLayout = new QVBoxLayout(heatCard);
    QLabel *heatTitle = new QLabel("本月 DDL 热力图");
    heatTitle->setStyleSheet("font-weight:700;color:#222;margin-bottom:8px;");
    heatLayout->addWidget(heatTitle);
    heatGrid = new QGridLayout;
    heatGrid->setSpacing(4);
    heatLayout->addLayout(heatGrid);
    root->addWidget(heatCard);

    // 趋势图（最近7天）
    QFrame *trendCard = new QFrame;
    trendCard->setStyleSheet("QFrame{background:white;border-radius:20px;padding:16px;}");
    QVBoxLayout *trendLayout = new QVBoxLayout(trendCard);
    QLabel *trendTitle = new QLabel("最近7天任务完成趋势");
    trendTitle->setStyleSheet("font-weight:700;color:#222;margin-bottom:8px;");
    trendLayout->addWidget(trendTitle);
    trendContainer = new QHBoxLayout;
    trendContainer->setSpacing(8);
    trendLayout->addLayout(trendContainer);
    root->addWidget(trendCard);

    // 智能建议
    QFrame *suggestCard = new QFrame;
    suggestCard->setStyleSheet("QFrame{background:white;border-radius:20px;padding:16px;}");
    QVBoxLayout *suggestLayout = new QVBoxLayout(suggestCard);
    QLabel *suggestTitle = new QLabel("智能建议");
    suggestTitle->setStyleSheet("font-weight:700;color:#222;margin-bottom:8px;");
    suggestLayout->addWidget(suggestTitle);
    suggestContainer = new QVBoxLayout;
    suggestContainer->setSpacing(8);
    suggestLayout->addLayout(suggestContainer);
    root->addWidget(suggestCard);

    refresh();

    connect(&DataManager::instance(), &DataManager::tasksChanged, this, &StatsPage::refresh);
    connect(&DataManager::instance(), &DataManager::coursesChanged, this, &StatsPage::refresh);

    scrollArea->setWidget(contentWidget);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void StatsPage::refresh()
{
    const QList<Task>& tasks = DataManager::instance().tasks();
    const QList<Course>& courses = DataManager::instance().courses();

    if (tasks.isEmpty()) {
        updateEmptyState();
        return;
    }

    updateSummary(tasks);
    updateRanking(tasks);
    updateHeatmap(tasks);
    updateTrend(tasks);
    updateSuggestions(tasks, courses);
}

void StatsPage::updateEmptyState()
{
    QLabel* totalVal = cardTotal->findChild<QLabel*>("value");
    QLabel* doneVal = cardDone->findChild<QLabel*>("value");
    QLabel* onTimeVal = cardOnTime->findChild<QLabel*>("value");
    QLabel* avgVal = cardAvg->findChild<QLabel*>("value");
    if (totalVal) totalVal->setText("0");
    if (doneVal) doneVal->setText("0%");
    if (onTimeVal) onTimeVal->setText("0%");
    if (avgVal) avgVal->setText("0 天");

    clearLayout(rankContainer);
    clearLayout(heatGrid);
    clearLayout(trendContainer);
    clearLayout(suggestContainer);

    QLabel* empty = new QLabel("暂无任务数据");
    empty->setStyleSheet("color:#999;font-size:13px;padding:20px;");
    rankContainer->addWidget(empty);

    QLabel* empty2 = new QLabel("暂无DDL数据");
    empty2->setStyleSheet("color:#999;font-size:13px;padding:20px;");
    heatGrid->addWidget(empty2, 0, 0);

    QLabel* empty3 = new QLabel("暂无完成记录");
    empty3->setStyleSheet("color:#999;font-size:13px;padding:20px;");
    trendContainer->addWidget(empty3);
}

void StatsPage::clearLayout(QLayout* layout)
{
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void StatsPage::updateSummary(const QList<Task>& tasks)
{
    int total = tasks.size();
    int completedCount = 0;
    int onTimeCount = 0;
    double totalDays = 0;
    int validCount = 0;

    for (const Task& t : tasks) {
        if (t.completed) {
            completedCount++;
            if (t.completedAt.isValid()) {
                if (t.completedAt <= t.deadline) {
                    onTimeCount++;
                }
                qint64 secs = t.completedAt.secsTo(t.deadline);
                double days = secs / 86400.0;
                totalDays += days;
                validCount++;
            }
        }
    }

    double completionRate = total > 0 ? completedCount * 100.0 / total : 0;
    double onTimeRate = completedCount > 0 ? onTimeCount * 100.0 / completedCount : 0;
    double avgEarly = validCount > 0 ? totalDays / validCount : 0;

    QLabel* totalVal = cardTotal->findChild<QLabel*>("value");
    QLabel* doneVal = cardDone->findChild<QLabel*>("value");
    QLabel* onTimeVal = cardOnTime->findChild<QLabel*>("value");
    QLabel* avgVal = cardAvg->findChild<QLabel*>("value");

    if (totalVal) totalVal->setText(QString::number(total));
    if (doneVal) doneVal->setText(QString::number(qRound(completionRate)) + "%");
    if (onTimeVal) onTimeVal->setText(QString::number(qRound(onTimeRate)) + "%");
    if (avgVal) avgVal->setText(QString::number(qRound(avgEarly)) + " 天");
}

void StatsPage::updateRanking(const QList<Task>& tasks)
{
    clearLayout(rankContainer);

    QMap<QString, int> counter;
    for (const Task& t : tasks) {
        if (!t.completed) {
            counter[t.course]++;
        }
    }

    QVector<QPair<QString, int>> sorted;
    for (auto it = counter.begin(); it != counter.end(); ++it) {
        sorted.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    int maxCount = sorted.isEmpty() ? 1 : sorted.first().second;

    for (const auto& pair : sorted) {
        QFrame* row = new QFrame;
        row->setStyleSheet("background:#FEFEFE;border-radius:8px;padding:8px;");

        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(8,4,8,4);

        QLabel* name = new QLabel(pair.first);
        name->setStyleSheet("color:#333;font-size:12px;font-weight:500;min-width:80px;");
        rowLayout->addWidget(name);

        QProgressBar* bar = new QProgressBar;
        bar->setRange(0, maxCount);
        bar->setValue(pair.second);
        bar->setStyleSheet(QString(R"(
            QProgressBar {
                border: none;
                background: %1;
                border-radius: 4px;
                height: 8px;
            }
            QProgressBar::chunk {
                background: %2;
                border-radius: 4px;
            }
        )").arg(Theme::BORDER).arg(Theme::PRIMARY));
        bar->setTextVisible(false);
        rowLayout->addWidget(bar, 1);

        QLabel* count = new QLabel(QString::number(pair.second));
        count->setStyleSheet(QString("color:%1;font-size:12px;font-weight:600;min-width:24px;").arg(Theme::PRIMARY));
        count->setAlignment(Qt::AlignRight);
        rowLayout->addWidget(count);

        rankContainer->addWidget(row);
    }

    if (sorted.isEmpty()) {
        QLabel* empty = new QLabel("暂无未完成任务");
        empty->setStyleSheet("color:#999;font-size:12px;padding:8px;");
        rankContainer->addWidget(empty);
    }
}

void StatsPage::updateHeatmap(const QList<Task>& tasks)
{
    clearLayout(heatGrid);

    QDate today = QDate::currentDate();
    QDate firstDayOfMonth(today.year(), today.month(), 1);
    int daysInMonth = firstDayOfMonth.daysInMonth();

    QMap<QDate, int> ddlCount;
    for (const Task& t : tasks) {
        QDate d = t.deadline.date();
        if (d.year() == today.year() && d.month() == today.month()) {
            ddlCount[d]++;
        }
    }

    QLabel* header = new QLabel(QString("%1月").arg(today.month()));
    header->setStyleSheet("color:#666;font-size:11px;margin-bottom:4px;");
    heatGrid->addWidget(header, 0, 0, 1, 7);

    QStringList weekDays = {"一", "二", "三", "四", "五", "六", "日"};
    for (int i = 0; i < 7; ++i) {
        QLabel* day = new QLabel(weekDays[i]);
        day->setStyleSheet("color:#999;font-size:10px;");
        heatGrid->addWidget(day, 1, i);
    }

    int startWeekday = firstDayOfMonth.dayOfWeek();
    int row = 2;
    int col = startWeekday - 1;

    for (int day = 1; day <= daysInMonth; ++day) {
        QDate d(today.year(), today.month(), day);
        int count = ddlCount.value(d, 0);

        QString color;
        if (count == 0) {
            color = Theme::BORDER;
        } else if (count == 1) {
            color = Theme::PRIMARY_LIGHT;
        } else if (count == 2) {
            color = Theme::PRIMARY_LIGHTER;
        } else {
            color = Theme::PRIMARY;
        }

        QFrame* cell = new QFrame;
        cell->setFixedSize(28, 28);
        cell->setStyleSheet(QString("background:%1;border-radius:4px;").arg(color));

        if (d == today) {
            cell->setStyleSheet(QString("background:%1;border-radius:4px;border:2px solid #333;").arg(color));
        }

        if (d < today) {
            cell->setEnabled(false);
        }

        heatGrid->addWidget(cell, row, col);

        col++;
        if (col >= 7) {
            col = 0;
            row++;
        }
    }

    QLabel* legend = new QLabel("少→多");
    legend->setStyleSheet("color:#999;font-size:10px;margin-top:8px;");
    heatGrid->addWidget(legend, row + 1, 0);
}

void StatsPage::updateTrend(const QList<Task>& tasks)
{
    clearLayout(trendContainer);

    QDate today = QDate::currentDate();
    QMap<QDate, int> completedPerDay;

    for (int i = 6; i >= 0; i--) {
        QDate d = today.addDays(-i);
        completedPerDay[d] = 0;
    }

    for (const Task& t : tasks) {
        if (t.completed && t.completedAt.isValid()) {
            QDate d = t.completedAt.date();
            if (completedPerDay.contains(d)) {
                completedPerDay[d]++;
            }
        }
    }

    int maxCount = 1;
    for (auto v : completedPerDay.values()) {
        if (v > maxCount) maxCount = v;
    }

    for (int i = 6; i >= 0; i--) {
        QDate d = today.addDays(-i);
        int count = completedPerDay.value(d, 0);

        QFrame* dayFrame = new QFrame;
        dayFrame->setStyleSheet("background:#FEFEFE;border-radius:8px;padding:8px;");
        QVBoxLayout* dayLayout = new QVBoxLayout(dayFrame);
        dayLayout->setContentsMargins(4,4,4,4);
        dayLayout->setSpacing(6);

        QLabel* countLabel = new QLabel(QString::number(count));
        countLabel->setStyleSheet(QString("font-size:14px;font-weight:700;color:%1;").arg(Theme::PRIMARY));
        countLabel->setAlignment(Qt::AlignCenter);

        QString dayStr;
        if (i == 0) dayStr = "今天";
        else if (i == 1) dayStr = "昨天";
        else dayStr = QString("%1日").arg(d.day());

        QLabel* dayLabel = new QLabel(dayStr);
        dayLabel->setStyleSheet("color:#999;font-size:10px;");
        dayLabel->setAlignment(Qt::AlignCenter);

        QProgressBar* bar = new QProgressBar;
        bar->setOrientation(Qt::Vertical);
        bar->setRange(0, maxCount);
        bar->setValue(count);
        bar->setFixedWidth(12);
        bar->setMinimumHeight(60);
        bar->setStyleSheet(QString(R"(
            QProgressBar {
                border: none;
                background: %1;
                border-radius: 6px;
            }
            QProgressBar::chunk {
                background: %2;
                border-radius: 6px;
            }
        )").arg(Theme::BORDER).arg(Theme::PRIMARY));
        bar->setTextVisible(false);

        dayLayout->addWidget(countLabel, 0, Qt::AlignCenter);
        dayLayout->addWidget(bar, 1, Qt::AlignCenter);
        dayLayout->addWidget(dayLabel, 0, Qt::AlignCenter);

        trendContainer->addWidget(dayFrame, 1);
    }
}

void StatsPage::updateSuggestions(const QList<Task>& tasks, const QList<Course>& courses)
{
    clearLayout(suggestContainer);

    QDateTime now = QDateTime::currentDateTime();

    int overdueCount = 0;
    for (const Task& t : tasks) {
        if (t.isOverdue()) overdueCount++;
    }
    if (overdueCount > 0) {
        QLabel* tip = new QLabel(QString("⚠ 您有 %1 个逾期任务，请尽快处理").arg(overdueCount));
        tip->setStyleSheet("color:#C62828;font-size:12px;padding:8px;background:#FFEBEE;border-radius:8px;");
        suggestContainer->addWidget(tip);
    }

    QMap<QString, int> courseTaskCount;
    for (const Task& t : tasks) {
        if (!t.completed) courseTaskCount[t.course]++;
    }
    for (const auto& pair : courseTaskCount.toStdMap()) {
        if (pair.second >= 3) {
            QLabel* tip = new QLabel(QString("📚 %1 课程有 %2 个待办任务，建议提前规划").arg(pair.first).arg(pair.second));
            tip->setStyleSheet("color:#E65100;font-size:12px;padding:8px;background:#FFF3E0;border-radius:8px;");
            suggestContainer->addWidget(tip);
        }
    }

    int total = tasks.size();
    int completed = 0;
    for (const Task& t : tasks) {
        if (t.completed) completed++;
    }
    double rate = total > 0 ? completed * 100.0 / total : 0;
    if (total > 5 && rate >= 80) {
        QLabel* tip = new QLabel(QString("🎉 太棒了！任务完成率 %1%，继续保持！").arg(qRound(rate)));
        tip->setStyleSheet("color:#2E7D32;font-size:12px;padding:8px;background:#E8F5E9;border-radius:8px;");
        suggestContainer->addWidget(tip);
    }

    int urgentCount = 0;
    for (const Task& t : tasks) {
        if (!t.completed && t.daysLeft() >= 0 && t.daysLeft() <= 2) urgentCount++;
    }
    if (urgentCount > 0) {
        QLabel* tip = new QLabel(QString("⏰ 即将到期：%1 个任务需要在2天内完成").arg(urgentCount));
        tip->setStyleSheet("color:#F57C00;font-size:12px;padding:8px;background:#FFF8E1;border-radius:8px;");
        suggestContainer->addWidget(tip);
    }

    if (tasks.isEmpty()) {
        QLabel* tip = new QLabel("📝 暂无任务，快去添加一些任务吧！");
        tip->setStyleSheet("color:#666;font-size:12px;padding:8px;background:#F5F5F5;border-radius:8px;");
        suggestContainer->addWidget(tip);
    }

    if (suggestContainer->count() == 0) {
        QLabel* tip = new QLabel("💡 继续保持，当前没有需要特别注意的事项");
        tip->setStyleSheet("color:#666;font-size:12px;padding:8px;background:#F5F5F5;border-radius:8px;");
        suggestContainer->addWidget(tip);
    }
}

void StatsPage::refreshData()
{
    refresh();
}