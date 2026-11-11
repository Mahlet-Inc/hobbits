#include "widthframerform.h"
#include "ui_widthframerform.h"
#include "mathparser.h"
#include <QMetaObject>
#include <QVBoxLayout>
#include <QtGlobal>
#include "../../common/ffttransform.h"

WidthFramerForm::WidthFramerForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::WidthFramerForm()),
    m_delegate(delegate),
    m_peakSelector(new PeakSelector()),
    m_listModel(new QStringListModel(this))
{
    ui->setupUi(this);


    QVBoxLayout *layout = new QVBoxLayout();
    ui->tab->setLayout(layout);
    layout->addWidget(m_peakSelector);
    layout->addWidget(m_peakSelector->getZoomSlider());
    layout->addWidget(m_peakSelector->getHScroll());
    ui->lv_correlations->setModel(m_listModel);

    connect(ui->sb_width, SIGNAL(returnPressed()), this, SIGNAL(accepted()));
    connect(m_peakSelector, SIGNAL(peakSelected(QPointF)), this, SLOT(getPeak(QPointF)));

    connect(ui->lv_correlations, SIGNAL(clicked(QModelIndex)), this, SLOT(widthSelected(QModelIndex)));
    connect(ui->rb_all, SIGNAL(toggled(bool)), this, SLOT(setupScoreList(bool)));
    connect(ui->rb_top100, SIGNAL(toggled(bool)), this, SLOT(setupScoreList(bool)));
}

WidthFramerForm::~WidthFramerForm()
{
    delete m_peakSelector;
    delete ui;
}

QString WidthFramerForm::title()
{
    return "Configure Bit Width";
}

bool WidthFramerForm::setParameters(const Parameters &parameters)
{
    if (!m_delegate->validate(parameters).isEmpty()) {
        return false;
    }

    ui->sb_width->setText(QString("%1").arg(parameters.value("width").toInt()));

    return true;
}

Parameters WidthFramerForm::parameters()
{
    Parameters parameters = Parameters::nullParameters();
    MathParser mp;
    MathParser::ParseResult a = mp.parseInput(ui->sb_width->text());
    int frameWidth;

    if (a.isValid()) {
        frameWidth = int(a.getResult());
    }
    else {
        return parameters;
    }

    parameters.insert("width", frameWidth);

    return parameters;
}

bool sortPoints(
        const QPointF &a,
        const QPointF &b)
{
    return (b.y() < a.y());
}

void WidthFramerForm::previewBitsImpl(QSharedPointer<BitContainerPreview> container, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    if (container.isNull()) {
        m_pendingAutocorrelation.clear();
    }
    else {
        m_pendingAutocorrelation = autocorrelate(container->bits());
    }
    m_pendingSortedAutocorrelation = m_pendingAutocorrelation;
    std::sort(m_pendingSortedAutocorrelation.begin(), m_pendingSortedAutocorrelation.end(), sortPoints);
}

void WidthFramerForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
    m_autocorrelation = m_pendingAutocorrelation;
    m_sortedAutocorrelation = m_pendingSortedAutocorrelation;
    m_peakSelector->setData(m_autocorrelation);
    setupScoreList();
}

void WidthFramerForm::getPeak(QPointF peak)
{
    int value = int(peak.x());
    QString value_string = QString::number(value);
    ui->sb_width->setText(value_string);
    emit accepted();
}

void WidthFramerForm::setupScoreList(bool toggled)
{
    if (!toggled) {
        return;
    }
    QStringList pointList;
    if (ui->rb_top100->isChecked()) {
        for (int i = 0; i < 100 && i < m_sortedAutocorrelation.size(); i++) {
            pointList << QString("%1 : %2").arg(m_sortedAutocorrelation.at(i).x()).arg(m_sortedAutocorrelation.at(i).y());
        }
    }
    else {
        for (QPointF point : m_sortedAutocorrelation) {
            pointList << QString("%1 : %2").arg(point.x()).arg(point.y());
        }
    }
    m_listModel->setStringList(pointList);
}

void WidthFramerForm::widthSelected(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }

    if (index.row() >= 0 && index.row() < m_sortedAutocorrelation.size()) {
        ui->sb_width->setText(QString("%1").arg(m_sortedAutocorrelation.at(index.row()).x()));
        emit accepted();
    }
}

QVector<QPointF> WidthFramerForm::autocorrelate(QSharedPointer<const BitArray> bits)
{
    if (bits.isNull() || bits->sizeInBits() == 0) {
        return {};
    }
    constexpr int N = 1 << 19;
    try {
        std::vector<std::complex<float>> input(N);
        for (int i = 0; i < N && i < bits->sizeInBits(); ++i) {
            input[i] = bits->at(i) ? 1.0f : -1.0f;
        }
        const auto correlation = HobbitsDsp::autocorrelation(input);
        QVector<QPointF> results(N / 2);
        results[0] = QPointF(0, 0);
        for (int i = 1; i < N / 2; ++i) {
            results[i] = QPointF(i, correlation[i]);
        }
        return results;
    }
    catch (const std::exception &) {
        // Preview failures must not escape into Qt's event dispatch.
        return {};
    }
}
