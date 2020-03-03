#include "displaybase.h"
#include "settingsmanager.h"
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWheelEvent>

DisplayBase::DisplayBase(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef, QWidget *parent) :
    QWidget(parent),
    m_displayHandle(displayHandle),
    m_pluginRef(pluginRef)
{
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setMouseTracking(true);

    connect(m_displayHandle.data(), SIGNAL(containerFramesChanged()), this, SLOT(adjustScrollbars()));
    connect(m_displayHandle.data(), SIGNAL(containerHighlightsChanged()), this, SLOT(repaint()));
    connect(m_displayHandle.data(), SIGNAL(newOffsets(int,int)), this, SLOT(repaint()));
    connect(
            m_displayHandle.data(),
            SIGNAL(newFocusDisplays(QSet<DisplayInterface*>)),
            this,
            SLOT(checkFocus(QSet<DisplayInterface*>)));
    connect(m_displayHandle.data(), SIGNAL(newBitContainer()), this, SLOT(bitContainerChanged()));
    connect(this, SIGNAL(bitHover(bool,int,int)), m_displayHandle.data(), SLOT(setBitHover(bool,int,int)));

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
            this,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this,
            SLOT(showContextMenu(const QPoint&)));
}

void DisplayBase::wheelEvent(QWheelEvent *event)
{
    if (m_displayHandle.isNull()) {
        return;
    }
    if (event->delta() > 0) {
        m_displayHandle->setOffsets(m_displayHandle->getBitOffset(), m_displayHandle->getFrameOffset() - 1);
    }
    else if (event->delta() < 0) {
        m_displayHandle->setOffsets(m_displayHandle->getBitOffset(), m_displayHandle->getFrameOffset() + 1);
    }
}

void DisplayBase::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_displayHandle->setBitHover(false, 0, 0);
}

QPoint DisplayBase::getOffset(int x, int y, int xSize, int ySize, int xGrouping, int bitsPerX)
{
    int xDiff = x / xSize;
    int yDiff = y / ySize;

    if (xGrouping > 1) {
        int groupOffset = m_displayHandle->getBitOffset() % xGrouping;
        int xGroups = (xDiff + groupOffset) / (xGrouping + 1);
        xDiff -= xGroups;
    }
    xDiff *= bitsPerX;

    return QPoint(xDiff, yDiff);
}

void DisplayBase::sendHoverUpdate(QMouseEvent *event, int xSize, int ySize, int xGrouping, int bitsPerX, QPoint offset)
{
    if (m_displayHandle->getContainer().isNull()) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }

    if (event->x() < offset.x() || event->y() < offset.y()) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }

    QPoint diff = getOffset(event->x() - offset.x(), event->y() - offset.y(), xSize, ySize, xGrouping, bitsPerX);

    if (diff.x() < 0 || diff.y() < 0) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }

    int frameOffset = m_displayHandle->getFrameOffset() + diff.y();
    int bitOffset = m_displayHandle->getBitOffset() + diff.x();
    if (frameOffset < 0 || frameOffset >= m_displayHandle->getContainer()->getFrames().size()) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }
    if (bitOffset >= m_displayHandle->getContainer()->getFrames().at(frameOffset).size()) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }
    m_lastHover = QPoint(bitOffset, frameOffset);
    emit bitHover(true, bitOffset, frameOffset);
}

void DisplayBase::bitContainerChanged()
{
    if (!m_displayHandle->getContainer().isNull()) {
        this->adjustScrollbars();
    }
    else {
        this->repaint();
    }
}

void DisplayBase::checkFocus(QSet<DisplayInterface*> displays)
{
    if (displays.contains(m_pluginRef)) {
        this->adjustScrollbars();
    }
}

