/***************************************************************************
 profileview_dlg
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "profileview_dlg.h"
#include "vrlayerraster.h"
#include "vroperationvectorprofiler.h"



BEGIN_EVENT_TABLE( ProfileView_DLG, wxFrame )
EVT_CLOSE( ProfileView_DLG::OnClose )
EVT_MENU( wxID_ZOOM_FIT, ProfileView_DLG::OnZoomToFit )
EVT_MENU( wxID_EXIT, ProfileView_DLG::OnCloseMenu )

EVT_MENU(wxID_ZOOM_IN, ProfileView_DLG::OnToolZoom)
EVT_MENU(wxID_MORE, ProfileView_DLG::OnToolPan)

EVT_COMMAND( wxID_ANY, vrEVT_TOOL_ZOOM, ProfileView_DLG::OnToolZoomAction)
EVT_COMMAND( wxID_ANY, vrEVT_TOOL_ZOOMOUT, ProfileView_DLG::OnToolZoomAction)
EVT_COMMAND( wxID_ANY, vrEVT_TOOL_PAN, ProfileView_DLG::OnToolPanAction)
END_EVENT_TABLE()


ProfileView_DLG::ProfileView_DLG( wxWindow* parent, vrLayerManager * layermanager, vrLayer * profile, const vrArrayLayer & raster, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style ){
    m_LayerManagerRef = layermanager;
    m_Display = NULL;
    
    _CreateControls();
    _CreateMenu();
    
    m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManagerRef, this, m_Display, NULL);
    
    // search available name
    long loop = 1;
    wxFileName myProfilName;
    wxString myProfilFormat = _T("profil-%ld.memory");
    while (1) {
        myProfilName = wxFileName (wxString::Format(myProfilFormat,loop));
        if (m_LayerManagerRef->GetLayer(myProfilName) == NULL){
            break;
        }
        ++loop;
    }
    
    vrLayerVectorOGR * myProfileLayer = static_cast<vrLayerVectorOGR*>(profile);
    wxString myColours [] = {_T("RED"), _T("GREEN"), _T("BLUE"), _T("PURPLE"),_T("BROWN"),_T("CYAN"),_T("DARK GREY"),_T("ORANGE"),_T("YELLOW"),_T("KHAKI")};
    // create profiles and add them to the viewerlayermanager
    for (unsigned int i = 0; i< raster.GetCount(); i++) {
        OGRFeature * myFeature = myProfileLayer->GetFeature(myProfileLayer->GetSelectedIDs()->Item(0));
        if (myFeature == NULL) {
            wxLogError(_("Selected feature is NULL ?!"));
            continue;
        }
        
        vrLayerRasterGDAL * myLayerRaster = (vrLayerRasterGDAL*) raster.Item(i);
        vrOperationVectorProfiler myProfiler (myFeature->GetGeometryRef(), myLayerRaster);
        if (myProfiler.IsOk() == false) {
            continue;
        }
        
        myProfiler.DoProfile();
        if (myProfiler.GetResultRef()->GetCount() == 0) {
            wxLogWarning (_("No results when profiling: '%s'"), myLayerRaster->GetFileName().GetFullName());
            continue;
        }
        OGRFeature::DestroyFeature(myFeature);
        
        vrLayerVectorOGR * myLayerResult = new vrLayerVectorOGR();
        if(myLayerResult->Create(myProfilName, wkbLineString)==false){
            wxLogError(_("Unable to create profile: '%s'"), myProfilName.GetName());
            continue;
        }
        
        OGRLineString myLine;
        double myIncrementDist = myProfiler.GetIncrementDistance();
        for (unsigned int v = 0; v < myProfiler.GetResultRef()->GetCount(); v++) {
            myLine.addPoint(v * myIncrementDist, myProfiler.GetResultRef()->Item(v));
        }
        myLayerResult->AddFeature(&myLine);
        
        m_LayerManagerRef->Add(myLayerResult);
        
        vrRenderVector * myRender = new vrRenderVector();
        
        int myColorIndex = i % 10;
        wxASSERT(myColorIndex >= 0 && myColorIndex < (sizeof(myColours) / sizeof(wxString)));
        myRender->SetColorPen(wxTheColourDatabase->Find(myColours[myColorIndex]));
        m_ViewerLayerManager->Add(-1, myLayerResult, myRender);
        
        ++ loop;
        myProfilName = wxFileName(wxString::Format(myProfilFormat, loop));
    }
    

    
}



ProfileView_DLG::~ProfileView_DLG(){
	
}



void ProfileView_DLG::OnClose( wxCloseEvent& event ) {
    m_LayerManagerRef->RemoveViewerLayerManager(m_ViewerLayerManager);
    Destroy();
}



void ProfileView_DLG::OnZoomToFit( wxCommandEvent& event ) {
    m_ViewerLayerManager->ZoomToFit(true);
    m_ViewerLayerManager->Reload();
}



void ProfileView_DLG::OnToolZoom(wxCommandEvent & event) {
	m_Display->SetToolZoom();
}



void ProfileView_DLG::OnToolZoomAction(wxCommandEvent & event) {
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	// getting rectangle
	vrCoordinate * myCoord = m_Display->GetCoordinate();
	wxASSERT(myCoord);
    
	// get real rectangle
	vrRealRect myRealRect;
	bool bSuccess = myCoord->ConvertFromPixels(myMsg->m_Rect, myRealRect);
	wxASSERT(bSuccess == true);
    
	// get fitted rectangle
	vrRealRect myFittedRect =myCoord->GetRectFitted(myRealRect);
	wxASSERT(myFittedRect.IsOk());
    
	// zoom out
	if (myMsg->m_EvtType == vrEVT_TOOL_ZOOM) {
		m_ViewerLayerManager->Zoom(myFittedRect);
	}
	else {
		m_ViewerLayerManager->ZoomOut(myFittedRect);
	}
	wxDELETE(myMsg);
}



void ProfileView_DLG::OnToolPan(wxCommandEvent & event) {
	m_Display->SetToolPan();
}



void ProfileView_DLG::OnToolPanAction(wxCommandEvent & event) {
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
    
	vrCoordinate * myCoord = m_Display->GetCoordinate();
	wxASSERT(myCoord);
    
	wxPoint myMovedPos = myMsg->m_Position;
	wxPoint2DDouble myMovedRealPt;
	if (myCoord->ConvertFromPixels(myMovedPos, myMovedRealPt)==false){
		wxLogError("Error converting point : %d, %d to real coordinate",
				   myMovedPos.x, myMovedPos.y);
		wxDELETE(myMsg);
		return;
	}
    
	vrRealRect myActExtent = myCoord->GetExtent();
	myActExtent.MoveLeftTopTo(myMovedRealPt);
	myCoord->SetExtent(myActExtent);
	m_ViewerLayerManager->Reload();
	wxDELETE(myMsg);
}




void ProfileView_DLG::OnCloseMenu( wxCommandEvent& event ) {
    Close();
}



void ProfileView_DLG::_CreateControls(){
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_Display = new vrViewerDisplay( m_panel1);
	bSizer2->Add( m_Display, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	wxStatusBar* m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
    m_Display->SetStatusCoordinates(m_statusBar1, 0, _("No coordinates"));
    m_Display->SetNoDataText(_("No Profile Data"));
}



void ProfileView_DLG::_CreateMenu(){
    wxMenuBar* m_menubar1;
	m_menubar1 = new wxMenuBar( 0 );
	wxMenu* m_menu1;
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ZOOM_FIT, wxString( _("Zoom to fit\tCtrl+0") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );
	
    m_menu1->Append(wxID_ZOOM_IN, _("Zoom\tZ"));
    m_menu1->Append(wxID_MORE, _("Pan\tH"));
    
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_EXIT, wxString( _("Exit\tCtrl+W") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );
	
	m_menubar1->Append( m_menu1, _("Actions") );
	
	this->SetMenuBar( m_menubar1 );
}


