#include "courseinfopage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QStringList>
#include <QSet>

#include <algorithm>

#include "../../models/datamanager.h"

namespace {
QString dayText(int day)
{
    switch (day) {
    case 1: return "周一";
    case 2: return "周二";
    case 3: return "周三";
    case 4: return "周四";
    case 5: return "周五";
    case 6: return "周六";
    case 7: return "周日";
    default: return "未设置";
    }
}

QString weekTypeText(int weekType)
{
    switch (weekType) {
    case 1: return "单周";
    case 2: return "双周";
    default: return "每周";
    }
}

QString safeText(const QString& text, const QString& fallback)
{
    return text.trimmed().isEmpty() ? fallback : text;
}

QString scheduleLine(const Course& course)
{
    if (course.day < 1 || course.day > 7 || course.startPeriod <= 0 || course.endPeriod <= 0) {
        return QString();
    }

    QString line = QString("%1 %2-%3节").arg(dayText(course.day)).arg(course.startPeriod).arg(course.endPeriod);
    if (course.weekType == 1) {
        line += "（单周）";
    } else if (course.weekType == 2) {
        line += "（双周）";
    }
    return line;
}
}

CourseInfoPage::CourseInfoPage(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F8F6F4;");

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    QWidget* pageHeader = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(pageHeader);
    headerLayout->setContentsMargins(4, 0, 4, 0);
    headerLayout->setSpacing(10);

    QLabel* titleLabel = new QLabel("课程信息", pageHeader);
    titleLabel->setStyleSheet("font-size:18px;font-weight:700;color:#222;");

    editBtn = new QPushButton("编辑课程", pageHeader);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setStyleSheet(
        "QPushButton{background:white;border:1px solid #EFEAEA;border-radius:10px;padding:6px 12px;color:#4B3A35;}"
        "QPushButton:hover{border:1px solid #8B1E2D;}"
    );

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(editBtn);

    root->addWidget(pageHeader);
    root->addWidget(createProgressCard());

    QWidget* middleRow = new QWidget(this);
    QHBoxLayout* middleLayout = new QHBoxLayout(middleRow);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleLayout->setSpacing(12);
    middleLayout->addWidget(createScheduleCard(), 1);
    middleLayout->addWidget(createExamCard(), 1);
    root->addWidget(middleRow);

    root->addWidget(createTeacherCard());
    root->addWidget(createNoteCard(), 1);

    connect(editBtn, &QPushButton::clicked, this, [this]() {
        if (noteEdit) {
            noteEdit->setFocus();
        }
    });
}

QWidget* CourseInfoPage::createProgressCard()
{
    QFrame* card = new QFrame(this);
    card->setStyleSheet(
        "QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}"
    );

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    QLabel* title = new QLabel("学习进度", card);
    title->setStyleSheet("font-size:15px;font-weight:700;color:#222;");

    progressLabel = new QLabel("0%", card);
    progressLabel->setStyleSheet("font-size:28px;font-weight:800;color:#8B1E2D;");

    progressBar = new QProgressBar(card);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(12);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: none;
            background: #F5F5F5;
            border-radius: 6px;
        }
        QProgressBar::chunk {
            background: #8B1E2D;
            border-radius: 6px;
        }
    )");

    QLabel* detailLabel = new QLabel("0 / 0 tasks completed", card);
    detailLabel->setObjectName("progressDetailLabel");
    detailLabel->setStyleSheet("color:#7A746E;font-size:12px;");

    layout->addWidget(title);
    layout->addWidget(progressLabel);
    layout->addWidget(progressBar);
    layout->addWidget(detailLabel);

    return card;
}

QWidget* CourseInfoPage::createScheduleCard()
{
    QFrame* card = new QFrame(this);
    card->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    QLabel* title = new QLabel("上课安排", card);
    title->setStyleSheet("font-size:15px;font-weight:700;color:#222;");

    scheduleLabel = new QLabel("暂未设置", card);
    scheduleLabel->setWordWrap(true);
    scheduleLabel->setStyleSheet("font-size:18px;font-weight:700;color:#4B3A35;");

    locationLabel = new QLabel("", card);
    locationLabel->setStyleSheet("color:#7A746E;font-size:13px;");

    layout->addWidget(title);
    layout->addWidget(scheduleLabel);
    layout->addWidget(locationLabel);
    layout->addStretch();

    return card;
}

QWidget* CourseInfoPage::createExamCard()
{
    QFrame* card = new QFrame(this);
    card->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    QLabel* title = new QLabel("考试信息", card);
    title->setStyleSheet("font-size:15px;font-weight:700;color:#222;");

    examLabel = new QLabel("暂未设置", card);
    examLabel->setWordWrap(true);
    examLabel->setStyleSheet("font-size:18px;font-weight:700;color:#4B3A35;");

    layout->addWidget(title);
    layout->addWidget(examLabel);
    layout->addStretch();

    return card;
}

