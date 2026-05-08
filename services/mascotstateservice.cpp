#include "mascotstateservice.h"
#include "../models/datamanager.h"
#include "../models/task.h"
#include <QDateTime>
#include <QDebug>

MascotStateService& MascotStateService::instance()
{
    static MascotStateService instance;
    return instance;
}

MascotStateService::MascotStateService()
    : QObject(nullptr)
    , m_currentState(MascotState::Happy)
    , m_currentUrgency(0.0)
{
    calculateUrgency();
    updateState();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        calculateUrgency();
        updateState();
    });
    timer->start(30000);

    connect(&DataManager::instance(), &DataManager::tasksChanged, this, [this]() {
        calculateUrgency();
        updateState();
    });
}

void MascotStateService::calculateUrgency()
{
    const auto tasks = DataManager::instance().tasks();
    double maxUrgency = 0.0;

    for (const Task &task : tasks) {
        if (task.completed) continue;

        if (!task.deadline.isValid()) continue;

        QDateTime now = QDateTime::currentDateTime();
        if (task.isOverdue()) {
            maxUrgency = 1.0;
            break;
        }

        qint64 remainingSecs = now.secsTo(task.deadline);
        double remainingHours = remainingSecs / 3600.0;

        if (remainingHours <= 0) {
            maxUrgency = 1.0;
            break;
        }

        double urgency = 1.0 - (remainingHours / 72.0);
        if (urgency < 0) urgency = 0;

        if (urgency > maxUrgency) {
            maxUrgency = urgency;
        }
    }

    m_currentUrgency = maxUrgency;
    qDebug() << "[MascotStateService] current urgency:" << m_currentUrgency;
}

void MascotStateService::updateState()
{
    MascotState newState;

    if (m_currentUrgency == 0) {
        newState = MascotState::Happy;
    } else if (m_currentUrgency <= 0.33) {
        newState = MascotState::Worried;
    } else if (m_currentUrgency <= 0.66) {
        newState = MascotState::Sweating;
    } else {
        newState = MascotState::Dead;
    }

    if (newState != m_currentState) {
        m_currentState = newState;
        qDebug() << "[MascotStateService] state changed to:" << (int)newState;
        emit stateChanged(newState);
    }
}