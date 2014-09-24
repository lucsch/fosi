/***************************************************************************
 plintoperation
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plintoperation.h"
#include "vrlayerraster.h"
#include "vrcoordinate.h"
#include "vrlayervector.h"
#include "wx/stdpaths.h"


PlIntOperation::PlIntOperation(vrLayerRasterGDAL * mnt, int bandno, vrLayerVectorOGR * shape) {
    m_MNT = mnt;
    m_MNTBand = bandno;
    m_Shape = shape;
}



PlIntOperation::~PlIntOperation() {
}



bool PlIntOperation::ComputeAndAddPoint(double x, double y, long index) {
    if (index > m_Points.getNumGeometries()) {
        return false;
    }
    
    wxASSERT(m_MNT);
    wxArrayDouble myValues;
    if(m_MNT->GetPixelValue(x, y, myValues) == false){
        return false;
    }
    
    if (index == m_Points.getNumGeometries()) {
        OGRPoint myPt;
        m_Points.addGeometry(&myPt);
    }
    
    // TODO: Remove this test code
    /*
    if (index == 0) {
        OGRPoint * myPt = static_cast<OGRPoint*>(m_Points.getGeometryRef(index));
        wxASSERT(myPt);
        myPt->setX(598557);
        myPt->setY(115375);
        myPt->setZ(913.143);
    }
 
    
    if (index == 1) {
        OGRPoint * myPt = static_cast<OGRPoint*>(m_Points.getGeometryRef(index));
        wxASSERT(myPt);
        myPt->setX(598487);
        myPt->setY(115213);
        myPt->setZ(861.925);
    }
    
    if (index == 2) {
        OGRPoint * myPt = static_cast<OGRPoint*>(m_Points.getGeometryRef(index));
        wxASSERT(myPt);
        myPt->setX(598579);
        myPt->setY(115233);
        myPt->setZ(887.947);
    }
    return true;
    */
     //
    
    wxASSERT(m_MNTBand < myValues.GetCount());
    OGRPoint * myPt = static_cast<OGRPoint*>(m_Points.getGeometryRef(index));
    wxASSERT(myPt);
    myPt->setX(x);
    myPt->setY(y);
    myPt->setZ(myValues[m_MNTBand]);
    return true;
}



bool PlIntOperation::ClearPoints() {
    if (m_Points.getNumGeometries() == 0) {
        return false;
    }
    m_Points.empty();
    return true;
}



bool PlIntOperation::GetPlaneInfo(double & dip, double & direction) {
    if (m_Points.getNumGeometries() < 3) {
        return false;
    }
    
    OGRPoint ab = _ComputeVector(0, 1);
    OGRPoint ac = _ComputeVector(0, 2);
    OGRPoint abac = _MultiplyVector(ab, ac);

    double horiz = sqrt( abac.getX() * abac.getX() + abac.getY() * abac.getY() );
    double azimuth_radian = acos( abac.getY() / horiz  );
    double azimuth_degre = 180.0 * (azimuth_radian) / M_PI;
    
    if (abac.getX() < 0){
        azimuth_degre = 360.0 - azimuth_degre;
    }
   
    double norm = sqrt(abac.getX() * abac.getX() + abac.getY() * abac.getY() + abac.getZ() * abac.getZ());
    double bdip = -1.0 * 180 * asin( abac.getZ() / norm ) / M_PI;
    if (bdip < 0) {
        direction = azimuth_degre;
    }
    else {
        direction =  fmod (azimuth_degre + 180, 360);
    }
    
    dip = 90.0 - fabs(bdip);
    return true;
}



