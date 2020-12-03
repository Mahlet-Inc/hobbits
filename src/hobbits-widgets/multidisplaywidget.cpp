#include "multidisplaywidget.h"
#include "settingsmanager.h"
#include <QVBoxLayout>

MultiDisplayWidget::MultiDisplayWidget(QSharedPointer<HobbitsPluginManager> pluginManager,
                                       QSharedPointer<DisplayHandle> handle,
                                       QWidget *parent) : QWidget(parent)
{
    m_tabs = new QTabWidget(this);
    m_tabs->setElideMode(Qt::ElideNone);
    QVBoxLayout *vBox = new QVBoxLayout();
    this->setLayout(vBox);
    vBox->setContentsMargins(0, 0, 0, 0);
    vBox->addWidget(m_tabs);

    // Instantiate displays for this display set
    QList<QSharedPointer<DisplayInterface>> displays;
    QSet<QString> alreadyAdded;
    for (QString pluginString : SettingsManager::getPluginLoaderSetting(
            SettingsManager::DISPLAY_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<DisplayInterface> plugin = pluginManager->getDisplay(pluginString.trimmed());
        if (!plugin.isNull()) {
            displays.append(QSharedPointer<DisplayInterface>(plugin->createDefaultDisplay()));
            alreadyAdded.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<DisplayInterface> plugin : pluginManager->displays()) {
        if (!alreadyAdded.contains(plugin->name())) {
            displays.append(QSharedPointer<DisplayInterface>(plugin->createDefaultDisplay()));
        }
    }

    // Add the widgets to the tabs
    m_tabs->setUpdatesEnabled(false);
    for (QSharedPointer<DisplayInterface> displayPlugin : displays) {
        auto parts = DisplayParts::create(displayPlugin, handle);
        int idx = m_tabs->addTab(parts->display, displayPlugin->name());
        m_displayMap.insert(idx, parts);
    }
    m_tabs->setUpdatesEnabled(true);

    connect(m_tabs, &QTabWidget::currentChanged, [this](int) {
        this->activeDisplayChanged(activeDisplay());
    });
}

QSharedPointer<DisplayInterface> MultiDisplayWidget::activeDisplay()
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());
    if (parts.isNull()) {
        return QSharedPointer<DisplayInterface>();
    }
    return parts->interface;
}

DisplayWidget* MultiDisplayWidget::activeDisplayWidget()
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());
    if (parts.isNull()) {
        return nullptr;
    }
    return parts->display;
}

AbstractParameterEditor* MultiDisplayWidget::createEditorForActiveDisplay()
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());
    if (parts.isNull()) {
        return nullptr;
    }

    auto editor = parts->interface->parameterDelegate()->createEditor();
    if (editor == nullptr) {
        return nullptr;
    }

    if (parts->display->displayParameters().isEmpty()) {
        parts->display->setDisplayParameters(editor->parameters());
    }
    else {
        editor->setParameters(parts->display->displayParameters());
    }
    connect(editor, &AbstractParameterEditor::changed, parts->display, [parts, editor]() {
        parts->display->setDisplayParameters(editor->parameters());
    });
    connect(parts->display, &DisplayWidget::aboutToRedraw, editor, [parts, editor]() {
        auto preview = BitContainerPreview::wrap(parts->display->handle()->currentContainer());
        editor->previewBits(preview, QSharedPointer<PluginActionProgress>());
        parts->display->setDisplayParameters(editor->parameters());
    });

    return editor;
}

QSharedPointer<MultiDisplayWidget::DisplayParts> MultiDisplayWidget::DisplayParts::create(
        QSharedPointer<DisplayInterface> interface,
        QSharedPointer<DisplayHandle> handle)
{
    QSharedPointer<DisplayParts> parts(new DisplayParts());
    interface->setDisplayHandle(handle);
    parts->interface = interface;
    parts->display = new DisplayWidget(interface, handle);

    return parts;
}

MultiDisplayWidget::DisplayParts::~DisplayParts()
{
    display->deleteLater();
}
