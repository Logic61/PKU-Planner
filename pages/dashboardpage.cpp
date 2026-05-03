#include "dashboardpage.h"
#include "../components/coursecellwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QIODevice>
#include <QScrollArea>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    // ===== 创建滚动区域 =====
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background:transparent;");

    // 原 this 上的内容改为放到 container 里
    QWidget *container = new QWidget();
    container->setStyleSheet("background:#F7F3EF;");

    // 所有原有布局和控件都添加到 container 上
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // ===== 1. 顶部学期控制栏 =====
    mainLayout->addWidget(createTopBar());

    // ===== 2. 主体区域 =====
    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(12);

    // 左：课程表
    QFrame *courseCard = new QFrame;
    courseCard->setStyleSheet("background:white; border-radius:16px;");
    QVBoxLayout *courseLayout = new QVBoxLayout(courseCard);

    QLabel *courseTitle = new QLabel("课程表");
    courseTitle->setStyleSheet("font-weight:bold; font-size:16px;");
    courseLayout->addWidget(courseTitle);

    QWidget *gridContainer = new QWidget;
    grid = new QGridLayout(gridContainer);
    grid->setSpacing(6);

    courseLayout->addWidget(gridContainer);

    contentLayout->addWidget(courseCard, 7); // 70%

    // 右：侧栏
    contentLayout->addWidget(createRightPanel(), 3); // 30%

    mainLayout->addLayout(contentLayout);

    // ===== 3. 底部统计卡片 =====
    mainLayout->addWidget(createBottomStats());

    // 将 container 设置为滚动区域的内容
    scrollArea->setWidget(container);

    // DashboardPage 自身的布局只放 scrollArea
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);

    initGrid();
    loadCourses();
    renderCourses();
}

QWidget* DashboardPage::createTopBar()
{
    QWidget *bar = new QWidget;
    bar->setStyleSheet("background:white; border-radius:16px; padding:8px;");

    QHBoxLayout *layout = new QHBoxLayout(bar);
    layout->setContentsMargins(8,8,8,8);

    // 学期进度
    semesterProgress = new QProgressBar;
    semesterProgress->setRange(0,18);
    semesterProgress->setValue(currentWeek);
    semesterProgress->setTextVisible(false);

    semesterProgress->setStyleSheet(R"(
        QProgressBar {
            border:none;
            background:#F5F5F5;
            height:16px;
            border-radius:8px;
        }

        QProgressBar::chunk {
            background:#8B1E2D;
            border-radius:8px;
        }
    )");

    weekLabel = new QLabel;
    weekLabel->setStyleSheet("font-weight:bold; font-size:14px;");

    QPushButton *prevBtn = new QPushButton("上周");
    QPushButton *nextBtn = new QPushButton("下周");
    QPushButton *todayBtn = new QPushButton("返回本周");

    layout->addWidget(new QLabel("学期进度"), 0);
    layout->addWidget(semesterProgress, 2);
    layout->addWidget(weekLabel, 0);
    layout->addWidget(prevBtn, 0);
    layout->addWidget(nextBtn, 0);
    layout->addWidget(todayBtn, 0);

    connect(prevBtn,&QPushButton::clicked,this,[=](){
        currentWeek--;
        updateWeekInfo();
    });

    connect(nextBtn,&QPushButton::clicked,this,[=](){
        currentWeek++;
        updateWeekInfo();
    });

    connect(todayBtn,&QPushButton::clicked,this,[=](){
        currentWeek = realWeek;
        updateWeekInfo();
    });

    updateWeekInfo();

    return bar;
}

void DashboardPage::updateWeekInfo()
{
    if(currentWeek < 1) currentWeek = 1;
    if(currentWeek > 18) currentWeek = 18;

    semesterProgress->setValue(currentWeek);

    QString type = (currentWeek % 2 == 1) ? "单周" : "双周";

    weekLabel->setText(
        QString("第 %1 周 (%2)")
            .arg(currentWeek)
            .arg(type)
    );
}

