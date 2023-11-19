#include "examples.h"
#include <iostream>

TopoDS_Shape readStepFile(std::string path)
{
    STEPControl_Reader aReader;
    aReader.ReadFile(path.c_str());

    Standard_Integer nbr = aReader.NbRootsForTransfer();

    for (Standard_Integer n = 1; n<= nbr; n++) {
        aReader.TransferRoot(n);
    }
    return aReader.OneShape();
    // Collecting resulting entities
    /*Standard_Integer nbs = aReader.NbShapes();
    if (nbs == 0) {

    }
    else
    {
        for (Standard_Integer i=1; i<=nbs; i++) {
            aShape = aReader.Shape(i);
            osg::ref_ptr<osg::Geometry> geom = createGeometryFromShape(aShape,osg::Vec3f(1.0,1.0,1.0),transform);
            osg::ref_ptr<osg::Drawable> sd = (osg::ref_ptr<osg::Drawable>)geom->asDrawable();

            geode->addDrawable(sd);
        }
    }*/

//           osg::ref_ptr<osg::Program> program = new osg::Program;

//           osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
//           if (!vertShader->loadShaderSourceFromFile("../shaders/myShader.vert"))
//               std::cerr << "Could not read VERTEX shader from file" << std::endl;
//           program->addShader(vertShader);

//            osg::ref_ptr<osg::Shader> geomShader = new osg::Shader(osg::Shader::GEOMETRY);
//           if (!geomShader->loadShaderSourceFromFile("../shaders/myShader.geom"))
//               std::cerr << "Could not read GEOM shader from file" << std::endl;
//           program->addShader(geomShader);

//           osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
//           if (!fragShader->loadShaderSourceFromFile("../shaders/myShader.frag"))
//               std::cerr << "Could not read FRAGMENT shader from file" << std::endl;
//           program->addShader(fragShader);

//           stateSet->setAttributeAndModes(program.get(), osg::StateAttribute::ON);
//           stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

    //return geode.release();
}

//sample code to create bottle in osg, will remove it later
TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
           const Standard_Real myThickness)
{
  // Profile : Define Support Points
  gp_Pnt aPnt1(-myWidth / 2., 0, 0);
  gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
  gp_Pnt aPnt3(0, -myThickness / 2., 0);
  gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
  gp_Pnt aPnt5(myWidth / 2., 0, 0);

  // Profile : Define the Geometry
  Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2,aPnt3,aPnt4);
  Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
  Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

  // Profile : Define the Topology
  TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
  TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
  TopoDS_Wire aWire  = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

  // Complete Profile
  gp_Ax1 xAxis = gp::OX();
  gp_Trsf aTrsf;

  aTrsf.SetMirror(xAxis);
  BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
  TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
  TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

  BRepBuilderAPI_MakeWire mkWire;
  mkWire.Add(aWire);
  mkWire.Add(aMirroredWire);
  TopoDS_Wire myWireProfile = mkWire.Wire();

  // Body : Prism the Profile
  TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
  gp_Vec aPrismVec(0, 0, myHeight);
  TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

  // Body : Apply Fillets
  BRepFilletAPI_MakeFillet mkFillet(myBody);
  TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
  while(anEdgeExplorer.More()){
    TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    //Add edge to fillet algorithm
    mkFillet.Add(myThickness / 12., anEdge);
    anEdgeExplorer.Next();

  }

  myBody = mkFillet.Shape();

  // Body : Add the Neck
  gp_Pnt neckLocation(0, 0, myHeight);
  gp_Dir neckAxis = gp::DZ();
  gp_Ax2 neckAx2(neckLocation, neckAxis);

  Standard_Real myNeckRadius = myThickness / 4.;
  Standard_Real myNeckHeight = myHeight / 10.;

  BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
  TopoDS_Shape myNeck = MKCylinder.Shape();

  myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

  // Body : Create a Hollowed Solid
  TopoDS_Face   faceToRemove;
  Standard_Real zMax = -1;

  for(TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()){
    TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
    // Check if <aFace> is the top face of the bottle’s neck
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
    if(aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)){
      Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
      gp_Pnt aPnt = aPlane->Location();
      Standard_Real aZ   = aPnt.Z();
      if(aZ > zMax){
        zMax = aZ;
        faceToRemove = aFace;
      }
    }
  }

  TopTools_ListOfShape facesToRemove;
  facesToRemove.Append(faceToRemove);
  //myBody = BRepOffsetAPI_MakeThickSolid(myBody, facesToRemove, -myThickness / 50, 1.e-3);
  BRepOffsetAPI_MakeThickSolid aSolidMaker;
  aSolidMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
  myBody = aSolidMaker.Shape();
  //myBody = BRepOffsetAPI_MakeThickSolidByJoin(myBody,facesToRemove, -myThickness / 50, 1.e-3,0.01);
  // Threading : Create Surfaces
  Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
  Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

  // Threading : Define 2D Curves
  gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
  gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
  gp_Ax2d anAx2d(aPnt, aDir);

  Standard_Real aMajor = 2. * M_PI;
  Standard_Real aMinor = myNeckHeight / 10;

  Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
  Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
  Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
  Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
  gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
  gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

  Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
  // Threading : Build Edges and Wires
  TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
  TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
  TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
  TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
  TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
  TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
  BRepLib::BuildCurves3d(threadingWire1);
  BRepLib::BuildCurves3d(threadingWire2);

  // Create Threading
  BRepOffsetAPI_ThruSections aTool(Standard_True);
  aTool.AddWire(threadingWire1);
  aTool.AddWire(threadingWire2);
  aTool.CheckCompatibility(Standard_False);

  TopoDS_Shape myThreading = aTool.Shape();

  // Building the Resulting Compound
  TopoDS_Compound aRes;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aRes);
  aBuilder.Add (aRes, myBody);
  aBuilder.Add (aRes, myThreading);

  return aRes;
}

