// Copyright (c) 2021 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "OcctQtViewer.h"
#include "OcctLabelTools.hpp"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QSurfaceFormat>

#include <QFileDialog> // To open the models
#include <QPushButton> // Buttons
#include <QVBoxLayout> // Buttons
#include <QAction>
#include <QLabel>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <Standard_WarningsRestore.hxx>

#include <Standard_Version.hxx>

// occt includes
#include <TDocStd_Document.hxx>
#include <STEPCAFControl_Writer.hxx>

// System includes
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

//! Main application window.
class MyMainWindow : public QMainWindow
{
  OcctQtViewer* myViewer;
public:
  MyMainWindow() : myViewer (nullptr)
  {
    QWidget* buttonWidget = new QWidget();
    QListWidget* listWidget = new QListWidget();
    listWidget->setMaximumWidth(120);
    listWidget->setStyleSheet(QString(" border-radius: 10px; "));
    {
      QVBoxLayout* theLayoutButton = new QVBoxLayout(buttonWidget);
      
      QPushButton* theNextButton = new QPushButton ("Next", buttonWidget);
      theNextButton->setVisible(false);
      
      theLayoutButton->addWidget (theNextButton, 0, Qt::AlignVCenter | Qt::AlignRight);
      connect (theNextButton, &QPushButton::clicked, [this]()
      {
        currentIndexEntity++;

        myViewer->DisplayXCafDocumentByPart(true, currentIndexEntity);
      });
    
      QPushButton* thePrevButton = new QPushButton ("Previous", buttonWidget);
      thePrevButton->setVisible(false);

      theLayoutButton->addWidget (thePrevButton, 0, Qt::AlignVCenter | Qt::AlignLeft);
      connect (thePrevButton, &QPushButton::clicked, [this]()
      {
        currentIndexEntity--;

        myViewer->DisplayXCafDocumentByPart(true, currentIndexEntity);
      });
    
      QMenuBar* aMenuBar = new QMenuBar();
      {
        // menu bar with Quit item
        QMenu* aMenuWindow = aMenuBar->addMenu ("&File");
        {
          QAction* anActionOpen = new QAction (aMenuWindow);
          anActionOpen->setText ("Open step file");
          aMenuWindow->addAction (anActionOpen);
          connect (anActionOpen, &QAction::triggered, [this]()
          {
            QString theFilePath = QFileDialog::getOpenFileName(this, "Open STEP file", "", "STEP Files (*.stp *.step)");

            if (!theFilePath.isEmpty()) 
            {
              TCollection_AsciiString theModelPath(theFilePath.toStdString().c_str());
              
              bool isOpened = myViewer->OpenStep(theModelPath);
              if (!isOpened)
              {
                QMessageBox::information(0, "Open STEP file", QString()
                                        + "Error opening the file '"
                                        + theFilePath
                                        + "'\n");
              }
              else
              {
                myViewer->DumpXCafDocumentTree();
                myViewer->DisplayXCafDocument(true);
              }
            }
          });
        }
        {
          QAction* anActionSave = new QAction (aMenuWindow);
          anActionSave->setText ("Save as step");
          aMenuWindow->addAction (anActionSave);
          connect (anActionSave, &QAction::triggered, [this]()
          {
            QString theFilePath = QFileDialog::getSaveFileName(this, "Save STEP File", "", "STEP Files (*.stp *.step)");

            if (!theFilePath.isEmpty())
            {
              myViewer->SaveAsStep(theFilePath.toStdString().c_str());
            }
          });
        }
        {
          QAction* anActionQuit = new QAction (aMenuWindow);
          anActionQuit->setText ("Quit");
          aMenuWindow->addAction (anActionQuit);
          connect (anActionQuit, &QAction::triggered, [this]()
          {
            close();
          });
        }
      }
      {
        QMenu* aMenuWindowView = aMenuBar->addMenu ("&View");
        {
          QAction* anActionViewAll = new QAction (aMenuWindowView);
          anActionViewAll->setText ("View all");
          aMenuWindowView->addAction (anActionViewAll);
          connect (anActionViewAll, &QAction::triggered, [this, theNextButton, thePrevButton]()
          { 
            std::cout << "Button view all" << std::endl;
            myViewer->ClearContext();

            theNextButton->setVisible(false);
            thePrevButton->setVisible(false);

            myViewer->DisplayXCafDocument(true);
          });
        }
        {
          QAction* anActionViewEntity = new QAction (aMenuWindowView);
          anActionViewEntity->setText ("View by entity");
          aMenuWindowView->addAction (anActionViewEntity);
          connect (anActionViewEntity, &QAction::triggered, [this, theNextButton, thePrevButton]()
          {
            std::cout << "Button view by entity" << std::endl;
            myViewer->ClearContext();

            theNextButton->setVisible(true);
            thePrevButton->setVisible(true);

            myViewer->DisplayXCafDocumentByPart(true, currentIndexEntity);
          });
        }
      }
      {
        QMenu* aMenuWindowSetup = aMenuBar->addMenu ("&Setup");
        {
          QAction* anActionAddSetupFile = new QAction (aMenuWindowSetup);
          anActionAddSetupFile->setText ("Add config file");
          aMenuWindowSetup->addAction (anActionAddSetupFile);
          connect (anActionAddSetupFile, &QAction::triggered, [this, listWidget]()
          {
            std::cout << "Button add new setup file" << std::endl;

            QString theFilePath = QFileDialog::getOpenFileName(this, "Open config file", "", "Config files (*.json)");
            if (!theFilePath.isEmpty())
            {
              std::cout << "Config file path: " << theFilePath.toStdString().c_str() << std::endl;

              std::ifstream configFile (theFilePath.toStdString());
              json configData;
              try
              {
                configData = json::parse(configFile);
                configFile.close();
              }
              catch (...)
              {
                QMessageBox::warning(0, "File load error", QString()
                                    + "Error loading config file.\n\n"
                                    + "Check that the file is in the correct format (json).\n");
                configFile.close();
                return;
              }
              
              if (!configData.contains("classes"))
              {
                QMessageBox::critical(0, "Json format error", QString()
                                     + "The format of the json file is wrong.\n"
                                     + "Object 'classes' not found.\n");
              }

              std::vector<OcctLabelTools::Label> theLabels;
              try
              {
                theLabels = OcctLabelTools::createLabelsFromFile(configData["classes"]);

                myViewer->SetLabels(theLabels);
              }
              catch(const std::exception& e)
              {
                QMessageBox::critical(0, "Json format error", QString()
                                      + "Check if the 'classes' array has objects with 'class' name.\n");
              }
              
              for (const auto& theLabel : theLabels)
              {
                QListWidgetItem* item = new QListWidgetItem();
                item->setSizeHint(QSize(200, 50));

                QWidget* aLabelWidget = new QWidget();
                QHBoxLayout* aLabelLayout = new QHBoxLayout(aLabelWidget);

                QLabel* classNameLabel = new QLabel(theLabel.nameAsString.c_str());
                aLabelLayout->addWidget(classNameLabel);

                QLabel* theColorBlock = new QLabel();
                theColorBlock->setFixedSize(20, 20);
                theColorBlock->setStyleSheet(QString("background-color: %1;").arg(Quantity_Color::ColorToHex(theLabel.color).ToCString()));
                aLabelLayout->addWidget(theColorBlock);
                
                item->setSizeHint(aLabelWidget->sizeHint());
                listWidget->addItem(item);
                listWidget->setItemWidget(item, aLabelWidget);

              }
            }
          });
        }
      }
      setMenuBar (aMenuBar);
    }
    {
      // 3D Viewer and some controls on top of it
      myViewer = new OcctQtViewer();
      QVBoxLayout* aLayout = new QVBoxLayout (myViewer);
      aLayout->setDirection (QBoxLayout::BottomToTop);
      aLayout->setAlignment (Qt::AlignBottom);
      {
        QPushButton* aQuitBtn = new QPushButton ("About");
        aLayout->addWidget (aQuitBtn);
        connect (aQuitBtn, &QPushButton::clicked, [this]()
        {
          QMessageBox::information (0, "About Sample", QString()
                                  + "OCCT 3D Viewer sample embedded into Qt Widgets.\n\n"
                                  + "Open CASCADE Technology v." OCC_VERSION_STRING_EXT "\n"
                                  + "Qt v." QT_VERSION_STR "\n\n"
                                  + "OpenGL info:\n"
                                  + myViewer->getGlInfo());
        });
      }

      QWidget* aSliderBox = new QWidget();
      {
        QHBoxLayout* aSliderLayout = new QHBoxLayout (aSliderBox);
        {
          QLabel* aSliderLabel = new QLabel ("Background");
          aSliderLabel->setStyleSheet ("QLabel { background-color: rgba(0, 0, 0, 0); color: white; }");
          aSliderLabel->setGeometry (50, 50, 50, 50);
          aSliderLabel->adjustSize();
          aSliderLayout->addWidget (aSliderLabel);
        }
        {
          QSlider* aSlider = new QSlider (Qt::Horizontal);
          aSlider->setRange (0, 255);
          aSlider->setSingleStep (1);
          aSlider->setPageStep (15);
          aSlider->setTickInterval (15);
          aSlider->setTickPosition (QSlider::TicksRight);
          aSlider->setValue (0);
          aSliderLayout->addWidget (aSlider);
          connect (aSlider, &QSlider::valueChanged, [this](int theValue)
          {
            const float aVal = theValue / 255.0f;
            const Quantity_Color aColor (aVal, aVal, aVal, Quantity_TOC_sRGB);

            //myViewer->View()->SetBackgroundColor (aColor);
            myViewer->View()->SetBgGradientColors (aColor, Quantity_NOC_BLACK, Aspect_GradientFillMethod_Elliptical);
            myViewer->View()->Invalidate();
            myViewer->update();
          });
        }
        // aLayout->addWidget (aSliderBox);
      }
      aLayout->addWidget(aSliderBox);
      aLayout->addWidget(buttonWidget);
      aLayout->addWidget(listWidget, 0, Qt::AlignRight);
      setCentralWidget (myViewer);

      // myViewer->layout()->addWidget(aSliderBox);
      // myViewer->layout()->addWidget(buttonWidget);
      // myViewer->layout()->addWidget(listWidget);
    }
  }
private:
  size_t currentIndexEntity = 0;
};

int main (int theNbArgs, char** theArgVec)
{
  QApplication aQApp (theNbArgs, theArgVec);

  QCoreApplication::setApplicationName ("OCCT Qt Viewer sample");
  QCoreApplication::setOrganizationName ("OpenCASCADE");
  QCoreApplication::setApplicationVersion (OCC_VERSION_STRING_EXT);

#ifdef __APPLE__
  // suppress Qt warning "QCocoaGLContext: Falling back to unshared context"
  bool isCoreProfile = true;
  QSurfaceFormat aGlFormat;
  aGlFormat.setDepthBufferSize   (24);
  aGlFormat.setStencilBufferSize (8);
  if (isCoreProfile) { aGlFormat.setVersion (4, 5); }
  aGlFormat.setProfile (isCoreProfile ? QSurfaceFormat::CoreProfile : QSurfaceFormat::CompatibilityProfile);
  QSurfaceFormat::setDefaultFormat (aGlFormat);
#endif

  MyMainWindow aMainWindow;
  aMainWindow.resize (aMainWindow.sizeHint());
  aMainWindow.show();
  return aQApp.exec();
}