QWidget* DashboardPage::createBottomStats()
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);

    QStringList titles = {
        "今日课程",
        "今日DDL",
        "本周DDL",
        "当前时间"
    };

    for(auto t : titles)
    {
        QFrame *card = new QFrame;
        card->setStyleSheet(
            "background:white;border-radius:12px;padding:8px;"
        );

        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(8,8,8,8);

        QLabel *num = new QLabel("0");
        num->setStyleSheet(
            "font-size:18px;font-weight:bold;color:#8B1E2D;"
        );

        QLabel *title = new QLabel(t);
        title->setStyleSheet("color:#666;font-size:12px;");

        cl->addWidget(num);
        cl->addWidget(title);

        if(t == "当前时间")
        {
            timeLabel = num;
            timeLabel->setStyleSheet("font-size:14px;font-weight:bold;color:#8B1E2D;");

            QTimer *timer = new QTimer(this);
            connect(timer,&QTimer::timeout,this,[=](){
                timeLabel->setText(
                    QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                );
            });
            timer->start(1000);
            timeLabel->setText(
                QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            );
        }

        layout->addWidget(card);
    }

    return widget;
}

QWidget* DashboardPage::createRightPanel()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(12);

    // 今日课程卡
    QFrame *todayCard = new QFrame;
    todayCard->setStyleSheet("background:white; border-radius:16px;");
    QVBoxLayout *todayLayout = new QVBoxLayout(todayCard);
    
    QLabel *todayTitle = new QLabel("今日课程");
    todayTitle->setStyleSheet("font-weight:bold;");
    todayLayout->addWidget(todayTitle);
    
    QLabel *todayContent = new QLabel("暂无课程");
    todayContent->setStyleSheet("color:#999;");
    todayLayout->addWidget(todayContent);

    // DDL摘要卡
    QFrame *ddlCard = new QFrame;
    ddlCard->setStyleSheet("background:white; border-radius:16px;");
    ddlLayout = new QVBoxLayout(ddlCard);
    
    QLabel *ddlTitle = new QLabel("DDL提醒");
    ddlTitle->setStyleSheet("font-weight:bold; font-size:16px; margin-bottom: 8px;");
    ddlLayout->addWidget(ddlTitle);
    
    updateDDLWidget();

    layout->addWidget(todayCard);
    layout->addWidget(ddlCard);
    layout->addStretch();

    return widget;
}

void DashboardPage::updateDDLWidget()
{
    // Clear old items except title
    while (QLayoutItem *child = ddlLayout->takeAt(1)) {
        if(child->widget()) delete child->widget();
        delete child;
    }
    
    QFile file("tasks.json");
    bool hasDDL = false;
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            // Sort to find the nearest and upcoming DDLs
            std::vector<std::pair<QString, int>> ddls;
            
            for (auto item : arr) {
                QJsonObject obj = item.toObject();
                if (!obj["completed"].toBool()) {
                    QDateTime deadline = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
                    int daysLeft = QDateTime::currentDateTime().daysTo(deadline);
                    ddls.push_back({obj["course"].toString() + " - " + obj["title"].toString(), daysLeft});
                }
            }
            
            std::sort(ddls.begin(), ddls.end(), [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
            
            int count = 0;
            for (auto& ddl : ddls) {
                if (count >= 5) break; 
                QFrame *itemFrame = new QFrame;
                itemFrame->setStyleSheet("background:#FDFDFD; border-radius:10px; border:1px solid #EEE; padding:6px; margin-bottom:4px;");
                QVBoxLayout *vl = new QVBoxLayout(itemFrame);
                vl->setContentsMargins(6,6,6,6);
                
                QLabel *nameLbl = new QLabel(ddl.first);
                nameLbl->setStyleSheet("font-weight:bold; font-size:13px; color:#333; margin:0px;");
                nameLbl->setWordWrap(true);
                
                QLabel *timeLbl = new QLabel;
                if (ddl.second < 0) {
                    timeLbl->setText("已逾期");
                    timeLbl->setStyleSheet("color:#D32F2F; font-size:12px; font-weight:bold; margin:0px;");
                } else if (ddl.second == 0) {
                    timeLbl->setText("今晚截止");
                    timeLbl->setStyleSheet("color:#E64A19; font-size:12px; font-weight:bold; margin:0px;");
                } else {
                    timeLbl->setText(QString("剩余 %1 天").arg(ddl.second));
                    timeLbl->setStyleSheet("color:#8B1E2D; font-size:12px; margin:0px;");
                }
                
                vl->addWidget(nameLbl);
                vl->addWidget(timeLbl);
                
                ddlLayout->addWidget(itemFrame);
                hasDDL = true;
                count++;
            }
        }
    }
    
    if (!hasDDL) {
        QLabel *empty = new QLabel("暂无DDL");
        empty->setStyleSheet("color:#999;");
        ddlLayout->addWidget(empty);
    }
}

