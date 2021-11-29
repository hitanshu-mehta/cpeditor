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

#include "Widgets/TagManager.hpp"
#include "Core/EventLogger.hpp"

#include <QAction>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>

const static auto GET_ALL_TAGS = QLatin1String(R"(SELECT id, name FROM tag WHERE name LIKE :phrase)");

const static auto GET_ALL_TAGS_OF_PROBLEM = QLatin1String(
    R"(SELECT name FROM problem_tag JOIN tag ON problem_tag.tagid = tag.id WHERE problem_tag.problemid=:problemid)");

const static auto INSERT_TAG = QLatin1String(R"(INSERT INTO tag(name, removable) VALUES(?, ?))");

const static auto DELETE_TAG = QLatin1String(R"(DELETE FROM tag WHERE name = ?)");

const static auto GET_ID_OF_TAG = QLatin1String(R"(SELECT id FROM tag WHERE name = ?)");

namespace Widgets
{

TagManager::TagManager(QWidget *parent) : QWidget(parent)
{
    auto mainLayout = new QHBoxLayout(this);

    label = new QLabel(this);
    label->setText("Tags");

    tagsView = new QListView(this);
    tagsView->setMaximumHeight(50);
    QSizePolicy sizePolicy;
    sizePolicy.setVerticalPolicy(QSizePolicy::Maximum);
    tagsView->setSizePolicy(sizePolicy);
    tagsView->setFlow(QListView::Flow::LeftToRight);

    QVBoxLayout *searchBarLayout = new QVBoxLayout();

    editor = new QLineEdit();
    editor->setMaximumHeight(50);

    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(editor);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();
    popup->hide();

    msg = new QLabel();
    msg->hide();

    searchBarLayout->addWidget(editor);
    searchBarLayout->addWidget(msg);
    searchBarLayout->addWidget(popup);

    // Setup UI
    mainLayout->addWidget(label);
    mainLayout->addWidget(tagsView);
    mainLayout->addLayout(searchBarLayout);

    getTagsQuery = new QSqlQuery(GET_ALL_TAGS);
    getTagsOfProblemQuery = new QSqlQuery(GET_ALL_TAGS_OF_PROBLEM);
    insertTagQuery = new QSqlQuery(INSERT_TAG);
    deleteTagQuery = new QSqlQuery(DELETE_TAG);
    getIdOfTag = new QSqlQuery(GET_ID_OF_TAG);

    getTagsOfProblemQuery->prepare(GET_ALL_TAGS_OF_PROBLEM);
    getTagsOfProblemQuery->exec();

    tagsOfProblemModel = new QSqlQueryModel();
    tagsOfProblemModel->setQuery(*getTagsOfProblemQuery);
    tagsView->setModel(tagsOfProblemModel);

    connect(popup, &QTreeWidget::itemClicked, this, &TagManager::doneCompletion);

    timer.setSingleShot(true);
    timer.setInterval(500);
    connect(&timer, &QTimer::timeout, this, &TagManager::autoSuggest);
    connect(editor, &QLineEdit::textEdited, &timer, QOverload<>::of(&QTimer::start));

    // Set shortcuts for editor
    QAction *addAction = new QAction(this);
    addAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
    addAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    connect(addAction, &QAction::triggered, this, &TagManager::addTagShortCutTriggered);
    editor->addAction(addAction);

    QAction *deleteAction = new QAction(this);
    deleteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_D));
    deleteAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    connect(deleteAction, &QAction::triggered, this, &TagManager::deleteTagShortCutTriggered);
    editor->addAction(deleteAction);

    QAction *addTagToProblemAction = new QAction(this);
    addTagToProblemAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Return));
    addTagToProblemAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    connect(addTagToProblemAction, &QAction::triggered, this, &TagManager::doneCompletion);
    editor->addAction(addTagToProblemAction);

    displayMsg("Press Ctrl+Shift+Return to add tag to seleced problem", 5000);
}

TagManager::~TagManager()
{
    delete popup;
    delete label;
    delete editor;
    delete tagsView;
}

void TagManager::updateTagView(int problemid)
{
    if (problemid == -1)
        return;

    getTagsOfProblemQuery->bindValue(":problemid", problemid);
    getTagsOfProblemQuery->exec();
    tagsOfProblemModel->setQuery(*getTagsOfProblemQuery);

    tagsView->setModel(tagsOfProblemModel);
}

void TagManager::doneCompletion()
{
    timer.stop();
    popup->hide();
    editor->setFocus();
    QTreeWidgetItem *item = popup->currentItem();
    if (item)
    {
        editor->setText(item->text(0));
        getIdOfTag->addBindValue(editor->text());
        getIdOfTag->exec();
        if (getIdOfTag->next())
        {
            auto tagid = getIdOfTag->value(0);
            emit addTagToProblem(tagid);
        }
    }
}

void TagManager::showCompletion(const QVector<TagManager::Tag> &choices)
{
    if (choices.isEmpty())
    {
        QString currSearch = editor->text();
        if (!currSearch.isEmpty())
        {
            displayMsg("Press Ctrl+Shift+A to add new Tag.", 5000);
        }

        popup->clear();
        popup->hide();
        return;
    }

    const QPalette &pal = editor->palette();
    QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

    popup->setUpdatesEnabled(false);
    popup->clear();
    for (const auto &choice : choices)
    {
        auto item = new QTreeWidgetItem(popup);
        item->setText(0, choice.name);
        item->setForeground(0, color);
    }

    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->setUpdatesEnabled(true);

    popup->setFocus();
    popup->show();
}

void TagManager::autoSuggest()
{
    QString str = editor->text();
    if (str.length() == 0)
    {
        tags.clear();
        showCompletion(tags);
        return;
    }
    // Append '%' for prefix match
    str.append('%');

    getTagsQuery->prepare(GET_ALL_TAGS);
    getTagsQuery->bindValue(":phrase", str);
    getTagsQuery->exec();

    tags.clear();
    while (getTagsQuery->next())
    {
        QVariant id = getTagsQuery->value(0);
        QString name = getTagsQuery->value(1).toString();
        tags.append({id, name});
    }

    showCompletion(tags);
}

void TagManager::addTagShortCutTriggered()
{
    QString currSearch = editor->text();
    addTag(currSearch, true);
    displayMsg(currSearch + " tag is added", 2000);
}

QVariant TagManager::addTag(const QString &name, bool removable)
{
    insertTagQuery->addBindValue(name);
    insertTagQuery->addBindValue(removable);
    insertTagQuery->exec();
    return insertTagQuery->lastInsertId();
}

void TagManager::deleteTagShortCutTriggered()
{
    QString currSearch = editor->text();
    deleteTag(currSearch);

    popup->hide();
    displayMsg(currSearch + " tag is deleted", 2000);
}

void TagManager::deleteTag(const QString &name)
{
    deleteTagQuery->addBindValue(name);
    deleteTagQuery->exec();
}

void TagManager::displayMsg(const QString &txt, int ms)
{
    msg->show();
    msg->setText(txt);
    QTimer::singleShot(ms, [this] { msg->hide(); });
}

} // namespace Widgets