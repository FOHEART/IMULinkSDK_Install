@echo off
chcp 65001 >nul

echo [1/2] 安装运行时依赖到 libs\ ...
pip install vtk==9.6.0 numpy==2.4.2 matplotlib==3.10.8 Pillow==12.1.1 ^
    pyparsing==3.3.2 python-dateutil==2.9.0.post0 packaging==26.0 ^
    fonttools==4.61.1 contourpy==1.3.3 cycler==0.12.1 kiwisolver==1.4.9 six==1.17.0 ^
    --target libs

echo.
echo [2/2] 安装开发工具到系统 Python（pyinstaller、pybind11）...
pip install pyinstaller==6.19.0 pybind11==2.13.6

echo.
echo Done.
pause
