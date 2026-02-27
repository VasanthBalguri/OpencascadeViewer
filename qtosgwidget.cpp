#include "qtosgwidget.h"
#include "examples.h"
#include "ocafdelegatemodel.h"
#include "ocaftreedialog.h"

QtOSGWidget::QtOSGWidget(qreal scaleX, qreal scaleY, QWidget* parent)
      : QOpenGLWidget(parent)
        , _mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded( this->x(), this->y(),
                                                                 this->width(), this->height() ) )
        , _mViewer(new osgViewer::Viewer)
      , m_scaleX(scaleX)
      , m_scaleY(scaleY)
{

      try{
            //initialize camera

        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setViewport( this->x(), this->y(), this->width(), this->height() );
        camera->setClearColor( osg::Vec4( 0.9f, 0.9f, 1.f, 1.f ) );
        float aspectRatio = static_cast<float>( this->width()) / static_cast<float>( this->height() );
        camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
        camera->setGraphicsContext( _mGraphicsWindow );
        _mViewer->setCamera(camera);

        //intitalize hud
        osg::ref_ptr<osg::Camera> hud_camera = new osg::Camera;
        hud_camera->setProjectionMatrix(osg::Matrix::perspective(30.f,1.f,0.f,1000.f));// set the view matrix
        hud_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        hud_camera->setViewMatrix(osg::Matrix::identity());// only clear the depth buffer
        hud_camera->setClearMask(GL_DEPTH_BUFFER_BIT);// draw subgraph after main camera view.
        hud_camera->setRenderOrder(osg::Camera::POST_RENDER);

        hud_camera->setGraphicsContext(_mGraphicsWindow);
        hud_camera->setViewport(this->x(), this->y(),20, 20);

        _mViewer->addSlave(hud_camera, false);
        //trackball navigation
        osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator;
        manipulator->setAllowThrow( false );
        this->setMouseTracking(true);
        _mViewer->setCameraManipulator(manipulator);


        osg::Vec3d eye( 60.0, 40.0, 60.0 );
        osg::Vec3d center( 0.0, 0.0, 0.0 );
        osg::Vec3d up( 0.0, 1.0, 0.0 );

        manipulator->setHomePosition(eye,center,up);

        osg::ref_ptr<osg::Group> root = new osg::Group;
        root->addChild(hud_camera);
        _mViewer->setSceneData(root);
        _mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

        _mViewer->realize();
        }
        catch(std::exception e)
        {
            std::cout<<e.what()<<std::endl;
        }

}

void QtOSGWidget::setScene(osg::ref_ptr<osg::Group> root)
{
    this->root = root;
    _mViewer->setSceneData(root);
}

osg::ref_ptr<osg::Group> QtOSGWidget::getScene()
{
   osg::ref_ptr<osg::Group> root = dynamic_cast<osg::Group*>(_mViewer->getSceneData());
   return root;
}

