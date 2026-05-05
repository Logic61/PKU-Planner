#include "taskcardwidget.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString courseTagStyle()
{
    return "background:#FCECEC; color:#8B1E2D; border-radius:10px; padding:4px 10px; font-size:12px; font-weight:600;";
}

QString actionButtonStyle()
{
    return R"(
        QPushButton {
            background: #FAF7F7;
            color: #8B1E2D;
            border: 1px solid #E8D9DB;
            border-radius: 10px;
            padding: 6px 12px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #8B1E2D;
            color: white;
        }
    )";
}
}

TaskCardWidget::TaskCardWidget(const Task &task, QWidget *parent)
    : QFrame(parent), m_task(task)
{
    setObjectName("taskCard");
    setCursor(Qt::PointingHandCursor);
    setStyleSheet(R"(
        QFrame#taskCard {
            background: white;
            border-radius: 16px;
            border: 1px solid #F0E6E6;
        }
        QFrame#taskCard:hover {
            border: 1px solid #8B1E2D;
        }
    )");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(10);

    doneBox = new QCheckBox;
    doneBox->setChecked(m_task.completed);
    doneBox->setCursor(Qt::PointingHandCursor);
    doneBox->setStyleSheet(R"(
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 9px;
            border: 1px solid #8B1E2D;
            background: white;
        }
        QCheckBox::indicator:checked {
            background: #8B1E2D;
            border: 1px solid #8B1E2D;
        }
    )");

    courseTag = new QLabel;
    courseTag->setStyleSheet(courseTagStyle());

    titleLabel = new QLabel;
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("font-size:15px; font-weight:700; color:#222;");

    // First row: title and done checkbox
    QHBoxLayout *titleRow = new QHBoxLayout;
    titleRow->setContentsMargins(0, 0, 0, 0);
    titleRow->setSpacing(8);
    titleRow->addWidget(titleLabel, 1);
    titleRow->addWidget(doneBox, 0, Qt::AlignRight);
    root->addLayout(titleRow);

    priorityBadge = new QLabel;
    priorityBadge->setAlignment(Qt::AlignCenter);
    priorityBadge->setMinimumWidth(72);
    priorityBadge->setStyleSheet("border-radius:12px; padding:4px 10px; font-size:12px; font-weight:700;");

    statusBadge = new QLabel;
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setStyleSheet("color:#888; font-size:12px; font-weight:600;");

    deadlineLabel = new QLabel;
    deadlineLabel->setStyleSheet("color:#444; font-size:12px; font-weight:600;");

    QHBoxLayout *metaRow = new QHBoxLayout;
    metaRow->setContentsMargins(0, 0, 0, 0);
    metaRow->setSpacing(8);
    metaRow->addWidget(deadlineLabel);
    metaRow->addWidget(courseTag);
    metaRow->addWidget(priorityBadge);
    metaRow->addStretch();
    metaRow->addWidget(statusBadge);

    QHBoxLayout *actionRow = new QHBoxLayout;
    actionRow->setContentsMargins(0, 0, 0, 0);
    actionRow->setSpacing(8);

    QPushButton *editBtn = new QPushButton("✏ 编辑");
    completeBtn = new QPushButton;
    QPushButton *deleteBtn = new QPushButton("🗑 删除");
    for (QPushButton *btn : {editBtn, completeBtn, deleteBtn}) {
        btn->setStyleSheet(actionButtonStyle());
        btn->setCursor(Qt::PointingHandCursor);
    }

    actionRow->addWidget(editBtn);
    actionRow->addWidget(completeBtn);
    actionRow->addWidget(deleteBtn);
    actionRow->addStretch();

    root->addLayout(metaRow);
    root->addLayout(actionRow);

    connect(doneBox, &QCheckBox::toggled, this, [this](bool checked) {
        m_task.completed = checked;
        updateVisualState();
        emit completed(m_task);
    });

    connect(editBtn, &QPushButton::clicked, this, [this]() {
        emit edited(m_task);
    });

    connect(completeBtn, &QPushButton::clicked, this, [this]() {
        // Toggle completed state: if currently completed -> mark not completed; else mark completed
        m_task.completed = !m_task.completed;
        doneBox->setChecked(m_task.completed);
        updateVisualState();
        emit completed(m_task);
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this]() {
        emit deleted(m_task);
    });

    setTask(task);
}

