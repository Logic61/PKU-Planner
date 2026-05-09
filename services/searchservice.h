#ifndef SEARCHSERVICE_H
#define SEARCHSERVICE_H

#include <QString>
#include <QVector>

struct SearchResult {
    enum Type { Course, Task, File } type;
    QString title;
    QString subtitle;
    QString id;
    QString icon;
};

class SearchService {
public:
    static QVector<SearchResult> search(const QString& keyword);
    static QVector<SearchResult> searchCourses(const QString& keyword);
    static QVector<SearchResult> searchTasks(const QString& keyword);
    static QVector<SearchResult> searchFiles(const QString& keyword);

private:
    static bool fuzzyMatch(const QString& source, const QString& keyword);
};

#endif