void DisplayBase::adjustScrollbars()
{
    if (m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        if (m_displayHandle->getContainer().isNull()) {
            m_displayHandle->getHScroll()->setVisible(false);
            m_displayHandle->getVScroll()->setVisible(false);
        }
        else {
            m_displayHandle->getHScroll()->setEnabled(true);
            m_displayHandle->getHScroll()->setVisible(true);
            m_displayHandle->getHScroll()->setMinimum(0);
            m_displayHandle->getHScroll()->setMaximum(m_displayHandle->getContainer()->getMaxFrameWidth() - 1);
            m_displayHandle->getHScroll()->setSingleStep(1);
            m_displayHandle->getHScroll()->setPageStep(this->width());

            m_displayHandle->getVScroll()->setEnabled(true);
            m_displayHandle->getVScroll()->setVisible(true);
            m_displayHandle->getVScroll()->setMinimum(0);
            m_displayHandle->getVScroll()->setMaximum(m_displayHandle->getContainer()->getFrames().size() - 1);
            m_displayHandle->getVScroll()->setSingleStep(1);
            m_displayHandle->getVScroll()->setPageStep(this->height());
        }

        this->repaint();
    }
}

QList<Range> DisplayBase::getHighlightSpots(QList<Range> highlights, int &highlightMinIndex, Frame frame)
{
    QList<Range> spots;

    unsigned int intersection = Frame::Before;
    int highlightIndex = highlightMinIndex;
    while (intersection & (Frame::Before | Frame::Overlapping)) {
        if (highlightIndex >= highlights.size()) {
            break;
        }
        Range highlight = highlights.at(highlightIndex);
        intersection = frame.compare(highlight);

        if (intersection & Frame::Overlapping) {
            spots.append(frame.getOverlap(highlight));
        }
        else if (intersection & Frame::After) {
            break;
        }

        if ((!(intersection & Frame::After)) && highlightMinIndex == highlightIndex) {
            highlightMinIndex++;
        }
        highlightIndex++;
    }
    return spots;
}

void DisplayBase::drawHighlights(
        QPainter *painter,
        double colWidth,
        double rowHeight,
        int frameOffset,
        int bitOffset,
        int colCount,
        int rowCount,
        int colGroupSize,
        int colGroupMargin)
{
    painter->setPen(Qt::transparent);
    QVector<QRectF> highlightRects = getHighlightRects(
            "find",
            colWidth,
            rowHeight,
            frameOffset,
            bitOffset,
            colCount,
            rowCount,
            colGroupSize,
            colGroupMargin);
    if (!highlightRects.isEmpty()) {
        painter->setBrush(
                SettingsManager::getInstance().getUiSetting(
                        SettingsData::HIGHLIGHT_1_COLOR_KEY).value<QColor>());
        painter->drawRects(highlightRects);
    }
    highlightRects = getHighlightRects(
            "find-focus",
            colWidth,
            rowHeight,
            frameOffset,
            bitOffset,
            colCount,
            rowCount,
            colGroupSize,
            colGroupMargin);
    if (!highlightRects.isEmpty()) {
        painter->setBrush(
                SettingsManager::getInstance().getUiSetting(
                        SettingsData::HIGHLIGHT_2_COLOR_KEY).value<QColor>());
        painter->drawRects(highlightRects);
    }
    highlightRects = getHighlightRects(
            "manual_highlights",
            colWidth,
            rowHeight,
            frameOffset,
            bitOffset,
            colCount,
            rowCount,
            colGroupSize,
            colGroupMargin);
    if (!highlightRects.isEmpty()) {
        painter->setBrush(
                SettingsManager::getInstance().getUiSetting(
                        SettingsData::HIGHLIGHT_5_COLOR_KEY).value<QColor>());
        painter->drawRects(highlightRects);
    }
}

