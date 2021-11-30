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

#ifndef RENDERMARKDOWNITEMDELEGATE_HPP
#define RENDERMARKDOWNITEMDELEGATE_HPP

#include <QStyledItemDelegate>

namespace Widgets
{
class RenderMarkdownItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit RenderMarkdownItemDelegate(QObject *parent = 0);

    QString anchorAt(QString html, const QPoint &point) const;

  protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

} // namespace Widgets

#endif // RENDERMARKDOWNITEMDELEGATE_HPP
