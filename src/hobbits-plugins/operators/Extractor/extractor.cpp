#include "extractor.h"
#include "ui_extractor.h"
#include <QObject>

Extractor::Extractor() :
    ui(new Ui::Extractor()),
    m_stateHelper(new PluginStateHelper())
{
    m_stateHelper->addParameter("highlight_category", QJsonValue::String, [this](QJsonValue value) {
        int category_idx = ui->cb_category->findText(value.toString());
        if (category_idx < 0) {
            return false;
        }
        ui->cb_category->setCurrentIndex(category_idx);
        return true;
    }, [this]() {
        return ui->cb_category->currentText();
    });

    m_stateHelper->addParameter("highlight_label", QJsonValue::String, [this](QJsonValue value) {
        return m_highlightNav->selectRow(value.toString());
    }, [this]() {
        return m_highlightNav->currentlySelectedLabel();
    });

    m_stateHelper->addCheckBoxBoolParameter("extract_before", [this](){return ui->ck_before;});
    m_stateHelper->addCheckBoxBoolParameter("extract_highlight", [this](){return ui->ck_section;});
    m_stateHelper->addCheckBoxBoolParameter("extract_after", [this](){return ui->ck_after;});
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
    return m_stateHelper->validatePluginState(pluginState);
}

bool Extractor::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

QJsonObject Extractor::getStateFromUi()
{
    return m_stateHelper->getPluginStateFromUi();
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
        return OperatorResult::error(QString("No highlight found matching required label (%1) and category (%2)").arg(label).arg(category));
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

    QString name = highlight.label();
    QString prefix = "";
    if (takeBefore) {
        prefix = "pre";
    }
    if (takeAfter) {
        if (takeBefore) {
            prefix += "+";
        }
        prefix += "post";
    }
    if (!prefix.isEmpty()) {
        if (takeHighlight) {
            prefix += "+";
        }
        else {
            prefix += "-";
        }
    }
    outputContainer->setName(prefix + name);

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