bool PlIntOperation::ComputeLineFullResolution (vrCoordinate * coord){
    vrRealRect myImgReal;
    m_MNT->GetExtent(myImgReal);
    wxSize myImgPx = m_MNT->GetPixelSize();
    
    /////////////////////////////
    // Create temporary raster for results
    ////////////////////////////
    
    GDALDriverH	hDriver = GDALGetDriverByName("GTiff");
	if (hDriver == NULL) {
		wxLogError(_("Unable to load GTiff Driver, export unavailable!"));
		return false;
	}
    wxFileName destination (wxStandardPaths::Get().GetTempDir(), "test_pling_full.tif");
    
	GDALDatasetH hOutDS = GDALCreate(hDriver, destination.GetFullPath(),
									 myImgPx.GetWidth(),
                                     myImgPx.GetHeight(),
									 1, GDT_Float32, NULL);
	if (hOutDS == NULL) {
		wxLogError(_("Error creating '%s'!"), destination.GetFullName());
		return false;
	}
	wxLogMessage(_("Creating '%s'"), destination.GetFullPath());
    
    
    wxArrayDouble myGeoTransformArray;
    m_MNT->GetGeoTransform(myGeoTransformArray);
    wxASSERT(myGeoTransformArray.GetCount() == 6);
    
    double myGeoTransform[6] = {
        myGeoTransformArray[0],
        myGeoTransformArray[1],
        myGeoTransformArray[2],
        myGeoTransformArray[3],
        myGeoTransformArray[4],
        myGeoTransformArray[5]
    };
    GDALSetGeoTransform(hOutDS, &myGeoTransform[0]);
    
    ////////////////////////////
    // extract DEM data for
    // concerned area, compute above or under the plane
    // and write the result to the temporary raster.
    ///////////////////////////
    
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    _ComputeABCD(a, b, c, d);
    
    // read row by row the entire DEM
    for (unsigned int height_i = 0 ; height_i < myImgPx.GetHeight() ; height_i++) {
        float * imgdata = (float *) CPLMalloc(myImgPx.GetWidth() * GDALGetDataTypeSize(GDT_Float32) / 8);
        if(m_MNT->GetDatasetRef()->RasterIO(GF_Read,
                                            0,
                                            height_i,
                                            myImgPx.GetWidth(),
                                            1,
                                            imgdata,
                                            myImgPx.GetWidth(),
                                            1,
                                            GDT_Float32,
                                            1,
                                            NULL, 0, 0, 0) != CE_None){
            wxLogError(_("Error while reading loop : %d"), height_i);
            break;
        }
        
        // compute under / above plane
        for (unsigned int width_i = 0; width_i < myImgPx.GetWidth() ; width_i++) {
            OGRPoint myPt;
            myPt.setX( myImgReal.GetLeft() + ( width_i * coord->GetPixelSize() ) );
            myPt.setY( myImgReal.GetTop() + ( height_i * coord->GetPixelSize() ) );
            
            float myzValue = * (imgdata + width_i);
            myPt.setZ( myzValue );
            * (imgdata + width_i) = _IsUnderOrAbovePlane(a, b, c, d, &myPt);
        }
        
        if(GDALDatasetRasterIO (hOutDS,
                                GF_Write,
                                0,
                                height_i,
                                myImgPx.GetWidth(),
                                1,
                                imgdata,
                                myImgPx.GetWidth(),
                                1,
                                GDT_Float32,
                                1,
                                NULL,
                                0,
                                0,
                                0) != CE_None){
            break;
        }
        CPLFree(imgdata);
    }
    
    
    ////////////////////////////
    // Convert raster to polygon
    ///////////////////////////
    // TODO: Use in memory polygon layer when working
    
    wxFileName myVectorFileName (wxStandardPaths::Get().GetTempDir(), "test_pling_poly.shp");
    
    // try to delete vector file if existing
    const char *pszVectorDriverName = "ESRI Shapefile";
    OGRSFDriver * poVectorDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszVectorDriverName);
    if( poVectorDriver == NULL ){
        wxLogWarning(_("%s driver not available."), pszVectorDriverName );
        GDALClose(hOutDS);
        return false;
    }

    if (myVectorFileName.Exists() && poVectorDriver->DeleteDataSource(myVectorFileName.GetFullPath()) != OGRERR_NONE){
        wxLogWarning(_("Unable to delete : %s"), myVectorFileName.GetFullName());
    }

    // create polygon file
    vrLayerVectorOGR myOutVector;
    if(myOutVector.Create(myVectorFileName.GetFullPath(), 3) == false){ // 3 is for polygon
        wxLogError( _("Creation of %s file failed."), myVectorFileName.GetFullName());
    }
    
    OGRFieldDefn myFieldDefn ("Value", OFTInteger);
    myOutVector.AddField(myFieldDefn);
    
    GDALPolygonize(GDALGetRasterBand(hOutDS, 1),
                NULL,
                   myOutVector.GetLayerRef(),
                   0,
                   NULL,
                   NULL,
                   NULL);
    
    
    ////////////////////////////
    // Convert to line or
    // copy to polygon
    ///////////////////////////
    
    // convert to lines
    if (m_Shape->GetGeometryType() == wkbLineString) {
        bool bReturn = _ConvertPolygonToLines(&myOutVector);
        GDALClose(hOutDS);
        return bReturn;
    }
    
    // TODO: copy to polygons
    
    GDALClose(hOutDS);
	return true;
}



