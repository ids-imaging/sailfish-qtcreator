#ifndef MERVIRTUALMACHINESETTINGSWIDGET_H
#define MERVIRTUALMACHINESETTINGSWIDGET_H

#include <QWidget>
#include <QFormLayout>

namespace Mer {
namespace Internal {

namespace Ui {
class MerVirtualMachineSettingsWidget;
}

class MerVirtualMachineSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MerVirtualMachineSettingsWidget(QWidget *parent = nullptr);
    ~MerVirtualMachineSettingsWidget();
    void setMemorySizeMb(int sizeMb);
    void setCpuCount(int count);
    void setVdiInfo(const QString &vdiPath, int vdiCapacityMb, int vdiSizeOnDiskMb);
    void setVdiCapacityMb(int vdiCapacityMb);
    void setCanEdit(bool canEdit);
    QFormLayout *formLayout() const;

signals:
    void memorySizeChanged(int sizeMb);
    void cpuCountChanged(int count);
    void vdiSizeChnaged(int value);

private:
    void initGui();
    Ui::MerVirtualMachineSettingsWidget *ui;
};

} // Internal
} // Mer

#endif // MERVIRTUALMACHINESETTINGSWIDGET_H
