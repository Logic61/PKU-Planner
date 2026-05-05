#include "coursedetaildrawer.h"
#include "courseheaderwidget.h"
#include "coursetaskpage.h"
#include "coursefilepage.h"
#include "courseinfopage.h"
#include "coursetabbar.h"
#include "coursestatswidget.h"
#include <QVBoxLayout>
#include "../../dialogs/taskeditdialog.h"
#include "../../dialogs/courseeditdialog.h"
#include "../../models/datamanager.h"

#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QResizeEvent>
#include <QMouseEvent>

CourseDetailDrawer::CourseDetailDrawer(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("courseDrawer");
    setStyleSheet("QFrame#courseDrawer{background:white;border-left:1px solid #EAEAEA;}");
    setupUI();
    setupAnimation();
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
    hide();
}

void CourseDetailDrawer::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    headerWidget = new CourseHeaderWidget(this);
    tabBar = new CourseTabBar(this);
    stackedWidget = new QStackedWidget(this);

    basicInfoPage = new CourseInfoPage(this);
    taskPage = new CourseTaskPage(this);
    filePage = new CourseFilePage(this);
    statsWidget = new CourseStatsWidget(this);

    basicInfoPage->setObjectName("courseInfoPage");

    stackedWidget->addWidget(basicInfoPage);
    stackedWidget->addWidget(taskPage);
    stackedWidget->addWidget(filePage);
    stackedWidget->addWidget(statsWidget);

    root->addWidget(headerWidget);
    root->addWidget(tabBar);
    root->addWidget(stackedWidget, 1);

    connect(tabBar, &CourseTabBar::tabChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(headerWidget, &CourseHeaderWidget::closeClicked, this, &CourseDetailDrawer::closeDrawer);
    connect(headerWidget, &CourseHeaderWidget::addTaskClicked, this, [this]() { 
        // 优先将信号广播给外部（MainWindow）处理，外部可以切换页面并弹出对话框。
        emit addTaskRequested(currentCourse);
        // 如果没有外部接收者，则在抽屉内作为回退直接弹出添加任务对话框
        if (receivers(SIGNAL(addTaskRequested(Course))) == 0) {
            TaskEditDialog dlg(this, currentCourse.name);
            if (dlg.exec() == QDialog::Accepted) {
                Task t;
                t.course = dlg.getCourseName();
                t.title = dlg.getTitle();
                t.deadline = dlg.getDeadline();
                t.priority = dlg.getPriority();
                t.completed = false;
                DataManager::instance().addTask(t);
                emit taskUpdated();
            }
        }
    });
    connect(headerWidget, &CourseHeaderWidget::editClicked, this, [this]() { 
        emit editCourseRequested(currentCourse);
        if (receivers(SIGNAL(editCourseRequested(Course))) == 0) {
            CourseEditDialog dlg(currentCourse.startPeriod, currentCourse.endPeriod, this);
            dlg.setCourseData(currentCourse.name, currentCourse.teacher, currentCourse.location, currentCourse.examTime, currentCourse.startPeriod, currentCourse.endPeriod, currentCourse.weekType);
            if (dlg.exec() == QDialog::Accepted) {
                Course updated = currentCourse;
                updated.name = dlg.getName();
                updated.teacher = dlg.getTeacher();
                updated.location = dlg.getLocation();
                updated.examTime = dlg.getExamTime();
                updated.startPeriod = dlg.getStart();
                updated.endPeriod = dlg.getEnd();
                updated.weekType = dlg.getWeekType();
                // 找到匹配索引并更新
                int foundIndex = -1;
                const QList<Course> all = DataManager::instance().courses();
                for (int i = 0; i < all.size(); ++i) {
                    const Course &cc = all[i];
                    if (cc.name == currentCourse.name && cc.day == currentCourse.day && cc.startPeriod == currentCourse.startPeriod && cc.endPeriod == currentCourse.endPeriod) {
                        foundIndex = i;
                        break;
                    }
                }
                if (foundIndex >= 0) {
                    DataManager::instance().updateCourse(foundIndex, updated);
                    emit courseUpdated(updated);
                }
            }
        }
    });
    connect(headerWidget, &CourseHeaderWidget::folderClicked, this, [this]() { 
        stackedWidget->setCurrentIndex(2);
        tabBar->setCurrentTab(2);
    });
    connect(basicInfoPage, &CourseInfoPage::courseUpdated, this, &CourseDetailDrawer::courseUpdated);
    connect(taskPage, &CourseTaskPage::taskUpdated, this, &CourseDetailDrawer::taskUpdated);
    connect(taskPage, &CourseTaskPage::addTaskRequested, this, [this](const QString &) {
        emit addTaskRequested(currentCourse);
    });
    connect(filePage, &CourseFilePage::courseUpdated, this, &CourseDetailDrawer::courseUpdated);
}

