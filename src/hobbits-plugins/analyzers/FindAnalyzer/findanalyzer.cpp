#include "findanalyzer.h"
#include "ui_findanalyzer.h"
#include "pluginhelper.h"
#include "settingsmanager.h"

const QString FIND_COLOR_KEY = "find_color";
const QString FIND_FOCUS_COLOR_KEY = "find_focus_color";

const QString FOUND_HIGHLIGHT = "found_patterns";
const QString FOUND_FOCUS_HIGHLIGHT = "found_pattern_focus";
const QString FOUND_RESULT_LABEL = "found_result_label";


FindAnalyzer::FindAnalyzer() :
    ui(new Ui::FindAnalyzer()),
    m_focusIndex(0)
{
    // Initialize default color settings
    QVariant findColor = SettingsManager::getInstance().getPluginSetting(FIND_COLOR_KEY);
    QVariant findFocusColor = SettingsManager::getInstance().getPluginSetting(FIND_FOCUS_COLOR_KEY);
    if (findColor.isNull() || !findColor.canConvert<QColor>()) {
        findColor = QColor(0, 150, 230, 85);
    }
    if (findFocusColor.isNull() || !findFocusColor.canConvert<QColor>()) {
        findFocusColor = QColor(50, 190, 0, 85);
    }
    SettingsManager::getInstance().setPluginSetting(FIND_COLOR_KEY, findColor);
    SettingsManager::getInstance().setPluginSetting(FIND_FOCUS_COLOR_KEY, findFocusColor);
}

FindAnalyzer::~FindAnalyzer()
{
    delete ui;
}

QString FindAnalyzer::getName()
{
    return "Find";
}

void FindAnalyzer::previewBits(QSharedPointer<BitContainerPreview> container)
{
    m_previewContainer = container;
    m_focusIndex = 0;
    initializeFindView();
    resolveFindFocusHighlight();
}

void FindAnalyzer::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    m_pluginCallback = pluginCallback;
}

void FindAnalyzer::triggerRun()
{
    if (!m_pluginCallback.isNull()) {
        m_pluginCallback->requestAnalyzerRun(this->getName(), this->getStateFromUi());
    }
}

AnalyzerInterface* FindAnalyzer::createDefaultAnalyzer()
{
    return new FindAnalyzer();
}

void FindAnalyzer::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
    connect(ui->tb_findNext, SIGNAL(pressed()), this, SLOT(focusOnNext()));
    connect(ui->tb_findPrevious, SIGNAL(pressed()), this, SLOT(focusOnPrevious()));
    connect(ui->lw_findSummary, SIGNAL(currentRowChanged(int)), this, SLOT(focusOnIndex(int)));
    connect(ui->le_searchString, SIGNAL(returnPressed()), this, SLOT(triggerRun()));
}

bool FindAnalyzer::canRecallPluginState(const QJsonObject &pluginState)
{
    return pluginState.contains("search_string");
}

QJsonObject FindAnalyzer::getStateFromUi()
{
    if (ui->le_searchString->text().isEmpty()) {
        return QJsonObject();
    }
    QJsonObject result;
    result.insert("search_string", ui->le_searchString->text());
    return result;

}

bool FindAnalyzer::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->le_searchString->setText(pluginState.value("search_string").toString());
    return true;
}

QSharedPointer<BitArray> FindAnalyzer::getFindBits(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return QSharedPointer<BitArray>();
    }
    return BitArray::fromString(pluginState.value("search_string").toString());
}

QSharedPointer<const AnalyzerResult> FindAnalyzer::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QSharedPointer<BitArray> findBits = getFindBits(recallablePluginState);
    QSharedPointer<const BitArray> bits = container->bits();

    QSharedPointer<const AnalyzerResult> nullResult;
    if (findBits->sizeInBits() < 1) {
        return nullResult;
    }

    QColor findColor = SettingsManager::getInstance().getPluginSetting(FIND_COLOR_KEY).value<QColor>();
    QList<RangeHighlight> highlights;

    qint64 prev = 0;
    for (qint64 start = 0; start < bits->sizeInBits(); start++) {
        if (findBits->sizeInBits() > bits->sizeInBits() - start) {
            break;
        }
        bool match = true;
        for (int i = 0; i < findBits->sizeInBits() && start + i < bits->sizeInBits(); i++) {
            if (bits->at(start + i) != findBits->at(i)) {
                match = false;
                break;
            }
        }
        if (match) {
            qint64 end = start + findBits->sizeInBits() - 1;
            QString label = QString("bit: %1 , delta: %2").arg(start).arg(start - prev);
            highlights.append(RangeHighlight(FOUND_HIGHLIGHT, label, Range(start, end), findColor));
            prev = start;
            start = end;
            end = end + findBits->sizeInBits() - 1;
        }

        PluginHelper::recordProgress(progressTracker, start, bits->sizeInBits());
        if (progressTracker->getCancelled()) {
            return PluginHelper::analyzerErrorResult("Processing cancelled");
        }
    }

    QSharedPointer<AnalyzerResult> analyzerResult = QSharedPointer<AnalyzerResult>(new AnalyzerResult());
    QSharedPointer<BitInfo> bitInfo = QSharedPointer<BitInfo>(new BitInfo(*container->bitInfo().data()));
    bitInfo->clearHighlightCategory(FOUND_HIGHLIGHT);
    bitInfo->addHighlights(highlights);

    QString resultHead = "Results for pattern '";
    for (int i = 0; i < findBits->sizeInBits(); i++) {
        if (findBits->at(i)) {
            resultHead += "1";
        }
        else {
            resultHead += "0";
        }
    }
    resultHead += "'";
    bitInfo->setMetadata(FOUND_RESULT_LABEL, resultHead);

    analyzerResult->setBitInfo(bitInfo);
    analyzerResult->setPluginState(recallablePluginState);

    return std::move(analyzerResult);
}

