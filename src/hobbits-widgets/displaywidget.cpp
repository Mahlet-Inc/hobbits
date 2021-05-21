#include "displaywidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QtConcurrent/QtConcurrent>
#include <QMenu>
#include <QInputDialog>
#include <QLineEdit>
#include "settingsmanager.h"

DisplayWidget::DisplayWidget(QSharedPointer<DisplayInterface> display,
                             QSharedPointer<DisplayHandle> handle,
                             QWidget *parent) :
    QWidget(parent),
    m_display(display),
    m_handle(handle),
    m_displayParameters(Parameters::nullParameters()),
    m_repaintScheduled(false)
{
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setMouseTracking(true);

    connect(m_handle.data(), SIGNAL(containerChanged()), this, SLOT(checkFullRedraw()), Qt::QueuedConnection);
    connect(m_handle.data(), SIGNAL(currentContainerChanged()), this, SLOT(checkNewContainer()));

    connect(m_handle.data(), SIGNAL(overlayRedrawRequested(DisplayInterface*)), this, SLOT(checkOverlayRedraw(DisplayInterface*)));
    connect(m_handle.data(), SIGNAL(fullRedrawRequested(DisplayInterface*)), this, SLOT(checkFullRedraw(DisplayInterface*)));

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
            this,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this,
            SLOT(showContextMenu(const QPoint&)));

    // Use the display render config to set up more redraw triggers
    if (m_display->renderConfig()->fullRedrawTriggers() & DisplayRenderConfig::NewBitHover) {
        connect(m_handle.data(), SIGNAL(newBitHover(qint64, qint64)), this, SLOT(checkFullRedraw()));
    }
    else if (m_display->renderConfig()->overlayRedrawTriggers() & DisplayRenderConfig::NewBitHover) {
        connect(m_handle.data(), SIGNAL(newBitHover(qint64, qint64)), this, SLOT(checkOverlayRedraw()));
    }

    if (m_display->renderConfig()->fullRedrawTriggers() & DisplayRenderConfig::NewBitOffset) {
        connect(m_handle.data(), SIGNAL(newBitOffset(qint64)), this, SLOT(checkFullRedraw()));
    }
    else if (m_display->renderConfig()->overlayRedrawTriggers() & DisplayRenderConfig::NewBitOffset) {
        connect(m_handle.data(), SIGNAL(newBitOffset(qint64)), this, SLOT(checkOverlayRedraw()));
    }

    if (m_display->renderConfig()->fullRedrawTriggers() & DisplayRenderConfig::NewFrameOffset) {
        connect(m_handle.data(), SIGNAL(newFrameOffset(qint64)), this, SLOT(checkFullRedraw()));
    }
    else if (m_display->renderConfig()->overlayRedrawTriggers() & DisplayRenderConfig::NewFrameOffset) {
        connect(m_handle.data(), SIGNAL(newFrameOffset(qint64)), this, SLOT(checkOverlayRedraw()));
    }

    if (m_display->renderConfig()->fullRedrawTriggers() & DisplayRenderConfig::NewMouseHover) {
        connect(m_handle.data(), &DisplayHandle::newMouseHover, this, [this](DisplayInterface* display, QPoint) {
            checkFullRedraw(display);
        });
    }
    else if (m_display->renderConfig()->overlayRedrawTriggers() & DisplayRenderConfig::NewMouseHover) {
        connect(m_handle.data(), &DisplayHandle::newMouseHover, this, [this](DisplayInterface* display, QPoint) {
            checkOverlayRedraw(display);
        });
    }
}

DisplayWidget::~DisplayWidget()
{
    resetRendering();
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    if (!m_displayResult->errorString().isEmpty()) {
        drawError(&painter, m_displayResult->errorString());
    }
    else if (!m_displayResult->getImage().isNull()) {
        painter.drawImage(0, 0, m_displayResult->getImage());
    }

    QSharedPointer<DisplayResult> overlay = m_display->renderOverlay(this->size(), m_displayParameters);
    if (!overlay->errorString().isEmpty()) {
        drawError(&painter, overlay->errorString());
    }
    else if (!overlay->getImage().isNull()) {
        painter.drawImage(0, 0, overlay->getImage());
    }

    m_repaintScheduled = false;
}

void DisplayWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        m_handle->setOffsets(m_handle->bitOffset(), m_handle->frameOffset() - 1);
    }
    else if (event->angleDelta().y() < 0) {
        m_handle->setOffsets(m_handle->bitOffset(), m_handle->frameOffset() + 1);
    }

    if (event->angleDelta().x() > 0) {
        m_handle->setOffsets(m_handle->bitOffset() - 1, m_handle->frameOffset());
    }
    else if (event->angleDelta().x() < 0) {
        m_handle->setOffsets(m_handle->bitOffset() + 1, m_handle->frameOffset());
    }
}

void DisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_handle->setMouseHover(m_display.data(), event->pos());
}

void DisplayWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_handle->setMouseHover(m_display.data(), QPoint());
}

void DisplayWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    fullRedraw();
}

QSharedPointer<DisplayResult> DisplayWidget::render(QSharedPointer<DisplayInterface> display,
                             QSize viewportSize,
                             const Parameters &parameters,
                             QSharedPointer<PluginActionProgress> progress)
{
    return display->renderDisplay(viewportSize, parameters, progress);
}

Parameters DisplayWidget::displayParameters() const
{
    return m_displayParameters;
}

void DisplayWidget::setDisplayParameters(const Parameters &displayParameters)
{
    m_displayParameters = displayParameters;
    fullRedraw();
}

void DisplayWidget::performDisplayRender()
{
    resetRendering();

    if (m_display->renderConfig()->asynchronous()) {
        m_displayRenderProgress = QSharedPointer<PluginActionProgress>(new PluginActionProgress());
        connect(m_displayRenderProgress.data(), SIGNAL(progressUpdate(QString, QVariant)), this, SLOT(handleDisplayRenderPreview(QString, QVariant)), Qt::QueuedConnection);

        auto future = QtConcurrent::run(&DisplayWidget::render,
                                        m_display,
                                        this->size(),
                                        m_displayParameters,
                                        m_displayRenderProgress);
        
        m_displayRenderWatcher = QSharedPointer<QFutureWatcher<QSharedPointer<DisplayResult>>>(new QFutureWatcher<QSharedPointer<DisplayResult>>());

        connect(m_displayRenderWatcher.data(), &QFutureWatcher<QImage>::finished, this, [this]() {
            if (this->m_displayRenderWatcher->isFinished()) {
                this->setDisplayResult(this->m_displayRenderWatcher->result());
            }
        });

        m_displayRenderWatcher->setFuture(future);
    }
    else {
        m_displayResult = m_display->renderDisplay(this->size(), m_displayParameters, QSharedPointer<PluginActionProgress>());
    }
}

void DisplayWidget::handleDisplayRenderPreview(QString type, QVariant value)
{
    if (type != "image_preview") {
        return;
    }
    this->setDisplayResult(DisplayResult::result(value.value<QImage>(), m_displayParameters));
}

void DisplayWidget::setDisplayResult(QSharedPointer<DisplayResult> result)
{
    QMutexLocker lock(&m_mutex);
    m_displayResult = result;
    scheduleRepaint();
}

void DisplayWidget::resetRendering()
{
    m_displayResult = DisplayResult::nullResult();
    if (!m_displayRenderProgress.isNull()) {
        m_displayRenderProgress->setCancelled(true);
        disconnect(m_displayRenderProgress.data(), SIGNAL(progressUpdate(QString, QVariant)), this, SLOT(handleDisplayRenderPreview(QString, QVariant)));
    }
    if (!m_displayRenderWatcher.isNull() && m_displayRenderWatcher->isRunning()) {
        m_displayRenderWatcher->cancel();
    }
}

void DisplayWidget::fullRedraw()
{
    performDisplayRender();
    scheduleRepaint();
}

