#ifndef KAITAISTRUCTFORM_H
#define KAITAISTRUCTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "highlightnavigator.h"

const QString KAITAI_PATH_KEY = "kaitai_struct_compiler_path";
const QString KSY_PATH_KEY = "ksy_directory_path";
const QString KAITAI_STRUCT_CATEGORY = "kaitai_struct";
const QString KAITAI_RESULT_LABEL = "kaitai_struct_result_label";

namespace Ui
{
class KaitaiStructForm;
}

class KaitaiStructForm: public AbstractParameterEditor
{
    Q_OBJECT

public:
    KaitaiStructForm(QSharedPointer<ParameterDelegate> delegate);
    ~KaitaiStructForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

    void giveDisplayHandle(QSharedPointer<DisplayHandle> handle) override;

private slots:
    void openKscPathDialog();
    void updateOutputText(QString text);

private:
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::KaitaiStructForm *ui;
    QMenu* m_loadKsyMenu;
    QMenu* m_loadKsyPyMenu;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<ParameterHelper> m_paramHelper;
    QString m_selectedPrecompiledFile;
};

#endif // KAITAISTRUCTFORM_H
