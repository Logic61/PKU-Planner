#include "ddlpreviewwidget.h"
#include "../models/datamanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QPushButton>
#include <QScreen>
#include <QApplication>
#include <QTimer>
#include <QEnterEvent>
#include <QEvent>
#include <QCursor>

DDLPreviewWidget::DDLPreviewWidget(const QString &courseName, QWidget *parent)
    : QFrame(parent, Qt::ToolTip)
{
    setObjectName("ddlPreview");
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlag(Qt::FramelessWindowHint);
    setMouseTracking(true);
    setStyleSheet(R"(
        QFrame#ddlPreview { 
            background: rgba(255,255,255,0.98);
            border: 1px solid #E8E8E8;
            border-radius: 10px;
        }
        QLabel.title { font-weight:700; color:#222; }
        QLabel.meta { color:#666; font-size:12px; }
        QPushButton.link { background: transparent; color:#8B1E2D; border:none; }
    )");

    buildUI(courseName);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, [this]() {
        m_hideScheduled = false;
        if (!m_mouseInside) {
            hide();
        }
    });
}

void DDLPreviewWidget::buildUI(const QString &courseName)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setContentsMargins(12,10,12,10);
    l->setSpacing(8);

    QLabel *h = new QLabel(courseName);
    h->setObjectName("title");
    h->setProperty("class", "title");
    h->setStyleSheet("font-size:14px;");
    h->installEventFilter(this);
    l->addWidget(h);

    const auto tasks = DataManager::instance().tasks();
    int shown = 0;
    for (const Task &t : tasks) {
        if (t.course != courseName) continue;

        QWidget *row = new QWidget;
        row->installEventFilter(this);
        row->setMouseTracking(true);
        QHBoxLayout *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0,0,0,0);
        rl->setSpacing(8);

        QLabel *ttl = new QLabel(t.title);
        ttl->setStyleSheet("font-weight:600; color:#222; font-size:13px;");
        QLabel *meta = new QLabel(t.deadline.toString("MM-dd hh:mm"));
        meta->setStyleSheet("color:#888; font-size:12px;");

        rl->addWidget(ttl, 1);
        rl->addWidget(meta);

        l->addWidget(row);
        shown++;
        if (shown >= 6) break;
    }

    if (shown == 0) {
        QLabel *empty = new QLabel("暂无 DDL");
        empty->setStyleSheet("color:#999;");
        empty->installEventFilter(this);
        l->addWidget(empty);
    }

    // footer link
    QPushButton *open = new QPushButton("在待办页打开全部 →");
    open->setObjectName("openLink");
    open->setProperty("class","link");
    open->setCursor(Qt::PointingHandCursor);
    open->installEventFilter(this);
    l->addWidget(open, 0, Qt::AlignRight);

    connect(open, &QPushButton::clicked, this, [this](){
        emit requestNavigateToTodoPage();
        this->hide();
    });
}

void DDLPreviewWidget::showNear(const QPoint &globalPos)
{
    adjustSize();
    QRect screen = QApplication::primaryScreen()->availableGeometry();
    QPoint p = globalPos + QPoint(12, 12);
    if (p.x() + width() > screen.right()) p.setX(screen.right() - width() - 8);
    if (p.y() + height() > screen.bottom()) p.setY(globalPos.y() - height() - 8);
    move(p);
    show();
    setHoverState(true);
}

void DDLPreviewWidget::enterEvent(QEnterEvent *event)
{
    setHoverState(true);
    QFrame::enterEvent(event);
}

void DDLPreviewWidget::leaveEvent(QEvent *event)
{
    setHoverState(false);
    QFrame::leaveEvent(event);
}

bool DDLPreviewWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        setHoverState(true);
    } else if (event->type() == QEvent::Leave) {
        setHoverState(false);
    }

    return QObject::eventFilter(watched, event);
}

void DDLPreviewWidget::setHoverState(bool hovering)
{
    m_mouseInside = hovering;
    if (hovering) {
        if (m_hideTimer) {
            m_hideTimer->stop();
        }
        m_hideScheduled = false;
        return;
    }

    if (!m_hideTimer || m_hideScheduled) {
        return;
    }

    m_hideScheduled = true;
    m_hideTimer->start(180);
}
