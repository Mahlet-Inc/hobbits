#ifndef WIDTHFRAMERFORM_H
#define WIDTHFRAMERFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "peakselector.h"
#include <QStringListModel>

namespace Ui
{
class WidthFramerForm;
}

class WidthFramerForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    WidthFramerForm(QSharedPointer<ParameterDelegate> delegate);
    ~WidthFramerForm() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

    int getAutoWidth(QSharedPointer<BitArray> bits, int start, int end);
    QVector<QPointF> autocorrelate(QSharedPointer<const BitArray> bits);

public slots:
    void getPeak(QPointF);
    void setupScoreList(bool toggled = true);

private slots:
    void widthSelected(QModelIndex index);

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::WidthFramerForm *ui;
    QSharedPointer<ParameterDelegate> m_delegate;

    PeakSelector *m_peakSelector;
    // Shared with the worker only while AbstractParameterEditor holds its preview mutex.
    QVector<QPointF> m_pendingAutocorrelation;
    QVector<QPointF> m_pendingSortedAutocorrelation;
    // These buffers are accessed exclusively by the UI thread.
    QVector<QPointF> m_autocorrelation;
    QVector<QPointF> m_sortedAutocorrelation;
    QStringListModel *m_listModel;
};

#endif // WIDTHFRAMERFORM_H