void DisplayWidget::showContextMenu(const QPoint &point)
{
    if (m_handle->bitOffsetHover() < 0
            || m_handle->frameOffsetHover() < 0
            || m_handle->currentContainer().isNull()
            || m_handle->frameOffsetHover() >= m_handle->currentContainer()->frameCount()
            || m_handle->bitOffsetHover() >= m_handle->currentContainer()->frameAt(m_handle->frameOffsetHover()).size()) {
        return;
    }

    QMenu menu(this);

    qint64 bitOffsetHover = m_handle->bitOffsetHover();
    qint64 frameOffsetHover = m_handle->frameOffsetHover();
    Frame frame = m_handle->currentContainer()->frameAt(frameOffsetHover);

    auto labelAction = menu.addAction(
            QString("Frame %1, Bit %2 of %3")
            .arg(m_handle->frameOffsetHover())
            .arg(m_handle->bitOffsetHover())
            .arg(frame.size()));
    labelAction->setEnabled(false);

    QMenu gotoMenu("Go to Location");
    gotoMenu.addAction(
            tr("End of Frame"),
            [this, frame]() {
        this->m_handle->setBitOffset(frame.size() - 8);
    });
    gotoMenu.addAction(
            tr("Start of frame"),
            [this]() {
        this->m_handle->setBitOffset(0);
    });
    gotoMenu.addAction(
            tr("Next Bit Change in Frame"),
            [this, frame, bitOffsetHover]() {
        bool value = frame.at(bitOffsetHover);
        for (qint64 i = bitOffsetHover + 1; i < frame.size(); i++) {
            if (frame.at(i) != value) {
                this->m_handle->setBitOffset(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Bit Change in Frame"),
            [this, frame, bitOffsetHover]() {
        bool value = frame.at(bitOffsetHover);
        for (qint64 i = bitOffsetHover - 1; i >= 0; i--) {
            if (frame.at(i) != value) {
                this->m_handle->setBitOffset(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Next Bit Change in Column"),
            [this, frame, bitOffsetHover, frameOffsetHover]() {
        bool value = frame.at(bitOffsetHover);
        for (qint64 i = frameOffsetHover + 1; i < m_handle->currentContainer()->frameCount(); i++) {
            if (m_handle->currentContainer()->frameAt(i).at(bitOffsetHover) != value) {
                this->m_handle->setFrameOffset(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Bit Change in Column"),
            [this, frame, bitOffsetHover, frameOffsetHover]() {
        bool value = frame.at(bitOffsetHover);
        for (qint64 i = frameOffsetHover - 1; i >= 0; i--) {
            if (m_handle->currentContainer()->frameAt(i).at(bitOffsetHover) != value) {
                this->m_handle->setFrameOffset(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Next Constant(ish) Column"),
            [this, frame, bitOffsetHover, frameOffsetHover]() {
        for (qint64 i = bitOffsetHover + 1; i < frame.size(); i++) {
            bool value = frame.at(i);
            bool constantish = true;
            for (qint64 adjacent = qMax(0ll, frameOffsetHover - 6);
                 adjacent <= qMin(m_handle->currentContainer()->frameCount() - 1, qint64( frameOffsetHover + 6));
                 adjacent++) {
                Frame adjFrame = m_handle->currentContainer()->frameAt(adjacent);
                if (i >= adjFrame.size()
                    || adjFrame.at(i) != value) {
                    constantish = false;
                    break;
                }
            }
            if (constantish) {
                this->m_handle->setBitOffset(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Constant(ish) Column"),
            [this, frame, bitOffsetHover, frameOffsetHover]() {
        for (qint64 i = bitOffsetHover - 1; i >= 0; i--) {
            bool value = frame.at(i);
            bool constantish = true;
            for (qint64 adjacent = qMax(0ll, frameOffsetHover - 6);
                 adjacent <= qMin(m_handle->currentContainer()->frameCount() - 1, qint64( frameOffsetHover + 6));
                 adjacent++) {
                Frame adjFrame = m_handle->currentContainer()->frameAt(adjacent);
                if (i >= adjFrame.size()
                    || adjFrame.at(i) != value) {
                    constantish = false;
                    break;
                }
            }
            if (constantish) {
                this->m_handle->setBitOffset(i);
                break;
            }
        }
    });

    menu.addMenu(&gotoMenu);

    menu.addSeparator();

    menu.addAction(
            tr("Set Marker"),
            [this, frame, bitOffsetHover]() {
        qint64 focusBit = frame.start() + bitOffsetHover;
        auto container = m_handle->currentContainer();
        auto markers = container->info()->metadata("location_markers").toStringList();

        bool ok;
        QString text = QInputDialog::getText(
                this,
                tr("Create New Marker"),
                QString(tr("Marker name for bit %1:")).arg(focusBit),
                QLineEdit::Normal,
                QString("%1").arg(markers.size() + 1),
                &ok);
        if (!ok || text.isEmpty()) {
            return;
        }

        markers.append(QString("%1,%2").arg(focusBit).arg(text));
        container->info()->setMetadata("location_markers", markers);
    });


    auto container = m_handle->currentContainer();
    auto markers = container->info()->metadata("location_markers").toStringList();
    QMenu gotoMarkerMenu("Go to Marker");

    for (auto marker : markers) {
        int bit = marker.mid(0, marker.indexOf(",")).toInt();
        QString name = marker.mid(marker.indexOf(",") + 1);
        gotoMarkerMenu.addAction(
                QString("\"%1\"").arg(name),
                [this, container, name, bit]() {
            qint64 frameOffset = container->info()->frameOffsetContaining(bit);
            if (frameOffset < 0 || frameOffset >= container->frameCount()) {
                return;
            }
            Frame frame = container->frameAt(frameOffset);
            qint64 bitOffset = bit - frame.start();
            m_handle->setOffsets(bitOffset, frameOffset);
        });
    }

    QAction *gotoMarkerMenuAction = menu.addMenu(&gotoMarkerMenu);
    if (markers.isEmpty()) {
        gotoMarkerMenuAction->setEnabled(false);
    }

    menu.addSeparator();

    menu.addAction(
            tr("Add Highlight..."),
            [this, frame, bitOffsetHover]() {
        qint64 focusBit = frame.start() + bitOffsetHover;
        auto container = m_handle->currentContainer();

        bool ok;
        int length = QInputDialog::getInt(
                this,
                tr("Create New Highlight"),
                QString(tr("Length of highlight in bits:")),
                8,
                1,
                INT_MAX,
                1,
                &ok);
        if (!ok) {
            return;
        }

        Range range = Range(focusBit,qMin(container->bits()->sizeInBits() - 1,focusBit + length - 1));
        RangeHighlight highlight(
            "manual_highlights",
            QString("%1 to %2").arg(range.start()).arg(range.end()),
            range,
            SettingsManager::getUiSetting(SettingsManager::HIGHLIGHT_5_COLOR_KEY).value<QColor>().rgba()
        );
        container->info()->addHighlight(highlight);
    });

    menu.addAction(
            tr("Add Highlight to Frame"),
            [this, frame]() {
        auto container = m_handle->currentContainer();
        RangeHighlight highlight(
            "manual_highlights",
            QString("%1 to %2").arg(frame.start()).arg(frame.end()),
            frame,
            SettingsManager::getUiSetting(SettingsManager::HIGHLIGHT_5_COLOR_KEY).value<QColor>().rgba()
        );
        container->info()->addHighlight(highlight);
    });

    menu.addAction(
            tr("Clear All Highlights"),
            [this, frame]() {
        auto container = m_handle->currentContainer();
        container->info()->clearHighlightCategory("manual_highlights");
    });

    menu.exec(this->mapToGlobal(point));
}

void DisplayWidget::checkNewContainer()
{
    emit hasNewContainer();
    checkFullRedraw();
}

void DisplayWidget::checkFullRedraw(DisplayInterface *display)
{
    QMutexLocker lock(&m_mutex);
    if (!m_handle->activeDisplays().contains(this)) {
        return;
    }
    if (display != nullptr && display != m_display.data()) {
        return;
    }
    fullRedraw();
}

void DisplayWidget::checkOverlayRedraw(DisplayInterface *display)
{
    QMutexLocker lock(&m_mutex);
    if (!m_handle->activeDisplays().contains(this)) {
        return;
    }
    if (display != nullptr && display != m_display.data()) {
        return;
    }
    scheduleRepaint();
}

void DisplayWidget::scheduleRepaint()
{
    if (m_repaintScheduled) {
        return;
    }
    m_repaintScheduled = true;
    update();
}

void DisplayWidget::drawError(QPainter *painter, QString error)
{
    painter->save();
    painter->setPen(QColor(255, 128, 128));
    QFont font("Roboto Mono");
    font.setPointSize(14);
    painter->setFont(font);
    painter->drawText(QRect({0, 0}, this->size()), Qt::TextWordWrap, error);
    painter->restore();
}

QSharedPointer<DisplayHandle> DisplayWidget::handle() const
{
    return m_handle;
}

QSharedPointer<DisplayInterface> DisplayWidget::display() const
{
    return m_display;
}
