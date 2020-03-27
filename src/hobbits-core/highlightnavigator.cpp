#include "highlightnavigator.h"
#include "ui_highlightnavigator.h"
#include "settingsmanager.h"

static const QString FOCUS_HIGHLIGHT_CATEGORY = "Highlight Nav Focus";

HighlightNavigator::HighlightNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighlightNavigator)
{
    ui->setupUi(this);

    connect(ui->tb_gotoNext, &QToolButton::pressed, this, &HighlightNavigator::selectNext);
    connect(ui->tb_gotoPrevious, &QToolButton::pressed, this, &HighlightNavigator::selectPrevious);
    connect(ui->lw_highlights, SIGNAL(currentRowChanged(int)), this, SLOT(updateSelection()));

    Q_INIT_RESOURCE(icons);
    ui->tb_gotoNext->setIcon(QIcon(":/hobbits-core/images/icons/arrow-right.png"));
    ui->tb_gotoPrevious->setIcon(QIcon(":/hobbits-core/images/icons/arrow-left.png"));
}

HighlightNavigator::~HighlightNavigator()
{
    delete ui;
}


int HighlightNavigator::currentlySelectedRow()
{
    return ui->lw_highlights->currentRow();
}

void HighlightNavigator::setPluginCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    if (!m_pluginCallback.isNull()) {
        disconnect(m_pluginCallback.data(), SIGNAL(changed()), this, SLOT(refresh()));
    }
    m_pluginCallback = pluginCallback;
    connect(m_pluginCallback.data(), SIGNAL(changed()), this, SLOT(refresh()));
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

void HighlightNavigator::refresh()
{
    ui->lw_highlights->clear();
    m_highlights.clear();

    ui->tb_gotoNext->setEnabled(false);
    ui->tb_gotoPrevious->setEnabled(false);
    ui->lb_selected->setText("No Results");

    if (m_category.isEmpty()) {
        return;
    }

    if (m_container.isNull() || m_container->bitInfo()->highlights(m_category).isEmpty()) {
        return;
    }

    ui->tb_gotoNext->setEnabled(true);
    ui->tb_gotoPrevious->setEnabled(true);
    ui->lb_selected->setText("");

    m_highlights = m_container->bitInfo()->highlights(m_category);

    ui->lw_highlights->clear();
    QStringList labels;
    for (auto highlight: m_container->bitInfo()->highlights(m_category)) {
        labels.append(highlight.label());
    }
    ui->lw_highlights->addItems(labels);
    ui->lw_highlights->setCurrentRow(0);
}

void HighlightNavigator::selectNext()
{
    if (currentlySelectedRow() < 0) {
        return;
    }
    selectRow(currentlySelectedRow() + 1);
}

void HighlightNavigator::selectPrevious()
{
    if (currentlySelectedRow() < 0) {
        return;
    }
    selectRow(currentlySelectedRow() - 1);
}

void HighlightNavigator::selectRow(int row)
{
    if (currentlySelectedRow() < 0) {
        return;
    }
    if (row < 0) {
        row = m_highlights.size() - 1;
    }
    else if (row >= m_highlights.size()) {
        row = 0;
    }
    ui->lw_highlights->setCurrentRow(row);
}


void HighlightNavigator::updateSelection()
{
    if (m_container.isNull()) {
        return;
    }

    int currIndex = ui->lw_highlights->currentRow();
    if (currIndex >= m_highlights.size() || currIndex < 0) {
        return;
    }
    RangeHighlight selected = m_highlights.at(currIndex);

    QColor focusColor = SettingsManager::getInstance().getUiSetting(SettingsData::FOCUS_COLOR_KEY).value<QColor>();
    RangeHighlight focus = RangeHighlight(FOCUS_HIGHLIGHT_CATEGORY, selected.label(), selected.range(), focusColor);

    int containingFrame = m_container->bitInfo()->frameOffsetContaining(focus.range());
    if (containingFrame >= 0) {
        int bitOffset = qMax(
                0,
                int(focus.range().start() - m_container->bitInfo()->frames().at(containingFrame).start() - 16));
        int frameOffset = qMax(0, containingFrame - 16);

        m_container->clearHighlightCategory(FOCUS_HIGHLIGHT_CATEGORY);
        m_container->addHighlight(focus);

        if (!m_pluginCallback.isNull()) {
            m_pluginCallback->getDisplayHandle()->setOffsets(bitOffset, frameOffset);
        }
    }

    ui->lb_selected->setText(
                QString("%1 of %2").arg(
                        currIndex + 1).arg(m_highlights.size()));
}