void FindAnalyzer::resolveFindFocusHighlight()
{
    if (m_previewContainer.isNull()) {
        return;
    }

    QColor findFocusColor = SettingsManager::getInstance().getPluginSetting(FIND_FOCUS_COLOR_KEY).value<QColor>();
    QList<RangeHighlight> findFocus;
    QList<RangeHighlight> findHighlights = m_previewContainer->bitInfo()->highlights(FOUND_HIGHLIGHT);
    if (m_previewContainer.isNull() || findHighlights.isEmpty()) {
        m_focusIndex = 0;
    }
    else {
        if (m_focusIndex >= findHighlights.size()) {
            m_focusIndex = 0;
        }
        else if (m_focusIndex < 0) {
            m_focusIndex = findHighlights.size() - 1;
        }
        RangeHighlight focus = findHighlights.at(m_focusIndex);
        focus = RangeHighlight(FOUND_FOCUS_HIGHLIGHT, focus.label(), focus.range(), findFocusColor);

        int containingFrame = m_previewContainer->bitInfo()->frameOffsetContaining(focus.range());
        if (containingFrame >= 0) {
            int bitOffset = qMax(
                    0,
                    int(focus.range().start() - m_previewContainer->bitInfo()->frames().at(containingFrame).start() - 16));
            int frameOffset = qMax(0, containingFrame - 16);

            m_previewContainer->clearHighlightCategory(FOUND_FOCUS_HIGHLIGHT);
            m_previewContainer->addHighlight(focus);

            if (!m_pluginCallback.isNull()) {
                m_pluginCallback->getDisplayHandle()->setOffsets(bitOffset, frameOffset);
            }
        }
    }

    checkFindButtons();
}

void FindAnalyzer::checkFindButtons()
{
    if (m_previewContainer.isNull() || m_previewContainer->bitInfo()->highlights(FOUND_HIGHLIGHT).isEmpty()) {
        ui->tb_findNext->hide();
        ui->tb_findPrevious->hide();
        ui->lb_findGo->hide();
        ui->lb_findSummary->hide();
        ui->lw_findSummary->hide();
        return;
    }
    ui->tb_findNext->show();
    ui->tb_findPrevious->show();
    ui->lb_findGo->show();
    ui->lw_findSummary->setCurrentRow(m_focusIndex);
    ui->lb_findGo->setText(
            QString("%1 of %2").arg(
                    m_focusIndex
                    + 1).arg(m_previewContainer->bitInfo()->highlights(FOUND_HIGHLIGHT).size()));
}

void FindAnalyzer::initializeFindView()
{
    if (m_previewContainer.isNull() || m_previewContainer->bitInfo()->highlights(FOUND_HIGHLIGHT).isEmpty()) {
        return;
    }
    m_focusIndex = 0;
    QString results_head = m_previewContainer->bitInfo()->metadata(FOUND_RESULT_LABEL).toString();
    if (results_head.isEmpty()) {
        ui->lb_findSummary->hide();
        ui->lw_findSummary->hide();
    }
    else {
        ui->lb_findSummary->setText(results_head);
        ui->lb_findSummary->show();
        ui->lw_findSummary->clear();
        QStringList labels;
        for (auto highlight: m_previewContainer->bitInfo()->highlights(FOUND_HIGHLIGHT)) {
            labels.append(highlight.label());
        }
        ui->lw_findSummary->addItems(labels);
        ui->lw_findSummary->setCurrentRow(m_focusIndex);
        ui->lw_findSummary->show();
    }
}

void FindAnalyzer::focusOnPrevious()
{
    m_focusIndex -= 1;
    resolveFindFocusHighlight();
}

void FindAnalyzer::focusOnNext()
{
    m_focusIndex += 1;
    resolveFindFocusHighlight();
}

void FindAnalyzer::focusOnIndex(int index)
{
    if (index == -1) {
        m_focusIndex = 0;
        resolveFindFocusHighlight();
    }
    else {
        m_focusIndex = index;
        resolveFindFocusHighlight();
    }
}
