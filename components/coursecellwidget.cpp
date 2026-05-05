#include "coursecellwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QTimer>
#include <QEnterEvent>
#include "ddlpreviewwidget.h"
#include <QMouseEvent>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QApplication>
#include <QCursor>
#include "../models/datamanager.h"
#include <QMenu>
#include <QContextMenuEvent>

CourseCellWidget::CourseCellWidget(int row, int col, QWidget *parent)
    : QFrame(parent), m_row(row), m_col(col), m_index(-1)
{
    setMinimumSize(50, 48);               // 覆盖原本的10px最小高度
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    setStyleSheet(R"(
        QFrame {
            background:#FAFAFA;
            border-radius:10px;
            border:1px solid transparent;
        }
        QFrame:hover {
            background:#FDECEC;
            border:1px solid #8B1E2D;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4,2,4,2);
    layout->setSpacing(2);

    title = new QLabel("");
    title->setStyleSheet("font-weight:bold; font-size:12px;");
    title->setWordWrap(true);
    title->setAttribute(Qt::WA_TransparentForMouseEvents);

    info = new QLabel("");
    info->setStyleSheet("font-size:11px; color:#666;");
    info->setWordWrap(true);
    info->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(title);
    layout->addWidget(info);

    m_preview = nullptr;
    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, [this]() {
        if (m_showToken != m_hoverSerial) {
            return;
        }
        if (title->text().isEmpty()) {
            return;
        }

        if (m_preview) {
            m_preview->showNear(QCursor::pos());
            m_preview->raise();
            m_preview->activateWindow();
        } else {
            m_preview = new DDLPreviewWidget(title->text());
            connect(m_preview, &DDLPreviewWidget::requestNavigateToTodoPage, 
                    this, &CourseCellWidget::navigateToTodoPageRequested);
            m_preview->showNear(QCursor::pos());
        }
    });

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, [this]() {
        if (!m_preview || !m_preview->isVisible()) {
            return;
        }
        QWidget *hoverWidget = QApplication::widgetAt(QCursor::pos());
        if (hoverWidget && (hoverWidget == m_preview || m_preview->isAncestorOf(hoverWidget))) {
            return;
        }
        m_preview->hide();
    });
}

void CourseCellWidget::setCourse(QString name, QString location, QString teacher, int index, int daysLeft, QString scheduleSummary)
{
    m_index = index;
    m_courseName = name;
    title->setText(name);

    QStringList infoLines;
    infoLines << location << teacher;
    if (!scheduleSummary.trimmed().isEmpty()) {
        infoLines << scheduleSummary;
    }
    info->setText(infoLines.join("\n"));

    // 默认：无DDL（绿色系）
    QString bg = "#E8F5E9";
    QString hover = "#C8E6C9";
    QString border = "transparent";
    QString hoverBorder = "#4CAF50";

    if (daysLeft != -999) {
        if (daysLeft < 0) {
            // 逾期：红色
            bg = "#FFCDD2"; hover = "#FFCDD2"; hoverBorder = "#D32F2F";
        } else if (daysLeft == 0) {
            // 今晚截止：橙色
            bg = "#FFAB91"; hover = "#FFAB91"; hoverBorder = "#E64A19";
        } else if (daysLeft <= 3) {
            // 3天内：黄色
            bg = "#FFCC80"; hover = "#FFCC80"; hoverBorder = "#F57C00";
        } else if (daysLeft <= 7) {
            // 7天内：浅黄
            bg = "#FFF59D"; hover = "#FFF59D"; hoverBorder = "#FBC02D";
        } else {
            // DDL不急（>7天）：蓝色系
            bg = "#E3F2FD"; hover = "#BBDEFB"; hoverBorder = "#1976D2";
        }
    }

    setStyleSheet(QString(R"(
        QFrame {
            background: %1;
            border-radius: 10px;
            border:1px solid %3;
        }
        QFrame:hover {
            background: %2;
            border:1px solid %4;
        }
    )").arg(bg).arg(hover).arg(border).arg(hoverBorder));
}

void CourseCellWidget::enterEvent(QEnterEvent *)
{
    ++m_hoverSerial;
    m_showToken = m_hoverSerial;
    if (m_hideTimer) {
        m_hideTimer->stop();
    }
    if (m_showTimer) {
        m_showTimer->stop();
        m_showTimer->start(220);
    }
}

void CourseCellWidget::leaveEvent(QEvent *)
{
    QToolTip::hideText();
    ++m_hoverSerial;
    if (m_showTimer) {
        m_showTimer->stop();
    }
    if (m_hideTimer) {
        m_hideTimer->start(120);
    }
}

void CourseCellWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if(title->text().isEmpty())
    {
        emit createCourseRequested(m_row, m_col);
    }
    else
    {
        if(m_index != -1)
        {
            // Emit drawer-opening signal with full Course data
            const auto courses = DataManager::instance().courses();
            if (m_index >=0 && m_index < courses.size()) {
                emit courseDoubleClicked(courses[m_index]);
            } else {
                emit editCourseRequested(m_index);
            }
        }
    }
}

void CourseCellWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.setWindowFlag(Qt::NoDropShadowWindowHint);
    menu.setStyleSheet(R"(
        QMenu {
            background-color: white;
            border: 1px solid #E0E0E0;
            border-radius: 8px;
        }
        QMenu::item:selected {
            background-color: #8B1E2D;
            color: white;
        }
        QMenu::item:pressed {
            background-color: #7A1C2C;
        }
    )");

    if (title->text().isEmpty()) {
        // Empty cell: show only "Create Course" option
        QAction *createAction = menu.addAction("➕ 创建课程");
        QAction *selected = menu.exec(event->globalPos());
        if (selected == createAction) {
            emit createCourseRequested(m_row, m_col);
        }
    } else {
        // Cell with course: show edit, delete, and add DDL options
        QAction *editAction = menu.addAction("✏ 编辑课程");
        QAction *deleteAction = menu.addAction("🗑 删除课程");
        menu.addSeparator();
        QAction *addDDLAction = menu.addAction("➕ 添加 DDL");

        QAction *selected = menu.exec(event->globalPos());

        if (selected == editAction && m_index != -1) {
            // Right-click edit goes directly to edit dialog, bypassing ActionDialog
            emit editCourseDirectlyRequested(m_index);
        } else if (selected == deleteAction && m_index != -1) {
            emit deleteCourseRequested(m_index);
        } else if (selected == addDDLAction) {
            emit addDDLRequested(m_courseName);
        }
    }
}