void QtOSGWidget::addTopoDSObject(TopoDS_Shape &shape, const osg::Vec3 &geomColor, gp_Trsf &transformation)
{
    // vector to save vertices
    osg::ref_ptr<osg::Vec3Array> vertexList = new osg::Vec3Array();


    osg::Array::Binding colorBinding = osg::Array::BIND_OVERALL;

    // vector to save _colorTool
    osg::ref_ptr<osg::Vec3Array> colorList = new osg::Vec3Array();
    if (colorBinding==osg::Array::BIND_OVERALL)
    {
        colorList->push_back(geomColor);
    }

    // create one osg primitive set
    osg::ref_ptr<osg::DrawElementsUInt> triangleStrip = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

    //osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;

    unsigned int noOfTriangles = 0;

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    // large vertex datasets work best with VBO.
    geom->setUseVertexBufferObjects(true);

    if(!shape.IsNull())
    {
        // clean any previous triangulation
        BRepTools::Clean(shape);

        //_healShape(shape);

        /// call to incremental mesh on this shape
        /// \todo not sure why this 1 is passed. Its called deflection BTW
        ///       need to find a way to calculate it
        double linearDeflection = 1.0;
        BRepMesh_IncrementalMesh(shape, linearDeflection);

        ///iterate faces
        // this variable will help in keeping track of face indices
        unsigned int index = 0;
        for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next())
        {
            TopoDS_Face face = TopoDS::Face(ex.Current());
            TopLoc_Location location;

            /// triangulate current face
            Handle (Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
            if (!triangulation.IsNull())
            {
                int noOfNodes = triangulation->NbNodes();

                // Store vertices. Build vertex array here
                for(int j = 1; j <= triangulation->NbNodes(); j++)
                {
                    // populate vertex list
                    // Ref: http://www.opencascade.org/org/forum/thread_16694/?forum=3
                    gp_Pnt pt = triangulation->Node(j).Transformed(transformation * location.Transformation());
                    //gp_Pnt pt = (triangulation->Nodes())(j).Transformed(transformation * location.Transformation());
                    vertexList->push_back(osg::Vec3(pt.X(), pt.Y(), pt.Z()));
                   // std::cout<<pt.X()<<"\t"<<pt.Y()<<"\t"<<pt.Z()<<std::endl;
                    // populate color list
                    if (colorBinding==osg::Array::BIND_PER_VERTEX)
                    {
                        colorList->push_back(geomColor);
                    }
                }

                /// now we need to get face indices for triangles
                // get list of triangle first
                const Poly_Array1OfTriangle& triangles = triangulation->Triangles();

                //No of triangles in this triangulation
                noOfTriangles = triangulation->NbTriangles();

                Standard_Integer v1, v2, v3;
                for (unsigned int j = 1; j <= noOfTriangles; j++)
                {
                    /// If face direction is reversed then we add verticews in reverse order
                    /// order of vertices is important for normal calculation later
                    if (face.Orientation() == TopAbs_REVERSED)
                    {
                        triangles(j).Get(v1, v3, v2);
                    }
                    else
                    {
                        triangles(j).Get(v1, v2, v3);
                    }

                    //gp_XYZ aPnt1 = (triangulation->Nodes())(v1).XYZ();
                    //gp_XYZ aPnt2 = (triangulation->Nodes())(v2).XYZ();
                    //gp_XYZ aPnt3 = (triangulation->Nodes())(v3).XYZ();

                    gp_XYZ aPnt1 = triangulation->Node(v1).XYZ();
                    gp_XYZ aPnt2 = triangulation->Node(v2).XYZ();
                    gp_XYZ aPnt3 = triangulation->Node(v3).XYZ();
//                    gp_XYZ aV12 = aPnt2 - aPnt1;
//                    gp_XYZ aV23 = aPnt3 - aPnt2;
//                    gp_XYZ aNor = aV12^aV23;

//                    normalArray->push_back(osg::Vec3(aNor.X(),aNor.Y(),aNor.Z()));


                    triangleStrip->push_back(index + v1 - 1);
                    triangleStrip->push_back(index + v2 - 1);
                    triangleStrip->push_back(index + v3 - 1);
                }
                index = index + noOfNodes;
            }
        }


        osg::ref_ptr<osg::DrawElementsUInt> lineStrip = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP, 0);
        for(TopExp_Explorer ex(shape, TopAbs_EDGE);ex.More();ex.Next())
        {
            TopoDS_Edge edge = TopoDS::Edge(ex.Current());
            TopLoc_Location location;

            Handle(Poly_Polygon3D) edgeMesh = BRep_Tool::Polygon3D(edge,location);
            if(!edgeMesh.IsNull())
            {
                int noOfNodes = edgeMesh->NbNodes();

                for(int j= 1; j <= noOfNodes; j++)
                {
                    gp_Pnt pt = (edgeMesh->Nodes())(j);
                    vertexList->push_back(osg::Vec3(pt.X(), pt.Y(), pt.Z()));
                    lineStrip->push_back(index);
                    index++;
                }
            }

        }

        osg::ref_ptr<osg::DrawElementsUInt> points = new osg::DrawElementsUInt(osg::PrimitiveSet::POINTS, 0);

        for (TopExp_Explorer ex(shape, TopAbs_VERTEX); ex.More(); ex.Next())
        {
            TopoDS_Vertex vertex = TopoDS::Vertex(ex.Current());
            gp_XYZ location = vertex.Location().Transformation().TranslationPart();
            vertexList->push_back(osg::Vec3(location.X(),location.Y(),location.Z()));
            points->push_back(index);
            index++;
        }

        geom->setVertexArray(vertexList.get());
        //geom->setVertexAttribArray(0, geom->getVertexArray(), osg::Array::BIND_PER_VERTEX);
        geom->setColorArray(colorList.get(), colorBinding);
        //geom->setVertexAttribArray(2, geom->getColorArray(), osg::Array::BIND_OVERALL);
        //geom->setNormalArray(normalArray.get());
        //geom->setVertexAttribArray(1, geom->getNormalArray(), osg::Array::BIND_PER_VERTEX);
        geom->addPrimitiveSet(triangleStrip.get());
        if(lineStrip->getNumIndices() != 0)
            geom->addPrimitiveSet(lineStrip.get());
        if(points->getNumIndices() != 0)
            geom->addPrimitiveSet(points.get());
        //osgUtil::SmoothingVisitor::smooth(*geom);
        osgUtil::SmoothingVisitor::smooth(*geom,osg::PI/6.0);
    }

    //return geom.release();

    osg::ref_ptr<osg::Drawable> sd = (osg::ref_ptr<osg::Drawable>)geom->asDrawable();
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(sd);

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();
    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(geode);
    _mViewer->setSceneData(root);
}

