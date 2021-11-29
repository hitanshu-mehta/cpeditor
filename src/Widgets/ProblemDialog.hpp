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

#ifndef PROBLEMDIALOG_HPP
#define PROBLEMDIALOG_HPP

#include <QDataWidgetMapper>
#include <QDialog>
#include <QSqlQuery>
#include <QtSql>

#include "TagManager.hpp"

#include "../../ui/ui_problemDialog.h"

namespace Widgets
{

class ProblemDialog : public QDialog
{
    Q_OBJECT

  public:
    ProblemDialog(QWidget *parent = nullptr);

  private slots:
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_cancelButton_clicked();

    void updateTagsComboBox();

    void addTagToCurrentProblem(const QVariant tagid);

  private:
    void showError(const QSqlError &err);

    /**
     * @brief setup model of `problem` table
     * @return true if model is successfully setup, false otherwise.
     */
    bool setupProblemModel();
    void setupMapper();

    void addProblem();
    void deleteProblem();

    void addProblemTag(const QVariant problemid, const QVariant tagid);

    void clearForm();

    /**
     * @brief get problem id of currently selected row. problem id is non-negative.
     * @return -1 if no row is seleted otherwise problem id
     */
    int getCurrentProblemid() const;

  private:
    QSqlRelationalTableModel *problemModel;
    QDataWidgetMapper *mapper;

    QSqlQuery *addProblemTagQuery;

    Ui::problemDialog ui;

    Widgets::TagManager *tagManager;
};

} // namespace Widgets

#endif // PROBLEMDIALOG_HPP