TopoDS_Shape make2DProfile()
{
    TColgp_Array1OfPnt2d Pnts2d;			// Array of 2d points
    TColGeom2d_Array1OfCurve Curves2d;		// Array of 2d curves

    TColgp_Array1OfPnt Pnts3d;				// Array of 3d points
    TColGeom_Array1OfCurve curves3D;		// Array of 3d curves
    TColGeom_Array1OfSurface surfaces;

    gp_Pnt2d Cnt1(20,20);
    gp_Pnt2d P1(20,0);
    gp_Pnt2d P2(120,0);

    gp_Pnt Q1(0,0,0);
    gp_Pnt Q2(10,10,10);
    gp_Pnt Q3(4,0,0);
    opencascade::handle<Geom2d_TrimmedCurve> L1 = GCE2d_MakeSegment(P1,P1).Value();

    opencascade::handle<Geom_TrimmedCurve> S1 = GC_MakeSegment(Q1,Q2);

    TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(S1);
    TopoDS_Vertex aVert = BRepBuilderAPI_MakeVertex(Q3);
    //TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
    //TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Wire aWire  = BRepBuilderAPI_MakeWire(anEdge1);


    return aWire;
}



opencascade::handle<TDocStd_Document> createOCAFDoc(opencascade::handle<TDocStd_Application> &app)
{
    //opencascade::handle<TDocStd_Application> app = new TDocStd_Application;
    //BinDrivers::DefineFormat(app);

    opencascade::handle<TDocStd_Document> doc;
    app->NewDocument("BinOcaf", doc);
    //
    if ( doc.IsNull() )
    {
      std::cout << "Error: cannot create an OCAF document." << std::endl;
      return NULL;
    }

    TDF_Label partsLab  = doc->Main();
    TDF_Label meshesLab = partsLab.FindChild(1);

    TDataStd_Integer::Set(partsLab, 199);
    TCollection_ExtendedString name("Test");
    TDataStd_Name::Set(meshesLab,name);

    return doc;
    // Create parts.
    /*IPart part1( TDF_TagSource::NewChild(partsLab) );
    IPart part2( TDF_TagSource::NewChild(partsLab) );
    IPart part3( TDF_TagSource::NewChild(partsLab) );
    //
    part2.SetShape( BRepPrimAPI_MakeBox(10, 10, 10) );
    part2.SetColor( 255, 0, 0 );

    TColStd_PackedMapOfInteger fids1, fids2;
    fids1.Add(1);
    fids1.Add(2);
    fids1.Add(3);
    fids2.Add(3);
    fids2.Add(4);
    fids2.Add(5);

    part3.CreateFeature(fids1);
    part3.CreateFeature(fids2);

    std::vector<IFeature> part3Features;
    part3.GetFeatures(part3Features);

    // Create meshes.
    IMesh mesh( TDF_TagSource::NewChild(meshesLab) );
    //
    mesh.Create(part2);

    PCDM_StoreStatus sstatus = app->SaveAs(doc, "C:/users/user/Desktop/test.cbf");
    //
    if ( sstatus != PCDM_SS_OK )
    {
      app->Close(doc);

      std::cout << "Cannot write OCAF document." << std::endl;
      return 1;
    }*/


    //app->Close(doc);

}

void readOCAFDoc(opencascade::handle<TDocStd_Document> &doc)
{
    TDF_Label root = doc->Main();
    std::cout << root.Tag() << std::endl;
    opencascade::handle<TDataStd_Integer> intRootAttr;
    if(root.FindAttribute(TDataStd_Integer::GetID(),intRootAttr))
    {
        std::cout << intRootAttr->Get() << std::endl;
    }
    opencascade::handle<TDataStd_Name> nameRootAttr;
    if(root.FindAttribute(TDataStd_Name::GetID(),nameRootAttr))
    {
        std::cout << nameRootAttr->Get().ToExtString() << std::endl;
    }
    for (TDF_ChildIterator itall (root,Standard_True); itall.More(); itall.Next()) {
        TDF_Label aChild = itall.Value();
        std::cout << aChild.Tag()<< std::endl;
        opencascade::handle<TDataStd_Integer> intAttr;
        if(aChild.FindAttribute(TDataStd_Integer::GetID(),intAttr))
        {
            std::cout << intAttr->Get() << std::endl;
        }
        opencascade::handle<TDataStd_Name> nameAttr;
        if(aChild.FindAttribute(TDataStd_Name::GetID(),nameAttr))
        {
            TCollection_ExtendedString str = nameAttr->Get();
            str.Print(std::cout);
            std::cout <<  std::endl;
        }
    }
}
