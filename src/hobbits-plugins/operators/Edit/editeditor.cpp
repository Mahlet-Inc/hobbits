#include "editeditor.h"
#include "ui_editeditor.h"

const qint64 START_MAX = 2147483647;
const qint64 LENGTH_MAX = 1024;




EditEditor::EditEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::EditEditor()),
    m_paramHelper(new ParameterHelper(delegate))
{
    m_bitStart = 0;
    m_bitLength = 8;
    m_changeBits = true; //when false the spinBox values can change without affecting m_bitStart, m_bitLength

    ui->setupUi(this);

    

    ui->sb_length->setMinimum(0);
    ui->sb_start->setMinimum(0);

    ui->hs_length->setMinimum(0);
    ui->hs_start->setMinimum(0);

    ui->sb_length->setMaximum(LENGTH_MAX);
    ui->sb_start->setMaximum(START_MAX);

    ui->hs_length->setMaximum(LENGTH_MAX);
    ui->hs_start->setMaximum(START_MAX);

    ui->sb_start->setValue(0);
    ui->sb_length->setValue(8);
    ui->hs_start->setValue(0);
    ui->hs_length->setValue(8);

    
    //On spinbox value change, change the bits shown in pte_bits
    connect(ui->sb_start, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChange()));
    connect(ui->sb_length, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChange()));


    //On slider value change, change the bits shown in pte_bits
    connect(ui->hs_start, SIGNAL(valueChanged(int)), this, SLOT(changeSpinBoxStart()));
    connect(ui->hs_length, SIGNAL(valueChanged(int)), this, SLOT(changeSpinBoxLength()));

    //On radio button value change

    connect(ui->rb_bit, SIGNAL(clicked()), this, SLOT(adjustToUnit()));
    connect(ui->rb_hex, SIGNAL(clicked()), this, SLOT(adjustToUnit()));
    connect(ui->rb_ascii, SIGNAL(clicked()), this, SLOT(adjustToUnit()));

    //on insert mode change
    connect(ui->cb_insert, SIGNAL(toggled(bool)), this, SLOT(toggleInsert()));



    m_paramHelper->addSpinBoxIntParameter("start", ui->sb_start);
    m_paramHelper->addSpinBoxIntParameter("length", ui->sb_length);

    //m_paramHelper->addLabelParameter("start_label", ui->lb_start);
    //m_paramHelper->addLabelParameter("length_label", ui->lb_length);

    m_paramHelper->addTextEditStringParameter("new_bits_in_range", ui->pte_bits);

    m_paramHelper->addParameter("edit_type", [this](QJsonValue value) {
        if (value.toString() == "bit") {
            ui->rb_bit->setChecked(true);
        }
        else if (value.toString() == "hex") {
            ui->rb_hex->setChecked(true);
        } else {
            ui->rb_ascii->setChecked(true);
        }
        return true;
    }, [this]() {
        if (ui->rb_bit->isChecked()) {
            return QJsonValue("bit");
        }
        else if (ui->rb_hex->isChecked()) {
            return QJsonValue("hex");
        } else {
            return QJsonValue("ascii");
        }
    });

}

void EditEditor::setHighlight() {
    
    quint32 color = 0x553498db;
    Range range(m_bitStart, m_bitStart + m_bitLength - 1);
    if (! m_bitContainer.isNull()) {
        //clear current highlights
        m_bitContainer->clearHighlightCategory("edit_highlights");
        m_bitContainer->addHighlight(RangeHighlight("edit_highlights",
                                        QString("%1 to %2").arg(range.start()).arg(range.end()),
                                        range,
                                        color));
    }
}

void EditEditor::editFromHere(RangeHighlight highlight) {
    
    qint64 start2;

    start2 = highlight.range().start() / getUnitSize();

    //set spinbox start to highlight start
    if (start2 != m_bitStart / getUnitSize()) {
        ui->sb_start->setValue(start2);
        //highlight length will set to spinbox length automatically
    } else {
        setHighlight();
    }
}

void EditEditor::spinBoxChange() {
    if (m_changeBits) {
        m_bitStart = (qint64) ui->sb_start->value() * getUnitSize();
        int l = ui->sb_length->value() * getUnitSize();
        
        //if (m_bitStart + l <= START_MAX) {
        m_bitLength = l;
        /*
        } else {
            ui->sb_length->setValue(m_bitLength);
            return;
        }
        */
        
        
    }
    changeSliderStart();
    changeSliderLength();
    changeTextBox();
    if (!m_bitContainer.isNull()) {
        setHighlight();
    }
    
}

void EditEditor::changeSliderStart() {
    int start = ui->sb_start->value();
    ui->hs_start->setValue(start);
}

void EditEditor::changeSliderLength() {
    int length = ui->sb_length->value();
    ui->hs_length->setValue(length);
}