void DashboardPage::initGrid()
{
    QStringList days = {"一","二","三","四","五","六","日"};

    // 星期
    for(int col=0; col<7; col++)
    {
        QLabel *label = new QLabel("周" + days[col]);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-weight:bold; color:#666;");
        grid->addWidget(label, 0, col+1);
    }

// 时间（节数 + 时间段）
QStringList timeSlots = {
    "8:00-8:50",    // 第1节
    "9:00-9:50",    // 第2节
    "10:10-11:00",  // 第3节
    "11:10-12:00",  // 第4节
    "13:00-13:50",  // 第5节
    "14:00-14:50",  // 第6节
    "15:10-16:00",  // 第7节
    "16:10-17:00",  // 第8节
    "17:10-18:00",  // 第9节
    "18:40-19:30",  // 第10节
    "19:40-20:30",  // 第11节
    "20:40-21:30"   // 第12节
};

for(int row=0; row<12; row++)
{
    QLabel *timeLabel = new QLabel(QString("%1\n%2").arg(row+1).arg(timeSlots[row]));
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("color:#aaa; font-size:10px; line-height:1.2;");
    grid->addWidget(timeLabel, row+1, 0);
}

    // Initialize all cells as empty first to provide a clickable background
    for(int row=0; row<12; row++)
    {
        for(int col=0; col<7; col++)
        {
            CourseCellWidget *cell = new CourseCellWidget(row+1, col+1);
            grid->addWidget(cell, row+1, col+1);
            connect(cell, &CourseCellWidget::createCourseRequested, this, &DashboardPage::createCourse);
        }
    }
}

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "../dialogs/courseeditdialog.h"
#include "../dialogs/courseactiondialog.h"
#include "../dialogs/taskeditdialog.h"

void DashboardPage::saveCourses()
{
    QFile file("courses.json");

    if(!file.exists())
    {
        if (!file.open(QIODevice::WriteOnly)) {
            return;
        }
        file.close();
    }

    if(file.open(QIODevice::WriteOnly))
    {
        QJsonArray arr;

        for(auto &c:courses)
        {
            arr.append(c.toJson());
        }

        QJsonDocument doc(arr);

        file.write(doc.toJson());
        file.close();
    }
}

void DashboardPage::loadCourses()
{
    QFile file("courses.json");

    if(!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc =
        QJsonDocument::fromJson(file.readAll());

    QJsonArray arr = doc.array();

    courses.clear();
    for(auto item : arr)
    {
        courses.push_back(
            Course::fromJson(item.toObject())
        );
    }
}

int DashboardPage::getNearestDDL(const QString& courseName)
{
    int minDays = 9999;
    QFile file("tasks.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (auto item : arr) {
                QJsonObject obj = item.toObject();
                if (obj["course"].toString() == courseName && !obj["completed"].toBool()) {
                    QDateTime deadline = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
                    int daysLeft = QDateTime::currentDateTime().daysTo(deadline);
                    if (daysLeft < minDays) {
                        minDays = daysLeft;
                    }
                }
            }
        }
    }
    return minDays == 9999 ? -999 : minDays;
}

