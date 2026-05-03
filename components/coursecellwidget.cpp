#include "coursecellwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QTimer>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

CourseCellWidget::CourseCellWidget(int row, int col, QWidget *parent)
    : QFrame(parent), m_row(row), m_col(col), m_index(-1)
{
    setMinimumSize(80, 60);

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
    layout->setContentsMargins(6,6,6,6);

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
}

void CourseCellWidget::setCourse(QString name, QString location, QString teacher, int index, int daysLeft)
{
    m_index = index;
    title->setText(name);

    info->setText(
        location + "\n" + teacher
    );

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
    QTimer::singleShot(300, this, [this]() {
        if(!title->text().isEmpty())
        {
            QToolTip::showText(cursor().pos(),
                title->text() + "\n信息: " + info->text());
        }
    });
}

void CourseCellWidget::leaveEvent(QEvent *)
{
    QToolTip::hideText();
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
            emit editCourseRequested(m_index);
        }
    }
}
