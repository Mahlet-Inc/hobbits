#include "findform.h"
#include "ui_findform.h"
#include "settingsmanager.h"

FindForm::FindForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::FindForm()),
    m_highlightNav(new HighlightNavigator()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(m_highlightNav);
    ui->verticalLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    m_highlightNav->setShouldHighlightSelection(true);
    m_highlightNav->setHighlightCategory(FOUND_HIGHLIGHT);

    m_paramHelper->addLineEditStringParameter("search_string", ui->le_searchString);

    connect(ui->le_searchString, SIGNAL(returnPressed()), this, SIGNAL(accepted()));

    // Initialize default color settings
    QVariant findColor = SettingsManager::getPluginSetting(FIND_COLOR_KEY);
    if (!findColor.isValid() || findColor.isNull() || !findColor.canConvert<QColor>()) {
        findColor = QColor(0, 150, 230, 85);
    }
    SettingsManager::setPluginSetting(FIND_COLOR_KEY, findColor);
}

FindForm::~FindForm()
{
    delete ui;
}

QString FindForm::title()
{
    return "Configure Find";
}

QJsonObject FindForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

void FindForm::giveDisplayHandle(QSharedPointer<DisplayHandle> handle)
{
    m_highlightNav->giveDisplayHandle(handle);
}

bool FindForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void FindForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    m_highlightNav->setContainer(container);
    if (container.isNull()) {
        m_highlightNav->setTitle("");
    }
    else {
        QString resultLabel = container->info()->metadata(FOUND_RESULT_LABEL).toString();
        if (resultLabel.size() > 28) {
            resultLabel.truncate(25);
            resultLabel += "...";
        }
        m_highlightNav->setTitle(resultLabel);
    }
}