bool PlIntOperation::ComputeLineSmallResolution (vrCoordinate * coord){
    
    ////////////////////////////
    // compute visible DEM part and resolution
    ////////////////////////////
    vrRealRect myImgReal;
    m_MNT->GetExtent(myImgReal);
    wxSize myImgPx = m_MNT->GetPixelSize();
    
    vrRealRect myWndReal = coord->GetExtent();
    vrRealRect myIntersect = myWndReal.Intersect(myImgReal);
    if (myIntersect.IsOk() == false) {
        return false;
    }
    
    // width of image to display (in pixels)
    int pxWidthVisible = wxRound(myIntersect.m_width * myImgPx.GetWidth() / myImgReal.m_width);
    int pxHeightVisible = wxRound(myIntersect.m_height * myImgPx.GetHeight() / myImgReal.m_height);
    
    // starting position from where we get image data (px)
    int ximg = wxRound((myIntersect.GetLeft() - myImgReal.GetLeft())  * myImgPx.GetWidth() / myImgReal.m_width);
    int yimg = wxRound((myIntersect.GetTop() - myImgReal.GetTop()) * myImgPx.GetHeight() / myImgReal.m_height);
    
    // returning values
    wxRect myImgPxExtractedResult;
    myImgPxExtractedResult.SetTopLeft(wxPoint(ximg, yimg));
    myImgPxExtractedResult.width = pxWidthVisible;
    myImgPxExtractedResult.height = pxHeightVisible;
    
    double myRasterPxSizeX = wxRound(fabs(myIntersect.m_width / coord->GetPixelSize()));
    double myRasterPxSizeY = wxRound(fabs(myIntersect.m_height / coord->GetPixelSize()));
    
    if (myImgPxExtractedResult.IsEmpty()) {
        wxLogMessage("Image is outside the display.");
        return false;
        
    }
    
    /////////////////////////////
    // Create memory raster for results
    ////////////////////////////
    
    //TODO: Change to memory when working
    GDALDriverH	hDriver = GDALGetDriverByName("GTiff");
    if (hDriver == NULL) {
        wxLogError(_("Unable to load GTiff Driver, export unavailable!"));
        return false;
    }
    wxFileName destination (wxStandardPaths::Get().GetTempDir(), "test_pling.tif");
    
    GDALDatasetH hOutDS = GDALCreate(hDriver, destination.GetFullPath(),
                                     myRasterPxSizeX,
                                     myRasterPxSizeY,
                                     1, GDT_Float32, NULL);
    if (hOutDS == NULL) {
        wxLogError(_("Error creating '%s'!"), destination.GetFullName());
        return false;
    }
    wxLogMessage(_("Creating '%s'"), destination.GetFullPath());
    
    double myGeoTransform[6] = {
        myIntersect.GetLeft(),
        coord->GetPixelSize(),
        0.0,
        myIntersect.GetTop(),
        0.0,
        coord->GetPixelSize() * -1.0
    };
    GDALSetGeoTransform(hOutDS, &myGeoTransform[0]);
    
    
    ////////////////////////////
    // extract DEM data for
    // concerned area, compute above or under the plane
    // and write the result to the temporary raster.
    ///////////////////////////
    
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    _ComputeABCD(a, b, c, d);
    
    // read row by row
    double myYinc = myImgPxExtractedResult.GetHeight() / myRasterPxSizeY;
    for (unsigned int i = 0 ; i < myRasterPxSizeY; i++) {
        float * imgdata = (float *) CPLMalloc(myRasterPxSizeX * GDALGetDataTypeSize(GDT_Float32) / 8);
        if(m_MNT->GetDatasetRef()->RasterIO(GF_Read,
                                            myImgPxExtractedResult.GetLeft(),
                                            myImgPxExtractedResult.GetTop() + ( i * myYinc ),
                                            myImgPxExtractedResult.GetWidth(),
                                            wxRound(myImgPxExtractedResult.GetHeight() / myRasterPxSizeY),
                                            imgdata,
                                            myRasterPxSizeX,
                                            1,
                                            GDT_Float32,
                                            1,
                                            NULL, 0, 0, 0) != CE_None){
            wxLogError(_("Error while reading loop : %d"), i);
            break;
        }
        
        // compute under / above plane
        for (unsigned int y = 0; y < myRasterPxSizeX ; y++) {
            OGRPoint myPt;
            myPt.setX( myIntersect.GetLeft() + ( y * coord->GetPixelSize() ) );
            myPt.setY( myIntersect.GetTop() + ( i * coord->GetPixelSize() ) );
            
            float myzValue = * (imgdata + y);
            myPt.setZ( myzValue );
            * (imgdata + y) = _IsUnderOrAbovePlane(a, b, c, d, &myPt);
        }
        
        if(GDALDatasetRasterIO (hOutDS,
                                GF_Write,
                                0,
                                i,
                                myRasterPxSizeX,
                                1,
                                imgdata,
                                myRasterPxSizeX,
                                1,
                                GDT_Float32,
                                1,
                                NULL,
                                0,
                                0,
                                0) != CE_None){
            break;
        }
        CPLFree(imgdata);
    }
    
    
    ////////////////////////////
    // Convert raster to polygon
    ///////////////////////////
    // TODO: Use in memory polygon layer when working
    
    wxFileName myVectorFileName (wxStandardPaths::Get().GetTempDir(), "test_pling_poly.shp");
    
    // try to delete vector file if existing
    const char *pszVectorDriverName = "ESRI Shapefile";
    OGRSFDriver * poVectorDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszVectorDriverName);
    if( poVectorDriver == NULL ){
        wxLogWarning(_("%s driver not available."), pszVectorDriverName );
        GDALClose(hOutDS);
        return false;
    }
    
    if (myVectorFileName.Exists() && poVectorDriver->DeleteDataSource(myVectorFileName.GetFullPath()) != OGRERR_NONE){
        wxLogWarning(_("Unable to delete : %s"), myVectorFileName.GetFullName());
    }
    
    // create polygon file
    vrLayerVectorOGR myOutVector;
    if(myOutVector.Create(myVectorFileName.GetFullPath(), 3) == false){ // 3 is for polygon
        wxLogError( _("Creation of %s file failed."), myVectorFileName.GetFullName());
    }
    
    OGRFieldDefn myFieldDefn ("Value", OFTInteger);
    myOutVector.AddField(myFieldDefn);
    
    GDALPolygonize(GDALGetRasterBand(hOutDS, 1),
                   NULL,
                   myOutVector.GetLayerRef(),
                   0,
                   NULL,
                   NULL,
                   NULL);
    
    
    ////////////////////////////
    // Convert to line or
    // copy to polygon
    ///////////////////////////
    
    // convert to lines
    if (m_Shape->GetGeometryType() == wkbLineString) {
        bool bReturn = _ConvertPolygonToLines(&myOutVector);
        GDALClose(hOutDS);
        return bReturn;
    }
    
    // TODO: copy to polygons
    
    GDALClose(hOutDS);
    return true;
}



