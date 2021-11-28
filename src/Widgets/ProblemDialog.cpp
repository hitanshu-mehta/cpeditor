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

#include "Widgets/ProblemDialog.hpp"
#include "../Core/EventLogger.hpp"
#include "../InitDB.h"

#include <QDataWidgetMapper>
#include <QMessageBox>

namespace Widgets
{

ProblemDialog::ProblemDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle("Add a new problem");

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(this, "Unable to load database", "");

    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError)
    {
        showError(err);
        return;
    }

    // Create the data model:
    model = new QSqlRelationalTableModel(ui.problemTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("problems");

    tagIdx = model->fieldIndex("tags");
    model->setRelation(tagIdx, QSqlRelation("tags", "id", "name"));

    // Set the localized header captions:
    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("difficulty"), Qt::Horizontal, tr("Difficulty"));
    model->setHeaderData(model->fieldIndex("time_taken"), Qt::Horizontal, tr("Time Taken"));
    model->setHeaderData(model->fieldIndex("no_of_attempts"), Qt::Horizontal, tr("No of attempts"));
    model->setHeaderData(tagIdx, Qt::Horizontal, tr("Tags"));
    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("description"));

    // Populate the model:
    if (!model->select())
    {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column:
    ui.problemTable->setModel(model);
    ui.problemTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.problemTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui.tagEdit->setModel(model->relationModel(tagIdx));
    ui.tagEdit->setModelColumn(model->relationModel(tagIdx)->fieldIndex("name"));

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(ui.titleEdit, model->fieldIndex("title"));
    mapper->addMapping(ui.filePathEdit, model->fieldIndex("file_path"));
    mapper->addMapping(ui.problemUrlEdit, model->fieldIndex("problem_url"));
    mapper->addMapping(ui.submissionUrlEdit, model->fieldIndex("solution_url"));
    mapper->addMapping(ui.descriptionEdit, model->fieldIndex("description"));
    mapper->addMapping(ui.tagEdit, model->fieldIndex("tags"));
    mapper->addMapping(ui.difficultyEdit, model->fieldIndex("difficulty"));
    mapper->addMapping(ui.timeTakenEdit, model->fieldIndex("time_taken"));

    connect(ui.problemTable->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper,
            &QDataWidgetMapper::setCurrentModelIndex);

    ui.problemTable->setCurrentIndex(model->index(0, 0));
}

void ProblemDialog::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database", "Error initializing database: " + err.text());
}

} // namespace Widgets