@echo off
set /p shader="Shader name: "

%VULKAN_SDK%/Bin32/glslangValidator.exe -V %shader% -o %shader%.spv
pause