void DashboardPage::renderCourses()
{
    QLayoutItem *child;
    while ((child = grid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    updateDDLWidget();
    initGrid();

    int index = 0;
    for(auto &c : courses)
    {
        int daysLeft = getNearestDDL(c.name);
        CourseCellWidget *cell = new CourseCellWidget(c.startPeriod, c.day);
        cell->setCourse(c.name, c.location, c.teacher, index, daysLeft);
        
        connect(cell, &CourseCellWidget::editCourseRequested,
                this, &DashboardPage::editCourse);

        grid->addWidget(
            cell,
            c.startPeriod,
            c.day,
            c.endPeriod - c.startPeriod + 1,
            1
        );
        index++;
    }
}

void DashboardPage::createCourse(int row, int col)
{
    CourseEditDialog dialog(row, row);
    dialog.setWindowTitle("添加课程");
    
    if (dialog.exec() == QDialog::Accepted) {
        Course c;
        c.name = dialog.getName();
        c.teacher = dialog.getTeacher();
        c.location = dialog.getLocation();
        c.examTime = dialog.getExamTime();
        
        c.startPeriod = dialog.getStart();
        c.endPeriod = dialog.getEnd();
        c.day = col;
        
        courses.push_back(c);
        
        saveCourses();
        renderCourses();
    }
}

void DashboardPage::editCourse(int index)
{
    if (index < 0 || index >= courses.size()) return;
    
    Course &c = courses[index];
    
    CourseActionDialog actionDialog(this);
    actionDialog.exec();
    
    if (actionDialog.editSelected()) {
        CourseEditDialog dialog(c.startPeriod, c.endPeriod);
        dialog.setWindowTitle("编辑课程");
        dialog.setCourseData(c.name, c.teacher, c.location, c.examTime, c.startPeriod, c.endPeriod);
        
        if (dialog.exec() == QDialog::Accepted) {
            c.name = dialog.getName();
            c.teacher = dialog.getTeacher();
            c.location = dialog.getLocation();
            c.examTime = dialog.getExamTime();
            c.startPeriod = dialog.getStart();
            c.endPeriod = dialog.getEnd();
            // c.day remains unchanged from edit interface (unless col added to dialog)
            
            saveCourses();
            renderCourses();
        }
    } else if (actionDialog.deleteSelected()) {
        courses.erase(courses.begin() + index);
        saveCourses();
        renderCourses();
    } else if (actionDialog.ddlSelected()) {
        TaskEditDialog taskDialog(this, c.name);
        if (taskDialog.exec() == QDialog::Accepted) {
            Task newTask;
            newTask.course = taskDialog.getCourseName();
            newTask.title = taskDialog.getTitle();
            newTask.deadline = taskDialog.getDeadline();
            newTask.priority = taskDialog.getPriority();
            // Assuming saveTasks or dealing with tasks.json
            QFile taskFile("tasks.json");
            QJsonArray taskArr;
            if (taskFile.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(taskFile.readAll());
                if (doc.isArray()) {
                    taskArr = doc.array();
                }
                taskFile.close();
            }
            QJsonObject taskObj;
            taskObj["course"] = newTask.course;
            taskObj["title"] = newTask.title;
            taskObj["deadline"] = newTask.deadline.toString(Qt::ISODate);
            taskObj["priority"] = newTask.priority;
            taskObj["completed"] = false;
            // Also estimated_hours if you want
            taskArr.append(taskObj);

            if (taskFile.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(taskArr);
                taskFile.write(doc.toJson());
                taskFile.close();
            }
            renderCourses();
        }
    }
}