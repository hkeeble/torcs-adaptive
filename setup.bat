@echo off

goto start

/----------------------------------------------------------------------------------------------\
| This batch file is part of TORCS Adaptive, and must be run to set up the folder			   |
| structure before compiling the project. It is also responsible for copying all			   |
| game data into runtime folders.															   |
|																							   |
| TORCS Adaptive is a TORCS modification with procedural tracks.							   |
| It can be found at https://github.com/hkeeble/torcs-adaptive/tree/master					   |
|																							   |
| TORCS, The Open Source Racing Car Simulator, can be found at http://torcs.sourceforge.net/   |
\----------------------------------------------------------------------------------------------/

:start

echo Welcome to TORCS Adaptive source setup!
echo.
echo This must be run for each build configuration before the game can be built
echo in the given build configuration in Visual Studio.
echo.

set /p build=Setup a Debug or Release build? (d/r, or q to exit):%=%

IF %build% == r GOTO correct
IF %build% == d GOTO correct
IF %build% == q GOTO end
echo Please enter a valid build type!
goto start

:correct
IF %build% == r GOTO release
IF %build% == d GOTO debug

:debug
echo Setting up debug build...
set RUNTIME_DIR=runtimed
GOTO begin

:release
@echo Setting up release build...
set RUNTIME_DIR=runtime

:begin
set CARS_DIR=%RUNTIME_DIR%/cars
set CATEGORIES_DIR=%RUNTIME_DIR%/categories
set CFG_DIR=%RUNTIME_DIR%/config
set DATA_DIR=%RUNTIME_DIR%/data
set DRIVERS_DIR=%RUNTIME_DIR%/drivers
set MENU_DIR=%RUNTIME_DIR%/menu
set MODULE_DIR=%RUNTIME_DIR%/modules

set RESULTS_DIR=%RUNTIME_DIR%/results
set SCREENSHOTS_DIR=%RUNTIME_DIR%/screenshots
set TELEMETRY_DIR=%RUNTIME_DIR%/telemetry
set TRACKS_DIR=%RUNTIME_DIR%/tracks
set WHEELS_DIR=%RUNTIME_DIR%/wheels

set SIMU_DIR=%MODULE_DIR%/simu
set TRACK_DIR=%MODULE_DIR%/track

echo 	Creating runtime directories...

REM Create root directories.
IF NOT EXIST "%RUNTIME_DIR%/"		mkdir "%RUNTIME_DIR%/"
IF NOT EXIST "%CARS_DIR%/" 			mkdir "%CARS_DIR%/"
IF NOT EXIST "%CATEGORIES_DIR%/"	mkdir "%CATEGORIES_DIR%/"
IF NOT EXIST "%CFG_DIR%/" 			mkdir "%CFG_DIR%/"
IF NOT EXIST "%DATA_DIR%/" 			mkdir "%DATA_DIR%/"
IF NOT EXIST "%MENU_DIR%/" 			mkdir "%MENU_DIR%/"
IF NOT EXIST "%MODULE_DIR%/"		mkdir "%MODULE_DIR%/"
IF NOT EXIST "%RESULTS_DIR%/" 		mkdir "%RESULTS_DIR%/"
IF NOT EXIST "%SCREENSHOTS_DIR%/" 	mkdir "%SCREENSHOTS_DIR%/"
IF NOT EXIST "%TELEMETRY_DIR%/" 	mkdir "%TELEMETRY_DIR%/"
IF NOT EXIST "%TRACKS_DIR%/" 		mkdir "%TRACKS_DIR%/"
IF NOT EXIST "%WHEELS_DIR%/"		mkdir "%WHEELS_DIR%/"

REM Create module directories.
echo %SIMU_DIR%
IF NOT EXIST "%SIMU_DIR%/" 			mkdir "%SIMU_DIR%/"
IF NOT EXIST "%TRACK_DIR%/"			mkdir "%TRACK_DIR%/"

echo 	Runtime directories created successfully!

REM Create driver directories.
echo 	Copying driver data...
xcopy /E/Y/D "data/drivers" "%DRIVERS_DIR%/*"
echo 	Driver data copied successfully!

REM Copy Data
echo	Copying data...
xcopy /E/Y/D "data/fonts" 	   "%DATA_DIR%/fonts/*"
xcopy /E/Y/D "data/img" 	   "%DATA_DIR%/img/*"
xcopy /E/Y/D "data/music" 	   "%DATA_DIR%/music/*"
xcopy /E/Y/D "data/objects"    "%DATA_DIR%/objects/*"
xcopy /E/Y/D "data/sound" 	   "%DATA_DIR%/sound/*"
xcopy /E/Y/D "data/textures"   "%DATA_DIR%/textures/*"
xcopy /E/Y/D "data/track" 	   "%DATA_DIR%/tracks/*"
xcopy /E/Y/D "data/cars"	   "%RUNTIME_DIR%/cars/*"
xcopy /E/Y/D "data/categories" "%RUNTIME_DIR%/categories/*"
xcopy /E/Y/D "data/config" 	   "%RUNTIME_DIR%/config/*"
xcopy /E/Y/D "data/tracks"	   "%RUNTIME_DIR%/tracks/*"

echo	Data copied successfully!

echo Build setup complete!

:end