void CourseDetailDrawer::setupAnimation()
{
    anim = new QPropertyAnimation(this, "geometry", this);
    anim->setDuration(250);
}

void CourseDetailDrawer::loadCourse(const Course& course)
{
    currentCourse = course;
    headerWidget->setCourse(course);
    basicInfoPage->loadCourse(course);
    taskPage->loadCourseTasks(course.name);
    filePage->loadCourse(course);
}

void CourseDetailDrawer::openDrawer()
{
    if (!parentWidget()) return;
    isOpen = true;
    show();
    updateDockGeometry();
    QRect parentRect = parentWidget()->rect();
    int w = effectiveDrawerWidth();
    QRect start(parentRect.right(), 0, w, parentRect.height());
    QRect end(parentRect.right() - w, 0, w, parentRect.height());
    setGeometry(start);
    anim->stop();
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->start();
}

void CourseDetailDrawer::closeDrawer()
{
    if (!parentWidget()) return;
    isOpen = false;
    QRect parentRect = parentWidget()->rect();
    int w = effectiveDrawerWidth();
    QRect start(parentRect.right() - w, 0, w, parentRect.height());
    QRect end(parentRect.right(), 0, w, parentRect.height());
    anim->stop();
    anim->setStartValue(start);
    anim->setEndValue(end);
    disconnect(anim, nullptr, this, nullptr);
    connect(anim, &QPropertyAnimation::finished, this, &CourseDetailDrawer::onAnimationFinished, Qt::UniqueConnection);
    anim->start();
}

void CourseDetailDrawer::onAnimationFinished()
{
    if (!isOpen) {
        hide();
    }
}

void CourseDetailDrawer::updateDockGeometry()
{
    if (!parentWidget()) {
        return;
    }

    const QRect parentRect = parentWidget()->rect();
    const int w = effectiveDrawerWidth();
    setFixedWidth(w);
    if (isOpen) {
        setGeometry(parentRect.right() - w, 0, w, parentRect.height());
    } else {
        setGeometry(parentRect.right(), 0, w, parentRect.height());
    }
}

int CourseDetailDrawer::effectiveDrawerWidth() const
{
    const int parentWidth = parentWidget() ? parentWidget()->width() : drawerWidth;
    return qBound(420, parentWidth / 3, 560);
}

bool CourseDetailDrawer::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
        updateDockGeometry();
    }
    return QFrame::eventFilter(watched, event);
}

void CourseDetailDrawer::mousePressEvent(QMouseEvent *event)
{
    if (event->position().x() < DRAG_EDGE_WIDTH && isOpen) {
        isDragging = true;
        dragStartX = event->globalPosition().x();
        dragStartWidth = width();
        event->accept();
    } else {
        QFrame::mousePressEvent(event);
    }
}

void CourseDetailDrawer::mouseMoveEvent(QMouseEvent *event)
{
    if (isOpen) {
        if (event->position().x() < DRAG_EDGE_WIDTH) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    
    if (isDragging) {
        int delta = dragStartX - event->globalPosition().x();
        int newWidth = dragStartWidth + delta;
        newWidth = qBound(MIN_DRAWER_WIDTH, newWidth, MAX_DRAWER_WIDTH);
        drawerWidth = newWidth;
        
        if (parentWidget() && isOpen) {
            QRect parentRect = parentWidget()->rect();
            setGeometry(parentRect.right() - newWidth, 0, newWidth, parentRect.height());
            setFixedWidth(newWidth);
        }
        event->accept();
    } else {
        QFrame::mouseMoveEvent(event);
    }
}

void CourseDetailDrawer::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDragging) {
        isDragging = false;
        event->accept();
    } else {
        QFrame::mouseReleaseEvent(event);
    }
}
