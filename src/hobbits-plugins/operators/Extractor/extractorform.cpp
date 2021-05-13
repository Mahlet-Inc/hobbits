#include "extractorform.h"
#include "ui_extractorform.h"

ExtractorForm::ExtractorForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::ExtractorForm()),
    m_delegate(delegate),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_highlightNav = new HighlightNavigator();
    m_highlightNav->setTitle("");
    ui->sectionLayout->addWidget(m_highlightNav);
    ui->sectionLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    connect(ui->cb_category, &QComboBox::currentTextChanged, m_highlightNav, &HighlightNavigator::setHighlightCategory);


    m_stateHelper->addParameter("highlight_category", [this](QJsonValue value) {
        int category_idx = ui->cb_category->findText(value.toString());
        if (category_idx < 0) {
            return false;
        }
        ui->cb_category->setCurrentIndex(category_idx);
        return true;
    }, [this]() {
        return ui->cb_category->currentText();
    });

    m_stateHelper->addParameter("highlight_label", [this](QJsonValue value) {
        return m_highlightNav->selectRow(value.toString());
    }, [this]() {
        return m_highlightNav->currentlySelectedLabel();
    });

    m_stateHelper->addCheckBoxBoolParameter("extract_before", ui->ck_before);
    m_stateHelper->addCheckBoxBoolParameter("extract_highlight", ui->ck_section);
    m_stateHelper->addCheckBoxBoolParameter("extract_after", ui->ck_after);
}

ExtractorForm::~ExtractorForm()
{
    delete ui;
}

QString ExtractorForm::title()
{
    return "Select Section to Extract";
}

bool ExtractorForm::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters ExtractorForm::parameters()
{
    return m_stateHelper->getParametersFromUi();
}

void ExtractorForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    Parameters uiState = parameters();
    m_previewContainer = container;
    m_highlightNav->setContainer(m_previewContainer);
    if (m_previewContainer.isNull()) {
        return;
    }
    ui->cb_category->clear();
    for (QString category: m_previewContainer->info()->highlightCategories()) {
        ui->cb_category->addItem(category);
    }
    setParameters(uiState);
}

void ExtractorForm::setHighlightCategory(QString category)
{
    m_highlightNav->setHighlightCategory(category);
}
