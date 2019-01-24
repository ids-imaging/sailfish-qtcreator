/****************************************************************************
**
** Copyright (C) 2012 - 2014 Jolla Ltd.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Digia.
**
****************************************************************************/

#include "meremulatordevicewidget.h"
#include "ui_meremulatordevicewidget.h"

#include "merconnection.h"
#include "merconstants.h"
#include "meremulatordevice.h"
#include "mersdkmanager.h"
#include "mervirtualboxmanager.h"
#include "mervirtualmachinesettingswidget.h"

#include <projectexplorer/devicesupport/devicemanager.h>
#include <ssh/sshconnection.h>
#include <ssh/sshkeycreationdialog.h>
#include <utils/fancylineedit.h>
#include <utils/portlist.h>
#include <utils/utilsicons.h>

#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextStream>

using namespace ProjectExplorer;
using namespace QSsh;
using namespace Utils;

namespace Mer {
namespace Internal {

MerEmulatorDeviceWidget::MerEmulatorDeviceWidget(
        const IDevice::Ptr &deviceConfig, QWidget *parent)
    : IDeviceWidget(deviceConfig, parent)
    , m_ui(new Ui::MerEmulatorDeviceWidget)
    , m_virtualMachineSettingsWidget(new MerVirtualMachineSettingsWidget)
{
    m_ui->setupUi(this);
    connect(m_ui->factorySnapshotToolButton, &QAbstractButton::clicked,
            this, &MerEmulatorDeviceWidget::selectFactorySnapshot);
    connect(m_ui->userLineEdit, &QLineEdit::editingFinished,
            this, &MerEmulatorDeviceWidget::userNameEditingFinished);
    connect(m_ui->timeoutSpinBox, &QSpinBox::editingFinished,
            this, &MerEmulatorDeviceWidget::timeoutEditingFinished);
    connect(m_ui->timeoutSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MerEmulatorDeviceWidget::timeoutEditingFinished);
    connect(m_ui->sshPortSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MerEmulatorDeviceWidget::handleSshPortChanged);
    connect(m_ui->portsLineEdit, &QLineEdit::editingFinished,
            this, &MerEmulatorDeviceWidget::handleFreePortsChanged);
    connect(m_ui->qmlLivePortsLineEdit, &QLineEdit::editingFinished,
            this, &MerEmulatorDeviceWidget::handleQmlLivePortsChanged);

    initGui();
}

MerEmulatorDeviceWidget::~MerEmulatorDeviceWidget()
{
    delete m_ui;
    delete m_virtualMachineSettingsWidget;
}

void MerEmulatorDeviceWidget::onStoredDevicesChanged()
{
    updatePortInputsEnabledState();
}

void MerEmulatorDeviceWidget::selectFactorySnapshot()
{
    auto device = this->device().dynamicCast<MerEmulatorDevice>();
    QTC_ASSERT(device, return);

    VirtualMachineInfo info = MerVirtualBoxManager::fetchVirtualMachineInfo(device->virtualMachine(),
            MerVirtualBoxManager::SnapshotInfo);
    if (info.snapshots.isEmpty()) {
        QMessageBox::warning(this, tr("No snapshot found"),
                tr("No snapshot exists for the '%1' virtual machine.")
                .arg(device->connection()->virtualMachine()));
        return;
    }

    const bool editable = false;
    bool ok;
    QString selected = QInputDialog::getItem(this, tr("Select factory snapshot"),
            tr("Select the virtual machine snapshot to be used as the factory snapshot for '%1'")
            .arg(device->displayName()),
            info.snapshots, info.snapshots.indexOf(device->factorySnapshot()), editable, &ok);
    if (!ok)
        return;

    m_ui->factorySnapshotLineEdit->setText(selected);
    device->setFactorySnapshot(selected);
}

void MerEmulatorDeviceWidget::onVirtualMachineOffChanged(bool vmOff)
{
    updatePortInputsEnabledState();

    // If the VM is started, cancel any unsaved changes to SSH/QmlLive ports to prevent inconsistencies
    if (!vmOff) {
        auto device = this->device().dynamicCast<MerEmulatorDevice>();
        QTC_ASSERT(device, return);

        bool restored = MerEmulatorDeviceManager::restoreSystemSettings(device);
        if (restored) {
            m_ui->sshPortSpinBox->setValue(device->sshParameters().port());
            m_ui->qmlLivePortsLineEdit->setText(device->qmlLivePorts().toString());
            m_virtualMachineSettingsWidget->setMemorySizeMb(device->memorySizeMb());
            m_virtualMachineSettingsWidget->setCpuCount(device->cpuCount());
            m_virtualMachineSettingsWidget->setVdiCapacityMb(device->vdiCapacityMb());
        }
    }
}

void MerEmulatorDeviceWidget::updatePortInputsEnabledState()
{
    auto device = this->device().staticCast<MerEmulatorDevice>();
    bool vmOff = device->connection()->isVirtualMachineOff();
    bool isStored = MerEmulatorDeviceManager::isStored(device);

    QString message = tr("Stop emulator to unlock this field for editing.");
    if (!isStored)
        message = tr("Finish adding the device to unlock this field for editing.");

    m_ui->qmlLivePortsInfoLabel->setToolTip(QString("<font color=\"red\">%1</font>").arg(message));
    m_ui->sshPortInfoLabel->setToolTip(QString("<font color=\"red\">%1</font>").arg(message));

    bool canEdit = vmOff && isStored;

    m_ui->sshPortSpinBox->setEnabled(canEdit);
    m_ui->sshPortInfoLabel->setVisible(!canEdit);
    m_ui->qmlLivePortsLineEdit->setEnabled(canEdit);
    m_ui->qmlLivePortsInfoLabel->setVisible(!canEdit);
    m_virtualMachineSettingsWidget->setCanEdit(canEdit);
}

void MerEmulatorDeviceWidget::timeoutEditingFinished()
{
    Q_ASSERT(dynamic_cast<MerEmulatorDevice *>(this->device().data()) != 0);
    MerEmulatorDevice* device = static_cast<MerEmulatorDevice*>(this->device().data());

    SshConnectionParameters sshParams = device->sshParameters();
    sshParams.timeout = m_ui->timeoutSpinBox->value();
    device->setSshParameters(sshParams);
}

void MerEmulatorDeviceWidget::userNameEditingFinished()
{
    Q_ASSERT(dynamic_cast<MerEmulatorDevice *>(this->device().data()) != 0);
    MerEmulatorDevice* device = static_cast<MerEmulatorDevice*>(this->device().data());

    if(!device->sharedConfigPath().isEmpty()) {
        QString index = QLatin1String("/ssh/private_keys/%1/");
        SshConnectionParameters sshParams = device->sshParameters();
        const QString& user = m_ui->userLineEdit->text();
        //TODO fix me:
        const QString privKey = device->sharedConfigPath() +
                index.arg(device->id().toString()).replace(QLatin1Char(' '),QLatin1Char('_'))
                + user;

        sshParams.setUserName(user);
        sshParams.privateKeyFile = privKey;
        m_ui->sshKeyLabelEdit->setText(privKey);
        device->setSshParameters(sshParams);
    }
}

void MerEmulatorDeviceWidget::handleSshPortChanged()
{
    auto device = this->device().staticCast<MerDevice>();
    auto sshParams = device->sshParameters();
    sshParams.setPort(m_ui->sshPortSpinBox->value());
    device->setSshParameters(sshParams);
}

void MerEmulatorDeviceWidget::handleFreePortsChanged()
{
    device()->setFreePorts(PortList::fromString(m_ui->portsLineEdit->text()));
    updatePortsWarningLabel();
}

void MerEmulatorDeviceWidget::handleQmlLivePortsChanged()
{
    device().staticCast<MerDevice>()->setQmlLivePorts(PortList::fromString(m_ui->qmlLivePortsLineEdit->text()));
    updateQmlLivePortsWarningLabel();
}

void MerEmulatorDeviceWidget::handleMemorySizeChanged(int sizeMb)
{
    device().staticCast<MerEmulatorDevice>()->setMemorySize(sizeMb);
}

void MerEmulatorDeviceWidget::handleCpuCountChanged(int cpuCount)
{
    device().staticCast<MerEmulatorDevice>()->setCpuCount(cpuCount);
}

void MerEmulatorDeviceWidget::handleVdiSizeChanged(int sizeMb)
{
    device().staticCast<MerEmulatorDevice>()->setVdiSize(sizeMb);
}

void MerEmulatorDeviceWidget::updateDeviceFromUi()
{
    timeoutEditingFinished();
    userNameEditingFinished();
    handleFreePortsChanged();
    handleQmlLivePortsChanged();
}

void MerEmulatorDeviceWidget::updatePortsWarningLabel()
{
    m_ui->portsWarningLabel->setVisible(!device()->freePorts().hasMore());
}

void MerEmulatorDeviceWidget::updateQmlLivePortsWarningLabel()
{
    const int count = device().staticCast<MerDevice>()->qmlLivePorts().count();
    m_ui->qmlLivePortsWarningLabel->setVisible(count < 1 || count > Constants::MAX_QML_LIVE_PORTS);
}

void MerEmulatorDeviceWidget::initGui()
{
    m_ui->portsWarningLabel->setPixmap(Utils::Icons::WARNING.pixmap());
    m_ui->portsWarningLabel->setToolTip(QLatin1String("<font color=\"red\">")
                                        + tr("You will need at least two ports for debugging.")
                                        + QLatin1String("</font>"));
    m_ui->qmlLivePortsInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    m_ui->qmlLivePortsWarningLabel->setPixmap(Utils::Icons::WARNING.pixmap());
    m_ui->qmlLivePortsWarningLabel->setToolTip(
            QLatin1String("<font color=\"red\">")
            + tr("You will need at least one and at most %1 ports for QmlLive use.").arg(Constants::MAX_QML_LIVE_PORTS)
            + QLatin1String("</font>"));
    m_ui->sshPortInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    QRegExpValidator * const portsValidator
            = new QRegExpValidator(QRegExp(PortList::regularExpression()), this);
    m_ui->portsLineEdit->setValidator(portsValidator);
    m_ui->qmlLivePortsLineEdit->setValidator(portsValidator);

    Q_ASSERT(dynamic_cast<MerEmulatorDevice *>(this->device().data()) != 0);
    const MerEmulatorDevice* device = static_cast<MerEmulatorDevice*>(this->device().data());
    const SshConnectionParameters &sshParams = device->sshParameters();
    m_ui->timeoutSpinBox->setValue(sshParams.timeout);
    m_ui->userLineEdit->setText(sshParams.userName());
    if (!sshParams.privateKeyFile.isEmpty())
        m_ui->sshKeyLabelEdit->setText(QDir::toNativeSeparators(sshParams.privateKeyFile));
    else
        m_ui->sshKeyLabelEdit->setText(tr("none"));
    m_ui->sshPortSpinBox->setValue(sshParams.port());
    m_ui->portsLineEdit->setText(device->freePorts().toString());
    m_ui->qmlLivePortsLineEdit->setText(device->qmlLivePorts().toString());
    m_ui->emulatorVmLabelEdit->setText(device->virtualMachine());
    m_ui->factorySnapshotLineEdit->setText(device->factorySnapshot());
    if(!device->sharedConfigPath().isEmpty())
        m_ui->configFolderLabelEdit->setText(QDir::toNativeSeparators(device->sharedConfigPath()));
    else
        m_ui->configFolderLabelEdit->setText(tr("none"));
    if(!device->sharedSshPath().isEmpty())
        m_ui->sshFolderLabelEdit->setText(QDir::toNativeSeparators(device->sharedSshPath()));
    else
        m_ui->sshFolderLabelEdit->setText(tr("none"));

    if(!device->mac().isEmpty())
        m_ui->macLabelEdit->setText(device->mac());
    else
        m_ui->macLabelEdit->setText(tr("none"));
    //block "nemo" user
    m_ui->userLineEdit->setEnabled(false);

    connect(m_virtualMachineSettingsWidget, &MerVirtualMachineSettingsWidget::memorySizeChanged,
            this, &MerEmulatorDeviceWidget::handleMemorySizeChanged);
    connect(m_virtualMachineSettingsWidget, &MerVirtualMachineSettingsWidget::cpuCountChanged,
            this, &MerEmulatorDeviceWidget::handleCpuCountChanged);
    connect(m_virtualMachineSettingsWidget, &MerVirtualMachineSettingsWidget::vdiSizeChnaged,
            this, &MerEmulatorDeviceWidget::handleVdiSizeChanged);

    m_virtualMachineSettingsWidget->setMemorySizeMb(device->memorySizeMb());
    m_virtualMachineSettingsWidget->setCpuCount(device->cpuCount());
    m_virtualMachineSettingsWidget->setVdiInfo(device->vdiPath(), device->vdiCapacityMb(), device->vdiSizeOnDiskMb());

    for (int i = 0; i < m_virtualMachineSettingsWidget->formLayout()->rowCount(); ++i) {
        auto row = m_virtualMachineSettingsWidget->formLayout()->takeRow(0);
        m_ui->formLayout->addRow(row.labelItem->widget(), row.fieldItem->widget());
    }

    updatePortInputsEnabledState();
    updatePortsWarningLabel();
    updateQmlLivePortsWarningLabel();

    onVirtualMachineOffChanged(device->connection()->isVirtualMachineOff());
    connect(device->connection(), &MerConnection::virtualMachineOffChanged,
            this, &MerEmulatorDeviceWidget::onVirtualMachineOffChanged);

    connect(MerEmulatorDeviceManager::instance(), &MerEmulatorDeviceManager::storedDevicesChanged,
            this, &MerEmulatorDeviceWidget::onStoredDevicesChanged);
}

} // Internal
} // Mer
