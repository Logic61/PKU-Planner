#include "weeklysummaryservice.h"
#include "../models/datamanager.h"
#include "../models/task.h"
#include "../services/configservice.h"

QDate WeeklySummaryService::getWeekStart(const QDate& date)
{
    int daysToMonday = (date.dayOfWeek() - 1) % 7;
    return date.addDays(-daysToMonday);
}

bool WeeklySummaryService::isThisWeek(const QDateTime& deadline)
{
    QDate today = QDate::currentDate();
    QDate weekStart = getWeekStart(today);
    QDate weekEnd = weekStart.addDays(7);
    QDate deadlineDate = deadline.date();
    return deadlineDate >= weekStart && deadlineDate < weekEnd;
}

QString WeeklySummaryService::generateSuggestion(int overdue, double rate, const QString& busiest)
{
    if (overdue > 3) {
        return "建议提升DDL规划能力，逾期任务较多需要重点关注";
    }
    if (!busiest.isEmpty() && rate < 60) {
        return QString("建议优先处理 %1 课程的任务，该课程任务较多").arg(busiest);
    }
    if (rate >= 90) {
        return "你本周状态非常棒！继续保持高效学习";
    }
    if (rate >= 70) {
        return "本周表现不错，建议保持当前节奏";
    }
    if (overdue > 0) {
        return "建议关注逾期任务，及时补上进度";
    }
    return "本周任务较少，建议做好下周规划";
}

WeeklySummary WeeklySummaryService::generate()
{
    WeeklySummary summary;
    summary.totalTasks = 0;
    summary.completedTasks = 0;
    summary.overdueTasks = 0;
    summary.upcomingTasks = 0;
    summary.completionRate = 0;
    summary.busiestCourse = "";
    summary.busiestCourseTasks = 0;
    summary.avgEarlyDays = 0;

    const auto tasks = DataManager::instance().tasks();
    QMap<QString, int> courseTaskCount;
    int earlyDaysSum = 0;
    int earlyCount = 0;

    QDate today = QDate::currentDate();
    QDate weekStart = getWeekStart(today);
    QDate weekEnd = weekStart.addDays(7);

    for (const Task& task : tasks) {
        if (!isThisWeek(task.deadline)) continue;

        summary.totalTasks++;
        
        if (task.completed && task.completedAt.isValid()) {
            summary.completedTasks++;
            int earlyDays = task.deadline.date().daysTo(task.completedAt.date());
            if (earlyDays > 0) {
                earlyDaysSum += earlyDays;
                earlyCount++;
            }
        } else if (task.deadline < QDateTime::currentDateTime()) {
            summary.overdueTasks++;
        } else {
            summary.upcomingTasks++;
        }

        courseTaskCount[task.course]++;
    }

    if (summary.totalTasks > 0) {
        summary.completionRate = (double)summary.completedTasks / summary.totalTasks * 100;
    }

    if (earlyCount > 0) {
        summary.avgEarlyDays = (double)earlyDaysSum / earlyCount;
    }

    for (auto it = courseTaskCount.constBegin(); it != courseTaskCount.constEnd(); ++it) {
        if (it.value() > summary.busiestCourseTasks) {
            summary.busiestCourse = it.key();
            summary.busiestCourseTasks = it.value();
        }
    }

    summary.suggestion = generateSuggestion(
        summary.overdueTasks,
        summary.completionRate,
        summary.busiestCourse
    );

    if (summary.completionRate >= 90) {
        summary.mascotMessage = "🎓 太棒了！这周你简直是无敌学霸，继续保持这个状态！";
    } else if (summary.completionRate >= 70) {
        summary.mascotMessage = "💪 这周表现不错，保持节奏就能赢！";
    } else if (summary.overdueTasks > 2) {
        summary.mascotMessage = "😴 攻城狮提醒：是不是又睡过头了？快起来赶作业！";
    } else if (summary.totalTasks == 0) {
        summary.mascotMessage = "🌸 本周没有 DDL，可以好好休息一下啦！";
    } else {
        summary.mascotMessage = "📝 这周还需努力，DDL 正在逼近！";
    }

    return summary;
}

bool WeeklySummaryService::shouldShowOnStartup()
{
    ConfigService& config = ConfigService::instance();
    QDate lastShown = config.getLastSummaryDate();
    QDate today = QDate::currentDate();
    
    int todayWeek = getWeekStart(today).toJulianDay();
    int lastWeek = lastShown.isValid() ? getWeekStart(lastShown).toJulianDay() : -1;
    
    if (todayWeek > lastWeek) {
        return true;
    }
    return false;
}

void WeeklySummaryService::markSummaryShown()
{
    ConfigService& config = ConfigService::instance();
    config.setLastSummaryDate(QDate::currentDate());
}