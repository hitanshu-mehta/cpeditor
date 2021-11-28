

#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

// void addProblem(QSqlQuery &q, const QString &title, int difficulty, const QVariant &tagId, int time_taken,
//                 const QString &problemUrl, const QString &solutionUrl, const QString &filePath, int noAttempts,
//                 const QString &description)
// {
//     q.addBindValue(title);
//     q.addBindValue(difficulty);
//     q.addBindValue(tagId);
//     q.addBindValue(time_taken);
//     q.addBindValue(problemUrl);
//     q.addBindValue(solutionUrl);
//     q.addBindValue(filePath);
//     q.addBindValue(noAttempts);
//     q.addBindValue(description);
//     q.exec();
// }

// QVariant addTag(QSqlQuery &q, const QString &name, bool removable)
// {
//     q.addBindValue(name);
//     q.addBindValue(removable);
//     q.exec();
//     return q.lastInsertId();
// }

const auto PROBLEMS_SQL = QLatin1String(R"(
    create table if not exists problems(id integer primary key, title varchar, difficulty integer,
                       tags integer, time_taken integer, problem_url varchar,
                       solution_url varchar, file_path varchar, no_of_attempts integer, description varchar)
    )");

const auto TAGS_SQL = QLatin1String(R"(
    create table if not exists tags(id integer primary key, name varchar, removable bool)
    )");

// const auto INSERT_PROBLEM_SQL = QLatin1String(R"(
//     insert into problems(title, difficulty, tags, time_taken, problem_url, solution_url, file_path, no_of_attempts,
//     description)
//                       values(?, ?, ?, ?, ?, ?, ?, ?, ?)
//     )");

// const auto INSERT_TAG_SQL = QLatin1String(R"(
//     insert into tags(name, removable) values(?, ?)
//     )");

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath().append("/cpeditor.db"));

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("problems", Qt::CaseInsensitive) && tables.contains("tags", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(PROBLEMS_SQL))
        return q.lastError();
    if (!q.exec(TAGS_SQL))
        return q.lastError();

    return QSqlError();
}

#endif
