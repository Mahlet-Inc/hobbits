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
    const bool bundled = app.arguments().contains("--bundled");
    if (bundled || app.arguments().contains("--system-python")) {
        // Exercise the installed runtime without relying on Python on PATH or
        // the absolute interpreter path recorded on the build machine.
        script.write(
            "import sys, pathlib, encodings, json, ctypes, ssl, sqlite3, bz2, lzma\n"
            "import _ctypes, _ssl, _sqlite3, _bz2, _lzma\n");
        if (bundled) {
            script.write(
                "assert sys.version_info[:2] == (3, 14), sys.version\n"
                "exe_dir = pathlib.Path(sys.executable).resolve().parent\n"
                "root = (exe_dir / ('python' if sys.platform == 'win32' else '../lib/hobbits/python')).resolve()\n"
                "stdlib = root / ('Lib' if sys.platform == 'win32' else 'lib/python3.14')\n"
                "native = root / 'DLLs' if sys.platform == 'win32' else stdlib / 'lib-dynload'\n"
                "assert sys.flags.isolated and sys.flags.ignore_environment\n"
                "assert sys.flags.no_user_site\n"
                "from compression import zstd\n"
                "assert zstd.decompress(zstd.compress(b'hobbits')) == b'hobbits'\n"
                "import importlib.util\n"
                "if importlib.util.find_spec('_tkinter') is not None:\n"
                "    import tkinter\n"
                "    tcl = tkinter.Tcl()\n"
                "    assert tcl.eval('expr 6 * 7') == '42'\n"
                "    assert root in pathlib.Path(tcl.eval('info library')).resolve().parents\n");
        }
        else {
            script.write(
                "assert sys.version_info[:2] == (3, 12), sys.version\n"
                "root = pathlib.Path('/usr')\n"
                "stdlib = root / 'lib/python3.12'\n"
                "native = stdlib / 'lib-dynload'\n");
        }
        script.write(
            "assert pathlib.Path(sys.prefix).resolve() == root, sys.prefix\n"
            "assert pathlib.Path(sys.base_prefix).resolve() == root, sys.base_prefix\n"
            "for module in (encodings, json, ctypes, ssl, sqlite3, bz2, lzma):\n"
            "    assert stdlib in pathlib.Path(module.__file__).resolve().parents, module.__file__\n"
            "for module in (_ctypes, _ssl, _sqlite3, _bz2, _lzma):\n"
            "    assert native in pathlib.Path(module.__file__).resolve().parents, module.__file__\n"
            "assert json.loads('{\"value\": 42}')['value'] == 42\n"
            "assert bz2.decompress(bz2.compress(b'hobbits')) == b'hobbits'\n"
            "assert lzma.decompress(lzma.compress(b'hobbits')) == b'hobbits'\n"
            "with sqlite3.connect(':memory:') as db:\n"
            "    assert db.execute('select 42').fetchone() == (42,)\n"
            "print('Python runtime OK')\n");
        script.close();
        auto result = PythonInterpreter::runProcessScript(PythonRequest::create(script.fileName()));
        return require(result->errors().isEmpty(), qPrintable(result->errors().join("\n") + "\n" + result->getStdErr())) &&
               require(result->getStdErr().isEmpty(), qPrintable(result->getStdErr())) &&
               require(result->getStdOut() == "Python runtime OK\n",
                       "Packaged Python runtime check did not complete") ? 0 : 1;
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
