#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QSettings>
#include <QTemporaryDir>
#include <QVariant>
#include <iostream>
#include <stdexcept>
#include "bitcontainermanager.h"
#include "displayhandle.h"
#include "displayhelper.h"
#include "rangesequence.h"
#include "rangehighlight.h"
#include "settingsmanager.h"

static void require(bool condition, const char *message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

static Range selection(QSharedPointer<BitContainer> container)
{
    const auto highlights = container->info()->highlights("mouse_selection", "selection_1");
    return highlights.isEmpty() ? Range() : highlights.first().range();
}

static void testHighlightMetaTypeStreaming()
{
    const auto child = RangeHighlight::simple("child", "payload", Range(8, 15), 0xff123456);
    const RangeHighlight original("category", "label", Range(0, 31), 0xffabcdef,
            {child}, {"tag"});
    const QVariant value = QVariant::fromValue(original);
    require(value.metaType().hasRegisteredDataStreamOperators(),
            "highlight stream operators must be available to Qt metatypes across libraries");

    QByteArray serialized;
    QDataStream output(&serialized, QIODevice::WriteOnly);
    output << value;
    require(output.status() == QDataStream::Ok, "highlight QVariant serialization failed");

    QVariant restored;
    QDataStream input(serialized);
    input >> restored;
    require(input.status() == QDataStream::Ok && restored.metaType() == value.metaType(),
            "highlight QVariant deserialization failed");
    const auto result = restored.value<RangeHighlight>();
    require(result.category() == original.category() && result.label() == original.label()
            && result.range().start() == 0 && result.range().end() == 31
            && result.color() == original.color() && result.tags() == original.tags(),
            "highlight serialization changed its metadata");
    require(result.children().size() == 1 && result.children().first().label() == child.label()
            && result.children().first().range().start() == 8
            && result.children().first().range().end() == 15,
            "highlight serialization lost its children");
}

static void testMouseSelection()
{
    auto manager = QSharedPointer<BitContainerManager>::create();
    auto handle = QSharedPointer<DisplayHandle>::create(manager);
    QObject displayObject;
    // Exercise the same signal/coordinate conversion path used by raster and text views.
    DisplayHelper::connectHoverUpdates(nullptr, &displayObject, handle,
            [](QPoint &offset, QSize &symbolSize, int &grouping, int &bits) {
        offset = QPoint(20, 20);
        symbolSize = QSize(10, 10);
        grouping = 1;
        bits = 1;
        return true;
    });

    handle->startMouseSelect(nullptr, QPoint(25, 55)); // No current container.
    auto container = BitContainer::create(QByteArray(3, '\xff'), 20);
    container->info()->setFrames(RangeSequence::fromConstantSize(8, 20));
    manager->addContainer(container);
    manager->selectContainer(container);
    require(container->frameCount() == 3, "selection fixture must have three frames");

    handle->startMouseSelect(nullptr, QPoint(25, 55)); // Frame 3: used to throw Invalid range index '3'.
    handle->setBitHover(true, 0, 0);
    require(selection(container).isNull(), "click below the data must not begin a selection");
    handle->startMouseSelect(nullptr, QPoint(19, 25)); // Inside the left header.
    require(selection(container).isNull(), "header click must not select data");
    handle->startMouseSelect(nullptr, QPoint(65, 45)); // Beyond the short final frame.
    require(selection(container).isNull(), "click past the final frame width must be ignored");

    handle->setOffsets(2, 1);
    handle->startMouseSelect(nullptr, QPoint(25, 25));
    require(selection(container).start() == 10 && selection(container).end() == 10,
            "click selection must include both scroll offsets");
    handle->endMouseSelect(nullptr, QPoint(25, 25));

    handle->setOffsets(0, 0);
    handle->setSelecting(true, 0, 2, 8);
    require(selection(container).start() == 16 && selection(container).end() == 19,
            "selection must clip a partial final symbol to available bits");
    handle->setSelecting(true, 0, 0, 0);
    handle->setBitHover(true, 0, 0); // Invalid chunk sizes must not divide by zero.

    auto other = BitContainer::create(QByteArray(1, '\0'), 8);
    manager->addContainer(other);
    manager->selectContainer(other);
    handle->setBitHover(true, 0, 0);
    require(selection(other).isNull(), "switching containers must end an existing selection");

    require(DisplayHelper::getOffset(0, -1, 0, 10, 10, 1, 1) == QPoint(-1, -1),
            "negative pixel coordinates must not round toward a valid cell");
    require(DisplayHelper::getOffset(0, 0, 0, 0, 10, 1, 1) == QPoint(-1, -1),
            "zero-sized symbols must not divide by zero");
}

static void verifyRanges(QSharedPointer<RangeSequence> ranges)
{
    qint64 start = 0;
    for (qint64 i = 0; i < ranges->size(); ++i) {
        const qint64 width = i % 7 + 1;
        const Range range = ranges->at(i);
        require(range.start() == start && range.size() == width, "cached frame boundaries changed");
        require(ranges->indexOf(start, Range()) == i, "frame lookup returned the wrong frame");
        start += width;
    }
    require(ranges->indexOf(start, Range()) == -1, "end-of-data lookup must be rejected");
    require(ranges->indexOf(0, Range(1, ranges->size() - 1)) == -1, "lookup must obey lower bounds");
}

static void testRangeCache()
{
    {
        auto growing = RangeSequence::createEmpty();
        for (int i = 0; i < 10001; ++i) {
            growing->appendRange(8);
        }
        require(growing->at(0).size() == 8, "rewinding after cache growth changed the first frame");
        // This used to free the first cache block twice on destruction.
    }
    auto ranges = RangeSequence::createEmpty();
    // Cross multiple cache growth boundaries and exceed the five resident cache blocks.
    for (qint64 i = 0; i < 60003; ++i) {
        ranges->appendRange(i % 7 + 1);
    }
    verifyRanges(ranges);
    verifyRanges(RangeSequence::fromOther(ranges));

    QByteArray serialized;
    QDataStream output(&serialized, QIODevice::WriteOnly);
    ranges->serialize(output);
    QDataStream input(serialized);
    verifyRanges(RangeSequence::deserialize(input));
    require(input.status() == QDataStream::Ok, "range serialization failed");

    const auto constant = RangeSequence::fromConstantSize(8, 20);
    require(constant->indexOf(0, Range(1, 2)) == -1, "constant frames must respect index bounds");
    require(constant->indexOf(19, Range()) == 2, "partial final frame lookup failed");
}

static void testPluginPathsAndSettings()
{
    QTemporaryDir temporary;
    require(temporary.isValid(), "temporary settings directory unavailable");
    const QString config = temporary.filePath("settings.ini");
    {
        QSettings settings(config, QSettings::IniFormat);
        settings.setValue("Plugin Loader/Plugin Path", QString());
    }
    SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, temporary.path());
    SettingsManager::setConfigFilePath(config);
    require(SettingsManager::getTransientSetting(SettingsManager::PYTHON_HOME_KEY).toString() == temporary.path(),
            "choosing a config file must preserve command-line Python overrides");
    SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, QString());
    const QString absolutePath = temporary.filePath("custom plugins");
    const QString extras = absolutePath + QDir::listSeparator() + QString("../plugins")
            + QDir::listSeparator() + QString("~/hobbits-test-plugins");
    const QStringList paths = SettingsManager::pluginPaths(extras);
    require(paths.first() == absolutePath, "absolute plugin paths must remain absolute");
    require(paths.contains(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../plugins"))
            || paths.contains(QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../plugins")),
            "a saved empty plugin path must retain build-tree defaults");
    require(paths.contains(QDir::homePath() + "/hobbits-test-plugins"), "home plugin path was not expanded");
    auto unique = paths;
    unique.removeDuplicates();
    require(unique == paths, "plugin locations must be deduplicated");

    SettingsData defaults;
    SettingsData copied(defaults);
    require(copied.getPluginLoaderSetting(SettingsManager::PLUGIN_PATH_KEY).isValid(),
            "copying uninitialized settings must preserve lazy defaults");
    copied.setTransientSetting(SettingsManager::PYTHON_HOME_KEY, absolutePath);
    SettingsData assigned;
    assigned = copied;
    require(assigned.getTransientSetting(SettingsManager::PYTHON_HOME_KEY).toString() == absolutePath,
            "settings copies must preserve the Python home override");
    require(defaults.getTransientSetting(SettingsManager::PYTHON_HOME_KEY).toString().isEmpty(),
            "Python home must default to normal interpreter discovery");
}

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    try {
        testHighlightMetaTypeStreaming();
        testMouseSelection();
        testRangeCache();
        testPluginPathsAndSettings();
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "Hobbits regression tests passed\n";
    return 0;
}
