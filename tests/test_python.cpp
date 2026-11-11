#include "pythoninterpreter.h"
#include "settingsmanager.h"
#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>
#include <future>
#include <iostream>
#include <vector>

static bool require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << message << '\n';
    }
    return condition;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    if (app.arguments().contains("--invalid-home")) {
        SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY,
                                             QDir::tempPath() + "/hobbits-python-does-not-exist");
        auto result = PythonInterpreter::runProcessScript(PythonRequest::create("unused.py"));
        return require(!result->errors().isEmpty(), "Invalid Python home must return an error") ? 0 : 1;
    }

    // Include an apostrophe and space to exercise safe Python path handling.
    QTemporaryDir directory(QDir::tempPath() + "/hobbits user's scripts-XXXXXX");
    if (!require(directory.isValid(), "Could not create script directory")) {
        return 1;
    }
    QFile helper(directory.filePath("hobbits_test_helper.py"));
    if (!helper.open(QIODevice::WriteOnly)) {
        return 1;
    }
    helper.write("value = 42\n");
    helper.close();
    QFile script(directory.filePath("script.py"));
    if (!script.open(QIODevice::WriteOnly)) {
        return 1;
    }
    script.write(
        "import hobbits_test_helper, sys, hobbits\n"
        "assert hobbits_test_helper.value == 42\n"
        "assert len([p for p in sys.path if 'HobbitsPython' in p]) == 1\n"
        "print('loaded once')\n"
        "for cls in (hobbits.BitArray, hobbits.BitInfo, hobbits.BitContainer, hobbits.DisplayHandle, hobbits.PluginActionProgress):\n"
        "    try:\n"
        "        cls(None)\n"
        "    except TypeError:\n"
        "        pass\n"
        "    else:\n"
        "        raise AssertionError('invalid capsule accepted')\n"
        "pixels = bytearray(8)\n"
        "image = hobbits.ImageBuffer(memoryview(pixels), 2, 1)\n"
        "image.set_bytes(b'abcdefgh')\n"
        "image.set_bytes(b'ABCDEFGH')\n"
        "assert pixels == b'ABCDEFGH'\n"
        "try:\n"
        "    hobbits.ImageBuffer(memoryview(b'abcd'), 1, 1)\n"
        "except TypeError:\n"
        "    pass\n"
        "else:\n"
        "    raise AssertionError('read-only image memory accepted')\n"
        "def run(bits, info):\n"
        "    info.set_metadata('key', 'value')\n"
        "    assert info.get_metadata('key') == 'value'\n"
        "    bits.set(3, True)\n"
        "    assert bits.at(3)\n"
        "    print('finished', end='')\n");
    script.close();

    // Cover both GUI initialization and the first request originating in a worker.
    if (app.arguments().contains("--worker-init")) {
        std::async(std::launch::async, [] { PythonInterpreter::waitForLock(); }).get();
    }
    else {
        PythonInterpreter::waitForLock();
    }
    std::vector<std::future<bool>> workers;
    for (int i = 0; i < 4; ++i) {
        workers.push_back(std::async(std::launch::async, [&directory, &script]() {
            auto bits = QSharedPointer<BitArray>::create(8);
            auto info = BitInfo::create();
            auto request = PythonRequest::create(script.fileName())
                ->setFunctionName("run")
                ->addPathExtension(directory.path())
                ->addArg(PythonArg::bitArray(bits))
                ->addArg(PythonArg::bitInfo(info));
            auto result = PythonInterpreter::runProcessScript(request);
            return require(result->errors().isEmpty(), qPrintable(result->errors().join("\n"))) &&
                   require(result->getStdErr().isEmpty(), qPrintable(result->getStdErr())) &&
                   require(result->getStdOut() == "loaded once\nfinished", "Script executed twice or output was not flushed") &&
                   require(bits->at(3), "Python bit binding failed");
        }));
    }
    bool passed = true;
    for (auto &worker : workers) {
        passed = worker.get() && passed;
    }

    QFile failing(directory.filePath("failing.py"));
    if (!failing.open(QIODevice::WriteOnly)) {
        return 1;
    }
    failing.write("def run():\n    raise RuntimeError('expected failure')\n");
    failing.close();
    auto result = PythonInterpreter::runProcessScript(
        PythonRequest::create(failing.fileName())->setFunctionName("run"));
    passed = require(!result->errors().isEmpty() && result->getStdErr().contains("expected failure"),
                     "Python exceptions must produce a failed result") && passed;
    return passed ? 0 : 1;
}