QVector<QRectF> DisplayBase::getHighlightRects(
        QString type,
        double colWidth,
        double rowHeight,
        int frameOffset,
        int bitOffset,
        int colCount,
        int rowCount,
        int colGroupSize,
        int colGroupMargin)
{
    if (m_displayHandle->getContainer().isNull()) {
        return QVector<QRectF>();
    }

    QVector<QRectF> rects;
    if (m_displayHandle->getContainer()->getHighlights(type).size() > 0) {
        int lastHighlight = 0;
        int rowOffset = -1;
        for (int i = frameOffset; i < m_displayHandle->getContainer()->getFrames().size(); i++) {
            rowOffset++;
            if (rowOffset >= rowCount) {
                break;
            }
            Frame frame = m_displayHandle->getContainer()->getFrames().at(i);
            Frame displayFrame =
                Frame(
                        m_displayHandle->getContainer()->getBaseBits(),
                        frame.start() + bitOffset,
                        qMin(frame.end(), frame.start() + bitOffset + colCount - 1));
            QList<Range> spots = getHighlightSpots(
                    m_displayHandle->getContainer()->getHighlights(type),
                    lastHighlight,
                    displayFrame);

            for (Range spot : spots) {
                qint64 displayStart = (spot.start() - displayFrame.start());
                double hx = getGroupedOffset(displayStart, colWidth, colGroupSize, bitOffset, colGroupMargin);
                double hy = (i - frameOffset) * rowHeight;
                qint64 displayEnd = (spot.end() - displayFrame.start());
                double hw =
                    getGroupedOffset(displayEnd, colWidth, colGroupSize, bitOffset, colGroupMargin) + colWidth - hx;
                double hh = rowHeight;
                rects.append(QRectF(hx, hy, hw, hh));
            }
        }
    }
    return rects;
}

double DisplayBase::getGroupedOffset(int idx, double width, int groupSize, int offset, int groupMargin)
{
    if (groupSize > 1) {
        int groupOffset = offset % groupSize;
        int groups = (idx + groupOffset) / groupSize;
        return width * ((groups * groupMargin) + idx);
    }
    else {
        return width * idx;
    }
}

