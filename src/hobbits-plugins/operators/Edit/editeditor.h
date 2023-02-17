#pragma once



#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class EditEditor;
}

class EditEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:

    EditEditor(QSharedPointer<ParameterDelegate> delegate);
    ~EditEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    void adjustMax();
    int getUnitSize();
    void editFromHere(RangeHighlight highlight);
    void overflow();
    void adjust();
    

    Ui::EditEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;

    qint64 m_bitStart;
    qint64 m_bitLength;
    int m_lengthMax;
    int m_startMax;
    QSharedPointer<const BitArray> m_bits;
    QSharedPointer<BitContainerPreview> m_bitContainer;
    bool m_changeBits;

signals:
public slots:

    void adjustToUnit();
    void changeSliderStart();
    void changeSliderLength();
    void changeSpinBoxStart();
    void changeSpinBoxLength();
    void changeTextBox();
    void toggleInsert();
    void setHighlight();

    void spinBoxChange();

};