QWidget* CourseInfoPage::createTeacherCard()
{
    QFrame* card = new QFrame(this);
    card->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(6);

    QLabel* title = new QLabel("教师信息", card);
    title->setStyleSheet("font-size:15px;font-weight:700;color:#222;");

    teacherLabel = new QLabel("未填写", card);
    teacherLabel->setStyleSheet("font-size:18px;font-weight:700;color:#4B3A35;");

    contactLabel = new QLabel("未填写联系方式", card);
    contactLabel->setStyleSheet("color:#7A746E;font-size:13px;");

    layout->addWidget(title);
    layout->addWidget(teacherLabel);
    layout->addWidget(contactLabel);

    return card;
}

QWidget* CourseInfoPage::createNoteCard()
{
    QFrame* card = new QFrame(this);
    card->setStyleSheet("QFrame{background:white;border-radius:18px;border:1px solid #EFEAEA;}");

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    QLabel* title = new QLabel("课程备注", card);
    title->setStyleSheet("font-size:15px;font-weight:700;color:#222;");

    noteEdit = new QTextEdit(card);
    noteEdit->setPlaceholderText("记录课堂重点、考试提醒、老师要求...");
    noteEdit->setAcceptRichText(true);
    noteEdit->setMinimumHeight(150);
    noteEdit->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #EFEAEA;
            border-radius: 14px;
            background: #FCFBFA;
            padding: 10px;
            color: #2F2926;
            font-size: 13px;
            selection-background-color: #8B1E2D;
        }
        QTextEdit:focus {
            border: 1px solid #8B1E2D;
            background: white;
        }
    )");

    layout->addWidget(title);
    layout->addWidget(noteEdit, 1);

    connect(noteEdit, &QTextEdit::textChanged, this, [this]() {
        currentCourse.note = noteEdit->toPlainText();
        emit courseUpdated(currentCourse);
    });

    return card;
}

void CourseInfoPage::loadCourse(const Course& course)
{
    currentCourse = course;

    const QString scheduleSummary = scheduleSummaryForCourse(course.name);
    const QString locationSummary = locationSummaryForCourse(course.name);

    scheduleLabel->setText(scheduleSummary.isEmpty() ? QString("暂未设置") : scheduleSummary);
    locationLabel->setText(locationSummary.isEmpty() ? QString("暂无上课地点") : locationSummary);

    examLabel->setText(safeText(course.examTime, "暂未设置"));
    teacherLabel->setText(safeText(course.teacher, "未填写"));
    contactLabel->setText(course.contact.trimmed().isEmpty() ? "未填写联系方式" : course.contact);

    {
        QSignalBlocker blocker(noteEdit);
        noteEdit->setPlainText(course.note);
    }

    const QList<Task> tasks = DataManager::instance().tasks();
    int totalTasks = 0;
    int completedTasks = 0;
    for (const Task& task : tasks) {
        if (task.course != course.name) {
            continue;
        }
        ++totalTasks;
        if (task.completed) {
            ++completedTasks;
        }
    }
    refreshProgress(completedTasks, totalTasks);
}

QString CourseInfoPage::scheduleSummaryForCourse(const QString& courseName) const
{
    const QList<Course> courses = DataManager::instance().courses();
    QStringList lines;
    QSet<QString> seen;

    for (const Course& item : courses) {
        if (item.name != courseName) {
            continue;
        }
        const QString line = scheduleLine(item);
        if (!line.isEmpty() && !seen.contains(line)) {
            seen.insert(line);
            lines.append(line);
        }
    }

    return lines.join("\n");
}

QString CourseInfoPage::locationSummaryForCourse(const QString& courseName) const
{
    const QList<Course> courses = DataManager::instance().courses();
    QStringList locations;
    QSet<QString> seen;

    for (const Course& item : courses) {
        if (item.name != courseName) {
            continue;
        }
        const QString location = item.location.trimmed();
        if (!location.isEmpty() && !seen.contains(location)) {
            seen.insert(location);
            locations.append(location);
        }
    }

    return locations.join(" · ");
}

void CourseInfoPage::refreshProgress(int completedTasks, int totalTasks)
{
    const int safeTotal = std::max(totalTasks, 0);
    const int safeCompleted = std::max(0, std::min(completedTasks, safeTotal));
    const int progress = safeTotal > 0 ? (safeCompleted * 100) / safeTotal : 0;

    progressBar->setValue(progress);
    progressLabel->setText(QString::number(progress) + "%");

    QLabel* detailLabel = findChild<QLabel*>("progressDetailLabel");
    if (detailLabel) {
        if (safeTotal > 0) {
            detailLabel->setText(QString("%1 / %2 tasks completed").arg(safeCompleted).arg(safeTotal));
        } else {
            detailLabel->setText("暂未关联任务");
        }
    }
}