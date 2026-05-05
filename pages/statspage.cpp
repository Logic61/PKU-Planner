#include "statspage.h"
#include "../models/datamanager.h"
#include "../models/task.h"
#include "../models/course.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QGridLayout>
#include <QDate>
#include <QList>

namespace {
QFrame* makeCard(const QString &title, const QString &value) {
    QFrame *card = new QFrame;
    card->setStyleSheet("QFrame{background:white;border-radius:16px;padding:18px;} QLabel.title{color:#666;font-size:12px;} QLabel.value{font-size:20px;font-weight:700;color:#222;} ");
    QVBoxLayout *l = new QVBoxLayout(card);
    QLabel *t = new QLabel(title);
    t->setObjectName("title");
    t->setProperty("class", "title");
    QLabel *v = new QLabel(value);
    v->setObjectName("value");
    v->setProperty("class", "value");
    l->addWidget(t);
    l->addWidget(v);
    return card;
}
}

StatsPage::StatsPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background:#F7F6F4;");
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    QLabel *title = new QLabel("学习分析\n本学期效率概览");
    title->setStyleSheet("font-size:18px;font-weight:700;color:#222;");
    root->addWidget(title);

    // 四个统计卡片
    QHBoxLayout *cards = new QHBoxLayout;
    cards->setSpacing(12);
    QFrame *cardBox = makeCard("任务总数", "0");
    QFrame *cardDone = makeCard("已完成率", "0%");
    QFrame *cardOnTime = makeCard("按时完成率", "0%");
    QFrame *cardAvg = makeCard("平均提前时间", "0 天");
    cards->addWidget(cardBox,1);
    cards->addWidget(cardDone,1);
    cards->addWidget(cardOnTime,1);
    cards->addWidget(cardAvg,1);
    root->addLayout(cards);

    // 课程任务数量排行（进度条）
    QFrame *rankCard = new QFrame;
    rankCard->setStyleSheet("QFrame{background:white;border-radius:16px;padding:12px;}");
    QVBoxLayout *rankLayout = new QVBoxLayout(rankCard);
    QLabel *rankTitle = new QLabel("课程任务数量排行");
    rankTitle->setStyleSheet("font-weight:700;color:#222;");
    rankLayout->addWidget(rankTitle);
    // placeholder container
    QVBoxLayout *bars = new QVBoxLayout;
    rankLayout->addLayout(bars);
    root->addWidget(rankCard);

    // 热力日历（本月）
    QFrame *heatCard = new QFrame;
    heatCard->setStyleSheet("QFrame{background:white;border-radius:16px;padding:12px;}");
    QVBoxLayout *heatLayout = new QVBoxLayout(heatCard);
    QLabel *heatTitle = new QLabel("本月 DDL 热力图");
    heatTitle->setStyleSheet("font-weight:700;color:#222;");
    heatLayout->addWidget(heatTitle);
    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(6);
    heatLayout->addLayout(grid);
    root->addWidget(heatCard);

    // 趋势图（最近7天）
    QFrame *trendCard = new QFrame;
    trendCard->setStyleSheet("QFrame{background:white;border-radius:16px;padding:12px;}");
    QVBoxLayout *trendLayout = new QVBoxLayout(trendCard);
    QLabel *trendTitle = new QLabel("最近7天任务完成趋势");
    trendTitle->setStyleSheet("font-weight:700;color:#222;");
    trendLayout->addWidget(trendTitle);
    QHBoxLayout *trendBars = new QHBoxLayout;
    trendLayout->addLayout(trendBars);
    root->addWidget(trendCard);

    // simple refresh on construction
    refresh();
}

void StatsPage::refresh()
{
    // Compute stats from DataManager on demand when later hooked up.
    // For brevity this function is a placeholder — detail rendering will be filled when integrating with UI.
}
