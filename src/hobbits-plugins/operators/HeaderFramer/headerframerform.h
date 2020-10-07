#ifndef HEADERFRAMERFORM_H
#define HEADERFRAMERFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class HeaderFramerForm;

}
class HeaderFramerForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    HeaderFramerForm(QSharedPointer<ParameterDelegate> delegate);
    ~HeaderFramerForm() override;

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

private slots:
    void validateHeader(QString header);
    void addHeader();
    void checkSelectedHeader();
    void removeHeader();
    void showSpinBoxes();

private:
    QString getHeaderString();

    Ui::HeaderFramerForm *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
};

#endif // HEADERFRAMERFORM_H
