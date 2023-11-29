#include "MainWindow.hpp"
#include "ui_mainwindow.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupOpenCASCADE();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupOpenCASCADE()
{
    myViewer = new V3d_Viewer();
    myView = myViewer->CreateView();
    myAISContext = new AIS_InteractiveContext(myViewer);

    // Create a simple box
    BRepPrimAPI_MakeBox aBox(gp_Pnt(-50, -50, -50), 100, 100, 100);
    Handle(AIS_Shape) anAisBox = new AIS_Shape(aBox.Shape());
    myAISContext->Display(anAisBox, Standard_True);

    // Set up the view
    myView->SetBackgroundColor(Quantity_NOC_BLACK);
    myView->MustBeResized();
    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

    // Set up the QT widget
    QWidget *aContainer = QWidget::createWindowContainer(myView, this);
    setCentralWidget(aContainer);
}
