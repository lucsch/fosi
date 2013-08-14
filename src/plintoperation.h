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

class vrLayerRasterGDAL;
class PlIntOperation {
public:
    PlIntOperation(vrLayerRasterGDAL * mnt, int bandno);
    virtual ~PlIntOperation();
    
    bool ComputeAndAddPoint(double x, double y, long index);
    bool ClearPoints();
    bool GetPlaneInfo(double & dip, double & direction);
    OGRMultiPoint * GetPointsRef() {return &m_Points;}
    
private:
    vrLayerRasterGDAL * m_MNT;
    int m_MNTBand;
    OGRMultiPoint m_Points;
    
    OGRPoint _ComputeVector (int index1, int index2);
    OGRPoint _MultiplyVector (OGRPoint v1, OGRPoint v2);
};

#endif

