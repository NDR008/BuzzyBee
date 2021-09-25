::---------------------------------------------------------------
:: NAME			- Images to header
:: DESCRIPTION	- Converts images in a folder to a single header
:: YEAR			- 2018
:: AUTHOR 		- Wituz (Frederik W.)
:: WEBSITE 		- http://wituz.com
:: VERSION		- 1.0
::---------------------------------------------------------------

@echo off
setlocal enabledelayedexpansion
echo -----------------------------------
echo  CONVERING IMAGES TO A HEADER FILE
echo -----------------------------------

:: Delete existing files and re-create the working directories
if exist "images\8bit" rmdir /s /q "images\8bit"
if exist "images\headers" rmdir /s /q "images\headers"
mkdir "images\8bit"
mkdir "images\headers"

:: Delete the current images.h
echo Creating images.h
if exist "images.h" del "images.h"

:: Convert the images to 8-bit
:: for %%i in (images/*.*) do (
:: 	.\bin\convert.exe images/%%i -depth 4 images\8bit\img_%%i 
:: 	echo Converted %%i to 8-bit .BMP
:: )

SET currentX=320
SET currentY=0
SET maxHeight=0
SET canvasWidth=1024
SET canvasHeight=512
SET index=0


echo Converting TIM files to header

:: Convert the TIM images to C headers
for %%i in (images/tim/*.*) do (
	.\bin\bin2h.exe images\tim\%%i images\headers\%%~ni.h %%~ni -nosize 
	echo Converted %%i to a C header
)

:: Write image width and height to images.h
for %%i in (images/8bit/*.*) do (
	FOR /F "tokens=* USEBACKQ" %%F IN (`.\bin\magick identify -format "%%w" images\8bit\%%i`) DO (
		SET width=%%F
	)
	FOR /F "tokens=* USEBACKQ" %%F IN (`.\bin\magick identify -format "%%h" images\8bit\%%i`) DO (
		SET height=%%F
	)
	SET rawname=%%i
	echo unsigned short !rawname:~0,-4!_width = !width!; >> images.h
	echo unsigned short !rawname:~0,-4!_height = !height!; >> images.h
)

:: Write the actual images to images.h
echo Merging headers to one file
for /r "images\headers" %%F in (*.h) do (
	type "%%F" >> images.h
)

set /A imageCount = !index! + 1
echo.
echo -----------------------------------
echo              SUCCESS
echo -----------------------------------
echo images.h contains !imageCount! images
echo Header images.h successfully generated

:: Delete the working directories
::if exist "images\tim" rmdir /s /q "images\tim"
if exist "images\8bit" rmdir /s /q "images\8bit"
::if exist "images\tim" rmdir /s /q "images\tim"
if exist "images\headers" rmdir /s /q "images\headers"

PAUSE

