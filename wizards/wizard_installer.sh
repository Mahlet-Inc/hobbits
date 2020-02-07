#!/bin/bash

# make the wizard directory if it doesn't exist
mkdir -p $HOME/.config/QtProject/qtcreator/templates/wizards

# remove any old wizards
rm -rf $HOME/.config/QtProject/qtcreator/templates/wizards/*plugin

# copy new wizards to wizard directory
cp -r operatorplugin $HOME/.config/QtProject/qtcreator/templates/wizards
cp -r displayplugin $HOME/.config/QtProject/qtcreator/templates/wizards
cp -r analyzerplugin $HOME/.config/QtProject/qtcreator/templates/wizards
cp -r importexportplugin $HOME/.config/QtProject/qtcreator/templates/wizards