void TaskCardWidget::setTask(const Task &task)
{
    m_task = task;
    if (doneBox->isChecked() != m_task.completed) {
        doneBox->blockSignals(true);
        doneBox->setChecked(m_task.completed);
        doneBox->blockSignals(false);
    }

    courseTag->setText(QString("  %1  ").arg(m_task.course));
    titleLabel->setText(m_task.title);
    deadlineLabel->setText(QString("截止 %1").arg(m_task.deadline.toString("MM-dd hh:mm")));
    priorityBadge->setText(priorityText());
    statusBadge->setText(m_task.completed ? "已完成" : (m_task.isOverdue() ? "已逾期" : "进行中"));
    updateVisualState();
}

Task TaskCardWidget::task() const
{
    return m_task;
}

QString TaskCardWidget::priorityText() const
{
    switch (m_task.priority) {
    case 2: return "🔴 高优先级";
    case 1: return "🟡 中优先级";
    default: return "🔵 低优先级";
    }
}

QString TaskCardWidget::countdownText() const
{
    const int days = m_task.daysLeft();
    if (m_task.completed) {
        return "已完成";
    }
    if (days < 0) {
        return QString("已逾期 %1 天").arg(-days);
    }
    if (days == 0) {
        return "今晚截止";
    }
    return QString("还剩 %1 天").arg(days);
}

QString TaskCardWidget::priorityColor() const
{
    switch (m_task.priority) {
    case 2: return "#D32F2F";
    case 1: return "#F9A825";
    default: return "#1E88E5";
    }
}

void TaskCardWidget::updateVisualState()
{
    const QString priorityBg = m_task.priority == 2 ? "#FCE8E8" : (m_task.priority == 1 ? "#FFF4D8" : "#E8F1FE");
    const QString priorityFg = priorityColor();
    priorityBadge->setStyleSheet(QString("background:%1; color:%2; border-radius:12px; padding:4px 10px; font-size:12px; font-weight:700;")
                                     .arg(priorityBg, priorityFg));

    if (m_task.completed) {
        titleLabel->setStyleSheet("font-size:15px; font-weight:700; color:#9E9E9E; text-decoration:line-through;");
        courseTag->setStyleSheet("background:#F1F1F1; color:#888; border-radius:10px; padding:4px 10px; font-size:12px; font-weight:600;");
        statusBadge->setStyleSheet("color:#9E9E9E; font-size:12px; font-weight:600;");
        deadlineLabel->setStyleSheet("color:#9E9E9E; font-size:12px; font-weight:600;");
        setStyleSheet("QFrame#taskCard { background: #FAFAFA; border-radius: 16px; border: 1px solid #ECECEC; }");
    } else {
        titleLabel->setStyleSheet("font-size:15px; font-weight:700; color:#222;");
        courseTag->setStyleSheet(courseTagStyle());
        if (m_task.isOverdue()) {
            statusBadge->setStyleSheet("color:#C62828; font-size:12px; font-weight:700;");
            deadlineLabel->setStyleSheet("color:#C62828; font-size:12px; font-weight:700;");
        } else if (m_task.daysLeft() == 0) {
            statusBadge->setStyleSheet("color:#E64A19; font-size:12px; font-weight:700;");
            deadlineLabel->setStyleSheet("color:#E64A19; font-size:12px; font-weight:700;");
        } else {
            statusBadge->setStyleSheet("color:#666; font-size:12px; font-weight:600;");
            deadlineLabel->setStyleSheet("color:#444; font-size:12px; font-weight:600;");
        }
        setStyleSheet("QFrame#taskCard { background: white; border-radius: 16px; border: 1px solid #F0E6E6; }");
    }

    deadlineLabel->setText(countdownText());
    if (completeBtn) {
        completeBtn->setText(m_task.completed ? QStringLiteral("标为未完成") : QStringLiteral("✓ 完成"));
    }
}
