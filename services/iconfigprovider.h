#ifndef IConfigProvider_H
#define IConfigProvider_H

#include <QObject>
#include <QDate>
#include <QString>

class IConfigProvider
{
public:
    virtual ~IConfigProvider() = default;

    virtual void onConfigChanged() = 0;

    // Getters
    virtual QDate getSemesterStart() const = 0;
    virtual QDate getSemesterEnd() const = 0;
    virtual int getCurrentWeek() const = 0;
    virtual bool isSingleWeek() const = 0;
    virtual bool isReminderEnabled() const = 0;
    virtual int getReminderHours() const = 0;
    virtual bool isDetailDrawerMode() const = 0;
    virtual QString getExportPath() const = 0;
    virtual bool isOnboardingShown() const = 0;
    virtual QString getDataPath() const = 0;

    // Setters
    virtual void setSemesterStart(const QDate& date) = 0;
    virtual void setSemesterEnd(const QDate& date) = 0;
    virtual void setReminderEnabled(bool enabled) = 0;
    virtual void setReminderHours(int hours) = 0;
    virtual void setDetailDrawerMode(bool drawerMode) = 0;
    virtual void setExportPath(const QString& path) = 0;
    virtual void setOnboardingShown(bool shown) = 0;
    virtual void setLastSummaryDate(const QDate& date) = 0;

    // Actions
    virtual void resetOnboarding() = 0;
    virtual void resetAllData() = 0;

signals:
    void configChanged();
};

#endif // IConfigProvider_H