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

#ifndef _OcctQtViewer_HeaderFile
#define _OcctQtViewer_HeaderFile

#include <Standard_WarningsDisable.hxx>
#include <QOpenGLWidget>
#include <QLabel>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <AIS_TextLabel.hxx>

#include "OcctLabelTools.hpp"

class AIS_ViewCube;

//! OCCT 3D View.
class OcctQtViewer : public QOpenGLWidget, public AIS_ViewController
{
  Q_OBJECT
public:

  //! Main constructor.
  OcctQtViewer (QWidget* theParent = nullptr);

  //! Destructor.
  virtual ~OcctQtViewer();

  //! Return Viewer.
  const Handle(V3d_Viewer)& Viewer() const { return myViewer; }

  //! Return View.
  const Handle(V3d_View)& View() const { return myView; }

  //! Return AIS context.
  const Handle(AIS_InteractiveContext)& Context() const { return myContext; }

  //! Return OpenGL info.
  const QString& getGlInfo() const { return myGlInfo; }

  //! Minial widget size.
  virtual QSize minimumSizeHint() const override { return QSize(200, 200); }

  //! Default widget size.
  virtual QSize sizeHint()        const override { return QSize(720, 480); }

public:

  //! Handle subview focus change.
  virtual void OnSubviewChanged(const Handle(AIS_InteractiveContext)&,
                                const Handle(V3d_View)&,
                                const Handle(V3d_View)& theNewView) override;

private: // My implementations

  void newDocument();
  bool createXCafApp();
  // static TCollection_AsciiString getXCafNodePathNames(const XCAFPrs_DocumentExplorer& theExp,
  //                                                     const bool theIsInstanceName,
  //                                                     const int theLowerDepth = 0);

  virtual void OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx,
                                  const Handle(V3d_View)& theView) override;
  
  virtual void ProcessExpose() override;  
  virtual void ProcessConfigure(bool theIsResized) override;
  virtual void ProcessInput() override;
  void DisplayCube();
  void ShowCurrentClass(TCollection_ExtendedString aNodeNameGot) const;
  void ShowCurrentClass() const;

public: // My implementations

  bool OpenStep(const TCollection_AsciiString& theFilePath);
  void SaveAsStep(const char* aFilePath);

  void DumpXCafDocumentTree();
  void DisplayXCafDocument(bool theToExplode);
  void DisplayXCafDocumentByPart(bool theToExplode, size_t startIndex);

  void ClearContext();
  void SetLabels(std::vector<OcctLabelTools::Label> theLabels);
  Handle(TDocStd_Document) GetXdeDoc() const { return myXdeDoc; }
  static TCollection_AsciiString getXCafNodePathNames(const XCAFPrs_DocumentExplorer& theExp,
                                                      const bool theIsInstanceName,
                                                      const int theLowerDepth = 0);

protected: // OpenGL events

  virtual void initializeGL() override;
  virtual void paintGL() override;
  //virtual void resizeGL(int , int ) override;

protected: // user input events

  virtual void closeEvent       (QCloseEvent*  theEvent) override;
  virtual void keyPressEvent    (QKeyEvent*    theEvent) override;
  virtual void mousePressEvent  (QMouseEvent*  theEvent) override;
  virtual void mouseReleaseEvent(QMouseEvent*  theEvent) override;
  virtual void mouseMoveEvent   (QMouseEvent*  theEvent) override;
  virtual void wheelEvent       (QWheelEvent*  theEvent) override;

private:

  //! Dump OpenGL info.
  void dumpGlInfo (bool theIsBasic, bool theToPrint);

  //! Request widget paintGL() event.
  void updateView();

  //! Handle view redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) override;

private:
  Handle(V3d_Viewer)             myViewer;
  Handle(V3d_View)               myView;
  Handle(AIS_InteractiveContext) myContext;
  Handle(AIS_ViewCube)           myViewCube;

  Handle(V3d_View)               myFocusView;

  Handle(TDocStd_Document)       myXdeDoc;
  Handle(TDocStd_Application)    myXdeApp;

  QLabel* entityName;

  std::vector<OcctLabelTools::Label> myLabels;

  QString myGlInfo;
  bool myIsCoreProfile;

};

#endif // _OcctQtViewer_HeaderFile
