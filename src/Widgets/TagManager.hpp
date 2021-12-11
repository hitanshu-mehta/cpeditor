/*
 * Copyright (C) 2019-2021 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CP Editor.
 *
 * CP Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CP Editor behaves in unexpected way and
 * causes your ratings to go down and or lose any important contest.
 *
 * Believe Software is "Software" and it isn't immune to bugs.
 *
 */

#ifndef TAGMANAGER_HPP
#define TAGMANAGER_HPP

#include <QWidget>
#include <QtSql>

class QLineEdit;
class QTreeWidget;
class QTimer;
class QLabel;
class QListView;
class QVariant;
class QCompleter;

namespace Widgets
{

class TagManager : public QWidget
{
    Q_OBJECT

  public:
    explicit TagManager(QWidget *parent = nullptr);
    ~TagManager();

  signals:
    void addTagToProblem(const QVariant tagid);

  public slots:
    void updateTagView(int problemid);

  private slots:
    void doneCompletion();
    void addTagShortCutTriggered();
    void deleteTagShortCutTriggered();

  private:
    struct Tag
    {
        QVariant id;
        QString name;
    };

    QVariant addTag(const QString &name, bool removable);
    void deleteTag(const QString &name);

    /**
     * @brief Display given messsage in label
     * @param msg to display
     * @param timeout messsage will be hiden after given time
     */
    void displayMsg(const QString &msg, int timeout);

  private:
    QLabel *label;
    QListView *tagsView;
    QLineEdit *editor;
    QCompleter *completer;
    QLabel *msg;

    QSqlQueryModel *tagsOfProblemModel;
    QSqlQueryModel *getTagsModel;

    QSqlQuery *getTagsOfProblemQuery;
    QSqlQuery *insertTagQuery;
    QSqlQuery *deleteTagQuery;
    QSqlQuery *getIdOfTagQuery;
};

} // namespace Widgets

#endif // TAGMANAGER_HPP
