#include "kaitaistruct.h"
#include "kaitaistructform.h"
#include "settingsmanager.h"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTemporaryDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QJsonArray>
#include <QDirIterator>
#include "hobbitspython.h"


KaitaiStruct::KaitaiStruct()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"katai_struct_yaml", QJsonValue::String, true},
        {"precompiled_py_file", QJsonValue::String, true}
    };

    m_delegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    infos,
                    [](const QJsonObject &parameters) {
                        if (parameters.contains("kaitai_struct_yaml")
                                && !parameters.value("kaitai_struct_yaml").toString().isEmpty()) {
                            return QString("Custom Kaitai Parse");
                        }
                        else if (parameters.contains("precompiled_py_file")
                                 && !parameters.value("precompiled_py_file").toString().isEmpty()) {
                            QFileInfo info(parameters.value("precompiled_py_file").toString());
                            return QString("Kaitai Parse '%1'").arg(info.baseName());
                        }
                        return QString();
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new KaitaiStructForm(delegate);
                    }));
}

AnalyzerInterface* KaitaiStruct::createDefaultAnalyzer()
{
    return new KaitaiStruct();
}

QString KaitaiStruct::name()
{
    return "Kaitai Struct";
}

QString KaitaiStruct::description()
{
    return "Parses data using precompiled or custom Kaitai Struct specs";
}

QStringList KaitaiStruct::tags()
{
    return {"Generic", "Integration"};
}

