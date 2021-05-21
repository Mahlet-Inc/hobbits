#include "displaysplitter.h"
#include <QMouseEvent>

DisplaySplitter::DisplaySplitter(QSharedPointer<HobbitsPluginManager> pluginManager,
                    QSharedPointer<DisplayHandle> handle,
                    MultiDisplayWidget* nonSplitWidget,
                    QWidget *parent) :
    m_mousePressing(false),
    m_pluginManager(pluginManager),
    m_handle(handle),
    m_nonSplitWidget(nonSplitWidget),
    m_vBox(new QVBoxLayout()),
    m_splitter(nullptr),
    QWidget(parent)
{
    this->setLayout(m_vBox);
    m_vBox->setContentsMargins(0, 0, 0, 0);

    if (m_nonSplitWidget == nullptr) {
        m_nonSplitWidget = new MultiDisplayWidget(m_pluginManager, m_handle);
        m_nonSplitWidget->activateCurrentDisplay();
    }
    m_vBox->addWidget(m_nonSplitWidget);
}

bool DisplaySplitter::isSplit() const
{
    return m_splitter != nullptr;
}

MultiDisplayWidget* DisplaySplitter::takeNonSplitWidget()
{
    if (isSplit()) {
        return nullptr;
    }

    m_nonSplitWidget->setParent(nullptr);
    m_vBox->takeAt(0);
    auto nonSplitWidget = m_nonSplitWidget;
    m_nonSplitWidget = nullptr;

    return nonSplitWidget;
}

QPair<DisplaySplitter*, DisplaySplitter*> DisplaySplitter::takeSplitWidgets()
{
    if (!isSplit()) {
        return {nullptr, nullptr};
    }
    
    m_vBox->takeAt(0);

    QPair<DisplaySplitter*, DisplaySplitter*> pair;
    pair.first = qobject_cast<DisplaySplitter*>(m_splitter->widget(0));
    pair.second = qobject_cast<DisplaySplitter*>(m_splitter->widget(1));

    m_splitter->hide();
    pair.first->setParent(nullptr);
    pair.second->setParent(nullptr);

    m_splitter->deleteLater();
    m_splitter = nullptr;

    return pair;
}

Qt::Orientation DisplaySplitter::splitOrientation() const
{
    if (!isSplit()) {
        return Qt::Orientation::Vertical;
    }

    return m_splitter->orientation();
}

void DisplaySplitter::split(Qt::Orientation orientation) 
{
    if (isSplit()) {
        return;
    }

    auto splitOne = new DisplaySplitter(m_pluginManager, m_handle, takeNonSplitWidget());
    auto splitTwo = new DisplaySplitter(m_pluginManager, m_handle);
    m_splitter = new QSplitter(orientation);
    m_splitter->addWidget(splitOne);
    m_splitter->addWidget(splitTwo);
    m_vBox->addWidget(m_splitter);
}

void DisplaySplitter::unSplit(bool keepSecond) 
{
    if (!isSplit()) {
        return;
    }

    auto pair = takeSplitWidgets();
    DisplaySplitter* kept;
    if (keepSecond) {
        kept = pair.second;
    }
    else {
        kept = pair.first;
    }

    if (kept->isSplit()) {
        auto orientation = kept->splitOrientation();
        auto keptPair = kept->takeSplitWidgets();
        m_splitter = new QSplitter(orientation);
        m_splitter->addWidget(keptPair.first);
        m_splitter->addWidget(keptPair.second);
        m_vBox->addWidget(m_splitter);
    }
    else {
        m_nonSplitWidget = kept->takeNonSplitWidget();
        m_vBox->addWidget(m_nonSplitWidget);
    }

    pair.first->deleteLater();
    pair.second->deleteLater();
}

QByteArray DisplaySplitter::getConfig() const
{
    QByteArray config;
    QDataStream stream(&config, QIODevice::WriteOnly);
    stream << isSplit();
    if (isSplit()) {
        QByteArray splitterState = m_splitter->saveState();
        stream.writeBytes(splitterState.data(), splitterState.size());
        
        QByteArray side1 = qobject_cast<DisplaySplitter*>(m_splitter->widget(0))->getConfig();
        stream.writeBytes(side1.data(), side1.size());

        QByteArray side2 = qobject_cast<DisplaySplitter*>(m_splitter->widget(1))->getConfig();
        stream.writeBytes(side2.data(), side2.size());
    }
    else {
        QByteArray name = m_nonSplitWidget->activeDisplay()->name().toLatin1();
        stream.writeBytes(name.data(), name.size());
    }

    return config;
}

QByteArray readStreamBytes(QDataStream &stream) {
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

bool DisplaySplitter::applyConfig(QByteArray config) 
{
    QDataStream stream(config);
    bool configSplit;
    stream >> configSplit;
    if (configSplit) {
        split(Qt::Vertical);

        QByteArray splitterState = readStreamBytes(stream);
        if (splitterState.isEmpty()) {
            return false;
        }
        if (!m_splitter->restoreState(splitterState)) {
            return false;
        }

        QByteArray side1 = readStreamBytes(stream);
        if (side1.isEmpty()) {
            return false;
        }
        if (!qobject_cast<DisplaySplitter*>(m_splitter->widget(0))->applyConfig(side1)) {
            return false;
        }

        QByteArray side2 = readStreamBytes(stream);
        if (side2.isEmpty()) {
            return false;
        }
        if (!qobject_cast<DisplaySplitter*>(m_splitter->widget(1))->applyConfig(side2)) {
            return false;
        }
    }
    else {
        unSplit();
        QByteArray nameBytes = readStreamBytes(stream);
        if (nameBytes.isEmpty()) {
            return false;
        }
        QString name = QString::fromLatin1(nameBytes);
        m_nonSplitWidget->setActiveDisplay(name);
    }

    return true;
}

void DisplaySplitter::leaveEvent(QEvent *event) 
{
    if (m_mousePressing) {
        m_mousePressing = false;
        event->accept();
    }
    else {
        event->ignore();
    }
}

void DisplaySplitter::mouseDoubleClickEvent(QMouseEvent *event) 
{
    if (m_mousePressing) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void DisplaySplitter::mouseMoveEvent(QMouseEvent *event) 
{
    if (m_mousePressing) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void DisplaySplitter::mousePressEvent(QMouseEvent *event) 
{
    if (event->modifiers() & Qt::AltModifier) {
        if (event->modifiers() & Qt::ShiftModifier) {
            if (isSplit()) {
                m_mousePressing = true;
                event->accept();
                unSplit();
            }
            else {
                event->ignore();
            }
            return;
        }

        m_mousePressing = true;
        event->accept();

        if (event->modifiers() & Qt::ControlModifier) {
            split(Qt::Vertical);
        }
        else {
            split(Qt::Horizontal);
        }
    }
    else {
        event->ignore();
    }
}

void DisplaySplitter::mouseReleaseEvent(QMouseEvent *event) 
{
    if (m_mousePressing) {
        m_mousePressing = false;
        event->accept();
    }
    else {
        event->ignore();
    }
}
