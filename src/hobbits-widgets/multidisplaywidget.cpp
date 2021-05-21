#include "multidisplaywidget.h"
#include "settingsmanager.h"
#include <QVBoxLayout>

MultiDisplayWidget::MultiDisplayWidget(QSharedPointer<HobbitsPluginManager> pluginManager,
                                        QSharedPointer<DisplayHandle> handle,
                                        QWidget *parent) :
    m_handle(handle),
    QWidget(parent)
{
    m_tabs = new QTabWidget();
    m_tabs->setElideMode(Qt::ElideNone);
    m_tabs->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_splitter = new QSplitter(Qt::Vertical);
    m_splitter->addWidget(m_tabs);
    m_splitter->setStretchFactor(0, 15);

    QVBoxLayout *vBox = new QVBoxLayout();
    this->setLayout(vBox);
    vBox->setContentsMargins(0, 0, 0, 0);
    vBox->addWidget(m_splitter);

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
        m_tabs->widget(idx)->setWhatsThis(QString("<b>%1:</b> %2").arg(displayPlugin->name()).arg(displayPlugin->description()));
        m_displayMap.insert(idx, parts);
    }
    m_tabs->setUpdatesEnabled(true);

    connect(m_tabs, &QTabWidget::currentChanged, [this](int) {
        this->activateCurrentDisplay();
    });
}

bool MultiDisplayWidget::setActiveDisplay(QString name) 
{
    for (int idx : m_displayMap.keys()) {
        if (m_displayMap.value(idx)->interface->name() == name) {
            m_tabs->setCurrentIndex(idx);
            return true;
        }
    }

    return false;
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

void MultiDisplayWidget::activateCurrentDisplay() 
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());

    auto activeDisplays = m_handle->activeDisplays();
    for (auto p : m_displayMap.values()) {
        activeDisplays.remove(p->display);
    }

    if (m_splitter->count() > 1) {
        auto oldEditor = m_splitter->widget(1);
        oldEditor->setParent(nullptr);
        oldEditor->deleteLater();
    }

    if (parts.isNull()) {
        m_handle->setActiveDisplays(activeDisplays);
        return;
    }

    auto editor = parts->createEditor();
    if (editor != nullptr) {
        m_splitter->addWidget(editor);
    }
    
    activeDisplays.insert(parts->display);
    m_handle->setActiveDisplays(activeDisplays);
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

AbstractParameterEditor* MultiDisplayWidget::DisplayParts::createEditor() 
{
    auto editor = this->interface->parameterDelegate()->createEditor();
    if (editor == nullptr) {
        return nullptr;
    }

    if (this->display->displayParameters().isNull()) {
        this->display->setDisplayParameters(editor->parameters());
    }
    else {
        editor->setParameters(this->display->displayParameters());
    }
    connect(editor, &AbstractParameterEditor::changed, this->display, [this, editor]() {
        this->display->setDisplayParameters(editor->parameters());
    });
    connect(this->display, &DisplayWidget::hasNewContainer, editor, [this, editor]() {
        auto preview = BitContainerPreview::wrap(this->display->handle()->currentContainer());
        editor->previewBits(preview, QSharedPointer<PluginActionProgress>());
        this->display->setDisplayParameters(editor->parameters());
    });

    return editor;
}