void EditEditor::changeSpinBoxStart() {
    int start = ui->hs_start->value();
    ui->sb_start->setValue(start);
}

void EditEditor::changeSpinBoxLength() {
    int length = ui->hs_length->value();
    ui->sb_length->setValue(length);
}

void EditEditor::changeTextBox() {
    QString str;
    if (! m_bits.isNull()) {
        qint64 size = m_bits->sizeInBits();

        int unitSize = getUnitSize();

        overflow();

        qint64 start = m_bitStart / unitSize;
        qint64 length = m_bitLength / unitSize;
        
        /*
        if (start + length > START_MAX) {
            length = start + length - START_MAX;
        }
        */

        if (start+length > size / unitSize) {
            length = size / unitSize - start;
        }
        
        if (ui->rb_bit->isChecked()) {          
            str = m_bits->toBin(start, length);
        } else if (ui->rb_hex->isChecked()) {           
            str = m_bits->toHex(start, length);
        } else {
            str = m_bits->toAscii(start, length);
        }

        if (ui->rb_hex->isChecked()) {
            size/=4;
        } else if (ui->rb_ascii->isChecked()) {
            size/=8;
        }

        ui->pte_bits->document()->setPlainText(str);
    }
}

int EditEditor::getUnitSize() {
    if (ui->rb_bit->isChecked()) {
        return 1;
    } else if (ui->rb_hex->isChecked()) {
        return 4;
    } else {
        return 8;
    }
}

void EditEditor::adjustMax() {

    qint64 containerBits = m_bits->sizeInBits();
    qint64 max64 = containerBits / getUnitSize();
    m_startMax = (int) qMin(max64, START_MAX);
    m_lengthMax = (int) qMin(max64, LENGTH_MAX); 

    ui->sb_start->setMaximum(m_startMax);
    ui->hs_start->setMaximum(m_startMax);

    ui->hs_length->setMaximum(m_lengthMax);
    ui->sb_length->setMaximum(m_lengthMax);
}

void EditEditor::overflow() {
    int unitSize = getUnitSize();
    if (m_bitStart + m_lengthMax > m_startMax
    && m_startMax - m_bitStart / unitSize >= 0) {
        m_lengthMax = m_startMax-m_bitStart/unitSize;
        ui->sb_length->setMaximum(m_lengthMax);
    } else {
        adjustMax();
    }
}

void EditEditor::adjust() {
    m_bits = m_bitContainer->bits();
    adjustMax();
    changeTextBox();
    setHighlight();
}

void EditEditor::adjustToUnit() {

    if (m_bits.isNull()) {
        return;
    }

    m_changeBits = false; //don't trigger bitContainer change

    adjustMax();

    int unitSize = getUnitSize();

    qint64 unitStart = m_bitStart / unitSize;
    m_bitStart = unitStart * unitSize;
    
    ui->sb_start->setValue(unitStart);

    qint64 unitLength = m_bitLength / unitSize;
    m_bitLength = unitLength * unitSize;

    ui->sb_length->setValue(unitLength);
    m_changeBits = true;
    
    if (ui->rb_bit->isChecked()) {
        ui->lb_start->setText("Bit Start");
        ui->lb_length->setText("Bit Length");
    } else if (ui->rb_hex->isChecked()) {
        ui->lb_start->setText("Nibble Start");
        ui->lb_length->setText("Nibble Length");
    } else {
        ui->lb_start->setText("Byte Start");
        ui->lb_length->setText("Byte Length");
    }

    if (m_lengthMax == LENGTH_MAX) {
        adjust();
    }
        
    
    
}

void EditEditor::toggleInsert() {
    if (ui->cb_insert->isChecked()) {
        ui->pte_bits->setOverwriteMode(true);
    } else {
        ui->pte_bits->setOverwriteMode(false);
    }
}

EditEditor::~EditEditor()
{
    delete ui;
}

QString EditEditor::title()
{
    // TODO: Make this more descriptive
    return "Configure Edit";
}

Parameters EditEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool EditEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void EditEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void EditEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    //QSharedPointer<BitContainerPreview> oldContainer = m_bitContainer;
    m_bitContainer = container;
  
    if (! container.isNull()) {

        if (m_bitContainer->info()->containsHighlightCategory("edit_highlights")) {
            QList<RangeHighlight> highlights = m_bitContainer->info()->highlights("edit_highlights");
            for (RangeHighlight highlight : highlights) {
                qint64 a = highlight.range().end();
                if (highlight.range().end() == -2) {
                    editFromHere(highlight);
                    break;
                }
                //does this if consume too much pp
                if (m_bits != m_bitContainer->bits()) {
                    adjust();
                }
            }
            
            

        } else {
            adjust();
        }
    } else {
        //clear pte_bits
        ui->pte_bits->document()->setPlainText("");
        m_bits = QSharedPointer<BitArray>();
    }

    

    

    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