bool PlIntOperation::_ConvertPolygonToLines (vrLayerVectorOGR * polyvector){
    wxASSERT(polyvector);
    wxASSERT(m_Shape);
    
    // create rect, one pixel smaller than the raster extent
    vrRealRect myEnveloppe;
    polyvector->GetExtent(myEnveloppe);
    OGRLinearRing myRing;
    myRing.addPoint(myEnveloppe.GetLeft(), myEnveloppe.GetTop());
    myRing.addPoint(myEnveloppe.GetRight(), myEnveloppe.GetTop());
    myRing.addPoint(myEnveloppe.GetRight(), myEnveloppe.GetBottom());
    myRing.addPoint(myEnveloppe.GetLeft(), myEnveloppe.GetBottom());
    myRing.addPoint(myEnveloppe.GetLeft(), myEnveloppe.GetTop());

    OGRPolygon myIntersectPolygon;
    myIntersectPolygon.addRing(&myRing);
    OGRGeometry * myPolyBuffer = myIntersectPolygon.Buffer(-1.0 * m_MNT->GetPixelWidth());
    wxASSERT_MSG(myPolyBuffer, "Are you using a version of GDAL built without the GEOS library ?");
    
    // convert polygons with attribut (1) to lines
    polyvector->SetAttributeFilter("Value=1");
    OGRFeature * poFeature = NULL;
	while( (poFeature = polyvector->GetNextFeature(false)) != NULL )
	{
        OGRPolygon * myPoly = static_cast<OGRPolygon*>( poFeature->GetGeometryRef());
        OGRLineString myLineExt;
        myLineExt.addSubLineString(myPoly->getExteriorRing());
        OGRGeometry * myIntersectGeom = myPolyBuffer->Intersection(&myLineExt);
        if (myIntersectGeom) {
            m_Shape->AddFeature(myIntersectGeom);
        }
        
        for (unsigned int i = 0 ; i< myPoly->getNumInteriorRings(); i++) {
            OGRLineString myLineInt;
            myLineInt.addSubLineString(myPoly->getInteriorRing(i));
            OGRGeometry * myGeom = myPolyBuffer->Intersection(&myLineInt);
            if (myGeom) {
                m_Shape->AddFeature(myGeom);
            }
        }
		OGRFeature::DestroyFeature(poFeature);
	}
    return true;
}


