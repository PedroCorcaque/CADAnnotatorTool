# CAD Annotator Tool

The developed tool is a 3D data annotation platform that utilizes CAD models as a foundation. Its goal is to generate synthetically annotated 3D data for training semantic segmentation algorithms. Integration with CAD models allows intuitive semantic annotation in 3D environments. The tool provides 3D manipulation features, offering an efficient solution to create synthetic datasets crucial for training semantic segmentation algorithms.

## Necessary libraries

### OpenCASCADE Technology (7.7.0)

- Download the "tgz" file on https://dev.opencascade.org/release, than:
```
cd ~/Downloads
tar xf opencascade-7.7.0.tgz
cd opencascade-7.7.0
mkdir build && cd build
cmake ..
make
sudo make install
```

### QT Framework

[Get and Install Qt with Qt Online Installer](https://doc.qt.io/qt-6/qt-online-installation.html#:~:text=Download%20Qt%20Online%20Installer,for%20your%20Qt%20account%20credentials.)

## Contributors
@LuisMilczarek - [fixed race condition](https://github.com/PedroCorcaque/CADAnnotatorTool/commit/b4b05ea0b2ecebe03674c7554ca24b326bef2e2d)
