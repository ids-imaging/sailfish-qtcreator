/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#ifndef FORMWINDOWEDITOR_H
#define FORMWINDOWEDITOR_H

#include "designer_export.h"
#include "widgethost.h"

#include <QtCore/QStringList>
#include <QtCore/QPointer>

namespace ProjectExplorer {
class SessionNode;
class NodesWatcher;
}

namespace Core {
    class IFile;
}

namespace Designer {

// Master class maintaining a form window editor,
// containing file and widget host

class FormWindowEditor : public SharedTools::WidgetHost
{
    Q_OBJECT
public:
    explicit FormWindowEditor(QDesignerFormWindowInterface *form,
                              QWidget *parent = 0);
    ~FormWindowEditor();

    // IEditor
    bool createNew(const QString &contents);
    void setFile(Core::IFile *file);
    QString contents() const;
    Core::IFile *file() const;

signals:
    void changed();

private slots:
    void updateResources();
    void slotFormSizeChanged(int w, int h);

private:
    void initializeResources(const QString &fileName = QString());

    QWidget *m_containerWidget;
    QString m_displayName;
    QPointer<Core::IFile> m_file;
    QStringList m_originalUiQrcPaths;
    ProjectExplorer::SessionNode *m_sessionNode;
    ProjectExplorer::NodesWatcher *m_sessionWatcher;
};

} // namespace Designer

#endif // FORMWINDOWEDITOR_H
