#include "mervirtualmachinesettingswidget.h"
#include "ui_mervirtualmachinesettingswidget.h"
#include "mervirtualboxmanager.h"

namespace Mer {
namespace Internal {

namespace {
const int DEFAULT_MAX_MEMORY_SIZE = 3584;

} // namespace anonymous

MerVirtualMachineSettingsWidget::MerVirtualMachineSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MerVirtualMachineSettingsWidget)
{
    ui->setupUi(this);
    initGui();
}

MerVirtualMachineSettingsWidget::~MerVirtualMachineSettingsWidget()
{
    delete ui;
}

void MerVirtualMachineSettingsWidget::setMemorySizeMb(int sizeMb)
{
    if (ui->memorySpinBox->maximum() < sizeMb)
        ui->memorySpinBox->setRange(4, sizeMb);

    ui->memorySpinBox->setValue(sizeMb);
}

void MerVirtualMachineSettingsWidget::setCpuCount(int count)
{
    ui->cpuCountSpinBox->setValue(count);
}

void MerVirtualMachineSettingsWidget::setVdiInfo(const QString &vdiPath, int vdiCapacityMb, int vdiSizeOnDiskMb)
{
    int maxStorageSizeMb = MerVirtualBoxManager::getHostAvailableStorageMb(vdiPath);
    // the minimum size is the current size because VBoxManager can't reduce storage size
    ui->vdiCapacityGbSpinBox->setRange(vdiCapacityMb / 1024.0, maxStorageSizeMb / 1024.0);

    ui->vdiCapacityGbSpinBox->setValue(vdiCapacityMb / 1024.0);
    ui->vdiPathLabelEdit->setText(vdiPath);
    ui->vdiSizeOnDiskValueLabel->setText(QString("%1 Gb").arg(QString::number(vdiSizeOnDiskMb / 1024.0, 'f', 3)));
}

void MerVirtualMachineSettingsWidget::setVdiCapacityMb(int vdiCapacityMb)
{
    ui->vdiCapacityGbSpinBox->setValue(vdiCapacityMb / 1024.0);
}

void MerVirtualMachineSettingsWidget::setCanEdit(bool canEdit)
{
    ui->memorySpinBox->setEnabled(canEdit);
    ui->cpuCountSpinBox->setEnabled(canEdit);
    ui->vdiCapacityGbSpinBox->setEnabled(canEdit);
}

QFormLayout *MerVirtualMachineSettingsWidget::formLayout() const
{
    return ui->formLayout;
}

void MerVirtualMachineSettingsWidget::initGui()
{
    auto handler = [this](int sizeMb) {
        int maxMemorySizeMb = ui->memorySpinBox->value() > sizeMb ? ui->memorySpinBox->value() : sizeMb;
        ui->memorySpinBox->setRange(4, maxMemorySizeMb);
    };

    ui->memorySpinBox->setRange(4, DEFAULT_MAX_MEMORY_SIZE);
    MerVirtualBoxManager::getHostTotalMemorySizeMb(this, handler);

    const int maxCpuCount = MerVirtualBoxManager::getHostTotalCpuCount();
    ui->cpuCountSpinBox->setRange(1, maxCpuCount);

    connect(ui->memorySpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(memorySizeChanged(int)));
    connect(ui->cpuCountSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(cpuCountChanged(int)));
    connect(ui->vdiCapacityGbSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit vdiSizeChnaged(static_cast<int>(value * 1024));
    });
}

} // Internal
} // Mer
