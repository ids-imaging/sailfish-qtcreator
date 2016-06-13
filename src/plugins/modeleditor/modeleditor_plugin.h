/****************************************************************************
**
** Copyright (C) 2016 Jochen Becher
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

#ifndef MODELEDITORPLUGIN_H
#define MODELEDITORPLUGIN_H

#include "modeleditor_global.h"

#include <extensionsystem/iplugin.h>

namespace ModelEditor {
namespace Internal {

class ModelsManager;

class ModelEditorPlugin :
        public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "ModelEditor.json")
    class ModelEditorPluginPrivate;

public:
    ModelEditorPlugin();
    ~ModelEditorPlugin();

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

    static ModelsManager *modelsManager();

private:
    ModelEditorPluginPrivate *d;
};

} // namespace Internal
} // namespace ModelEditor

#endif // MODELEDITORPLUGIN_H
