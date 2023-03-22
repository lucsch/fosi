
/***************************************************************************
 test_plint
 -------------------
 copyright            : (C) 2013  Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _TEST_PLINT_H_
#define _TEST_PLINT_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cxxtest/TestSuite.h>
#include "plintoperation.h"

class TEST_Plint : public CxxTest::TestSuite{
public:
	void testGetPlaneInfo (void){
        PlIntOperation myOp (NULL, 0);
        
        double myDip = 0;
        double myDir = 0;
        TS_ASSERT(myOp.GetPlaneInfo(myDip, myDir) == false);
        
        OGRPoint myP1;
        myP1.setX(587494.64);
        myP1.setY(99399.84);
        myP1.setZ(1242.15);
        
        OGRPoint myP2;
        myP2.setX(587497.41);
        myP2.setY(99398.54);
        myP2.setZ(1240.59);

        OGRPoint myP3;
        myP3.setX(587502.90);
        myP3.setY(99391.81);
        myP3.setZ(1238.57);
        
        myOp.GetPointsRef()->addGeometry(&myP1);
        myOp.GetPointsRef()->addGeometry(&myP2);
        myOp.GetPointsRef()->addGeometry(&myP3);
         
        TS_ASSERT(myOp.GetPlaneInfo(myDip, myDir));
        wxLogMessage( wxString::FromDouble(myDip) + _T(" / ") +  wxString::FromDouble(myDir));
    }
};


#endif