QSharedPointer<ParameterDelegate> KaitaiStruct::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> KaitaiStruct::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    if (!m_delegate->validate(parameters)) {
        return AnalyzerResult::error("Invalid parameters given to plugin");
    }

    progress->setProgressPercent(2);

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return AnalyzerResult::error("Could not allocate a temporary directory");
    }

    QFile inputBitFile(dir.filePath("input_bit_container.bits"));
    QFile outputRangeFile(dir.filePath("parsed_ranges.json"));
    if (!inputBitFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return AnalyzerResult::error("Could not allocate a temporary directory");
    }
    container->bits()->writeTo(&inputBitFile);
    inputBitFile.close();

    progress->setProgressPercent(10);

    if (parameters.contains("kaitai_struct_yaml")
            && !parameters.value("kaitai_struct_yaml").toString().isEmpty()) {
        QString kscPath = SettingsManager::getPrivateSetting(KAITAI_PATH_KEY).toString();
        if (kscPath.isEmpty()) {
            return AnalyzerResult::error("A Kaitai Struct Compiler path must be specified");
        }

        QFile errorFile(dir.filePath("error.log"));
        QFile stdoutFile(dir.filePath("stdout.log"));

        QFile ksy(dir.filePath("custom.ksy"));
        if (!ksy.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            return AnalyzerResult::error("Could not open ksy file for writing");
        }
        ksy.write(parameters.value("katai_struct_yaml").toString().toLocal8Bit());
        ksy.close();

        progress->setProgressPercent(20);

    #ifdef Q_OS_WIN
        QStringList kscAgs = {"/C", kscPath, "--debug", "-t", "python", ksy.fileName()};
    #else
        QStringList kscAgs = {"--debug", "-t", "python", ksy.fileName()};
    #endif
        QProcess kscProcess;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QProcessEnvironment envUpdate;
        envUpdate.insert("PATH", env.value("PATH"));
        kscProcess.setProcessEnvironment(envUpdate);
        kscProcess.setWorkingDirectory(dir.path());
        kscProcess.setStandardErrorFile(errorFile.fileName());
        kscProcess.setStandardOutputFile(stdoutFile.fileName());
    #ifdef Q_OS_WIN
        kscProcess.start("cmd.exe", kscAgs);
    #else
        kscProcess.start(kscPath, kscAgs);
    #endif

        kscProcess.waitForFinished();

        QString stdOutput;
        QString errorOutput;
        if (errorFile.open(QIODevice::ReadOnly)) {
            errorOutput = errorFile.readAll();
            errorFile.close();
        }

        if (stdoutFile.open(QIODevice::ReadOnly)) {
            stdOutput = stdoutFile.readAll();
            stdoutFile.close();
        }

        if (!errorOutput.isEmpty()) {
            return AnalyzerResult::error(QString("kaitai-struct-compiler error:\n%1\n\nstdout: %2").arg(errorOutput).arg(stdOutput));
        }

    }
    else if (parameters.contains("precompiled_py_file")
             && !parameters.value("precompiled_py_file").toString().isEmpty()) {
        QString precompiledFilePath = parameters.value("precompiled_py_file").toString();
        QFileInfo info(precompiledFilePath);
        QFile::copy(precompiledFilePath, dir.filePath(info.fileName()));
    }
    else {
        return AnalyzerResult::error("Invalid parameters - must have a precompiled py or a custom ksy spec");
    }

    progress->setProgressPercent(40);


    QString pyDepDir = dir.filePath("pydeps");
    HobbitsPython::recursiveDirCopy(":/kaitaistruct/scripts/dependencies", pyDepDir);

    auto pyRequest = PythonRequest::create(":/kaitaistruct/scripts/runner.py")->setFunctionName("parse_data");
    pyRequest->addPathExtension(pyDepDir);
    pyRequest->addArg(PythonArg::qString(inputBitFile.fileName()));
    pyRequest->addArg(PythonArg::qString(outputRangeFile.fileName()));
    auto watcher = HobbitsPython::getInstance().runProcessScript(pyRequest, true);
    watcher->watcher()->future().waitForFinished();
    auto result = watcher->result();

    QString output = "";
    bool errors = false;
    if (!result->getStdOut().isEmpty()) {
        output += "Python stdout:\n" + result->getStdOut() + "\n\n";
    }
    if (!result->getStdErr().isEmpty()) {
        errors = true;
        output += "Python stderr:\n" + result->getStdErr() + "\n\n";
    }
    if (!result->errors().isEmpty()) {
        errors = true;
        output += "Other errors:\n" + result->errors().join("\n") + "\n\n";
    }
    if (errors) {
        return AnalyzerResult::error(QString("Failure Running parser in Python:\n%1").arg(output));
    }

    progress->setProgressPercent(85);

    if (!outputRangeFile.exists()) {
        QString errorString = "No analysis file was produced - check the Output tab to see if ksc or python produced any errors.";
        errorString += "\n\nCommon problems to check for:";
        errorString += "\n- Are you pointing to a valid Python 3 with the kaitaistruct package 0.9 installed?";
        errorString += "\n- Are you pointing to a valid kaitai-struct-compiler version 0.9?";
        return AnalyzerResult::error(errorString);
    }

    if (!outputRangeFile.open(QIODevice::ReadOnly)) {
        return AnalyzerResult::error("Could not open analysis file for reading");
    }
    QJsonDocument outputData = QJsonDocument::fromJson((outputRangeFile.readAll()));
    outputRangeFile.close();
    if (!outputData.isObject()) {
        return AnalyzerResult::error("Output analysis file has an invalid format");
    }
    QJsonObject outputObj = outputData.object();
    if (!outputObj.contains("sections") || !outputObj.value("sections").isArray()) {
        return AnalyzerResult::error("Output analysis file doesn't contain a 'sections' specification");
    }
    QList<RangeHighlight> highlights;
    QMap<QString, QPair<Range, QList<QString>>> labelMap;
    QList<QString> topLevel;
    QJsonArray sections = outputObj.value("sections").toArray();
    int sectionNum = 0;
    for (auto section: sections) {
        if (!section.isObject()) {
            continue;
        }
        sectionNum++;
        QJsonObject s = section.toObject();
        QString label = QString("<%1>").arg(sectionNum);
        if (s.contains("label") && s.value("label").isString()) {
            label = s.value("label").toString();
        }
        if (!s.contains("start") || !s.contains("end") || !s.value("start").isDouble() || !s.value("end").isDouble()) {
            labelMap.insert(label, {Range(), {}});
        }
        else {
            Range range(qint64(s.value("start").toDouble())*8, qint64(s.value("end").toDouble())*8 - 1);
            labelMap.insert(label, {range, {}});
        }

        if (!s.contains("parent") || s.value("parent").toString().isEmpty()) {
            topLevel.append(label);
        }
        else if (labelMap.contains(s.value("parent").toString())) {
            labelMap[s.value("parent").toString()].second.append(label);
        }
    }

    int colorIdx = 0;
    for (auto label : topLevel) {
        highlights.append(makeHighlight(label, labelMap, colorIdx));
    }

    QSharedPointer<BitInfo> bitInfo = BitInfo::copyFromContainer(container);
    bitInfo->addHighlights(highlights);

    return AnalyzerResult::result(bitInfo, parameters);
}

RangeHighlight KaitaiStruct::makeHighlight(QString label, const QMap<QString, QPair<Range, QList<QString>>> &rangeData, int &colorIdx)
{
    QList<QColor> colors = {
        QColor(100, 220, 100, 85),
        QColor(100, 0, 255, 50),
        QColor(0, 150, 230, 100),
        QColor(200, 140, 0, 100),
        QColor(250, 50, 0, 100)
    };
    auto pair = rangeData.value(label);
    if (pair.second.isEmpty()) {
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, pair.first, colors.at(colorIdx).rgba());
        colorIdx = (colorIdx + 1) % colors.size();
        return highlight;
    }
    else {
        int parentColorIndex = colorIdx;
        colorIdx = 0;
        QList<RangeHighlight> children;
        for (auto child : pair.second) {
            children.append(makeHighlight(child, rangeData, colorIdx));
        }
        colorIdx = parentColorIndex;
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, children, colors.at(colorIdx).rgba());
        colorIdx = (colorIdx + 1) % colors.size();
        return highlight;
    }
}
