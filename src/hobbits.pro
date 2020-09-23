TEMPLATE = subdirs

SUBDIRS += \
    gui \
    core \
    hobbits-widgets \
    python \
    runner \
    plugins

core.subdir = hobbits-core
gui.subdir = hobbits-gui
plugins.subdir = hobbits-plugins
runner.subdir = hobbits-runner
python.subdir = hobbits-python

python.depends = core
gui.depends = core python
plugins.depends = core python
runner.depends = core python
