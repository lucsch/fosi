/***************************************************************************
 plintoperation
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PLINT_OPERATION_H_
#define _PLINT_OPERATION_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/overlay.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include "vrrealrect.h"

class vrLayerRasterGDAL;
class vrLayerVectorOGR;
class vrCoordinate;
class PlIntOperation {
public:
    PlIntOperation(vrLayerRasterGDAL * mnt, int bandno, vrLayerVectorOGR * shape);
    virtual ~PlIntOperation();
    
    bool ComputeAndAddPoint(double x, double y, long index);
    bool ClearPoints();
    bool GetPlaneInfo(double & dip, double & direction);
    OGRMultiPoint * GetPointsRef() {return &m_Points;}
    bool ComputeLineFullResolution (vrCoordinate * coord);
    bool ComputeLineSmallResolution (vrCoordinate * coord);

    
private:
    vrLayerRasterGDAL * m_MNT;
    vrLayerVectorOGR * m_Shape;
    int m_MNTBand;
    OGRMultiPoint m_Points;
    
    OGRPoint _ComputeVector (int index1, int index2);
    OGRPoint _MultiplyVector (OGRPoint v1, OGRPoint v2);
    bool _ConvertPolygonToLines (vrLayerVectorOGR * polyvector);
    void _ComputeABCD (double & a, double & b, double & c, double & d);
    float _IsUnderOrAbovePlane (const double & a, const double & b, const double & c, const double & d, OGRPoint * pt);
};

#endif


