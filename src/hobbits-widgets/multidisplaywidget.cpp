#include "multidisplaywidget.h"
#include "settingsmanager.h"
#include <QVBoxLayout>
#include <QTabBar>

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

MultiDisplayWidget::~MultiDisplayWidget() 
{
    auto activeDisplays = m_handle->activeDisplays();
    for (auto p : m_displayMap.values()) {
        activeDisplays.remove(p->display);
    }
    m_handle->setActiveDisplays(activeDisplays);
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

QSharedPointer<DisplayInterface> MultiDisplayWidget::activeDisplay() const
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());
    if (parts.isNull()) {
        return QSharedPointer<DisplayInterface>();
    }
    return parts->interface;
}

DisplayWidget* MultiDisplayWidget::activeDisplayWidget() const
{
    auto parts = m_displayMap.value(m_tabs->currentIndex());
    if (parts.isNull()) {
        return nullptr;
    }
    return parts->display;
}

QByteArray MultiDisplayWidget::saveState() const
{
    QByteArray config;
    QDataStream stream(&config, QIODevice::WriteOnly);

    QByteArray activeName = activeDisplay()->name().toLatin1();
    stream.writeBytes(activeName.data(), activeName.size());

    QByteArray splitterState = m_splitter->saveState();
    stream.writeBytes(splitterState.data(), splitterState.size());

    auto params = activeDisplayWidget()->displayParameters();
    bool hasParams = !params.isNull();
    stream << hasParams;
    if (hasParams) {
        QJsonDocument jsonParams(params.values());
        stream << jsonParams.toJson(QJsonDocument::Compact);
    }

    return config;
}

QByteArray MultiDisplayWidget::readStreamBytes(QDataStream &stream) {
    char *readBuf;
    uint len;
    stream.readBytes(readBuf, len);
    if (len < 1) {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return QByteArray();
    }
    QByteArray bytes(readBuf, len);
    delete [] readBuf;
    return bytes;
}

bool MultiDisplayWidget::restoreState(QByteArray config) 
{
    QDataStream stream(config);

    QByteArray activeDisplayBytes = readStreamBytes(stream);
    if (activeDisplayBytes.isEmpty()) {
        return false;
    }
    if (!setActiveDisplay(QString::fromLatin1(activeDisplayBytes))) {
        // failed to load this display, but stream is still ok
        return true;
    }

    QByteArray splitterState = readStreamBytes(stream);
    if (splitterState.isEmpty()) {
        return false;
    }
    if (!m_splitter->restoreState(splitterState)) {
        return false;
    }

    bool hasParams;
    stream >> hasParams;
    if (hasParams) {
        QByteArray paramBytes = readStreamBytes(stream);
        if (paramBytes.isEmpty()) {
            return false;
        }
        auto jsonParams = QJsonDocument::fromJson(paramBytes);
        Parameters params(jsonParams.object());

        if (m_splitter->count() > 1) {
            auto editor = qobject_cast<AbstractParameterEditor*>(m_splitter->widget(1));
            editor->setParameters(params);
        }
        else {
            activeDisplayWidget()->setDisplayParameters(params);
        }
    }

    return true;
}

void MultiDisplayWidget::setShowViewSelect(bool show) 
{
    if (show) {
        m_tabs->tabBar()->show();
    }
    else {
        m_tabs->tabBar()->hide();
    }
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
    editor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

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
