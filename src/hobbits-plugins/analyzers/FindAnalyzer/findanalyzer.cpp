#include "findanalyzer.h"
#include "ui_findanalyzer.h"
#include "pluginhelper.h"
#include "settingsmanager.h"

const QString FIND_COLOR_KEY = "find_color";

const QString FOUND_HIGHLIGHT = "found_patterns";
const QString FOUND_RESULT_LABEL = "found_result_label";


FindAnalyzer::FindAnalyzer() :
    ui(new Ui::FindAnalyzer()),
    m_highlightNav(nullptr)
{
    // Initialize default color settings
    QVariant findColor = SettingsManager::getInstance().getPluginSetting(FIND_COLOR_KEY);
    if (findColor.isNull() || !findColor.canConvert<QColor>()) {
        findColor = QColor(0, 150, 230, 85);
    }
    SettingsManager::getInstance().setPluginSetting(FIND_COLOR_KEY, findColor);
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
    if (m_highlightNav) {
        m_highlightNav->setContainer(m_previewContainer);
        if (m_previewContainer.isNull()) {
            m_highlightNav->setTitle("");
        }
        else {
            QString resultLabel = m_previewContainer->bitInfo()->metadata(FOUND_RESULT_LABEL).toString();
            if (resultLabel.size() > 28) {
                resultLabel.truncate(25);
                resultLabel += "...";
            }
            m_highlightNav->setTitle(resultLabel);
        }
    }
}

void FindAnalyzer::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    m_pluginCallback = pluginCallback;
    if (m_highlightNav) {
        m_highlightNav->setPluginCallback(pluginCallback);
    }
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
    connect(ui->le_searchString, SIGNAL(returnPressed()), this, SLOT(triggerRun()));

    m_highlightNav = new HighlightNavigator(widget);
    ui->verticalLayout->addWidget(m_highlightNav);
    ui->verticalLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_highlightNav->setShouldHighlightSelection(true);
    m_highlightNav->setContainer(m_previewContainer);
    m_highlightNav->setHighlightCategory(FOUND_HIGHLIGHT);
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
    QSharedPointer<BitInfo> bitInfo = container->bitInfo()->copyMetadata();
    bitInfo->clearHighlightCategory(FOUND_HIGHLIGHT);
    bitInfo->addHighlights(highlights);

    QString resultHead = "";
    if (findBits->sizeInBits() % 8 == 0) {
        resultHead += "0x" + findBits->getPreviewBytes().toHex();
    }
    else {
        for (int i = 0; i < findBits->sizeInBits(); i++) {
            if (findBits->at(i)) {
                resultHead += "1";
            }
            else {
                resultHead += "0";
            }
        }
    }
    bitInfo->setMetadata(FOUND_RESULT_LABEL, resultHead);

    analyzerResult->setBitInfo(bitInfo);
    analyzerResult->setPluginState(recallablePluginState);

    return std::move(analyzerResult);
}
