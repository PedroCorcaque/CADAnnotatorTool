#pragma once

#include <QMainWindow>
#include <QWidget>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Handle(AIS_InteractiveContext) myAISContext;
    Handle(V3d_Viewer) myViewer;
    Handle(V3d_View) myView;

    void setupOpenCASCADE();
};

