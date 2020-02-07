TEMPLATE = subdirs

SUBDIRS += \
    gui \
    core \
    runner \
    plugins

core.subdir = hobbits-core
gui.subdir = hobbits-gui
plugins.subdir = hobbits-plugins
runner.subdir = hobbits-runner

gui.depends = core
plugins.depends = core
runner.depends = core
