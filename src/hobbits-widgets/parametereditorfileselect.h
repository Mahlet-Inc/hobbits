#ifndef PARAMETEREDITORFILESELECT_H
#define PARAMETEREDITORFILESELECT_H

#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include <QFileDialog>
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT ParameterEditorFileSelect : public AbstractParameterEditor
{
    Q_OBJECT

public:
    ParameterEditorFileSelect(
            QFileDialog::AcceptMode acceptMode,
            QString fileKey = "filename",
            QString editorTitle = "Select File");
    ~ParameterEditorFileSelect() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;
    bool isStandaloneDialog() override;

private:
    QFileDialog *m_fileDialog;
    QString m_fileKey;
    QString m_editorTitle;
};

#endif // PARAMETEREDITORFILESELECT_H
