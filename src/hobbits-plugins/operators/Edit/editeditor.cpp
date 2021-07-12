#include "editeditor.h"
#include "ui_editeditor.h"


enum data { bit, hexa, byte }; //compiler doesn't like the name 'hex' for some reason
data lastType = bit;
QSharedPointer<const BitArray> bits;
QSharedPointer<BitContainerPreview> bitContainer;

EditEditor::EditEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::EditEditor()),
    m_paramHelper(new ParameterHelper(delegate))
{
    

    
    ui->setupUi(this);

    

    ui->sb_length->setMinimum(0);
    ui->sb_start->setMinimum(0);

    //ui->hs_length->setMaximum(2147483647); //can this be set to the bitcontainer size?
    //ui->hs_start->setMaximum(2147483647);

    ui->hs_length->setMinimum(0);
    ui->hs_start->setMinimum(0);

    ui->hs_length->setValue(8);

    
    //On spinbox value change, change the bits shown in pte_bits
    connect(ui->sb_start, SIGNAL(valueChanged(int)), this, SLOT(changeSliderStart()));
    connect(ui->sb_length, SIGNAL(valueChanged(int)), this, SLOT(changeSliderLength()));
    connect(ui->sb_start, SIGNAL(valueChanged(int)), this, SLOT(changeTextBox()));
    connect(ui->sb_length, SIGNAL(valueChanged(int)), this, SLOT(changeTextBox()));
    connect(ui->sb_start, SIGNAL(valueChanged(int)), this, SLOT(setHighlight()));
    connect(ui->sb_length, SIGNAL(valueChanged(int)), this, SLOT(setHighlight()));

    //On slider value change, change the bits shown in pte_bits
    connect(ui->hs_start, SIGNAL(valueChanged(int)), this, SLOT(changeSpinBoxStart()));
    connect(ui->hs_length, SIGNAL(valueChanged(int)), this, SLOT(changeSpinBoxLength()));

    //On radio button value change

    connect(ui->rb_bit, SIGNAL(toggled(bool)), this, SLOT(setLabelText()));
    connect(ui->rb_hex, SIGNAL(toggled(bool)), this, SLOT(setLabelText()));
    connect(ui->rb_ascii, SIGNAL(toggled(bool)), this, SLOT(setLabelText()));

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


    // TODO: Correlate parameters in given delegate to form fields
    // Ex 1.
    // m_paramHelper->addSpinBoxIntParameter("myparametername", ui->spinBox);
    // Ex 2.
    // m_paramHelper->addParameter("otherparameter", [this](QJsonValue value) {
    //     // set the value in the editor
    //     ui->spinBox->setValue(value.toInt());
    //     return true; // return false if unable to set the value in the editor
    // }, [this]() {
    //     // get the QJsonValue from the editor
    //     return QJsonValue(ui->spinBox->value());
    // });
}

