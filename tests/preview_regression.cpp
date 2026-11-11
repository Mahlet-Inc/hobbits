#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QSemaphore>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <atomic>
#include <iostream>
#include <stdexcept>
#include "abstractparametereditor.h"

struct PreviewState
{
    std::atomic<int> workerCalls{0};
    std::atomic<int> uiCalls{0};
    std::atomic<bool> uiActive{false};
    std::atomic<bool> overlap{false};
    bool attemptObserved = false;
    QSemaphore startWorker;
    QSemaphore workerAttempted;
    QSemaphore workerEntered;
};

class PreviewEditor : public AbstractParameterEditor
{
public:
    PreviewEditor(PreviewState &state, bool exerciseOverlap) :
        m_state(state), m_exerciseOverlap(exerciseOverlap) {}
    QString title() override { return "Preview regression"; }
    bool setParameters(const Parameters &) override { return true; }
    Parameters parameters() override { return Parameters::nullParameters(); }

protected:
    void previewBitsImpl(QSharedPointer<BitContainerPreview>,
                         QSharedPointer<PluginActionProgress>) override
    {
        if (++m_state.workerCalls == 2) {
            m_state.overlap = m_state.uiActive.load();
            m_state.workerEntered.release();
        }
    }

    void previewBitsUiImpl(QSharedPointer<BitContainerPreview>) override
    {
        if (++m_state.uiCalls != 1 || !m_exerciseOverlap) {
            return;
        }
        m_state.uiActive = true;
        // Start the next real preview while the first UI callback consumes its results.
        m_state.startWorker.release();
        m_state.attemptObserved = m_state.workerAttempted.tryAcquire(1, 5000);
        // With proper serialization this worker cannot enter until this callback returns.
        m_state.workerEntered.tryAcquire(1, 100);
        m_state.uiActive = false;
    }

private:
    PreviewState &m_state;
    bool m_exerciseOverlap;
};

static void require(bool condition, const char *message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    try {
        QThreadPool workers;
        workers.setMaxThreadCount(2);
        PreviewState state;
        PreviewEditor editor(state, true);
        QSemaphore firstStarted;
        auto first = QtConcurrent::run(&workers, [&editor, &firstStarted]() {
            firstStarted.release();
            editor.previewBits({}, {});
        });
        // QFuture::waitForFinished may otherwise execute an unstarted task on this UI thread.
        firstStarted.acquire();
        first.waitForFinished();
        auto second = QtConcurrent::run(&workers, [&editor, &state]() {
            state.startWorker.acquire();
            state.workerAttempted.release();
            editor.previewBits({}, {});
        });
        QCoreApplication::sendPostedEvents(&editor, QEvent::MetaCall);
        second.waitForFinished();
        QCoreApplication::sendPostedEvents(&editor, QEvent::MetaCall);
        require(state.attemptObserved, "second preview worker did not start");
        require(state.workerCalls == 2 && state.uiCalls == 2, "preview callbacks were lost");
        require(!state.overlap, "preview worker modified results while the UI consumed them");

        PreviewState deletedState;
        auto deletedEditor = new PreviewEditor(deletedState, false);
        QSemaphore pendingStarted;
        auto pending = QtConcurrent::run(&workers, [deletedEditor, &pendingStarted]() {
            pendingStarted.release();
            deletedEditor->previewBits({}, {});
        });
        pendingStarted.acquire();
        pending.waitForFinished();
        delete deletedEditor;
        QCoreApplication::sendPostedEvents(nullptr, QEvent::MetaCall);
        require(deletedState.uiCalls == 0, "queued preview callback outlived its editor");
        workers.waitForDone();
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "Preview serialization and queued lifetime regressions passed\n";
    return 0;
}
