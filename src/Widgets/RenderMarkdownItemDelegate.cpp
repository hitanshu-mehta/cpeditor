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

#include "Widgets/RenderMarkdownItemDelegate.hpp"
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QTextDocument>
#include <QSize>

namespace Widgets
{

RenderMarkdownItemDelegate::RenderMarkdownItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QString RenderMarkdownItemDelegate::anchorAt(QString html, const QPoint &point) const
{
    QTextDocument doc;
    doc.setHtml(html);

    auto textLayout = doc.documentLayout();
    Q_ASSERT(textLayout != 0);
    return textLayout->anchorAt(point);
}

void RenderMarkdownItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setMarkdown(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    QSize iconSize = options.icon.actualSize(options.rect.size());
    // right shit the icon
    painter->translate(options.rect.left() + iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width() + iconSize.width(), options.rect.height());

    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;

    // Adjust color palette if the cell is selected
    if (option.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::Active, QPalette::HighlightedText));
    ctx.clip = clip;

    // Vertical Center alignment instead of the default top alignment
    painter->translate(0, 0.5 * (options.rect.height() - doc.size().height()));

    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize RenderMarkdownItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}

} // namespace Widgets