void EditEditor::setHighlight() {
    int start = ui->sb_start->value();
    int length = ui->sb_length->value();
    if (ui->rb_hex->isChecked()) {
        start*=4;
        length*=4;
    } else if (ui->rb_ascii->isChecked()) {
        start*=8;
        length*=8;
    }
    quint32 color = 0x553498db;
    Range range(start, start + length - 1);
    if (! bitContainer.isNull()) {
        //clear current highlights
        bitContainer->clearHighlightCategory("edit_highlights");
        bitContainer->addHighlight(RangeHighlight("edit_highlights",
                                        QString("%1 to %2").arg(range.start()).arg(range.end()),
                                        range,
                                        color));
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
    if (! bits.isNull()) {
        qint64 size = bits->sizeInBits();

        int start = ui->sb_start->value();
        int length = ui->sb_length->value();
        
        if (ui->rb_bit->isChecked()) {
            if (start+length > size) {
                length = size - start;
            }
            str = bits->toBin(start, length);
        } else if (ui->rb_hex->isChecked()) {
            if (start+length > size / 4) {
                length = size / 4 - start;
            }
            str = bits->toHex(start, length);
        } else {
            if (start+length > size / 8) {
                length = size / 8 - start;
            }
            str = bits->toAscii(start, length);
        }

        if (ui->rb_hex->isChecked()) {
            size/=4;
        } else if (ui->rb_ascii->isChecked()) {
            size/=8;
        }

        ui->pte_bits->document()->setPlainText(str);
    }
}


void EditEditor::setLabelText() {
    int start = ui->sb_start->value();
    int length = ui->sb_length->value();
    if (ui->rb_bit->isChecked()) {
        ui->lb_start->setText("Bit Start");
        ui->lb_length->setText("Bit Length");
        if (lastType == byte) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits() / 8;
                ui->sb_length->setMaximum(size*8);
                ui->sb_start->setMaximum(size*8);
                ui->hs_length->setMaximum(size*8);
                ui->hs_start->setMaximum(size*8);
            }
            ui->sb_start->setValue(start*8);
            ui->sb_length->setValue(length*8);
        } else if (lastType == hexa) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits() / 4;
                ui->sb_length->setMaximum(size*4);
                ui->sb_start->setMaximum(size*4);
                ui->hs_length->setMaximum(size*4);
                ui->hs_start->setMaximum(size*4);
            }
            ui->sb_start->setValue(start*4);
            ui->sb_length->setValue(length*4);
        }
        lastType = bit;
    } else if (ui->rb_hex->isChecked()) {
        ui->lb_start->setText("Nibble Start");
        ui->lb_length->setText("Nibble Length");
        if (lastType == bit) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits();
                ui->sb_length->setMaximum(size/4);
                ui->sb_start->setMaximum(size/4);
                ui->hs_length->setMaximum(size/4);
                ui->hs_start->setMaximum(size/4);
            }
            ui->sb_start->setValue(start/4);
            ui->sb_length->setValue(length/4);
        } else if (lastType == byte) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits() / 8;
                ui->sb_length->setMaximum(size*2);
                ui->sb_start->setMaximum(size*2);
                ui->hs_length->setMaximum(size*2);
                ui->hs_start->setMaximum(size*2);
            }
            ui->sb_start->setValue(start*2);
            ui->sb_length->setValue(length*2);
        }
        lastType = hexa;
    } else {
        ui->lb_start->setText("Byte Start");
        ui->lb_length->setText("Byte Length");
        if (lastType == bit) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits();
                ui->sb_length->setMaximum(size/8);
                ui->sb_start->setMaximum(size/8);
                ui->hs_length->setMaximum(size/8);
                ui->hs_start->setMaximum(size/8);
            }
            ui->sb_start->setValue(start/8);
            ui->sb_length->setValue(length/8);
        } else if (lastType == hexa) {
            
            if (! bits.isNull()) {
                qint64 size = bits->sizeInBits() / 4;
                ui->sb_length->setMaximum(size/2);
                ui->sb_start->setMaximum(size/2);
                ui->hs_length->setMaximum(size/2);
                ui->hs_start->setMaximum(size/2);
            }
            ui->sb_start->setValue(start/2);
            ui->sb_length->setValue(length/2);
        }
        lastType = byte;
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
    bitContainer = container;

    if (! container.isNull()) {
        QString str;
        bits = container->bits();

        qint64 size = bits->sizeInBits();

        if (ui->rb_hex->isChecked()) {
            size/=4;
        } else if (ui->rb_ascii->isChecked()) {
            size/=8;
        }

        ui->sb_length->setMaximum(size);
        ui->sb_start->setMaximum(size);
        ui->hs_length->setMaximum(size);
        ui->hs_start->setMaximum(size);

        int start = ui->sb_start->value();
        int length = ui->sb_length->value();

        if (ui->rb_bit->isChecked()) {
            str = bits->toBin(start, length);
        } else if (ui->rb_hex->isChecked()) {
            str = bits->toHex(start, length);
        } else {
            str = bits->toAscii(start, length);
        }

        //setHighlight();
        //if rangeHighlight in edit_highlights length == -1
        RangeHighlight highlight;
        QList<RangeHighlight> highlights = container->info()->highlights("edit_highlights");
        for (RangeHighlight h : highlights) {
            highlight = h;
        }
        if (highlight.range().end() == -1) {
            int start2;
            if (ui->rb_hex->isChecked()) {
                start2 = highlight.range().start() / 4;
            } else if (ui->rb_ascii->isChecked()) {
                start2 = highlight.range().start() / 8;
            } else {
                start2 = highlight.range().start();
            }
            //set spinbox start to highlight start
            ui->sb_start->setValue(start2);
            //highlight length will set to spinbox length automatically
        }
        //always set edit highlight
        else if (highlight.range().isNull()) {
            //getto way of setting range highlight
            ui->sb_start->setValue(start+1); 
            ui->sb_start->setValue(start);
        }

        
        
        
        ui->pte_bits->document()->setPlainText(str);
    } else {
        //clear pte_bits
        ui->pte_bits->document()->setPlainText("");
    }
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
