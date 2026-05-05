#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QWidget>

#include "ui/sidebarwidget.h"
#include "ui/topbarwidget.h"

#include "pages/dashboardpage.h"
#include "pages/todopage.h"
#include "widgets/coursedetail/coursedetaildrawer.h"
#include "pages/statspage.h"
#include "dialogs/taskeditdialog.h"
#include "dialogs/courseeditdialog.h"
#include "models/datamanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget;
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);

    // 左侧导航
    sidebar = new SidebarWidget;
    sidebar->setFixedWidth(200);

    // 右侧区域
    QWidget *right = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(0,0,0,0);

    // 顶栏
    topbar = new TopbarWidget;

    // 页面栈
    stack = new QStackedWidget;

    // Add placeholder widgets
    stack->addWidget(new QWidget()); // index 0 - will be replaced by DashboardPage
    stack->addWidget(new QWidget()); // index 1 - will be replaced by TodoPage

    rightLayout->addWidget(topbar);
    rightLayout->addWidget(stack);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(right);

    // Course detail drawer (hidden by default)
    courseDrawer = new CourseDetailDrawer(central);
    courseDrawer->hide();

    // 页面切换
    connect(sidebar, &SidebarWidget::pageChanged,
            stack, &QStackedWidget::setCurrentIndex);
    
    // Defer page initialization
    QMetaObject::invokeMethod(this, "initPages", Qt::QueuedConnection);
}

void MainWindow::initPages()
{
    if (pagesInitialized) return;
    pagesInitialized = true;
    // Remove placeholder widgets and add real pages
    QWidget *oldPage0 = stack->widget(0);
    QWidget *oldPage1 = stack->widget(1);
    stack->removeWidget(oldPage0);
    stack->removeWidget(oldPage1);
    delete oldPage0;
    delete oldPage1;
    dashboardPage = new DashboardPage;
    todoPage = new TodoPage;
    stack->insertWidget(0, dashboardPage);
    stack->insertWidget(1, todoPage);
    // Stats page
    StatsPage *statsPage = new StatsPage;
    stack->insertWidget(2, statsPage);
    
    // Connect navigation signal from DashboardPage
    connect(dashboardPage, &DashboardPage::navigateToTodoPageRequested,
            this, &MainWindow::onNavigateToTodoPage);
        connect(dashboardPage, &DashboardPage::openCourseDetail,
            this, &MainWindow::showCourseDrawer);
        connect(courseDrawer, &CourseDetailDrawer::courseUpdated,
            dashboardPage, &DashboardPage::applyCourseUpdate);
        connect(courseDrawer, &CourseDetailDrawer::taskUpdated,
            todoPage, &TodoPage::reloadTasks);
        connect(courseDrawer, &CourseDetailDrawer::taskUpdated,
            dashboardPage, &DashboardPage::refreshCourseUrgency);
        // Ensure sidebar page change maps to stats page index 2
        connect(sidebar, &SidebarWidget::pageChanged, this, [this, statsPage](int index){
            // default handlers already connected above; handle stats index separately
            if (index == 2) {
                stack->setCurrentIndex(2);
            }
        });
    // Drawer actions: add task / edit course
    connect(courseDrawer, &CourseDetailDrawer::addTaskRequested, this, &MainWindow::handleAddTaskRequested);
    connect(courseDrawer, &CourseDetailDrawer::editCourseRequested, this, &MainWindow::handleEditCourseRequested);
}

void MainWindow::onNavigateToTodoPage()
{
    stack->setCurrentIndex(1);
}

void MainWindow::showCourseDrawer(const Course& course)
{
    if (!courseDrawer) return;
    courseDrawer->loadCourse(course);
    courseDrawer->openDrawer();
}

void MainWindow::handleAddTaskRequested(Course course)
{
    // 切换到待办页面并弹出添加 DDL 对话框，默认课程名为当前课程
    if (todoPage) {
        stack->setCurrentIndex(1);
    }
    TaskEditDialog dlg(this, course.name);
    if (dlg.exec() == QDialog::Accepted) {
        Task t;
        t.course = dlg.getCourseName();
        t.title = dlg.getTitle();
        t.deadline = dlg.getDeadline();
        t.priority = dlg.getPriority();
        t.completed = false;
        DataManager::instance().addTask(t);
    }
}

void MainWindow::handleEditCourseRequested(Course course)
{
    CourseEditDialog dlg(course.startPeriod, course.endPeriod, this);
    dlg.setCourseData(course.name, course.teacher, course.location, course.examTime, course.startPeriod, course.endPeriod, course.weekType);
    if (dlg.exec() == QDialog::Accepted) {
        Course updated = course;
        updated.name = dlg.getName();
        updated.teacher = dlg.getTeacher();
        updated.location = dlg.getLocation();
        updated.examTime = dlg.getExamTime();
        updated.startPeriod = dlg.getStart();
        updated.endPeriod = dlg.getEnd();
        updated.weekType = dlg.getWeekType();

        // 查找原课程索引并更新
        int foundIndex = -1;
        const QList<Course> all = DataManager::instance().courses();
        for (int i = 0; i < all.size(); ++i) {
            const Course &cc = all[i];
            if (cc.name == course.name && cc.day == course.day && cc.startPeriod == course.startPeriod && cc.endPeriod == course.endPeriod) {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex >= 0) {
            DataManager::instance().updateCourse(foundIndex, updated);
        }
    }
}