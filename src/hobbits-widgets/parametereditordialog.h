#ifndef PARAMETEREDITORDIALOG_H
#define PARAMETEREDITORDIALOG_H

#include <QDialog>
#include "parameterdelegate.h"
#include "hobbits-widgets_global.h"

namespace Ui {
class ParameterEditorDialog;
}

class HOBBITSWIDGETSSHARED_EXPORT ParameterEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterEditorDialog(
            QSharedPointer<ParameterDelegate> delegate,
            Parameters parameters = Parameters(),
            QWidget *parent = nullptr);
    ~ParameterEditorDialog();

    void setParameters(const Parameters &parameters);
    Parameters parameters();

    static Parameters promptForParameters(QSharedPointer<ParameterDelegate> delegate, Parameters parameters = Parameters());

private:
    Ui::ParameterEditorDialog *ui;
    AbstractParameterEditor *m_editor;
};

#endif // PARAMETEREDITORDIALOG_H
