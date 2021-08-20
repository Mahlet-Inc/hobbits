#include "widthframerform.h"
#include "ui_widthframerform.h"
#include "mathparser.h"
#include <QMetaObject>
#include <QVBoxLayout>
#include <QtGlobal>
#include <fftw3.h>
#include <ctime>
#include "pffft.h"


#include <iostream>
using namespace std;

WidthFramerForm::WidthFramerForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::WidthFramerForm()),
    m_delegate(delegate),
    m_peakSelector(new PeakSelector()),
    m_listModel(new QStringListModel())
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
        m_autocorrelation = QVector<QPointF>();
    }
    else {
        m_autocorrelation = autocorrelate(container->bits());
    }
    m_sortedAutocorrelation = m_autocorrelation;
    std::sort(m_sortedAutocorrelation.begin(), m_sortedAutocorrelation.end(), sortPoints);
}

void WidthFramerForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
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
    //left shift
    int N = 1 << 19;

    //create set up for PFFFT
    PFFFT_Setup *setup = pffft_new_setup(N, PFFFT_COMPLEX);

    //allocate the arrays, or "float buffers," for input, output, and work
    float *input = (float*)pffft_aligned_malloc(N * 2 * sizeof(float));
    float *output = (float*)pffft_aligned_malloc(N * 2 * sizeof(float));
    float *work= (float*)pffft_aligned_malloc(N * 2 * sizeof(float));

    //prepare and run first FFT (with PFFFT)
    for (int i = 0; i < N; i++){
        input[i*2] = 0; //real
        input[i*2+1] = 0; //imaginary
        if (i < bits->sizeInBits()) {
            input[i*2] = bits->at(i) ? 1 : -1;
        }
        output[i*2] = 0;
        output[i*2+1]= 0;
    }

    //clock_t start2 = clock();
    //run the first FFT (with PFFFT)
    pffft_transform_ordered(setup, input, output, work, PFFFT_FORWARD);
    /*clock_t end2 = clock();
    double cpu_time_used2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
    //cout << "\n" << "here is the time (PFFFT1): " << cpu_time_used2 << endl;
    */
  
   //prepare and run second FFT
    for (int i = 0; i < N; i++) {
        float re = output[i*2];
        float im = output[i*2+1];
        input[i*2] = (re * re + im * im) / static_cast<float>(N);
        input[i*2+1] = 0.0;
    }

    //clock_t start4 = clock();
    //perform the second FFT (with PFFFT)
    pffft_transform_ordered(setup, input, output, work, PFFFT_BACKWARD);
    /*clock_t end4 = clock();
    double cpu_time_used4 = ((double) (end4 - start4)) / CLOCKS_PER_SEC;
    cout << "\n" << "here is the time in width framer (PFFFT): " << cpu_time_used4 << endl;
    */

    // get results
    QVector<QPointF> results(N / 2);
    results.insert(0, QPointF(0, 0));
    for (int i = 1; i < N / 2; i++) {
        double re = qAbs(double(output[i*2] / double(N)));
        //store in results vector
        results[i] = QPointF(i, re);
    }

    // clean up
    pffft_aligned_free(work);
    pffft_aligned_free(output);
    pffft_aligned_free(input);
    pffft_destroy_setup(setup);
   
    return results;
}
