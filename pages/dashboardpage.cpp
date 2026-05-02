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
#include <QTime>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F7F3EF;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16,16,16,16);
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

    initGrid();
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

            QTimer *timer = new QTimer(this);
            connect(timer,&QTimer::timeout,this,[=](){
                timeLabel->setText(
                    QTime::currentTime().toString("hh:mm:ss")
                );
            });
            timer->start(1000);
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
    QVBoxLayout *ddlLayout = new QVBoxLayout(ddlCard);
    
    QLabel *ddlTitle = new QLabel("DDL提醒");
    ddlTitle->setStyleSheet("font-weight:bold;");
    ddlLayout->addWidget(ddlTitle);
    
    QLabel *ddlContent = new QLabel("暂无DDL");
    ddlContent->setStyleSheet("color:#999;");
    ddlLayout->addWidget(ddlContent);

    layout->addWidget(todayCard);
    layout->addWidget(ddlCard);
    layout->addStretch();

    return widget;
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

    // 时间
    for(int row=0; row<10; row++)
    {
        QLabel *time = new QLabel(QString("%1").arg(row+1));
        time->setAlignment(Qt::AlignCenter);
        time->setStyleSheet("color:#aaa; font-size:11px;");
        grid->addWidget(time, row+1, 0);
    }

    // 单元格
    for(int row=0; row<10; row++)
    {
        for(int col=0; col<7; col++)
        {
            CourseCellWidget *cell = new CourseCellWidget;
            cell->setFixedHeight(50);
            grid->addWidget(cell, row+1, col+1);
        }
    }

    // 示例课程
    CourseCellWidget *c = new CourseCellWidget;
    c->setCourse("数据结构", "理教208");
    c->setFixedHeight(100);
    grid->addWidget(c, 1, 1, 2, 1); // 跨2行
}