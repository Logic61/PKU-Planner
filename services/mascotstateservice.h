#ifndef MASCOTSTATESERVICE_H
#define MASCOTSTATESERVICE_H

#include <QObject>
#include <QTimer>
#include "../models/mascotstate.h"

class MascotStateService : public QObject
{
    Q_OBJECT

public:
    static MascotStateService& instance();

    MascotState currentState() const { return m_currentState; }
    double currentUrgency() const { return m_currentUrgency; }

signals:
    void stateChanged(MascotState state);

private:
    MascotStateService();
    void calculateUrgency();
    void updateState();

    MascotState m_currentState;
    double m_currentUrgency;
};

#endif // MASCOTSTATESERVICE_H