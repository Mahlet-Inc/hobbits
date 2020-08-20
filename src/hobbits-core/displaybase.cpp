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

    connect(m_displayHandle.data(), SIGNAL(containerChanged()), this, SLOT(adjustScrollbars()));
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
    if (frameOffset < 0 || frameOffset >= m_displayHandle->getContainer()->frames().size()) {
        m_lastHover = QPoint();
        emit bitHover(false, 0, 0);
        return;
    }
    if (bitOffset >= m_displayHandle->getContainer()->frames().at(frameOffset).size()) {
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
            m_displayHandle->getHScroll()->setMaximum(m_displayHandle->getContainer()->maxFrameWidth() - 1);
            m_displayHandle->getHScroll()->setSingleStep(1);
            m_displayHandle->getHScroll()->setPageStep(this->width());

            m_displayHandle->getVScroll()->setEnabled(true);
            m_displayHandle->getVScroll()->setVisible(true);
            m_displayHandle->getVScroll()->setMinimum(0);
            m_displayHandle->getVScroll()->setMaximum(m_displayHandle->getContainer()->frames().size() - 1);
            m_displayHandle->getVScroll()->setSingleStep(1);
            m_displayHandle->getVScroll()->setPageStep(this->height());
        }

        this->repaint();
    }
}

QList<RangeHighlight> DisplayBase::getHighlightSpots(QList<RangeHighlight> highlights, int &highlightMinIndex, Frame frame)
{
    QList<RangeHighlight> spots;

    unsigned int intersection = Frame::Before;
    int highlightIndex = highlightMinIndex;
    while (intersection & (Frame::Before | Frame::Overlapping)) {
        if (highlightIndex >= highlights.size()) {
            break;
        }
        RangeHighlight highlight = highlights.at(highlightIndex);
        intersection = frame.compare(highlight.range());

        if (intersection & Frame::Overlapping) {
            RangeHighlight overlap(highlight.category(), highlight.label(), frame.getOverlap(highlight.range()), highlight.color(), highlight.children());
            spots.append(overlap);
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
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }
    if (frameOffset < 0 || bitOffset < 0) {
        return;
    }

    painter->setPen(Qt::transparent);
    for (QString category: m_displayHandle->getContainer()->bitInfo()->highlightCategories()) {
        drawHighlightRects(painter,
                           category,
                           colWidth,
                           rowHeight,
                           frameOffset,
                           bitOffset,
                           colCount,
                           rowCount,
                           colGroupSize,
                           colGroupMargin);
    }
}

QVector<QRectF> DisplayBase::drawHighlightRects(
        QPainter *painter,
        QString category,
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
    if (m_displayHandle->getContainer()->bitInfo()->highlights(category).size() > 0) {
        int lastHighlight = 0;
        int rowOffset = -1;
        for (int i = frameOffset; i < m_displayHandle->getContainer()->frames().size(); i++) {
            rowOffset++;
            if (rowOffset >= rowCount) {
                break;
            }
            Frame frame = m_displayHandle->getContainer()->frames().at(i);
            Frame displayFrame =
                Frame(
                        m_displayHandle->getContainer()->bits(),
                        frame.start() + bitOffset,
                        qMin(frame.end(), frame.start() + bitOffset + colCount - 1));
            QList<RangeHighlight> spots = getHighlightSpots(
                    m_displayHandle->getContainer()->bitInfo()->highlights(category),
                    lastHighlight,
                    displayFrame);

            while(!spots.isEmpty()) {
                RangeHighlight spot = spots.takeFirst();
                if (!spot.children().isEmpty()) {
                    painter->setOpacity(0.35);
                    int minIndex = 0;
                    spots.append(getHighlightSpots(spot.children(), minIndex, displayFrame));
                }
                else {
                    painter->setOpacity(1);
                }
                qint64 displayStart = (spot.range().start() - displayFrame.start());
                double hx = getGroupedOffset(displayStart, colWidth, colGroupSize, bitOffset, colGroupMargin);
                double hy = (i - frameOffset) * rowHeight;
                qint64 displayEnd = (spot.range().end() - displayFrame.start());
                double hw =
                    getGroupedOffset(displayEnd, colWidth, colGroupSize, bitOffset, colGroupMargin) + colWidth - hx;
                double hh = rowHeight;
                painter->setBrush(spot.color());
                painter->drawRect(QRectF(hx, hy, hw, hh));
            }
        }
    }
    return rects;
}

double DisplayBase::getGroupedOffset(qint64 idx, double width, int groupSize, int offset, int groupMargin)
{
    if (groupSize > 1) {
        qint64 groupOffset = offset % groupSize;
        qint64 groups = (idx + groupOffset) / groupSize;
        return width * double((groups * groupMargin) + idx);
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

    Frame frame = m_displayHandle->getContainer()->frames().at(m_lastHover.y());

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
        QVector<Frame> frames = m_displayHandle->getContainer()->frames();
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
        QVector<Frame> frames = m_displayHandle->getContainer()->frames();
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
        QVector<Frame> frames = m_displayHandle->getContainer()->frames();
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
        QVector<Frame> frames = m_displayHandle->getContainer()->frames();
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
        auto markers = container->bitInfo()->metadata("location_markers").toStringList();

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
    auto markers = container->bitInfo()->metadata("location_markers").toStringList();
    QMenu gotoMarkerMenu("Go to Marker");

    for (auto marker : markers) {
        int bit = marker.mid(0, marker.indexOf(",")).toInt();
        QString name = marker.mid(marker.indexOf(",") + 1);
        gotoMarkerMenu.addAction(
                QString("\"%1\"").arg(name),
                [this, container, name, bit]() {
            int frameOffset = container->bitInfo()->frameOffsetContaining(Range(bit, bit));
            if (frameOffset < 0 || frameOffset >= container->frames().size()) {
                return;
            }
            Frame frame = container->frames().at(frameOffset);
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
            SettingsManager::getInstance().getUiSetting(SettingsData::HIGHLIGHT_5_COLOR_KEY).value<QColor>()
        );
        container->addHighlight(highlight);
    });

    menu.addAction(
            tr("Add Highlight to Frame"),
            [this, frame]() {
        auto container = this->m_displayHandle->getContainer();
        RangeHighlight highlight(
            "manual_highlights",
            QString("%1 to %2").arg(frame.start()).arg(frame.end()),
            frame,
            SettingsManager::getInstance().getUiSetting(SettingsData::HIGHLIGHT_5_COLOR_KEY).value<QColor>()
        );
        container->addHighlight(highlight);
    });

    menu.addAction(
            tr("Clear All Highlights"),
            [this, frame]() {
        auto container = this->m_displayHandle->getContainer();
        container->clearHighlightCategory("manual_highlights");
    });

    menu.exec(this->mapToGlobal(point));
}
