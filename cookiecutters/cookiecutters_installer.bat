rem make the cookiecutter directory if it doesn't exist
mkdir %USERPROFILE%\.cookiecutters

rem # copy new cookiecutters to cookiecutter directory
xcopy importexportplugin %APPDATA%\QtProject\qtcreator\templates\wizards\importexportplugin\ /Y/H/E
xcopy operatorplugin %APPDATA%\QtProject\qtcreator\templates\wizards\operatorplugin\ /Y/H/E
xcopy displayplugin %APPDATA%\QtProject\qtcreator\templates\wizards\displayplugin\ /Y/H/E
xcopy analyzerplugin %APPDATA%\QtProject\qtcreator\templates\wizards\analyzerplugin\ /Y/H/E
