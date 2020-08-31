#include "algorithm"
#include "cmath"
#include "math.h"
#include "mathparser.h"
#include "parseresult.h"
#include "ui_widthframer.h"
#include "widthframer.h"
#include <QVBoxLayout>

WidthFramer::WidthFramer() :
    ui(new Ui::WidthFramer()),
    m_peakSelector(nullptr),
    m_listModel(new QStringListModel())
{
#ifdef FFTW_AUTOCORRELATION
    m_fftSize = 1 << 19;
    m_fft_in = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(m_fftSize)));
    m_fft_out = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(m_fftSize)));

    m_fft_plan1 = fftw_plan_dft_1d(m_fftSize, m_fft_in, m_fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
    m_fft_plan2 = fftw_plan_dft_1d(m_fftSize, m_fft_in, m_fft_out, FFTW_BACKWARD, FFTW_ESTIMATE);
#endif
}

WidthFramer::~WidthFramer()
{
#ifdef FFTW_AUTOCORRELATION
    fftw_destroy_plan(m_fft_plan1);
    fftw_destroy_plan(m_fft_plan2);
    fftw_free(m_fft_in);
    fftw_free(m_fft_out);
#endif

    if (m_peakSelector) {
        delete m_peakSelector;
    }
    delete ui;
}

AnalyzerInterface* WidthFramer::createDefaultAnalyzer()
{
    return new WidthFramer();
}

QString WidthFramer::getName()
{
    return "Width Framer";
}

void WidthFramer::applyToWidget(QWidget *widget)
{
    if (!m_peakSelector) {
        m_peakSelector = new PeakSelector();
        connect(m_peakSelector, SIGNAL(peakSelected(QPointF)), this, SLOT(getPeak(QPointF)));
    }
    ui->setupUi(widget);
    QVBoxLayout *layout = new QVBoxLayout();
    ui->tab->setLayout(layout);
    layout->addWidget(m_peakSelector);
    layout->addWidget(m_peakSelector->getZoomSlider());
    layout->addWidget(m_peakSelector->getHScroll());
    ui->lv_correlations->setModel(m_listModel);

    connect(ui->sb_width, SIGNAL(returnPressed()), this, SLOT(requestRun()));

    connect(ui->lv_correlations, SIGNAL(clicked(QModelIndex)), this, SLOT(widthSelected(QModelIndex)));
    connect(ui->rb_all, SIGNAL(toggled(bool)), this, SLOT(setupScoreList(bool)));
    connect(ui->rb_top100, SIGNAL(toggled(bool)), this, SLOT(setupScoreList(bool)));
}

void WidthFramer::widthSelected(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }

    if (index.row() >= 0 && index.row() < m_autocorrelation.size()) {
        ui->sb_width->setText(QString("%1").arg(m_autocorrelation.at(index.row()).x()));
        requestRun();
    }
}

bool sortPoints(
        const QPointF &a,
        const QPointF &b)
{
    return (b.y() < a.y());
}

void WidthFramer::previewBits(QSharedPointer<BitContainerPreview> container)
{

    if (container.isNull()) {
        m_peakSelector->setData(QVector<QPointF>());
    }
    else {
        m_autocorrelation = autocorrelate(container->bits());
        m_peakSelector->setData(m_autocorrelation);

        std::sort(m_autocorrelation.begin(), m_autocorrelation.end(), sortPoints);
        setupScoreList();
    }

}

void WidthFramer::setupScoreList(bool toggled)
{
    if (!toggled) {
        return;
    }
    QStringList pointList;
    if (ui->rb_top100->isChecked()) {
        for (int i = 0; i < 100 && i < m_autocorrelation.size(); i++) {
            pointList << QString("%1 : %2").arg(m_autocorrelation.at(i).x()).arg(m_autocorrelation.at(i).y());
        }
    }
    else {
        for (QPointF point : m_autocorrelation) {
            pointList << QString("%1 : %2").arg(point.x()).arg(point.y());
        }
    }
    m_listModel->setStringList(pointList);
}

void WidthFramer::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of analyzeBits
    m_pluginCallback = pluginCallback;
}

void WidthFramer::requestRun()
{
    if (m_pluginCallback.isNull()) {
        return;
    }

    m_pluginCallback->requestAnalyzerRun(getName(), getStateFromUi());
}

QJsonObject WidthFramer::getStateFromUi()
{
    QJsonObject pluginState;
    MathParser mp;
    ParseResult a = mp.parseInput(ui->sb_width->text());
    int frameWidth;

    if (a.isValid()) {
        frameWidth = a.getResult();
    }
    else {
        QJsonObject errorState;
        errorState.insert("error", QString("Invalid width value: '%1'").arg(ui->sb_width->text()));
        return errorState;
    }

    pluginState.insert("width", frameWidth);

    return pluginState;
}

bool WidthFramer::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->sb_width->setText(QString("%1").arg(pluginState.value("width").toInt()));

    return true;
}

bool WidthFramer::canRecallPluginState(const QJsonObject &pluginState)
{
    return pluginState.contains("width") && pluginState.value("width").isDouble();
}

QSharedPointer<const AnalyzerResult> WidthFramer::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{

    if (!canRecallPluginState(recallablePluginState)) {
        return AnalyzerResult::error("Invalid parameters passed to plugin");
    }

    qint64 frameWidth = recallablePluginState.value("width").toInt();
    QList<Range> frames;

    QSharedPointer<const BitArray> bits = container->bits();

    for (qint64 i = 0; i < bits->sizeInBits(); i += frameWidth) {
        qint64 width = qMin(frameWidth - 1, bits->sizeInBits() - i - 1);
        frames.append(Frame(bits, i, i + width));

        progressTracker->setProgress(i, bits->sizeInBits());
        if (progressTracker->getCancelled()) {
            return AnalyzerResult::error("Processing cancelled");
        }
    }

    QSharedPointer<BitInfo> bitInfo = container->bitInfo()->copyMetadata();

    bitInfo->setFrames(frames);

    return AnalyzerResult::result(bitInfo, recallablePluginState);
}

void WidthFramer::getPeak(QPointF peak)
{
    int value = int(peak.x());
    QString value_string = QString::number(value);
    ui->sb_width->setText(value_string);
    requestRun();
}

QVector<QPointF> WidthFramer::autocorrelate(QSharedPointer<const BitArray> bits)
{
#ifdef FFTW_AUTOCORRELATION
    int N = m_fftSize;

    // prepare and run first FFT
    for (int i = 0; i < N; i++) {
        m_fft_in[i][0] = 0;
        m_fft_in[i][1] = 0;
        if (i < bits->sizeInBits()) {
            m_fft_in[i][0] = bits->at(i) ? 1 : -1;
        }
        m_fft_out[i][0] = 0;
        m_fft_out[i][1] = 0;
    }
    fftw_execute(m_fft_plan1);

    // prepare and run second FFT
    for (int i = 0; i < N; i++) {
        double re = m_fft_out[i][0];
        double im = m_fft_out[i][1];
        m_fft_in[i][0] = (re * re + im * im) / static_cast<double>(N);
        m_fft_in[i][1] = 0.0;
    }
    fftw_execute(m_fft_plan2);

    // get results
    QVector<QPointF> results(N / 2);
    results.insert(0, QPointF(0, 0));
    for (int i = 1; i < N / 2; i++) {
        const double re = abs(m_fft_out[i][0] / static_cast<double>(N));
        results[i] = QPointF(i, re);
    }

    // clean up

    return results;
#else
    return QVector<QPointF>();
#endif
}
