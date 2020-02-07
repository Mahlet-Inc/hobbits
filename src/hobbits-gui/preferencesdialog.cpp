#include "preferencesdialog.h"
#include "settingsmanager.h"
#include "ui_preferencesdialog.h"
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpacerItem>
#include <QSpinBox>

PreferencesDialog::PreferencesDialog(QSharedPointer<const PluginManager> pluginManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    m_pluginManager(pluginManager)
{
    ui->setupUi(this);
    ui->pb_apply->setDisabled(true);
    this->setWindowTitle("Hobbits Preferences");

    m_data = SettingsManager::getInstance().getAll();

    ui->lw_groups->addItem("Display");
    ui->lw_groups->addItem("Plugin Loader");
    ui->lw_groups->addItem("Plugin Settings");

    ui->lw_groups->setCurrentRow(0);
    on_lw_groups_currentRowChanged(0);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

QPixmap PreferencesDialog::getColorPixmap(QColor color)
{
    QPixmap colorPix(32, 32);
    colorPix.fill(Qt::white);
    QPainter painter(&colorPix);
    painter.setPen(Qt::transparent);
    painter.setBrush(Qt::gray);
    painter.drawRect(0, 0, 8, 8);
    painter.drawRect(0, 16, 8, 8);
    painter.drawRect(8, 8, 8, 8);
    painter.drawRect(8, 24, 8, 8);
    painter.drawRect(16, 0, 8, 8);
    painter.drawRect(16, 16, 8, 8);
    painter.drawRect(24, 8, 8, 8);
    painter.drawRect(24, 24, 8, 8);
    painter.setBrush(color);
    painter.drawRect(0, 0, 32, 32);

    return colorPix;
}

QLayoutItem* PreferencesDialog::createEditor(
        std::function<QVariant()> getter,
        std::function<void(QVariant)> setter,
        QWidget *parent)
{
    QVariant value = getter();
    if (value.type() == QVariant::String) {
        QLineEdit *editor = new QLineEdit(value.toString(), parent);
        connect(
                editor,
                &QLineEdit::textChanged,
                [setter](QString text) {
            setter(QVariant(text));
        });
        return new QWidgetItem(editor);
    }
    else if (value.type() == QVariant::StringList) {
        QLineEdit *editor = new QLineEdit(value.toStringList().join(","), parent);
        connect(
                editor,
                &QLineEdit::textChanged,
                [setter](QString text) {
            setter(QVariant(text.split(",")));
        });
        return new QWidgetItem(editor);
    }
    else if (value.type() == QVariant::Int) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setRange(INT_MIN, INT_MAX);
        editor->setValue(value.toInt());
        connect(
                editor,
                QOverload<int>::of(&QSpinBox::valueChanged),
                [setter](int newVal) {
            setter(QVariant(newVal));
        });
        return new QWidgetItem(editor);
    }
    else if (value.type() == QVariant::Double) {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setValue(value.toInt());
        connect(
                editor,
                QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [setter](double newVal) {
            setter(QVariant(newVal));
        });
        return new QWidgetItem(editor);
    }
    else if (int(value.type()) == QMetaType::QColor) {
        QPushButton *dialogLauncher = new QPushButton("Select Color...", parent);
        QLabel *colorChip = new QLabel(parent);
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setContentsMargins(16, 0, 0, 0);
        layout->setSpacing(8);
        layout->addWidget(colorChip);
        layout->addWidget(dialogLauncher);
        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        QColor color = value.value<QColor>();
        colorChip->setPixmap(getColorPixmap(color));
        connect(
                dialogLauncher,
                &QPushButton::clicked,
                [getter, setter, parent, colorChip]() {
            QColor selectedColor = QColorDialog::getColor(
                    getter().value<QColor>(),
                    parent,
                    "Choose Color",
                    QColorDialog::ShowAlphaChannel);
            if (selectedColor.isValid()) {
                setter(QVariant(selectedColor));
                colorChip->setPixmap(getColorPixmap(selectedColor));
            }
        });

        return layout;
    }
    else {
        QLineEdit *readOnly = new QLineEdit(value.toString(), parent);
        readOnly->setReadOnly(true);

        return new QWidgetItem(readOnly);
    }
}

QWidget* PreferencesDialog::createUiPage()
{
    QWidget *uiSettingsWidget = new QWidget(this);
    QFormLayout *uiSettingsLayout = new QFormLayout();

    for (QString key : m_data.getUiSettingKeys()) {
        QVariant value = m_data.getUiSetting(key);
        QLayoutItem *editor = createEditor(
                [this, key]() {
            return m_data.getUiSetting(key);
        },
                [this, key](QVariant newValue) {
            m_data.setUiSetting(key, newValue);
            ui->pb_apply->setDisabled(false);
        },
                uiSettingsWidget);
        QHBoxLayout *editorLayout = new QHBoxLayout();
        editorLayout->addItem(editor);
        uiSettingsLayout->addRow(key, editorLayout);
    }

    uiSettingsWidget->setLayout(uiSettingsLayout);

    return uiSettingsWidget;
}

QWidget* PreferencesDialog::createPluginPage()
{
    QWidget *uiSettingsWidget = new QWidget(this);
    QFormLayout *uiSettingsLayout = new QFormLayout();

    for (QString key : m_data.getPluginSettingKeys()) {
        QVariant value = m_data.getPluginSetting(key);
        QLayoutItem *editor = createEditor(
                [this, key]() {
            return m_data.getPluginSetting(key);
        },
                [this, key](QVariant newValue) {
            m_data.setPluginSetting(key, newValue);
            ui->pb_apply->setDisabled(false);
        },
                uiSettingsWidget);
        QHBoxLayout *editorLayout = new QHBoxLayout();
        editorLayout->addItem(editor);
        uiSettingsLayout->addRow(key, editorLayout);
    }

    uiSettingsWidget->setLayout(uiSettingsLayout);

    return uiSettingsWidget;
}

QWidget* PreferencesDialog::createPluginLoaderPage()
{
    QWidget *uiSettingsWidget = new QWidget(this);
    QFormLayout *uiSettingsLayout = new QFormLayout();

    for (QString key : m_data.getPluginLoaderSettingKeys()) {
        QVariant value = m_data.getPluginLoaderSetting(key);
        QLayoutItem *editor = createEditor(
                [this, key]() {
            return m_data.getPluginLoaderSetting(key);
        },
                [this, key](QVariant newValue) {
            m_data.setPluginLoaderSetting(key, newValue);
            ui->pb_apply->setDisabled(false);
        },
                uiSettingsWidget);
        QHBoxLayout *editorLayout = new QHBoxLayout();
        editorLayout->addItem(editor);
        uiSettingsLayout->addRow(key, editorLayout);
    }

    uiSettingsLayout->addRow("Loaded plugins:", new QLabel());
    for (auto plugin : m_pluginManager->getAllDisplays()) {
        QLineEdit *path = new QLineEdit(m_pluginManager->getPluginLocation(plugin->getName()));
        path->setReadOnly(true);
        path->setDisabled(true);
        uiSettingsLayout->addRow(plugin->getName(), path);
    }
    for (auto plugin : m_pluginManager->getAllImporterExporters()) {
        QLineEdit *path = new QLineEdit(m_pluginManager->getPluginLocation(plugin->getName()));
        path->setReadOnly(true);
        path->setDisabled(true);
        uiSettingsLayout->addRow(plugin->getName(), path);
    }
    for (auto plugin : m_pluginManager->getAllAnalyzers()) {
        QLineEdit *path = new QLineEdit(m_pluginManager->getPluginLocation(plugin->getName()));
        path->setReadOnly(true);
        path->setDisabled(true);
        uiSettingsLayout->addRow(plugin->getName(), path);
    }
    for (auto plugin : m_pluginManager->getAllOperators()) {
        QLineEdit *path = new QLineEdit(m_pluginManager->getPluginLocation(plugin->getName()));
        path->setReadOnly(true);
        path->setDisabled(true);
        uiSettingsLayout->addRow(plugin->getName(), path);
    }

    uiSettingsWidget->setLayout(uiSettingsLayout);

    return uiSettingsWidget;
}

void PreferencesDialog::on_pb_apply_clicked()
{
    SettingsManager::getInstance().setAll(m_data);
    ui->pb_apply->setDisabled(true);
}

void PreferencesDialog::on_pb_cancel_clicked()
{
    this->reject();
}

void PreferencesDialog::on_pb_ok_clicked()
{
    this->on_pb_apply_clicked();
    this->accept();
}

void PreferencesDialog::on_lw_groups_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow)
    if (ui->lw_groups->currentItem()->text() == "Display") {
        ui->scrollArea->setWidget(createUiPage());
    }
    else if (ui->lw_groups->currentItem()->text() == "Plugin Loader") {
        ui->scrollArea->setWidget(createPluginLoaderPage());
    }
    else if (ui->lw_groups->currentItem()->text() == "Plugin Settings") {
        ui->scrollArea->setWidget(createPluginPage());
    }
    else {
    }
}
