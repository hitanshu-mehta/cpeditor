

#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

QVariant addProblem(QSqlQuery &q, const QString &title, int difficulty, int time_taken, const QString &problemUrl,
                    const QString &solutionUrl, const QString &filePath, int noAttempts, const QString &description)
{
    q.addBindValue(title);
    q.addBindValue(difficulty);
    q.addBindValue(time_taken);
    q.addBindValue(problemUrl);
    q.addBindValue(solutionUrl);
    q.addBindValue(filePath);
    q.addBindValue(noAttempts);
    q.addBindValue(description);
    q.exec();
    return q.lastInsertId();
}

QVariant addTag(QSqlQuery &q, const QString &name, bool removable)
{
    q.addBindValue(name);
    q.addBindValue(removable);
    q.exec();
    return q.lastInsertId();
}

void addProblemTag(QSqlQuery &q, const QVariant problemid, const QVariant tagid)
{
    q.addBindValue(problemid);
    q.addBindValue(tagid);
    q.exec();
}

const auto PROBLEM_SQL = QLatin1String(R"(
    CREATE TABLE IF NOT EXISTS problem(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title VARCHAR(40) NOT NULL,
        difficulty INTEGER,
        time_taken INTEGER,
        problem_url VARCHAR,
        solution_url VARCHAR,
        file_path varchar,
        no_of_attempts INTEGER,
        description VARCHAR))");

const auto TAG_SQL = QLatin1String(R"(
    CREATE TABLE IF NOT EXISTS tag(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name VARCHAR(40),
        removable BOOL)
    )");

const auto PROBLEM_TAG_SQL = QLatin1String(R"(
    CREATE TABLE IF NOT EXISTS problem_tag(
        problemid INTEGER NOT NULL,
        tagid INTEGER NOT NULL,
        FOREIGN KEY (problemid) REFERENCES problem,
        FOREIGN KEY (tagid) REFERENCES tag,
        PRIMARY KEY(problemid, tagid)
    ))");

const auto INSERT_PROBLEM_SQL = QLatin1String(R"(
    insert into problem(title, difficulty, time_taken, problem_url, solution_url, file_path, no_of_attempts,
    description)
                      values(?, ?, ?, ?, ?, ?, ?, ?)
    )");

const auto INSERT_TAG_SQL = QLatin1String(R"(
    insert into tag(name, removable) values(?, ?)
    )");

const auto INSERT_PROBLEM_TAG = QLatin1String(R"(
    INSERT INTO problem_tag(problemid, tagid) values(?, ?)
    )");

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath().append("/cpeditor.db"));

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("problem", Qt::CaseInsensitive) && tables.contains("tag", Qt::CaseInsensitive) &&
            tables.contains("problem_tag"),
        Qt::CaseInsensitive)
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(PROBLEM_SQL))
        return q.lastError();
    if (!q.exec(TAG_SQL))
        return q.lastError();
    if (!q.exec(PROBLEM_TAG_SQL))
        return q.lastError();

    if (!q.prepare(INSERT_TAG_SQL))
        return q.lastError();
    // auto dpIdx = addTag(q, "dp", false);
    // auto gIdx = addTag(q, "greedy", false);

    if (!q.prepare(INSERT_PROBLEM_SQL))
        return q.lastError();
    // auto pidx = addProblem(q, "problem1", 1, 10000, "", "", "", 3, "this is");

    if (!q.prepare(INSERT_PROBLEM_TAG))
        return q.lastError();
    // addProblemTag(q, pidx, dpIdx);
    // addProblemTag(q, pidx, gIdx);

    return QSqlError();
}

#endif
