#ifndef BATCHEDITITEMWIDGET_H
#define BATCHEDITITEMWIDGET_H

#include <QWidget>
#include "hobbitspluginmanager.h"
#include "hobbits-widgets_global.h"

namespace Ui {
class BatchEditItemWidget;
}

class HOBBITSWIDGETSSHARED_EXPORT BatchEditItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatchEditItemWidget(QString title, QSharedPointer<ParameterDelegate> delegate, Parameters parameters);
    ~BatchEditItemWidget();

    Parameters parameters() const;
    QString displayString() const;

public slots:
    void setInputEnabled(bool enabled);
    void setOutputEnabled(bool enabled);

signals:
    void inputPressed();
    void outputPressed();
    void parametersChanged(Parameters);

private slots:
    void on_pb_inputs_clicked();
    void on_pb_outputs_clicked();
    void on_pb_config_clicked();
    void on_pb_clear_clicked();

    void setParameters(const Parameters &parameters);

private:
    Ui::BatchEditItemWidget *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
    Parameters m_parameters;
};

#endif // BATCHEDITITEMWIDGET_H
