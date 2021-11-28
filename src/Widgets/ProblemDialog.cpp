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
    SELECT name FROM tag JOIN problem_tag ON tag.id = problem_tag.tagid WHERE problem_tag.problemid =:problemid 
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

    if (!setupProblemModel())
    {
        return;
    }

    setupMapper();

    if (!setupTagModel())
    {
        return;
    }

    connect(ui.problemTable->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &ProblemDialog::updateTagsComboBox);
}

bool ProblemDialog::setupTagModel()
{
    getTagsQuery = new QSqlQuery();
    if (!getTagsQuery->prepare(GET_ALL_TAGS_OF_PROBLEM))
    {
        return false;
    }

    int row = mapper->currentIndex();
    currProblemId = problemModel->record(row).value("id").toInt();
    getTagsQuery->bindValue(":problemid", currProblemId);
    getTagsQuery->exec();

    tagModel = new QSqlQueryModel();
    tagModel->setQuery(*getTagsQuery);
    ui.tagsView->setModel(tagModel);

    return true;
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
    problemModel->submitAll();
    mapper->submit();

    accept();
}

void ProblemDialog::updateTagsComboBox()
{
    int row = mapper->currentIndex();
    if (row == -1)
        return;
    currProblemId = problemModel->record(row).value("id").toInt();
    getTagsQuery->bindValue(":problemid", currProblemId);
    getTagsQuery->exec();
    tagModel->setQuery(*getTagsQuery);
    ui.tagsView->setModel(tagModel);
}

void ProblemDialog::deleteProblem()
{
    int row = mapper->currentIndex();
    problemModel->removeRow(row);
    mapper->submit();
    problemModel->submit();
    mapper->setCurrentIndex(qMin(row, problemModel->rowCount() - 1));
    updateTagsComboBox();
}

void ProblemDialog::addProblem()
{
    int row = mapper->currentIndex();
    if (row == -1)
        ++row;
    problemModel->insertRow(row);
    mapper->setCurrentIndex(row);
    mapper->submit();
    problemModel->submit();

    clearForm();
}

bool ProblemDialog::setupProblemModel()
{
    problemModel = new QSqlRelationalTableModel(ui.problemTable);
    problemModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    problemModel->setTable("problem");

    // Set the localized header captions:
    problemModel->setHeaderData(problemModel->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    problemModel->setHeaderData(problemModel->fieldIndex("difficulty"), Qt::Horizontal, tr("Difficulty"));
    problemModel->setHeaderData(problemModel->fieldIndex("time_taken"), Qt::Horizontal, tr("Time Taken"));
    problemModel->setHeaderData(problemModel->fieldIndex("no_of_attempts"), Qt::Horizontal, tr("No of attempts"));
    problemModel->setHeaderData(problemModel->fieldIndex("description"), Qt::Horizontal, tr("Description"));

    // Populate the problemModel:
    if (!problemModel->select())
    {
        showError(problemModel->lastError());
        return false;
    }

    // Set the problemModel and hide the ID column:
    ui.problemTable->setModel(problemModel);
    ui.problemTable->setColumnHidden(problemModel->fieldIndex("id"), true);
    ui.problemTable->setSelectionMode(QAbstractItemView::SingleSelection);

    return true;
}

void ProblemDialog::setupMapper()
{
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(problemModel);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->addMapping(ui.titleEdit, problemModel->fieldIndex("title"));
    mapper->addMapping(ui.filePathEdit, problemModel->fieldIndex("file_path"));
    mapper->addMapping(ui.problemUrlEdit, problemModel->fieldIndex("problem_url"));
    mapper->addMapping(ui.submissionUrlEdit, problemModel->fieldIndex("solution_url"));
    mapper->addMapping(ui.descriptionEdit, problemModel->fieldIndex("description"));
    mapper->addMapping(ui.difficultyEdit, problemModel->fieldIndex("difficulty"));
    mapper->addMapping(ui.timeTakenEdit, problemModel->fieldIndex("time_taken"));

    connect(ui.problemTable->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper,
            &QDataWidgetMapper::setCurrentModelIndex);

    ui.problemTable->setCurrentIndex(problemModel->index(0, 0));
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