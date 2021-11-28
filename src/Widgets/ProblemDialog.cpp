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

#include <QMessageBox>

namespace Widgets
{

const auto GET_ALL_TAGS_OF_PROBLEM = QLatin1String(R"(
    SELECT name FROM tag JOIN problem_tag ON tag.id = problem_tag.tagid WHERE problem_tag.problemid = ?
)");

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

    if (!setupModel())
    {
        return;
    }
    setupMapper();
}

void ProblemDialog::on_addButton_clicked()
{
    addProblem();
}

void ProblemDialog::on_deleteButton_clicked()
{
    deleteProblem();
}

void ProblemDialog::on_cancelButton_clicked()
{
    model->submitAll();
    mapper->submit();

    accept();
}

void ProblemDialog::deleteProblem()
{
    int row = mapper->currentIndex();
    model->removeRow(row);
    mapper->submit();
    model->submit();
    mapper->setCurrentIndex(qMin(row, model->rowCount() - 1));
}

void ProblemDialog::addProblem()
{
    int row = mapper->currentIndex();
    if (row == -1)
        ++row;
    model->insertRow(row);
    mapper->setCurrentIndex(row);
    mapper->submit();
    model->submit();

    clearForm();
}

bool ProblemDialog::setupModel()
{
    model = new QSqlRelationalTableModel(ui.problemTable);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setTable("problem");

    // Set the localized header captions:
    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("difficulty"), Qt::Horizontal, tr("Difficulty"));
    model->setHeaderData(model->fieldIndex("time_taken"), Qt::Horizontal, tr("Time Taken"));
    model->setHeaderData(model->fieldIndex("no_of_attempts"), Qt::Horizontal, tr("No of attempts"));
    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));

    // Populate the model:
    if (!model->select())
    {
        showError(model->lastError());
        return false;
    }

    // Set the model and hide the ID column:
    ui.problemTable->setModel(model);
    ui.problemTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.problemTable->setSelectionMode(QAbstractItemView::SingleSelection);

    return true;
}

void ProblemDialog::setupMapper()
{
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->addMapping(ui.titleEdit, model->fieldIndex("title"));
    mapper->addMapping(ui.filePathEdit, model->fieldIndex("file_path"));
    mapper->addMapping(ui.problemUrlEdit, model->fieldIndex("problem_url"));
    mapper->addMapping(ui.submissionUrlEdit, model->fieldIndex("solution_url"));
    mapper->addMapping(ui.descriptionEdit, model->fieldIndex("description"));
    mapper->addMapping(ui.difficultyEdit, model->fieldIndex("difficulty"));
    mapper->addMapping(ui.timeTakenEdit, model->fieldIndex("time_taken"));

    connect(ui.problemTable->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper,
            &QDataWidgetMapper::setCurrentModelIndex);

    ui.problemTable->setCurrentIndex(model->index(0, 0));
}

void ProblemDialog::clearForm()
{
    ui.titleEdit->clear();
    ui.problemUrlEdit->clear();
    ui.submissionUrlEdit->clear();
    ui.filePathEdit->clear();
    ui.difficultyEdit->clear();
    ui.descriptionEdit->clear();
    ui.timeTakenEdit->clear();
    ui.titleEdit->setFocus();
}

void ProblemDialog::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database", "Error initializing database: " + err.text());
}

} // namespace Widgets