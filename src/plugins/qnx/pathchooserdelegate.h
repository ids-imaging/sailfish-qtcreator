/****************************************************************************
**
** Copyright (C) 2016 BlackBerry Limited. All rights reserved.
** Contact: KDAB (info@kdab.com)
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#ifndef QNX_INTERNAL_PATHCHOOSERDELEGATE_H
#define QNX_INTERNAL_PATHCHOOSERDELEGATE_H

#include <QStyledItemDelegate>

#include <utils/pathchooser.h>

namespace Qnx {
namespace Internal {

// TODO: This whole class should probably go into utils

class PathChooserDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PathChooserDelegate(QObject *parent = 0);

    void setExpectedKind(Utils::PathChooser::Kind kind);
    void setPromptDialogFilter(const QString &filter);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setHistoryCompleter(const QString &key);

private slots:
    void emitCommitData();

private:
    Utils::PathChooser::Kind m_kind;
    QString m_filter;
    QString m_historyKey;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_PATHCHOOSERDELEGATE_H
