#include "mascotwidget.h"
#include "../../services/mascotstateservice.h"
#include "../../models/datamanager.h"
#include "../../models/task.h"
#include "../../ui/theme.h"
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QDateTime>
#include <algorithm>

MascotWidget::MascotWidget(QWidget *parent)
    : QWidget(parent)
    , mascotLabel(nullptr)
    , popupFrame(nullptr)
    , popupVisible(false)
{
    // MascotWidget itself is just a container for the popup. It doesn't display the mascot image.
    // The image is displayed in SidebarWidget.
    setFixedSize(0, 0); // Make it invisible and take no space
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
}

void MascotWidget::showPopup()
{
    if (popupVisible && popupFrame) {
        popupFrame->hide();
        popupVisible = false;
        return;
    }

    createPopup();
    if (!popupFrame) return;

    // Position calculation
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeom = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);

    QPoint globalPos = QCursor::pos();
    int popupWidth = 300;
    int popupHeight = 280;
    
    int popupX = globalPos.x() - popupWidth / 2;
    int popupY = globalPos.y() - popupHeight - 20;

    // Boundary checks
    if (popupX + popupWidth > screenGeom.right()) popupX = screenGeom.right() - popupWidth - 10;
    if (popupX < screenGeom.left()) popupX = screenGeom.left() + 10;
    if (popupY < screenGeom.top()) popupY = globalPos.y() + 20;

    popupFrame->move(popupX, popupY);
    popupFrame->show();
    popupFrame->activateWindow();
    popupVisible = true;

    // Shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(popupFrame);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 40));
    popupFrame->setGraphicsEffect(shadow);
}

void MascotWidget::createPopup()
{
    if (popupFrame) {
        popupFrame->hide();
        popupFrame->deleteLater();
        popupFrame = nullptr;
    }

    popupFrame = new QFrame(nullptr);
    popupFrame->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    popupFrame->setAttribute(Qt::WA_TranslucentBackground);
    popupFrame->setFixedSize(300, 280);
    popupFrame->setStyleSheet(QString(
        "QFrame#PopupInner {"
        "  background: white;"
        "  border-radius: 20px;"
        "}"
    ));

    QVBoxLayout *outerLayout = new QVBoxLayout(popupFrame);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    
    QFrame *innerFrame = new QFrame(popupFrame);
    innerFrame->setObjectName("PopupInner");
    QVBoxLayout *mainLayout = new QVBoxLayout(innerFrame);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    outerLayout->addWidget(innerFrame);

    // Title
    QLabel *title = new QLabel("🛡️ PKU 攻城狮", innerFrame);
    title->setStyleSheet(QString("font-size: 16px; font-weight: 700; color: %1;").arg(Theme::PRIMARY));
    mainLayout->addWidget(title);

    int mode = QRandomGenerator::global()->bounded(3);
    
    if (mode == 0) { // Urgent Tasks
        QLabel *sectionTitle = new QLabel("今日紧急任务", innerFrame);
        sectionTitle->setStyleSheet(QString("font-size: 13px; font-weight: 600; color: %1;").arg(Theme::TEXT_SECONDARY));
        mainLayout->addWidget(sectionTitle);

        auto tasks = DataManager::instance().tasks();
        QList<Task> urgent;
        for(auto& t : tasks) {
            if (!t.completed && t.deadline.isValid()) urgent.append(t);
        }
        std::sort(urgent.begin(), urgent.end(), [](const Task& a, const Task& b){
            return a.deadline < b.deadline;
        });

        int count = 0;
        for(auto& t : urgent) {
            if (count >= 3) break;
            QLabel *item = new QLabel(QString("• %1 (%2)").arg(t.title).arg(t.deadline.toString("MM-dd")), innerFrame);
            item->setStyleSheet("font-size: 13px; color: #333;");
            mainLayout->addWidget(item);
            count++;
        }
        if (urgent.isEmpty()) {
            mainLayout->addWidget(new QLabel("暂时没有待办，休息一下吧！", innerFrame));
        }
    } else if (mode == 1) { // Quote
        QStringList quotes = {"未名湖畔好读书", "DDL 是第一生产力", "再写一行，就一行...", "要劳逸结合哦！"};
        QLabel *qLabel = new QLabel(quotes[QRandomGenerator::global()->bounded(quotes.size())], innerFrame);
        qLabel->setWordWrap(true);
        qLabel->setAlignment(Qt::AlignCenter);
        qLabel->setStyleSheet(QString("font-size: 15px; font-style: italic; color: %1; padding: 20px;").arg(Theme::TEXT_SECONDARY));
        mainLayout->addWidget(qLabel);
    } else { // Progress
        auto tasks = DataManager::instance().tasks();
        int total = tasks.size();
        int done = 0;
        for(auto& t : tasks) if(t.completed) done++;
        int percent = total > 0 ? (done * 100 / total) : 0;
        
        QLabel *pLabel = new QLabel(QString("本周完成进度: %1%").arg(percent), innerFrame);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setStyleSheet(QString("font-size: 18px; font-weight: 600; color: %1;").arg(Theme::PRIMARY));
        mainLayout->addWidget(pLabel);
        
        QLabel *sub = new QLabel(QString("已完成 %1 / %2 项任务").arg(done).arg(total), innerFrame);
        sub->setAlignment(Qt::AlignCenter);
        sub->setStyleSheet("font-size: 13px; color: #666;");
        mainLayout->addWidget(sub);
    }

    mainLayout->addStretch();
    
    QPushButton *closeBtn = new QPushButton("我知道了", innerFrame);
    closeBtn->setFixedHeight(32);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: white; border-radius: 16px; font-weight: 600; border: none; }"
        "QPushButton:hover { background: %2; }"
    ).arg(Theme::PRIMARY).arg(Theme::PRIMARY_DARK));
    connect(closeBtn, &QPushButton::clicked, [this](){
        if(popupFrame) popupFrame->hide();
        popupVisible = false;
    });
    mainLayout->addWidget(closeBtn);

    popupFrame->installEventFilter(this);
}

bool MascotWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == popupFrame && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (!popupFrame->rect().contains(me->pos())) {
            popupFrame->hide();
            popupVisible = false;
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
