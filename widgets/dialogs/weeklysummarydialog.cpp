#include "weeklysummarydialog.h"
#include "../../ui/theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>

class WeeklySummaryDialog::PrivateData {
public:
    QPoint dragPos;
};

WeeklySummaryDialog::WeeklySummaryDialog(QWidget* parent)
    : QDialog(parent, Qt::FramelessWindowHint)
    , d(new PrivateData)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMinimumWidth(420);
    setMinimumHeight(500);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect geom = screen ? screen->availableGeometry() : QRect(0, 0, 1280, 800);
    setFixedWidth(qMin(500, geom.width() - 40));
    move((geom.width() - 480) / 2 + geom.x(), (geom.height() - 600) / 2 + geom.y());

    QFrame* outerContainer = new QFrame(this);
    outerContainer->setStyleSheet("QFrame { background: white; border-radius: 20px; }");

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(outerContainer);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 35));
    outerContainer->setGraphicsEffect(shadow);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(outerContainer);

    QVBoxLayout* contentLayout = new QVBoxLayout(outerContainer);
    contentLayout->setContentsMargins(28, 24, 28, 24);
    contentLayout->setSpacing(14);

    QHBoxLayout* headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(10);
    QLabel* headerIcon = new QLabel("📊", outerContainer);
    headerIcon->setStyleSheet("font-size: 28px;");
    headerLayout->addWidget(headerIcon);
    QLabel* titleLabel = new QLabel("本周学习总结", outerContainer);
    titleLabel->setStyleSheet(QString("font-size: 18px; font-weight: 700; color: %1;").arg(Theme::PRIMARY));
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    QPushButton* closeBtn = new QPushButton("✕", outerContainer);
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            font-size: 16px;
            color: #999;
        }
        QPushButton:hover {
            background: #F0F0F0;
            border-radius: 14px;
            color: #666;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    headerLayout->addWidget(closeBtn);
    contentLayout->addLayout(headerLayout);

    QFrame* divider = new QFrame(outerContainer);
    divider->setStyleSheet("background: #F0F0F0;");
    divider->setFixedHeight(1);
    contentLayout->addWidget(divider);

    WeeklySummary summary = WeeklySummaryService::generate();

    QHBoxLayout* statsRow1 = new QHBoxLayout;
    statsRow1->setSpacing(10);

    QFrame* completedCard = new QFrame;
    completedCard->setStyleSheet(QString("QFrame { background: %1; border-radius: 12px; padding: 12px; }").arg(Theme::PRIMARY_LIGHTER));
    QVBoxLayout* completedLayout = new QVBoxLayout(completedCard);
    completedLayout->setContentsMargins(8, 8, 8, 8);
    QLabel* completedTitle = new QLabel("完成任务", completedCard);
    completedTitle->setStyleSheet("font-size: 11px; color: #888;");
    completedLayout->addWidget(completedTitle);
    QLabel* completedValue = new QLabel(QString::number(summary.completedTasks), completedCard);
    completedValue->setStyleSheet("font-size: 24px; font-weight: 700; color: #2E7D32;");
    completedLayout->addWidget(completedValue);
    statsRow1->addWidget(completedCard);

    QFrame* overdueCard = new QFrame;
    overdueCard->setStyleSheet(QString("QFrame { background: %1; border-radius: 12px; padding: 12px; }").arg(Theme::PRIMARY_LIGHTER));
    QVBoxLayout* overdueLayout = new QVBoxLayout(overdueCard);
    overdueLayout->setContentsMargins(8, 8, 8, 8);
    QLabel* overdueTitle = new QLabel("逾期任务", overdueCard);
    overdueTitle->setStyleSheet("font-size: 11px; color: #888;");
    overdueLayout->addWidget(overdueTitle);
    QLabel* overdueValue = new QLabel(QString::number(summary.overdueTasks), overdueCard);
    overdueValue->setStyleSheet("font-size: 24px; font-weight: 700; color: #C62828;");
    overdueLayout->addWidget(overdueValue);
    statsRow1->addWidget(overdueCard);

    QFrame* upcomingCard = new QFrame;
    upcomingCard->setStyleSheet(QString("QFrame { background: %1; border-radius: 12px; padding: 12px; }").arg(Theme::PRIMARY_LIGHTER));
    QVBoxLayout* upcomingLayout = new QVBoxLayout(upcomingCard);
    upcomingLayout->setContentsMargins(8, 8, 8, 8);
    QLabel* upcomingTitle = new QLabel("进行中", upcomingCard);
    upcomingTitle->setStyleSheet("font-size: 11px; color: #888;");
    upcomingLayout->addWidget(upcomingTitle);
    QLabel* upcomingValue = new QLabel(QString::number(summary.upcomingTasks), upcomingCard);
    upcomingValue->setStyleSheet("font-size: 24px; font-weight: 700; color: #1565C0;");
    upcomingLayout->addWidget(upcomingValue);
    statsRow1->addWidget(upcomingCard);
    contentLayout->addLayout(statsRow1);

    QFrame* rateCard = new QFrame;
    rateCard->setStyleSheet(QString("QFrame { background: %1; border-radius: 12px; padding: 14px; }").arg(Theme::PRIMARY_LIGHT));
    QHBoxLayout* rateLayout = new QHBoxLayout(rateCard);
    rateLayout->setContentsMargins(14, 10, 14, 10);
    QLabel* rateTitle = new QLabel("完成率", rateCard);
    rateTitle->setStyleSheet("font-size: 14px; color: #666;");
    rateLayout->addWidget(rateTitle);
    rateLayout->addStretch();
    QLabel* rateValue = new QLabel(QString("%1%").arg(qRound(summary.completionRate)), rateCard);
    rateValue->setStyleSheet("font-size: 22px; font-weight: 700; color: #1565C0;");
    rateLayout->addWidget(rateValue);
    contentLayout->addWidget(rateCard);

    if (!summary.busiestCourse.isEmpty()) {
        QFrame* busiestCard = new QFrame;
        busiestCard->setStyleSheet("QFrame { background: #FFF3E0; border-radius: 12px; padding: 12px; }");
        QHBoxLayout* busiestLayout = new QHBoxLayout(busiestCard);
        busiestLayout->setContentsMargins(10, 8, 10, 8);
        QLabel* busiestIcon = new QLabel("🔥", busiestCard);
        busiestIcon->setStyleSheet("font-size: 18px;");
        busiestLayout->addWidget(busiestIcon);
        QLabel* busiestLabel = new QLabel(QString("最忙课程: %1 (%2 个任务)").arg(summary.busiestCourse).arg(summary.busiestCourseTasks), busiestCard);
        busiestLabel->setStyleSheet("font-size: 13px; color: #E65100;");
        busiestLayout->addWidget(busiestLabel, 1);
        contentLayout->addWidget(busiestCard);
    }

    if (summary.avgEarlyDays > 0) {
        QFrame* earlyCard = new QFrame;
        earlyCard->setStyleSheet("QFrame { background: #E8F5E9; border-radius: 12px; padding: 12px; }");
        QHBoxLayout* earlyLayout = new QHBoxLayout(earlyCard);
        earlyLayout->setContentsMargins(10, 8, 10, 8);
        QLabel* earlyIcon = new QLabel("⏰", earlyCard);
        earlyIcon->setStyleSheet("font-size: 18px;");
        earlyLayout->addWidget(earlyIcon);
        QLabel* earlyLabel = new QLabel(QString("平均提前 %1 天完成").arg(summary.avgEarlyDays, 0, 'f', 1), earlyCard);
        earlyLabel->setStyleSheet("font-size: 13px; color: #2E7D32;");
        earlyLayout->addWidget(earlyLabel, 1);
        contentLayout->addWidget(earlyCard);
    }

    QFrame* suggestionCard = new QFrame;
    suggestionCard->setStyleSheet(QString("QFrame { background: %1; border-radius: 12px; padding: 12px; border-left: 3px solid %2; }").arg(Theme::CARD_BG).arg(Theme::PRIMARY));
    QHBoxLayout* suggestionLayout = new QHBoxLayout(suggestionCard);
    suggestionLayout->setContentsMargins(10, 8, 10, 8);
    QLabel* suggestionIcon = new QLabel("💡", suggestionCard);
    suggestionIcon->setStyleSheet("font-size: 18px;");
    suggestionLayout->addWidget(suggestionIcon);
    QLabel* suggestionLabel = new QLabel(summary.suggestion, suggestionCard);
    suggestionLabel->setWordWrap(true);
    suggestionLabel->setStyleSheet("font-size: 13px; color: #444;");
    suggestionLayout->addWidget(suggestionLabel, 1);
    contentLayout->addWidget(suggestionCard);

    QFrame* mascotCard = new QFrame;
    mascotCard->setStyleSheet("QFrame { background: #FFF8E1; border-radius: 12px; padding: 12px; }");
    QHBoxLayout* mascotLayout = new QHBoxLayout(mascotCard);
    mascotLayout->setContentsMargins(10, 8, 10, 8);
    QLabel* mascotIcon = new QLabel("🦁", mascotCard);
    mascotIcon->setStyleSheet("font-size: 24px;");
    mascotLayout->addWidget(mascotIcon);
    QLabel* mascotLabel = new QLabel(summary.mascotMessage, mascotCard);
    mascotLabel->setWordWrap(true);
    mascotLabel->setStyleSheet("font-size: 12px; color: #795548; font-style: italic;");
    mascotLayout->addWidget(mascotLabel, 1);
    contentLayout->addWidget(mascotCard);

    QFrame* bottomDivider = new QFrame(outerContainer);
    bottomDivider->setStyleSheet("background: #F0F0F0;");
    bottomDivider->setFixedHeight(1);
    contentLayout->addWidget(bottomDivider);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();

    QPushButton* okBtn = new QPushButton("我知道了", outerContainer);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setFixedHeight(40);
    okBtn->setFixedWidth(120);
    okBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: %1;
            border: none;
            border-radius: 10px;
            color: white;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: %2;
        }
    )").arg(Theme::PRIMARY).arg(Theme::PRIMARY_DARK));
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okBtn);

    contentLayout->addLayout(buttonLayout);
}

WeeklySummaryDialog::~WeeklySummaryDialog()
{
    delete d;
}

void WeeklySummaryDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        d->dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void WeeklySummaryDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - d->dragPos);
        event->accept();
    }
}