#include "findanalyzer.h"
#include "ui_findanalyzer.h"

FindAnalyzer::FindAnalyzer() :
    ui(new Ui::FindAnalyzer()),
    m_focusIndex(0)
{
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
        m_pluginCallback->requestRun(this->getName(), this->getStateFromUi());
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
    QList<Range> results;
    QSharedPointer<BitArray> findBits = getFindBits(recallablePluginState);
    QSharedPointer<const BitArray> bits = container->getBaseBits();

    QSharedPointer<const AnalyzerResult> nullResult;
    if (findBits->sizeInBits() < 1) {
        return nullResult;
    }

    int lastPercent = 0;
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
            results.append(Range(start, end));
            start = end;
            end = end + findBits->sizeInBits() - 1;
        }

        int nextPercent = int(double(start) / double(bits->sizeInBits()) * 100.0);
        if (nextPercent > lastPercent) {
            lastPercent = nextPercent;
            progressTracker->setProgressPercent(nextPercent);
        }
        if (progressTracker->getCancelled()) {
            return QSharedPointer<const AnalyzerResult>(
                    (new AnalyzerResult())->setPluginState(
                            QJsonObject(
                                    {QPair<QString, QJsonValue>(
                                            "error",
                                            QJsonValue("Processing cancelled"))}))
                    );
        }
    }

    QSharedPointer<AnalyzerResult> analyzerResult = QSharedPointer<AnalyzerResult>(new AnalyzerResult());
    analyzerResult->addRanges("find", results);

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

    QStringList result;
    qint64 prev = 0;
    for (Range range : results) {
        result.append(QString("bit: %1 , delta: %2").arg(range.start()).arg(range.start() - prev));
        prev = range.start();
    }

    analyzerResult->addMetadata("find-results-head", resultHead);
    analyzerResult->addMetadata("find-results", result);
    analyzerResult->setPluginState(recallablePluginState);

    return analyzerResult;
}

void FindAnalyzer::resolveFindFocusHighlight()
{
    QList<Range> findFocus;
    if (m_previewContainer.isNull() || m_previewContainer->getHighlights("find").isEmpty()) {
        m_focusIndex = 0;
    }
    else {
        if (m_focusIndex >= m_previewContainer->getHighlights("find").size()) {
            m_focusIndex = 0;
        }
        else if (m_focusIndex < 0) {
            m_focusIndex = m_previewContainer->getHighlights("find").size() - 1;
        }
        Range focus = m_previewContainer->getHighlights("find").at(m_focusIndex);
        findFocus.append(focus);

        int containingFrame = m_previewContainer->getFrameOffsetContaining(focus);
        if (containingFrame >= 0) {
            int bitOffset = qMax(0, int(focus.start() - m_previewContainer->getFrames().at(containingFrame).start() - 16));
            int frameOffset = qMax(0, containingFrame - 16);

            QList<Range> findFocus;
            findFocus.append(focus);
            m_previewContainer->setHighlights("find-focus", findFocus);

            m_previewContainer->requestFocus(bitOffset, frameOffset);
        }
    }

    checkFindButtons();
}

void FindAnalyzer::checkFindButtons()
{
    if (m_previewContainer.isNull() || m_previewContainer->getHighlights("find").isEmpty()) {
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
                    + 1).arg(m_previewContainer->getHighlights("find").size()));
}

void FindAnalyzer::initializeFindView()
{
    if (m_previewContainer.isNull() || m_previewContainer->getHighlights("find").isEmpty()) {
        return;
    }
    m_focusIndex = 0;
    QString results_head = m_previewContainer->getMetadata("find-results-head").first();
    if (results_head.isEmpty()) {
        ui->lb_findSummary->hide();
        ui->lw_findSummary->hide();
        // ui->verticalSpacer->changeSize(20, 20, QSizePolicy::Expanding);
    }
    else {
        ui->lb_findSummary->setText(results_head);
        ui->lb_findSummary->show();
        ui->lw_findSummary->clear();
        ui->lw_findSummary->addItems(m_previewContainer->getMetadata("find-results"));
        ui->lw_findSummary->setCurrentRow(m_focusIndex);
        ui->lw_findSummary->show();
        // ui->verticalSpacer->changeSize(20, 20, QSizePolicy::Ignored);
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