void PlIntOperation::_ComputeABCD (double & a, double & b, double & c, double & d){
    
    OGRPoint * myP1 = static_cast<OGRPoint*>( m_Points.getGeometryRef(0) );
    OGRPoint * myP2 = static_cast<OGRPoint*>( m_Points.getGeometryRef(1) );
    OGRPoint * myP3 = static_cast<OGRPoint*>( m_Points.getGeometryRef(2) );
    
    double X1 = myP1->getX();
    double Y1 = myP1->getY();
    double Z1 = myP1->getZ();
    
    double X2 = myP2->getX();
    double Y2 = myP2->getY();
    double Z2 = myP2->getZ();
    
    double X3 = myP3->getX();
    double Y3 = myP3->getY();
    double Z3 = myP3->getZ();

    a = (Y2 - Y1)*(Z3 - Z1) - (Z2 - Z1)*(Y3 - Y1);
    b = (-1)*((X2 - X1)*(Z3 - Z1) - (Z2 -Z1)*(X3 - X1));
    c = (X2 - X1)*(Y3 - Y1) - (Y2 - Y1)*(X3 - X1);
    d = (-1)*(a*X1 + b*Y1 + c*Z1);
    
    // checking if plane normal is up or down
    // suggested by RM.
    if (c < 0) {
        a =  -1.0 * a;
        b = -1.0 * b;
        c = -1.0 * c;
        d = -1.0 * d;
    }
}



float PlIntOperation::_IsUnderOrAbovePlane (const double & a, const double & b, const double & c, const double & d, OGRPoint * pt){
    double myRetValue = a * pt->getX() + b * pt->getY() + c * pt->getZ() + d;
    
    if (myRetValue < 0.0) {
        return -1;
    }
    
    if (myRetValue > 0.0) {
        return 1;
    }
    
    return myRetValue;
}


OGRPoint PlIntOperation::_ComputeVector (int index1, int index2){
    OGRPoint myResult;
    OGRPoint * p1 = static_cast<OGRPoint*>(m_Points.getGeometryRef(index1));
    OGRPoint * p2 = static_cast<OGRPoint*>(m_Points.getGeometryRef(index2));
    wxASSERT(p1);
    wxASSERT(p2);
    
    myResult.setX( p2->getX() - p1->getX());
    myResult.setY( p2->getY() - p1->getY());
    myResult.setZ( p2->getZ() - p1->getZ());
    
    return myResult;
}


OGRPoint PlIntOperation::_MultiplyVector (OGRPoint v1, OGRPoint v2){
    OGRPoint myResult;
    // X = v1.y * v2.z - v2.y * v1*z
    myResult.setX(v1.getY() * v2.getZ() - v2.getY() * v1.getZ());
    // Y = v1.z * v2.x - v2.z * v1.x
    myResult.setY(v1.getZ() * v2.getX() - v2.getZ() * v1.getX());
    // Z = v1.x * v2.y - v2.x * v1.y
    myResult.setZ(v1.getX() * v2.getY() - v2.getX() * v1.getY());
    return myResult;
}



