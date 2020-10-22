#include "batchedititem.h"
#include <QGraphicsProxyWidget>
#include <QPainter>

BatchEditItem::BatchEditItem(QSharedPointer<HobbitsPluginManager> pluginManager, QSharedPointer<const PluginActionBatch::ActionStep> step)
{
    m_id = step->stepId;
    m_inputs = step->inputs;
    m_pluginType = step->action->pluginType();
    m_pluginName = step->action->pluginName();
    QSharedPointer<ParameterDelegate> parameterDelegate;
    if (step->action->pluginType() == PluginAction::Operator) {
        auto plugin = pluginManager->getOperator(step->action->pluginName());
        if (!plugin.isNull()) {
            parameterDelegate = plugin->parameterDelegate();
        }
    }
    else if (step->action->pluginType() == PluginAction::Analyzer) {
        auto plugin = pluginManager->getAnalyzer(step->action->pluginName());
        if (!plugin.isNull()) {
            parameterDelegate = plugin->parameterDelegate();
        }
    }
    else if (step->action->pluginType() == PluginAction::Importer) {
        auto plugin = pluginManager->getImporterExporter(step->action->pluginName());
        if (!plugin.isNull()) {
            parameterDelegate = plugin->importParameterDelegate();
        }
    }
    else if (step->action->pluginType() == PluginAction::Exporter) {
        auto plugin = pluginManager->getImporterExporter(step->action->pluginName());
        if (!plugin.isNull()) {
            parameterDelegate = plugin->exportParameterDelegate();
        }
    }
    m_editor = new BatchEditItemWidget(step->action->pluginName(), parameterDelegate, step->action->parameters());
    if (step->action->pluginType() == PluginAction::Importer) {
        m_editor->setInputEnabled(false);
    }
    else if (step->action->pluginType() == PluginAction::Exporter) {
        m_editor->setOutputEnabled(false);
    }
    else if (step->action->pluginType() == PluginAction::NoAction) {
        m_editor->setInputEnabled(false);
    }
    connect(m_editor, &BatchEditItemWidget::inputPressed, [this]() {
        this->inputPressed(m_id);
    });
    connect(m_editor, &BatchEditItemWidget::outputPressed, [this]() {
        this->outputPressed(m_id);
    });
    m_proxyWidget = new QGraphicsProxyWidget(this);
    m_proxyWidget->setPos(0,0);
    m_proxyWidget->setWidget(m_editor);
    m_proxyWidget->setFocusProxy(this);

    setAcceptHoverEvents(true);

    setFlags(QGraphicsItem::ItemIsMovable
             | QGraphicsItem::ItemIsSelectable
             | QGraphicsItem::ItemIsFocusable);
}

BatchEditItem::~BatchEditItem()
{
    emit deleted(m_id);
}

BatchEditItem *BatchEditItem::fromMimeData(QSharedPointer<HobbitsPluginManager> pluginManager, const QMimeData *data)
{
    QString pluginName;
    PluginAction::PluginType pluginType;

    if (data->hasFormat("text/hobbits/plugin/operator")) {
        pluginName = data->data("text/hobbits/plugin/operator");
        pluginType = PluginAction::Operator;
    }
    else if (data->hasFormat("text/hobbits/plugin/analyzer")) {
        pluginName = data->data("text/hobbits/plugin/analyzer");
        pluginType = PluginAction::Analyzer;
    }
    else if (data->hasFormat("text/hobbits/plugin/importer")) {
        pluginName = data->data("text/hobbits/plugin/importer");
        pluginType = PluginAction::Importer;
    }
    else if (data->hasFormat("text/hobbits/plugin/exporter")) {
        pluginName = data->data("text/hobbits/plugin/exporter");
        pluginType = PluginAction::Exporter;
    }
    else if (data->hasFormat("text/hobbits/batchinput")) {
        pluginName = "Batch Input";
        pluginType = PluginAction::NoAction;
    }
    else {
        return nullptr;
    }

    QSharedPointer<PluginActionBatch::ActionStep> step(
                new PluginActionBatch::ActionStep(QUuid::createUuid(),
                                                  PluginAction::createAction(pluginType, pluginName, QJsonObject())));
    return new BatchEditItem(pluginManager, step);
}

QRectF BatchEditItem::boundingRect() const
{
    return QRectF(0, 0, m_proxyWidget->size().width(), m_proxyWidget->size().height());
}

void BatchEditItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0, 0, boundingRect().width(), boundingRect().height(), Qt::lightGray);

    if (this->hasFocus()) {
        painter->setPen(QPen(QColor(40, 130, 230), 4));
        painter->setBrush(Qt::transparent);

        painter->drawRect(0, 0, boundingRect().width(), boundingRect().height());
    }
}

QSharedPointer<PluginActionBatch::ActionStep> BatchEditItem::toStep() const
{
    auto step = new PluginActionBatch::ActionStep(m_id, PluginAction::createAction(m_pluginType, m_pluginName, m_editor->parameters()));
    step->editorPosition = pos();
    step->inputs = m_inputs;
    return QSharedPointer<PluginActionBatch::ActionStep>(step);
}

QList<QPair<QUuid, int> > BatchEditItem::inputs() const
{
    return m_inputs;
}

QString BatchEditItem::displayString() const
{
    return m_editor->displayString();
}

void BatchEditItem::removeAllInputs()
{
    auto buffer = m_inputs;
    m_inputs.clear();
    for (auto input: buffer) {
        emit inputRemoved(input.first, input.second);
    }
}

void BatchEditItem::removeInput(QUuid id, int outputNumber)
{
    m_inputs.removeAll({id, outputNumber});
    emit inputRemoved(id, outputNumber);
}

void BatchEditItem::addInput(QUuid id, int outputNumber)
{
    m_inputs.append({id, outputNumber});
}

QUuid BatchEditItem::id() const
{
    return m_id;
}

