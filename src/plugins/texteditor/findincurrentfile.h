/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
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

#ifndef FINDINCURRENTFILE_H
#define FINDINCURRENTFILE_H

#include "basefilefind.h"

#include <QPointer>

namespace Core {
class IEditor;
class IDocument;
} // namespace Core

namespace TextEditor {
namespace Internal {

class FindInCurrentFile : public BaseFileFind
{
    Q_OBJECT

public:
    FindInCurrentFile();

    QString id() const;
    QString displayName() const;
    bool isEnabled() const;
    void writeSettings(QSettings *settings);
    void readSettings(QSettings *settings);

protected:
    Utils::FileIterator *files(const QStringList &nameFilters,
                               const QVariant &additionalParameters) const;
    QVariant additionalParameters() const;
    QString label() const;
    QString toolTip() const;

private:
    void handleFileChange(Core::IEditor *editor);

    QPointer<Core::IDocument> m_currentDocument;
};

} // namespace Internal
} // namespace TextEditor

#endif // FINDINCURRENTFILE_H
