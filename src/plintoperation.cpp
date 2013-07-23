/***************************************************************************
 plintoperation
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plintoperation.h"
#include "vrlayerraster.h"


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



