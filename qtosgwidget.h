#ifndef QTOSGWIDGET_H
#define QTOSGWIDGET_H

#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QtGlobal>
#include <QWindow>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/BlendFunc>
#include <osgUtil/SmoothingVisitor>

#include<vector>
#include <osg/Uniform>
#include <osg/Camera>
#include <osg/NodeVisitor>

#include <TopoDS.hxx>

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_ListOfShape.hxx>

struct ModelViewProjectionMatrixCallback: public osg::Uniform::Callback
{
    ModelViewProjectionMatrixCallback(osg::Camera* camera) :
            _camera(camera) {
    }

    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
        osg::Matrixd viewMatrix = _camera->getViewMatrix();
        osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
        osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * _camera->getProjectionMatrix();
        uniform->set(modelViewProjectionMatrix);
    }

    osg::Camera* _camera;
};

struct ViewportCallback: public osg::Uniform::Callback
{
    ViewportCallback(osg::Camera* camera) :
            _camera(camera) {
    }

    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* /*nv*/) {
        const osg::Viewport* viewport = _camera->getViewport();
        osg::Vec2f viewportVector = osg::Vec2f(viewport->width(), viewport->height());
        uniform->set(viewportVector);
    }

    osg::Camera* _camera;
};

class QtOSGWidget : public QOpenGLWidget
{
private:

  osgGA::EventQueue* getEventQueue() const;
  osg::ref_ptr<osg::Group> root;
  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _mGraphicsWindow;
  osg::ref_ptr<osgViewer::Viewer> _mViewer;
  qreal m_scaleX, m_scaleY;

public:
  QtOSGWidget(qreal scaleX = 1, qreal scaleY = 1, QWidget* parent = 0);

  void setScene(osg::ref_ptr<osg::Group> root);
  osg::ref_ptr<osg::Group> getScene();

  void addTopoDSObject(TopoDS_Shape& shape, const osg::Vec3& geomColor, gp_Trsf& transformation);
  virtual ~QtOSGWidget(){}

  void setScale(qreal X, qreal Y);
  osg::ref_ptr<osg::View> getView()
  {
      return _mViewer;
  }
  Q_OBJECT
public slots:
    void slotLoadStepFile(std::string path);
    void slotCreateBottle();
    void slotSampleProject();
    void slotOCAFExample();

signals:
    void orientationChanged(std::vector<double>);


protected:

  virtual void paintGL();

  virtual void resizeGL( int width, int height );

  virtual void initializeGL();

  virtual void mouseMoveEvent(QMouseEvent* event);

  virtual void mousePressEvent(QMouseEvent* event);

  virtual void mouseReleaseEvent(QMouseEvent* event);

  virtual void wheelEvent(QWheelEvent* event);

  virtual bool event(QEvent* event);

};
#endif // QTOSGWIDGET_H
