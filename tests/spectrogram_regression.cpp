#include "bitcontainermanager.h"
#include "displayhandle.h"
#include "displayinterface.h"
#include "displayresult.h"
#include <QApplication>
#include <QPluginLoader>
#include <iostream>
#include <stdexcept>

namespace {

void require(bool condition, const char *message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

}

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    try {
        require(argc == 2, "Expected the Spectrogram plugin path");
        QPluginLoader loader(QString::fromLocal8Bit(argv[1]));
        auto display = qobject_cast<DisplayInterface *>(loader.instance());
        if (!display) {
            throw std::runtime_error(loader.errorString().toStdString());
        }
        auto manager = QSharedPointer<BitContainerManager>::create();
        auto handle = QSharedPointer<DisplayHandle>::create(manager);
        display->setDisplayHandle(handle);
        Parameters parameters(QJsonObject{
            {"sample_format", "uint8"}, {"fft_overlap", 0}, {"fft_size", 32},
            {"data_type", 1}, {"sensitivity", 1}, {"sample_rate", 1000},
            {"show_headers", false}, {"show_slices", true}, {"logarithmic_scaling", true}
        });
        const QSize viewport(640, 480);
        auto selectData = [&](const QByteArray &bytes) {
            const auto container = BitContainer::create(bytes);
            manager->addContainer(container);
            manager->selectContainer(container);
        };
        selectData(QByteArray());
        require(display->renderDisplay(viewport, parameters, {})->getImage().isNull(),
                "An empty container must not index a nonexistent frame");

        selectData(QByteArray(32, '\0'));
        const auto rendered = display->renderDisplay(viewport, parameters, {});
        require(rendered->errorString().isEmpty(), "Unable to render an exact FFT window");
        require(!rendered->getImage().isNull() && qAlpha(rendered->getImage().pixel(0, 0)) != 0,
                "An exact FFT window was skipped");

        // While a larger FFT is pending, the overlay still holds the old 16-bin
        // spectrum. Its hover slice must index those bins, not the new FFT size.
        handle->setMouseHover(display, QPoint(320, 1));
        parameters.insert("fft_size", 16384);
        require(display->renderOverlay(viewport, parameters)->errorString().isEmpty(),
                "Changing FFT size invalidated the previous spectrum's slice");

        parameters.insert("fft_size", 32);
        parameters.insert("sample_format", "uint64_le");
        selectData(QByteArray(32 * 8, '\x7f'));
        require(display->renderDisplay(viewport, parameters, {})->errorString().isEmpty(),
                "Real uint64 samples must not read an imaginary sample out of bounds");

        parameters.insert("sample_format", "uint8");
        parameters.insert("data_type", 2);
        selectData(QByteArray(7, '\x7f'));
        const auto first = display->renderDisplay(viewport, parameters, {})->getImage();
        const auto second = display->renderDisplay(viewport, parameters, {})->getImage();
        require(!first.isNull() && first == second, "Partial I/Q frames must be zero-padded deterministically");

        parameters.insert("fft_overlap", 100);
        parameters.insert("sample_rate", 0.01);
        parameters.insert("show_headers", true);
        require(display->renderOverlay(viewport, parameters)->errorString().isEmpty(),
                "Valid UI overlap and sample-rate bounds were rejected");

        for (const QJsonValue invalid : {QJsonValue(0), QJsonValue(-32), QJsonValue(2.5)}) {
            parameters.insert("fft_size", invalid);
            require(!display->renderDisplay(viewport, parameters, {})->errorString().isEmpty(),
                    "Invalid FFT size was accepted");
            require(!display->renderOverlay(viewport, parameters)->errorString().isEmpty(),
                    "Overlay accepted an invalid FFT size");
        }
        parameters.insert("fft_size", 32);
        parameters.insert("sample_format", "unknown");
        require(!display->renderOverlay(viewport, parameters)->errorString().isEmpty(),
                "Unknown sample format was accepted");
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "Spectrogram regressions passed\n";
}