void DisplayBase::showContextMenu(const QPoint &point)
{
    if (m_lastHover.isNull()) {
        return;
    }
    QMenu menu(this);

    Frame frame = m_displayHandle->getContainer()->getFrames().at(m_lastHover.y());

    menu.addSection(
            QString("Frame %1, Bit %2 of %3")
            .arg(m_lastHover.y())
            .arg(m_lastHover.x())
            .arg(frame.size()));

    QMenu gotoMenu("Go to Location");
    gotoMenu.addAction(
            tr("End of Frame"),
            [this, frame]() {
        this->m_displayHandle->getHScroll()->setValue(int(frame.size() - 8));
    });
    gotoMenu.addAction(
            tr("Start of frame"),
            [this]() {
        this->m_displayHandle->getHScroll()->setValue(0);
    });
    gotoMenu.addAction(
            tr("Next Bit Change in Frame"),
            [this, frame]() {
        bool value = frame.at(m_lastHover.x());
        for (int i = m_lastHover.x() + 1; i < frame.size(); i++) {
            if (frame.at(i) != value) {
                this->m_displayHandle->getHScroll()->setValue(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Bit Change in Frame"),
            [this, frame]() {
        bool value = frame.at(m_lastHover.x());
        for (int i = m_lastHover.x() - 1; i >= 0; i--) {
            if (frame.at(i) != value) {
                this->m_displayHandle->getHScroll()->setValue(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Next Bit Change in Column"),
            [this, frame]() {
        bool value = frame.at(m_lastHover.x());
        QList<Frame> frames = m_displayHandle->getContainer()->getFrames();
        for (int i = m_lastHover.y() + 1; i < frames.size(); i++) {
            if (frames.at(i).at(m_lastHover.x()) != value) {
                this->m_displayHandle->getVScroll()->setValue(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Bit Change in Column"),
            [this, frame]() {
        bool value = frame.at(m_lastHover.x());
        QList<Frame> frames = m_displayHandle->getContainer()->getFrames();
        for (int i = m_lastHover.y() - 1; i >= 0; i--) {
            if (frames.at(i).at(m_lastHover.x()) != value) {
                this->m_displayHandle->getVScroll()->setValue(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Next Constant(ish) Column"),
            [this, frame]() {
        QList<Frame> frames = m_displayHandle->getContainer()->getFrames();
        for (int i = m_lastHover.x() + 1; i < frame.size(); i++) {
            bool value = frame.at(i);
            bool constantish = true;
            for (int adjacent = qMax(0, m_lastHover.y() - 6);
                 adjacent <= qMin(frames.size() - 1, m_lastHover.y() + 6);
                 adjacent++) {
                Frame adjFrame = frames.at(adjacent);
                if (i >= adjFrame.size()
                    || adjFrame.at(i) != value) {
                    constantish = false;
                    break;
                }
            }
            if (constantish) {
                this->m_displayHandle->getHScroll()->setValue(i);
                break;
            }
        }
    });
    gotoMenu.addAction(
            tr("Previous Constant(ish) Column"),
            [this, frame]() {
        QList<Frame> frames = m_displayHandle->getContainer()->getFrames();
        for (int i = m_lastHover.x() - 1; i >= 0; i--) {
            bool value = frame.at(i);
            bool constantish = true;
            for (int adjacent = qMax(0, m_lastHover.y() - 5);
                 adjacent <= qMin(frames.size() - 1, m_lastHover.y() + 5);
                 adjacent++) {
                Frame adjFrame = frames.at(adjacent);
                if (i >= adjFrame.size()
                    || adjFrame.at(i) != value) {
                    constantish = false;
                    break;
                }
            }
            if (constantish) {
                this->m_displayHandle->getHScroll()->setValue(i);
                break;
            }
        }
    });

    menu.addMenu(&gotoMenu);

    menu.addSeparator();

    menu.addAction(
            tr("Set Marker"),
            [this, frame]() {
        qint64 focusBit = frame.start() + m_lastHover.x();
        auto container = this->m_displayHandle->getContainer();
        auto markers = container->getMetadata("location_markers");

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
        container->setMetadata("location_markers", markers);
    });


    auto container = this->m_displayHandle->getContainer();
    auto markers = container->getMetadata("location_markers");
    QMenu gotoMarkerMenu("Go to Marker");

    for (auto marker : markers) {
        int bit = marker.mid(0, marker.indexOf(",")).toInt();
        QString name = marker.mid(marker.indexOf(",") + 1);
        gotoMarkerMenu.addAction(
                QString("\"%1\"").arg(name),
                [this, container, name, bit]() {
            int frameOffset = container->getFrameOffsetContaining(Range(bit, bit));
            if (frameOffset < 0 || frameOffset >= container->getFrames().size()) {
                return;
            }
            Frame frame = container->getFrames().at(frameOffset);
            qint64 bitOffset = bit - frame.start();
            this->m_displayHandle->setOffsets(int(bitOffset), frameOffset);
        });
    }

    QAction *gotoMarkerMenuAction = menu.addMenu(&gotoMarkerMenu);
    if (markers.isEmpty()) {
        gotoMarkerMenuAction->setEnabled(false);
    }

    menu.addSeparator();

    menu.addAction(
            tr("Add Highlight..."),
            [this, frame]() {
        qint64 focusBit = frame.start() + m_lastHover.x();
        auto container = this->m_displayHandle->getContainer();
        auto manualHighlights = container->getHighlights("manual_highlights");

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

        manualHighlights.append(
                Range(
                        focusBit,
                        qMin(
                                container->getBaseBits()->sizeInBits() - 1,
                                focusBit + length - 1)));
        std::sort(manualHighlights.begin(), manualHighlights.end());
        container->setHighlights("manual_highlights", manualHighlights);
    });

    menu.addAction(
            tr("Add Highlight to Frame"),
            [this, frame]() {
        auto container = this->m_displayHandle->getContainer();
        auto manualHighlights = container->getHighlights("manual_highlights");
        manualHighlights.append(frame);
        std::sort(manualHighlights.begin(), manualHighlights.end());
        container->setHighlights("manual_highlights", manualHighlights);
    });

    menu.addAction(
            tr("Clear All Highlights"),
            [this, frame]() {
        auto container = this->m_displayHandle->getContainer();
        container->setHighlights("manual_highlights", {});
    });

    menu.exec(this->mapToGlobal(point));
}
