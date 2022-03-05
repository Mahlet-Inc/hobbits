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
        {PARAM_KSY, ParameterDelegate::ParameterType::String, true},
        {PARAM_PY, ParameterDelegate::ParameterType::String, true}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const Parameters &parameters) {
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
                    });
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
        const Parameters &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
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

    QString kscOutput = "";

    QString pyDepDir = dir.filePath("pydeps");

    QString baseName = "";

    if (parameters.contains(PARAM_KSY)
            && !parameters.value(PARAM_KSY).toString().isEmpty()) {
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
        ksy.write(parameters.value(PARAM_KSY).toString().toLocal8Bit());
        ksy.close();

        HobbitsPython::recursiveDirCopy(":/kaitaidata/ksy/common", dir.filePath("common"));

        progress->setProgressPercent(20);

    #ifdef Q_OS_WIN
        QStringList kscAgs = {"/C", kscPath, "--debug", "--ksc-json-output", "-t", "python", ksy.fileName()};
    #else
        QStringList kscAgs = {"--debug", "--ksc-json-output", "-t", "python", "-I", ".", ksy.fileName()};
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
            kscOutput += QString("ksc stderr:\n%2").arg(errorOutput);
        }

        if (stdoutFile.open(QIODevice::ReadOnly)) {
            stdOutput = stdoutFile.readAll();
            stdoutFile.close();
            kscOutput += QString("ksc stdout:\n%2").arg(stdOutput);
        }

        // Check for unexpected stderr
        if (!errorOutput.isEmpty()) {
            return AnalyzerResult::error(QString("kaitai-struct-compiler error:\n%1").arg(kscOutput));
        }
        // Otherwise, parse the JSON
        QJsonObject json = QJsonDocument::fromJson(stdOutput.toLatin1()).object();
        for (QString key : json.keys()) {
            if (!json.value(key).isObject()) {
                continue;
            }
            QJsonObject obj = json.value(key).toObject();

            if (obj.contains("firstSpecName") && obj.value("firstSpecName").isString()) {
                baseName = obj.value("firstSpecName").toString();
            }

            if (!obj.contains("errors") || !obj.value("errors").isArray()) {
                continue;
            }

            QString errorString = "KSC Error:\n";
            QJsonArray arr = obj.value("errors").toArray();
            for (QJsonValue val: arr) {
                if (!val.isObject()) {
                    continue;
                }
                QJsonObject valObj = val.toObject();

                QString errFile = valObj.value("file").toString();
                QString errLoc = "";
                QString errMsg = valObj.value("message").toString();

                if (valObj.contains("path") && valObj.value("path").isArray()) {
                    QJsonArray errPathArr = valObj.value("path").toArray();
                    QStringList errPathStrings;
                    for (QJsonValue pathVal: errPathArr) {
                        errPathStrings.append(pathVal.toString());
                    }
                    errLoc += " " + errPathStrings.join("/");
                }

                if (valObj.contains("line")) {
                    errLoc += QString(" line:%1").arg(valObj.value("line").toInt());
                }

                if (valObj.contains("col")) {
                    errLoc += QString(" col:%1").arg(valObj.value("col").toInt());
                }
                
                errorString += QString("%1%2 - %3\n").arg(errFile).arg(errLoc).arg(errMsg);
            }
            return AnalyzerResult::error(errorString);
        }

    }
    else if (parameters.contains(PARAM_PY)
             && !parameters.value(PARAM_PY).toString().isEmpty()) {
        QString precompiledFilePath = parameters.value(PARAM_PY).toString();
        QFileInfo info(precompiledFilePath);
        baseName = info.baseName();
        QFile::copy(precompiledFilePath, dir.filePath(info.fileName()));
        HobbitsPython::recursiveDirCopy(":/kaitaidata/ksy_py/common", dir.path());
    }
    else {
        return AnalyzerResult::error("Invalid parameters - must have a precompiled py or a custom ksy spec");
    }

    progress->setProgressPercent(40);

    HobbitsPython::recursiveDirCopy(":/kaitaistruct/scripts/dependencies", pyDepDir);

    auto pyRequest = PythonRequest::create(":/kaitaistruct/scripts/runner.py")->setFunctionName("parse_data");
    pyRequest->addPathExtension(pyDepDir);
    pyRequest->addArg(PythonArg::qString(inputBitFile.fileName()));
    pyRequest->addArg(PythonArg::qString(outputRangeFile.fileName()));
    pyRequest->addArg(PythonArg::qString(baseName));
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
    QMap<QString, QSharedPointer<KsField>> fieldMap;
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
        auto field = QSharedPointer<KsField>(new KsField);
        fieldMap.insert(label, field);
        field->label = label;

        if (s.contains("start") && s.contains("end") && s.value("start").isDouble() && s.value("end").isDouble()) {
            Range range(qint64(s.value("start").toDouble())*8, qint64(s.value("end").toDouble())*8 - 1);
            field->range = range;
        }

        if (s.contains("value")) {
            field->value = s.value("value").toVariant().toString();
        }

        if (s.contains("type")) {
            field->type = s.value("type").toVariant().toString();
        }

        if (!s.contains("parent") || s.value("parent").toString().isEmpty()) {
            topLevel.append(label);
        }
        else if (fieldMap.contains(s.value("parent").toString())) {
            fieldMap[s.value("parent").toString()]->children.append(label);
        }
    }

    int colorIdx = 0;
    for (auto label : topLevel) {
        highlights.append(makeHighlight(label, fieldMap, colorIdx));
    }

    QSharedPointer<BitInfo> bitInfo = BitInfo::copyFromContainer(container);
    bitInfo->clearHighlightCategory(KAITAI_STRUCT_CATEGORY);
    bitInfo->addHighlights(highlights);

    if (!kscOutput.isEmpty()) {
        bitInfo->setMetadata(KSC_OUT_LABEL, kscOutput);
    }
    if (!output.isEmpty()) {
        bitInfo->setMetadata(PYTHON_OUT_LABEL, output);
    }

    return AnalyzerResult::result(bitInfo, parameters);
}

RangeHighlight KaitaiStruct::makeHighlight(QString label, const QMap<QString, QSharedPointer<KsField>> &fieldData, int &colorIdx)
{
    QList<QColor> colors = {
        QColor(100, 220, 100, 200),
        QColor(100, 0, 255, 200),
        QColor(0, 150, 230, 200),
        QColor(200, 140, 0, 200),
        QColor(250, 50, 0, 200)
    };
    auto field = fieldData.value(label);
    if (field->children.isEmpty()) {
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, field->range, colors.at(colorIdx).rgba(), {}, {field->type, field->value});
        colorIdx = (colorIdx + 1) % colors.size();
        return highlight;
    }
    else {
        int parentColorIndex = colorIdx;
        colorIdx = 0;
        QList<RangeHighlight> children;
        for (auto child : field->children) {
            children.append(makeHighlight(child, fieldData, colorIdx));
        }
        colorIdx = parentColorIndex;
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, children, colors.at(colorIdx).rgba(), {field->type});
        colorIdx = (colorIdx + 1) % colors.size();
        return highlight;
    }
}
