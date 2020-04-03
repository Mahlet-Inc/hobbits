#include "extractor.h"
#include "ui_extractor.h"
#include "pluginhelper.h"
#include <QObject>

Extractor::Extractor() :
    ui(new Ui::Extractor())
{

}

OperatorInterface* Extractor::createDefaultOperator()
{
    return new Extractor();
}

//Return name of operator
QString Extractor::getName()
{
    return "Extractor";
}

void Extractor::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}


void Extractor::sendPluginConfigUpdate() {
    if (m_pluginCallback.isNull()) {
        return;
    }
    m_pluginCallback->sendOperatorStateChanged(getName());
}

void Extractor::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);

    connect(ui->cb_category, &QComboBox::currentTextChanged, this, &Extractor::setHighlightCategory);
    connect(ui->cb_category, SIGNAL(currentTextChanged(QString)), this, SLOT(sendPluginConfigUpdate()));
    connect(ui->ck_before, SIGNAL(stateChanged(int)), this, SLOT(sendPluginConfigUpdate()));
    connect(ui->ck_section, SIGNAL(stateChanged(int)), this, SLOT(sendPluginConfigUpdate()));
    connect(ui->ck_after, SIGNAL(stateChanged(int)), this, SLOT(sendPluginConfigUpdate()));

    m_highlightNav = new HighlightNavigator(widget);
    ui->sectionLayout->addWidget(m_highlightNav);
    ui->sectionLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_highlightNav->setContainer(m_previewContainer);
    connect(m_highlightNav, SIGNAL(selectionChanged()), this, SLOT(sendPluginConfigUpdate()));
}

bool Extractor::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
        return false;
    }

    if (!(pluginState.contains("highlight_category")
            && pluginState.contains("highlight_label")
            && pluginState.value("highlight_category").isString()
            && pluginState.value("highlight_label").isString()
            && ((pluginState.contains("extract_before") && pluginState.value("extract_before").isBool())
                || (pluginState.contains("extract_highlight") && pluginState.value("extract_highlight").isBool())
                || (pluginState.contains("extract_after") && pluginState.value("extract_after").isBool())))) {
        return false;
    }

    return true;
}

bool Extractor::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    if (pluginState.contains("extract_before")) {
        ui->ck_before->setChecked(pluginState.value("extract_before").toBool());
    }
    else {
        ui->ck_before->setChecked(false);
    }

    if (pluginState.contains("extract_highlight")) {
        ui->ck_section->setChecked(pluginState.value("extract_highlight").toBool());
    }
    else {
        ui->ck_section->setChecked(false);
    }

    if (pluginState.contains("extract_after")) {
        ui->ck_after->setChecked(pluginState.value("extract_after").toBool());
    }
    else {
        ui->ck_after->setChecked(false);
    }

    QString category = pluginState.value("highlight_category").toString();
    int category_idx = ui->cb_category->findText(category);
    if (category_idx < 0) {
        return false;
    }
    ui->cb_category->setCurrentIndex(category_idx);

    return m_highlightNav->selectRow(pluginState.value("highlight_label").toString());
}

QJsonObject Extractor::getStateFromUi()
{
    QJsonObject pluginState;

    QString category = ui->cb_category->currentText();
    QString label = m_highlightNav->currentlySelectedLabel();

    if (category.isEmpty() || label.isEmpty()) {
        return pluginState;
    }

    pluginState.insert("highlight_category", category);
    pluginState.insert("highlight_label", label);

    if (ui->ck_before->isChecked()) {
        pluginState.insert("extract_before", true);
    }
    if (ui->ck_section->isChecked()) {
        pluginState.insert("extract_highlight", true);
    }
    if (ui->ck_after->isChecked()) {
        pluginState.insert("extract_after", true);
    }

    return pluginState;
}

int Extractor::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int Extractor::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> Extractor::operateOnContainers(
        QList<QSharedPointer<const BitContainer> > inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    if (!canRecallPluginState(recallablePluginState)) {
        return OperatorResult::error("Invalid input parameters");
    }

    if (inputContainers.size() != 1) {
        return OperatorResult::error("Invalid number of input containers (requires 1)");
    }
    QSharedPointer<const BitContainer> inputContainer = inputContainers.at(0);

    bool takeBefore = recallablePluginState.value("extract_before").toBool();
    bool takeHighlight = recallablePluginState.value("extract_highlight").toBool();
    bool takeAfter = recallablePluginState.value("extract_after").toBool();
    QString category = recallablePluginState.value("highlight_category").toString();
    QString label = recallablePluginState.value("highlight_label").toString();
    auto highlights = inputContainer->bitInfo()->highlights(category, label);
    if (highlights.size() < 1) {
        return OperatorResult::error("No highlight found matching required label and category");
    }
    RangeHighlight highlight = highlights.at(0);
    Range range = highlight.range();

    QSharedPointer<BitContainer> outputContainer(new BitContainer());
    qint64 outputSize = inputContainer->bits()->sizeInBits();
    if (!takeBefore) {
        outputSize -= range.start();
    }
    if (!takeHighlight) {
        outputSize -= range.size();
    }
    if (!takeAfter) {
        outputSize -= (inputContainer->bits()->sizeInBits() - 1 - range.end());
    }
    auto outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
    qint64 outputIndex = 0;
    if (takeBefore) {
        for (qint64 i = 0; i < range.start(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->getCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }
    if (takeHighlight) {
        for (qint64 i = range.start(); i <= range.end(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->getCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }
    if (takeAfter) {
        for (qint64 i = range.end() + 1; i < inputContainer->bits()->sizeInBits(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->getCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }

    outputContainer->setBits(outBits);

    return OperatorResult::result({outputContainer}, recallablePluginState);
}

void Extractor::previewBits(QSharedPointer<BitContainerPreview> container)
{
    QJsonObject uiState = getStateFromUi();
    m_previewContainer = container;
    m_highlightNav->setContainer(m_previewContainer);
    if (m_previewContainer.isNull()) {
        return;
    }
    ui->cb_category->clear();
    for (QString category: m_previewContainer->bitInfo()->highlightCategories()) {
        ui->cb_category->addItem(category);
    }
    setPluginStateInUi(uiState);
}

void Extractor::setHighlightCategory(QString category)
{
    m_highlightNav->setHighlightCategory(category);
}
