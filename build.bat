@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"


SET includes=/IHeaders /I%VULKAN_SDK%/Include
SET links=/link /LIBPATH:%VULKAN_SDK%/Lib vulkan-1.lib /LIBPATH:/Lib 
SET defines=/D DEBUG

echo "Building ..."

cl /EHsc %includes% %defines% src/main.cpp %links%