void QtOSGWidget::setScale(qreal X, qreal Y)
  {
      m_scaleX = X;
      m_scaleY = Y;
      this->resizeGL(this->width(), this->height());
  }


  void QtOSGWidget::paintGL() {
    _mViewer->frame();
  }

  void QtOSGWidget::resizeGL( int width, int height )
  {
      this->getEventQueue()->windowResize(this->x()*m_scaleX, this->y() * m_scaleY, width*m_scaleX, height*m_scaleY);
      _mGraphicsWindow->resized(this->x()*m_scaleX, this->y() * m_scaleY, width*m_scaleX, height*m_scaleY);
      osg::ref_ptr<osg::Camera> camera = _mViewer->getCamera();
      camera->setViewport(0, 0, this->width()*m_scaleX, this->height()* m_scaleY);
  }

  void QtOSGWidget::initializeGL(){

      //basic shader setup
      // disabled shader as of now, but in essence this is how it will work
      // osg::ref_ptr<osg::StateSet> stateSet = this->getScene()->getOrCreateStateSet();
//       osg::ref_ptr<osg::Program> program = new osg::Program;

//       osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
//       if (!vertShader->loadShaderSourceFromFile("../shaders/myShader.vert"))
//           std::cerr << "Could not read VERTEX shader from file" << std::endl;
//       program->addShader(vertShader);

//        osg::ref_ptr<osg::Shader> geomShader = new osg::Shader(osg::Shader::GEOMETRY);
//       if (!geomShader->loadShaderSourceFromFile("../shaders/myShader.geom"))
//           std::cerr << "Could not read GEOM shader from file" << std::endl;
//       program->addShader(geomShader);

//       osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
//       if (!fragShader->loadShaderSourceFromFile("../shaders/myShader.frag"))
//           std::cerr << "Could not read FRAGMENT shader from file" << std::endl;
//       program->addShader(fragShader);

//       stateSet->setAttributeAndModes(program.get(), osg::StateAttribute::ON);
//       stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

//       osg::Uniform* modelViewProjectionMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "osg_ModelViewProjectionMatrix");
//       modelViewProjectionMatrix->setUpdateCallback(new ModelViewProjectionMatrixCallback(_mViewer->getCamera()));
//       stateSet->addUniform(modelViewProjectionMatrix);
  }

  void QtOSGWidget::mouseMoveEvent(QMouseEvent* event)
  {
      this->getEventQueue()->mouseMotion(event->x()*m_scaleX, event->y()*m_scaleY);
      osg::Matrixd viewMatrix = _mViewer->getCamera()->getViewMatrix();
      osg::Vec3d center;
      osg::Vec3d eye;
      osg::Vec3d up;
      viewMatrix.getLookAt(eye,center,up);
  }

  void QtOSGWidget::mousePressEvent(QMouseEvent* event)
  {
      unsigned int button = 0;
      switch (event->button()){
      case Qt::LeftButton:
          button = 1;
          break;
      case Qt::MiddleButton:
          button = 2;
          break;
      case Qt::RightButton:
          button = 3;
          break;
      default:
          break;
      }
      this->getEventQueue()->mouseButtonPress(event->x()*m_scaleX, event->y()*m_scaleY, button);
  }

  void QtOSGWidget::mouseReleaseEvent(QMouseEvent* event)
  {
      unsigned int button = 0;
      switch (event->button()){
      case Qt::LeftButton:
          button = 1;
          break;
      case Qt::MiddleButton:
          button = 2;
          break;
      case Qt::RightButton:
          button = 3;
          break;
      default:
          break;
      }
      this->getEventQueue()->mouseButtonRelease(event->x()*m_scaleX, event->y()*m_scaleY, button);
  }

  void QtOSGWidget::wheelEvent(QWheelEvent* event)
  {
      int delta = event->delta();
      osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
                  osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
      this->getEventQueue()->mouseScroll(motion);
    osg::Matrixd viewMatrix = _mViewer->getCamera()->getViewMatrix();
    osg::Vec3d center;
    osg::Vec3d eye;
    osg::Vec3d up;
    viewMatrix.getLookAt(eye,center,up);
  }

  bool QtOSGWidget::event(QEvent* event)
  {
      bool handled = QOpenGLWidget::event(event);
      this->update();
      return handled;
  }

  osgGA::EventQueue* QtOSGWidget::getEventQueue() const {
    osgGA::EventQueue* eventQueue = _mGraphicsWindow->getEventQueue();
    return eventQueue;
  }

  void QtOSGWidget::slotLoadStepFile(std::string path)
  {
      gp_Trsf transform;
      TopoDS_Shape shape = readStepFile(path);
      addTopoDSObject(shape,osg::Vec3(1.0,1.0,1.0),transform);
  }

  void QtOSGWidget::slotCreateBottle()
  {
    gp_Trsf transform;
    TopoDS_Shape shape = MakeBottle(10.0,15.0,5.0);
    addTopoDSObject(shape,osg::Vec3(1.0,1.0,1.0),transform);
  }

  void QtOSGWidget::slotSampleProject()
  {
      gp_Trsf transform;
      TopoDS_Shape shape = make2DProfile();
      addTopoDSObject(shape,osg::Vec3(1.0,1.0,1.0),transform);
  }

  void QtOSGWidget::slotOCAFExample()
  {
      opencascade::handle<TDocStd_Application> app = new TDocStd_Application;
      BinDrivers::DefineFormat(app);

      opencascade::handle<TDocStd_Document> doc = createOCAFDoc(app);

      OcafDelegateModel *model = new OcafDelegateModel(doc);
      OcafTreeDialog dialog(model,this);
      dialog.show();
      dialog.exec();

      app->Close(doc);
  }

void QtOSGWidget::slotOCAFExample2()
  {
      opencascade::handle<TDocStd_Application> app = new TDocStd_Application;
      BinDrivers::DefineFormat(app);

      opencascade::handle<TDocStd_Document> doc = createOCAFBottle(app,10.0,15.0,5.0);

      OcafDelegateModel *model = new OcafDelegateModel(doc);
      OcafTreeDialog dialog(model,this);
      gp_Trsf transform;
        TDF_Label mainLabel = doc->Main();
  TDF_Label ftreeLabel = mainLabel.FindChild(1);
  TDF_Label opLabel = ftreeLabel.FindChild(2);
   opencascade::handle<TNaming_NamedShape> namedShape;
   if(opLabel.FindAttribute(TNaming_NamedShape::GetID(),namedShape))
   {
        TopoDS_Shape shape = namedShape->Get();

        addTopoDSObject(shape,osg::Vec3(1.0,1.0,1.0),transform);
   }
      dialog.show();
      dialog.exec();

      app->Close(doc);


  }
