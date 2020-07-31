TEMPLATE = subdirs

SUBDIRS += \
    gui \
    core \
    python \
    runner \
    plugins

core.subdir = hobbits-core
gui.subdir = hobbits-gui
plugins.subdir = hobbits-plugins
runner.subdir = hobbits-runner
python.subdir = hobbits-python

gui.depends = core python
plugins.depends = core python
runner.depends = core python
