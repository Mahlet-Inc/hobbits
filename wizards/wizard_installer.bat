rem make the wizard directory if it doesn't exist
mkdir %APPDATA%\QtProject\qtcreator\templates\wizards

rem remove any old wizards
del /s /q "%APPDATA%\QtProject\qtcreator\templates\wizards\*.*

rem # copy new wizards to wizard directory
xcopy importexportplugin %APPDATA%\QtProject\qtcreator\templates\wizards\importexportplugin\ /Y/H/E
xcopy operatorplugin %APPDATA%\QtProject\qtcreator\templates\wizards\operatorplugin\ /Y/H/E
xcopy displayplugin %APPDATA%\QtProject\qtcreator\templates\wizards\displayplugin\ /Y/H/E
xcopy analyzerplugin %APPDATA%\QtProject\qtcreator\templates\wizards\analyzerplugin\ /Y/H/E
