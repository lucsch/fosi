/***************************************************************************
 plintoperation
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plintoperation.h"
#include "vrlayerraster.h"
#include "vrcoordinate.h"


PlIntOperation::PlIntOperation(vrLayerRasterGDAL * mnt, int bandno) {
    m_MNT = mnt;
    m_MNTBand = bandno;
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



bool PlIntOperation::ComputeLine (vrCoordinate * coord){
    // extract visible part of the DEM into memory
    return _ExtractRaster(coord);
}



bool PlIntOperation::_ExtractRaster (vrCoordinate * coord){
    
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
    wxFileName destination (wxFileName::GetHomeDir(), "test_pling.tif");
    
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
        myIntersect.GetTop(),
        0.0,
        0.0,
        coord->GetPixelSize()
    };
    GDALSetGeoTransform(hOutDS, &myGeoTransform[0]);
    
    
    ////////////////////////////
    // extract DEM data for
    // concerned area, compute above or under the plane
    // and write the result to the temporary raster.
    ///////////////////////////
    
    // read row by row
    double myYinc = myImgPxExtractedResult.GetHeight() / myRasterPxSizeY;
    for (unsigned int i = 0 ; i < myRasterPxSizeY; i++) {
        
        // TODO: Fix error here !!!!
        double * imgdata = (double *) malloc( myRasterPxSizeX * 1 );
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
        
    GDALClose(hOutDS);
	return true;
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



