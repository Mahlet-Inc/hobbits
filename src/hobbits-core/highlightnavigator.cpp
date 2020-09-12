#include "highlightnavigator.h"
#include "ui_highlightnavigator.h"
#include "settingsmanager.h"

static const QString FOCUS_HIGHLIGHT_CATEGORY = "Highlight Nav Focus";

HighlightNavigator::HighlightNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighlightNavigator),
    m_allHighlightCount(0),
    m_shouldHighlightSelection(false)
{
    ui->setupUi(this);

    connect(ui->tb_gotoNext, &QToolButton::pressed, this, &HighlightNavigator::selectNext);
    connect(ui->tb_gotoPrevious, &QToolButton::pressed, this, &HighlightNavigator::selectPrevious);
    connect(ui->tw_highlights, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
    connect(ui->tw_highlights, SIGNAL(itemSelectionChanged()), this, SIGNAL(selectionChanged()));

    Q_INIT_RESOURCE(hobbitscoreicons);
    ui->tb_gotoNext->setIcon(QIcon(":/hobbits-core/images/icons/arrow-right.png"));
    ui->tb_gotoPrevious->setIcon(QIcon(":/hobbits-core/images/icons/arrow-left.png"));
}

HighlightNavigator::~HighlightNavigator()
{
    delete ui;
}

void HighlightNavigator::setShouldHighlightSelection(bool shouldHighlight)
{
    m_shouldHighlightSelection = shouldHighlight;
}

QString HighlightNavigator::currentlySelectedLabel()
{
    auto item = ui->tw_highlights->currentItem();
    if (item) {
        return item->text(0);
    }
    return QString();
}

bool HighlightNavigator::selectRow(QString text)
{
    auto items = ui->tw_highlights->findItems(text, Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (items.empty()) {
        return false;
    }
    ui->tw_highlights->setCurrentItem(items.at(0));
    return true;
}

void HighlightNavigator::setPluginCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    if (!m_pluginCallback.isNull()) {
        disconnect(m_pluginCallback.data(), SIGNAL(changed()), this, SLOT(refresh()));
    }
    m_pluginCallback = pluginCallback;
    refresh();
}

void HighlightNavigator::setContainer(QSharedPointer<BitContainerPreview> container)
{
    m_container = container;
    refresh();
}

void HighlightNavigator::setHighlightCategory(QString category)
{
    m_category = category;
    refresh();
}

void HighlightNavigator::setTitle(QString title)
{
    ui->lb_title->setText(title);
}

QTreeWidgetItem* HighlightNavigator::highlightToItem(const RangeHighlight &highlight, int &count) const
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0, highlight.label());

    QPixmap pix(16,16);
    pix.fill(highlight.color());
    item->setIcon(0, QIcon(pix));

    QVariant userData;
    userData.setValue(highlight);
    item->setData(0, Qt::UserRole, userData);

    item->setData(0, Qt::UserRole + 1, ++count);

    for (auto child : highlight.children()) {
        item->addChild(highlightToItem(child, count));
    }

    return item;
}

void HighlightNavigator::refresh()
{
    // has the important stuff actually changed?
    // if not, just return
    if (!m_container.isNull()) {
        auto newHighlights = m_container->info()->highlights(m_category);
        if (newHighlights.size() == m_highlights.size()) {
            bool same = true;
            for (int i = 0; i < m_highlights.size(); i++) {
                if (m_highlights.at(i).label() != newHighlights.at(i).label()) {
                    same = false;
                    break;
                }
            }
            if (same) {
                return;
            }
        }
    }

    ui->tw_highlights->clear();
    m_highlights.clear();

    ui->tb_gotoNext->setEnabled(false);
    ui->tb_gotoPrevious->setEnabled(false);
    ui->lb_selected->setText("No Results");

    if (m_category.isEmpty()) {
        return;
    }

    if (m_container.isNull() || m_container->info()->highlights(m_category).isEmpty()) {
        return;
    }

    ui->tb_gotoNext->setEnabled(true);
    ui->tb_gotoPrevious->setEnabled(true);
    ui->lb_selected->setText("");

    m_highlights = m_container->info()->highlights(m_category);

    QList<QTreeWidgetItem*> items;
    m_allHighlightCount = 0;
    for (auto highlight: m_container->info()->highlights(m_category)) {
        items.append(highlightToItem(highlight, m_allHighlightCount));
    }
    ui->tw_highlights->addTopLevelItems(items);
    if (items.size() > 0) {
        ui->tw_highlights->setCurrentItem(items.first());
    }
}

void HighlightNavigator::selectNext()
{
    if (!ui->tw_highlights->currentItem()) {
        return;
    }
    auto next = ui->tw_highlights->itemBelow(ui->tw_highlights->currentItem());
    if (next) {
        ui->tw_highlights->setCurrentItem(next);
        return;
    }
    next = ui->tw_highlights->topLevelItem(0);
    if (next) {
        ui->tw_highlights->setCurrentItem(next);
        return;
    }
}

void HighlightNavigator::selectPrevious()
{
    if (!ui->tw_highlights->currentItem()) {
        return;
    }
    auto prev = ui->tw_highlights->itemAbove(ui->tw_highlights->currentItem());
    if (prev) {
        ui->tw_highlights->setCurrentItem(prev);
        return;
    }
    prev = ui->tw_highlights->topLevelItem(ui->tw_highlights->topLevelItemCount()-1);
    if (prev) {
        ui->tw_highlights->setCurrentItem(prev);
        return;
    }
}

void HighlightNavigator::updateSelection()
{
    if (m_container.isNull()) {
        return;
    }

    auto curr = ui->tw_highlights->currentItem();
    if (!curr) {
        return;
    }

    RangeHighlight selected = curr->data(0, Qt::UserRole).value<RangeHighlight>();
    // Make sure this exists in the container
    if (m_container->info()->highlights(selected.category(), selected.label()).isEmpty()) {
        return;
    }

    int selectedNum = curr->data(0, Qt::UserRole + 1).toInt();

    QColor focusColor = SettingsManager::getInstance().getUiSetting(SettingsData::FOCUS_COLOR_KEY).value<QColor>();
    RangeHighlight focus = RangeHighlight(FOCUS_HIGHLIGHT_CATEGORY, selected.label(), selected.range(), focusColor);

    qint64 containingFrame = m_container->info()->frameOffsetContaining(focus.range().start());
    if (containingFrame >= 0) {
        qint64 bitOffset = qMax(
                0ll,
                focus.range().start() - m_container->info()->frames()->at(containingFrame).start() - 16);
        if (bitOffset < 256) {
            bitOffset = 0;
        }
        qint64 frameOffset = qMax(0ll, containingFrame - 16);

        if (m_shouldHighlightSelection) {
            // Add it only if it is new
            if (m_container->info()->highlights(FOCUS_HIGHLIGHT_CATEGORY, focus.label()).isEmpty()) {
                m_container->clearHighlightCategory(FOCUS_HIGHLIGHT_CATEGORY);
                m_container->addHighlight(focus);
            }
        }

        if (!m_pluginCallback.isNull()) {
            m_pluginCallback->getDisplayHandle()->setOffsets(bitOffset, frameOffset);
        }
    }

    ui->lb_selected->setText(
                QString("%1 of %2").arg(
                        selectedNum).arg(m_allHighlightCount));
}
