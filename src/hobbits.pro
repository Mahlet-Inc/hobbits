TEMPLATE = subdirs

SUBDIRS += \
    gui \
    core \
    widgets \
    python \
    runner \
    plugins

core.subdir = hobbits-core
gui.subdir = hobbits-gui
plugins.subdir = hobbits-plugins
runner.subdir = hobbits-runner
python.subdir = hobbits-python
widgets.subdir = hobbits-widgets

python.depends = core
widgets.depends = core
runner.depends = core python widgets
plugins.depends = core python widgets
gui.depends = core python widgets
