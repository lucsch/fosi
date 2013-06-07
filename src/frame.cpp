/***************************************************************************
 frame.cpp
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "frame.h"
#include "log.h"  // for double logging process
#include <wx/splitter.h>
#include <wx/stdpaths.h>
#include "preference_dlg.h"
#include <wx/fileconf.h>
#include "vrrender.h"
#include "vrlabel.h"
#include "vrlayerraster.h"
#include "vrprogress.h"
#include "vroomgis_bmp.h"
#include "accelerators.h"
#include "wxhgversion_dlg.h"
#include "wxhgversion_core.h"
#include "general_bmp.h"


BEGIN_EVENT_TABLE( Frame, wxFrame )
	EVT_MENU( wxID_EXIT, Frame::OnQuit )
	EVT_MENU( wxID_PREFERENCES, Frame::OnPreferences)
    EVT_MENU( wxID_HELP, Frame::OnUserManual)
    EVT_MENU( MENU_WEBSITE, Frame::OnWebSite)
	EVT_MENU( wxID_ABOUT, Frame::OnAbout)
    EVT_MENU( MENU_CHECK_UPDATE, Frame::OnCheckUpdates)
	EVT_MENU( MENU_WINDOW_LOG, 	Frame::OnLogWindow)
	EVT_MENU( MENU_DATA_ADD, Frame::OnLayerAdd)
	EVT_MENU( MENU_DATA_REMOVE, Frame::OnLayerRemove)
	EVT_IDLE( Frame::OnUpdateIdle)
	EVT_CLOSE( Frame::OnClose)
	EVT_MENU( MENU_FRAME_SELECT, Frame::OnToolSelect)
    EVT_MENU( MENU_FRAME_CLEAR_SELECTION, Frame::OnToolClearSelection)
	EVT_MENU( wxID_ZOOM_IN, Frame::OnToolZoom)
	EVT_MENU( wxID_ZOOM_FIT, Frame::OnToolZoomToFit)
	EVT_MENU( MENU_FRAME_PAN,  Frame::OnToolPan)
	EVT_COMMAND( wxID_ANY, vrEVT_TOOL_ZOOM, Frame::OnToolZoomAction)
	EVT_COMMAND( wxID_ANY, vrEVT_TOOL_ZOOMOUT, Frame::OnToolZoomAction)
	EVT_COMMAND( wxID_ANY, vrEVT_TOOL_PAN, Frame::OnToolPanAction)
	EVT_COMMAND( wxID_ANY, vrEVT_TOOL_SELECT, Frame::OnToolSelectAction)
	EVT_KEY_DOWN(Frame::OnKeyDown)
	EVT_KEY_UP(Frame::OnKeyUp)	
    EVT_UPDATE_UI(MENU_FRAME_CLEAR_SELECTION, Frame::OnUpdateUIToolClearSelection)
END_EVENT_TABLE()



ctDropFiles::ctDropFiles(Frame * parent){
	wxASSERT(parent);
	m_Frame = parent;
}



bool ctDropFiles::OnDropFiles(wxCoord x, wxCoord y,const wxArrayString & filenames){
	if (filenames.GetCount() == 0) {
		return false;
	}
    
	m_Frame->AddLayers(filenames);
	return true;
}



void Frame::_CreateControls() {
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

    wxSplitterWindow* m_splitter1;
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_THIN_SASH );

	wxPanel* m_panel1;
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	m_vrTOC = new vrViewerTOCTree( m_panel1, wxID_ANY);
	bSizer2->Add( m_vrTOC->GetControl(), 1, wxEXPAND, 5 );
	m_panel1->SetSizer( bSizer2 );
    m_splitter1->Initialize(m_panel1);
	
    wxPanel* m_panel2;
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
    
	m_vrDisplay = new vrViewerDisplay( m_panel2, wxID_ANY);
	bSizer3->Add( m_vrDisplay, 1, wxEXPAND, 5 );
	m_panel2->SetSizer( bSizer3 );
    m_splitter1->Initialize(m_panel2);
    
	m_splitter1->SplitVertically( m_panel1, m_panel2, 230 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
    
    m_InfoBar = new WebUpdateInformationBar(this);
    bSizer1->Add(m_InfoBar, wxSizerFlags().Expand());

    this->SetSizer(bSizer1);
    bSizer1->SetSizeHints(this);
    
	m_vrDisplay->SetStatusCoordinates(GetStatusBar(), 0, _("Coordinates not available"));
	//this->SetSizer( bSizer1 );
	//this->Layout();
}



void Frame::_CreateMenus() {
	wxMenuBar* m_menubar1;
	m_menubar1 = new wxMenuBar( 0 );

	// PROJECT
	wxMenu* m_menu1 = new wxMenu();
#ifndef __WXMAC__
    m_menu1->Append( new wxMenuItem( m_menu1, wxID_EXIT));
	m_menubar1->Append( m_menu1, _("Project") );
#endif 
	
	// DATA
	wxMenu* m_menu5 = new wxMenu();
	m_menu5->Append( new wxMenuItem( m_menu5, MENU_DATA_ADD,_("Add layer..."), wxEmptyString, wxITEM_NORMAL ) );
	m_menu5->Append(  new wxMenuItem( m_menu5, MENU_DATA_REMOVE, _("Remove layer...") , wxEmptyString, wxITEM_NORMAL ) );
	m_menubar1->Append( m_menu5, _("Data") );
	
	// SELECTION
	wxMenu* m_menu51;
	m_menu51 = new wxMenu();
	m_menu51->Append( new wxMenuItem( m_menu51, MENU_FRAME_SELECT, _("Select tool\tV"), wxEmptyString, wxITEM_NORMAL ));
	m_menu51->Append( new wxMenuItem( m_menu51, MENU_FRAME_CLEAR_SELECTION,
									 _("Clear selection"), wxEmptyString, wxITEM_NORMAL ));
	m_menubar1->Append( m_menu51, _("Selection") ); 
	
	// VIEW
	wxMenu* m_menu6;
	m_menu6 = new wxMenu();
	m_menu6->Append( new wxMenuItem( m_menu6, wxID_ZOOM_IN, _("Zoom tool\tZ"), wxEmptyString, wxITEM_NORMAL ));
	m_menu6->Append( new wxMenuItem( m_menu6, MENU_FRAME_PAN, _("Pan tool\tH"), wxEmptyString, wxITEM_NORMAL ));
	m_menu6->AppendSeparator();
	m_menu6->Append( new wxMenuItem( m_menu6, wxID_ZOOM_FIT,_("Zoom to Fit\tCtrl+0"), wxEmptyString, wxITEM_NORMAL ) );
	m_menubar1->Append( m_menu6, _("View") ); 
	
	// WINDOW
	wxMenu* m_menu2 = new wxMenu();
	m_menu2->Append(new wxMenuItem( m_menu2, MENU_WINDOW_LOG, _("Log window...\tCtrl+L"), wxEmptyString, wxITEM_NORMAL ) );
	
	// Bug #42, append preference into the Window menu, except for Mac where it goes into Application menu 
	m_menu2->Append(new wxMenuItem(m_menu1, wxID_PREFERENCES));
	m_menubar1->Append( m_menu2, _("Window") );

	// HELP
	wxMenu* m_menu3 = new wxMenu();
	m_menu3->Append( new wxMenuItem( m_menu3, wxID_ABOUT));
	m_menu3->Append( new wxMenuItem( m_menu3, wxID_HELP, _("User Manual") , wxEmptyString, wxITEM_NORMAL ));
	m_menu3->Append( new wxMenuItem( m_menu3, MENU_WEBSITE, _("Website") , wxEmptyString, wxITEM_NORMAL  ));
    m_menu3->AppendSeparator();
	m_menu3->Append( new wxMenuItem( m_menu3, MENU_CHECK_UPDATE, _("Check for updates") , wxEmptyString, wxITEM_NORMAL ));
    m_menubar1->Append( m_menu3, _("Help") );
	this->SetMenuBar( m_menubar1 );
	
	// ACCELERATORS
	wxAcceleratorEntry myEntries[3];
	myEntries[0].Set(wxACCEL_NORMAL, (int) 'V', MENU_FRAME_SELECT);
	myEntries[1].Set(wxACCEL_NORMAL, (int) 'Z', wxID_ZOOM_IN);
	myEntries[2].Set(wxACCEL_NORMAL, (int) 'H', MENU_FRAME_PAN);
	wxAcceleratorTable myAccelTable (3, myEntries);
	wxASSERT(m_vrDisplay);
	m_vrDisplay->SetAcceleratorTable(myAccelTable);
}



void Frame::_CreateToolbar() {
	long myStyle = wxTB_FLAT | wxTB_HORIZONTAL;
	// conditionnal compilation for better look under win32
#ifndef __WXMSW__
	myStyle += wxTB_TEXT;
#endif

	wxToolBar* m_toolBar1;
	m_toolBar1 = this->CreateToolBar( myStyle, wxID_ANY );
	m_toolBar1->SetToolBitmapSize( wxSize( 32,32 ) );
	wxString mySelectName = _("Select");
	m_toolBar1->AddTool( MENU_FRAME_SELECT, mySelectName , wxBitmap(*_img_toolbar_select), wxNullBitmap, wxITEM_NORMAL, mySelectName, wxEmptyString );
	wxString myZoomName = _("Zoom to fit");
	m_toolBar1->AddTool( wxID_ZOOM_FIT, myZoomName, wxBitmap(*_img_toolbar_zoom_fit), wxNullBitmap, wxITEM_NORMAL,myZoomName, wxEmptyString );
	wxString myZoom2Name = _("Zoom");
	m_toolBar1->AddTool( wxID_ZOOM_IN, myZoom2Name , wxBitmap(*_img_toolbar_zoom), wxNullBitmap, wxITEM_NORMAL, myZoom2Name, wxEmptyString );
	wxString myPanName = _("Pan");
	m_toolBar1->AddTool( MENU_FRAME_PAN, myPanName, wxBitmap(*_img_toolbar_pan), wxNullBitmap, wxITEM_NORMAL, myPanName, wxEmptyString );
	m_toolBar1->Realize();
}



void Frame::_CreateAccelerators() {
	wxArrayAccelerators myAccels;
	wxAcceleratorEntry myEntry1 (wxACCEL_NORMAL, (int) 'V', MENU_FRAME_SELECT);
	wxAcceleratorEntry myEntry2 (wxACCEL_NORMAL, (int) 'Z', wxID_ZOOM_IN);
	wxAcceleratorEntry myEntry3 (wxACCEL_NORMAL, (int) 'H', MENU_FRAME_PAN);
	
	myAccels.Add(myEntry1);
	myAccels.Add(myEntry2);
	myAccels.Add(myEntry3);
	
	wxAcceleratorEntry * myEntries = new wxAcceleratorEntry[myAccels.GetCount()];
	for (unsigned int i = 0; i< myAccels.GetCount(); i++) {
		*(myEntries+i) = myAccels.Item(i);
	}
		
	wxAcceleratorTable myAccelTable (myAccels.GetCount(), myEntries);
	wxASSERT(m_vrDisplay);
	m_vrDisplay->SetAcceleratorTable(myAccelTable);
	wxDELETEA(myEntries);
}



void Frame::_SetSelectedFeature() {
	if (m_vrViewerLayerManager == NULL) {
		return;
	}
	
	int mySel = m_vrViewerLayerManager->GetSelectionCount();
	GetStatusBar()->SetStatusText(wxString::Format(_("%d Feature(s) selected"),mySel),1);
}



bool Frame::AddLayer(const wxFileName & file, long position, vrRender * render,
					 vrLabel * label, bool visible, bool readwrite) {
	if(m_vrLayerManager->Open(file, readwrite)==false){
			wxLogError(_("File %s not found, we will implement a search function later on"),
					   file.GetFullName());
		return false;
	}
	
	vrLayer * myLayer = m_vrLayerManager->GetLayer(file);
	wxASSERT(myLayer);
	
	// build overviews if needed.
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	wxASSERT(myConfig);
	myConfig->SetPath("GENERAL");
	bool myBuildOverviews = myConfig->ReadBool("BUILD_OVERVIEW", true);
	myConfig->SetPath("..");
	if (myBuildOverviews == true) {
		if(myLayer->GetType() == vrDRIVER_RASTER_C2D ||
		   myLayer->GetType() == vrDRIVER_RASTER_TIFF ||
		   myLayer->GetType() == vrDRIVER_RASTER_JPEG ||
		   myLayer->GetType() == vrDRIVER_RASTER_ESRIGRID){
			vrLayerRasterGDAL * myRaster = (vrLayerRasterGDAL*) myLayer;
			if (myRaster->HasOverviews() == false) {
				wxArrayInt myFactors;
				myFactors.Add(4);
				myFactors.Add(8);
				myFactors.Add(16);
				myFactors.Add(32);
				
				vrProgressSimple myProgressData (this, _("Building Overviews (Pyramids)"),
												 wxString::Format(_("Building Overviews: '%s'"),
																  myRaster->GetDisplayName().GetFullName()));
				myRaster->BuildOverviews(myFactors, vrOVERVIEW_NEAREST,
										 GDALUpdateSimple,
										 &myProgressData);
			}
		}
	}
	
	// add files to the viewer
	m_vrViewerLayerManager->Add(position, myLayer, render, label, visible);
	return true;
}



bool Frame::_RemoveLayer(long position) {
	wxASSERT(m_vrViewerLayerManager);
	vrRenderer * myRenderer = m_vrViewerLayerManager->GetRenderer(position);
	if (myRenderer == NULL) {
		wxLogError(_("Unable to found renderer at position : %ld"),position);
		return false;
	}
	
	vrLayer * myLayer = myRenderer->GetLayer();
	wxASSERT(myLayer);
	wxFileName myFile = myRenderer->GetLayer()->GetDisplayName();
	m_vrViewerLayerManager->Remove(myRenderer);

	// close layer (not used anymore);
	m_vrLayerManager->Close(myLayer);
    return true;
}



void Frame::_PreferenceChanged(bool refresh) {
	// update main panel color
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	wxASSERT(myConfig);
	myConfig->SetPath("GENERAL");
	wxColour myBackColour = myConfig->ReadObject("PREF_BACKGROUND_COLOUR", *wxWHITE);
	wxColour mySelColour = myConfig->ReadObject("SELECTION_COLOUR", *wxRED);
	myConfig->SetPath("..");
	wxASSERT(m_vrDisplay);
	m_vrDisplay->SetBackgroundColour(myBackColour);
	
	// update selection colour
	vrRender::SetDefaultSelectionColour(mySelColour);
	if (m_vrViewerLayerManager != NULL) {
		for (int i = 0; i< m_vrViewerLayerManager->GetCount(); i++) {
			m_vrViewerLayerManager->GetRenderer(i)->GetRender()->SetSelectionColour(mySelColour);
		}
	}
	
	if (refresh == true) {
		m_vrViewerLayerManager->Reload();
	}
	
}



void Frame::OnAbout(wxCommandEvent & event) {
	wxHgVersionDlg myDlg (this, wxID_ANY, _T(""));
    myDlg.SetBitmapLogo(*_img_app_sml);
    myDlg.ShowModal();
}



void Frame::OnCheckUpdates(wxCommandEvent & event){
    wxConfigBase * myConfig =  wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");
        
    long mySVNversion = wxNOT_FOUND;
    wxHgVersion::GetSoftNumber().ToLong(&mySVNversion);
    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    myUpdate->CheckNewVersion(mySVNversion, true, true, true);
}



void Frame::OnPreferences(wxCommandEvent & event){
	PreferenceDlg myPrefDlg (this);
	myPrefDlg.CenterOnParent(wxBOTH);
	int myValue = myPrefDlg.ShowModal();
	if(myValue == wxID_OK){
		_PreferenceChanged();
	}
}



void Frame::OnUserManual(wxCommandEvent & event) {
    wxLaunchDefaultBrowser("http://www.crealp.ch/coltopgis/documentation/doku.php");
}



void Frame::OnWebSite (wxCommandEvent & event){
    wxLaunchDefaultBrowser("http://www.crealp.ch/index.php?option=com_content&view=article&id=379&Itemid=292");
}



void Frame::OnQuit(wxCommandEvent & event) {
	Close(true);
}



void Frame::OnLogWindow(wxCommandEvent & event){
	m_LogWnd->Show(true);
	m_LogWnd->GetFrame()->Raise();
}



void Frame::OnLayerAdd(wxCommandEvent & event) {
	vrDrivers myDrivers;
	wxFileDialog myFileDlg (this, "Select GIS Layers",
							wxEmptyString,
							wxEmptyString,
							myDrivers.GetWildcards(vrDRIVERS_FLAG_ALL),
							wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
	
	wxArrayString myPathsFileName;
	
	if(myFileDlg.ShowModal()==wxID_OK){
		// try to open files
		myFileDlg.GetPaths(myPathsFileName);
		wxASSERT(myPathsFileName.GetCount() > 0);
		AddLayers(myPathsFileName);
	}
	
}



bool Frame::AddLayers (const wxArrayString & names){
	m_vrViewerLayerManager->FreezeBegin();
	for (unsigned int i = 0; i< names.GetCount(); i++) {
		AddLayer(names.Item(i));
	}
	m_vrViewerLayerManager->FreezeEnd();
	return true;
}



void Frame::OnLayerRemove(wxCommandEvent & event) {
	wxArrayString myLayersName;
	int myDipsIndex = wxNOT_FOUND;
	for (int i = 0; i<m_vrViewerLayerManager->GetCount(); i++) {
		vrRenderer * myRenderer = m_vrViewerLayerManager->GetRenderer(i);
		wxASSERT(myRenderer);
		if (myRenderer->GetLayer()->GetType() != vrDRIVER_VECTOR_C2P) {
			myLayersName.Add(myRenderer->GetLayer()->GetDisplayName().GetFullName());
		}
		else {
			myDipsIndex = i;
		}
	}
	
	if (myLayersName.IsEmpty()) {
		wxLogError("No layer opened, nothing to close");
		return;
	}
		
	wxMultiChoiceDialog myChoiceDlg (this, "Select Layer(s) to close",
									 "Close layer(s)",
									 myLayersName);
	if (myChoiceDlg.ShowModal() != wxID_OK) {
		return;
	}
	
	wxArrayInt myLayerToRemoveIndex = myChoiceDlg.GetSelections();
	if (myLayerToRemoveIndex.IsEmpty()) {
		wxLogWarning("Nothing selected, no layer will be closed");
		return;
	}

	// removing layer(s)
	m_vrViewerLayerManager->FreezeBegin();
	for (int j = (signed) myLayerToRemoveIndex.GetCount() -1; j >= 0 ; j--) {
		
		long myIndexToRemove = myLayerToRemoveIndex.Item(j);
		if (myDipsIndex != wxNOT_FOUND && myDipsIndex <= myIndexToRemove) {
			myIndexToRemove = myIndexToRemove + 1;
		}
		_RemoveLayer(myIndexToRemove);
	}
	
	m_vrViewerLayerManager->FreezeEnd();
}



void Frame::OnUpdateIdle(wxIdleEvent & event) {
	_SetSelectedFeature();
}



void Frame::OnClose(wxCloseEvent & event) {
	event.Skip();
}



void Frame::OnKeyDown(wxKeyEvent & event) {
	m_KeyBoardState = wxKeyboardState(event.ControlDown(),
									  event.ShiftDown(),
									  event.AltDown(),
									  event.MetaDown());
	if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
		event.Skip();
		return;
	}
	
	const vrDisplayTool * myTool = m_vrDisplay->GetTool();
	if (myTool == NULL) {
		event.Skip();
		return;
	}
	
	if (myTool->GetID() == wxID_ZOOM_IN) {
		m_vrDisplay->SetToolZoomOut();
	}
	event.Skip();
}



void Frame::OnKeyUp(wxKeyEvent & event) {
	if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
		event.Skip();
		return;
	}
	
	const vrDisplayTool * myTool = m_vrDisplay->GetTool();
	if (myTool == NULL) {
		event.Skip();
		return;
	}
	
	if (myTool->GetID() == wxID_ZOOM_OUT || myTool->GetID() == wxID_ZOOM_IN) {
		m_vrDisplay->SetToolZoom();
	}
	event.Skip();
}



void Frame::OnToolZoomToFit(wxCommandEvent & event) {
	m_vrViewerLayerManager->ZoomToFit(true);
	m_vrViewerLayerManager->Reload();
}



void Frame::OnToolSelect(wxCommandEvent & event) {
	m_vrDisplay->SetTool(new vrDisplayToolSelect(m_vrDisplay));
}



void Frame::OnToolSelectAction(wxCommandEvent & event) {
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
	m_vrViewerLayerManager->ClearSelection();
	if(m_vrViewerLayerManager->Select(myMsg->m_Rect) == wxNOT_FOUND){
		wxDELETE(myMsg);
		return;
	}
	
	m_vrViewerLayerManager->Reload();
	wxDELETE(myMsg);
	
}



void Frame::OnToolClearSelection(wxCommandEvent & event) {
    wxASSERT(m_vrViewerLayerManager);
    m_vrViewerLayerManager->ClearSelection();
    m_vrViewerLayerManager->Reload();
}



void Frame::OnToolZoom(wxCommandEvent & event) {
	m_vrDisplay->SetToolZoom();
}



void Frame::OnToolZoomAction(wxCommandEvent & event) {
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	// getting rectangle
	vrCoordinate * myCoord = m_vrViewerLayerManager->GetDisplay()->GetCoordinate();
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
		m_vrViewerLayerManager->Zoom(myFittedRect);
	}
	else {
		m_vrViewerLayerManager->ZoomOut(myFittedRect);
	}
	wxDELETE(myMsg);
}



void Frame::OnToolPan(wxCommandEvent & event) {
	m_vrDisplay->SetToolPan();
}



void Frame::OnToolPanAction(wxCommandEvent & event) {
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
	vrCoordinate * myCoord = m_vrViewerLayerManager->GetDisplay()->GetCoordinate();
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
	m_vrViewerLayerManager->Reload();
	wxDELETE(myMsg);
}



void Frame::OnUpdateUIToolClearSelection(wxUpdateUIEvent & event) {
    if (m_vrViewerLayerManager && m_vrViewerLayerManager->GetSelectionCount() > 0) {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}



Frame::Frame(const wxString & title) :
wxFrame(NULL, FRAME_WINDOW, title) {
	// Logging
	wxLog::SetLogLevel(wxLOG_Info);
	wxLog * myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
	delete wxLog::SetActiveTarget(myDlgLog);
	m_LogWnd = new wxLogWindow(this, "Fosi Log", false, true);
    
    //setlocale(LC_NUMERIC, "C");
	
	// preferences
	wxConfigBase::Set(new wxFileConfig(g_ProgName));
	m_vrViewerLayerManager = NULL;
	m_vrDisplay = NULL;
	m_vrLayerManager = NULL;

	// Design interface
	initialize_images();
	vroomgis_initialize_images();
	CreateStatusBar(3);
    // disable help menu into statusbar
    SetStatusBarPane(-1);
    SetStatusText(g_ProgName + _(" version: ") + wxHgVersion::GetSoftNumber(), 2);
    
	_CreateControls();
	_CreateMenus();
	_CreateToolbar();

	// add icon (windows / linux)
	wxIcon myIcon;
	myIcon.CopyFromBitmap(*_img_app_sml);
	SetIcon(myIcon);

	// vroomGIS
	m_vrLayerManager = new vrLayerManager();
	m_vrViewerLayerManager = new vrViewerLayerManager(m_vrLayerManager, this, m_vrDisplay , m_vrTOC);
		
	_CreateAccelerators();
	
	m_vrDisplay->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown),NULL, this);
	m_vrDisplay->Connect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp),NULL, this);

	m_vrDisplay->SetFocus();
	_PreferenceChanged(false);
	
    // DND
    m_vrTOC->GetControl()->SetDropTarget(new ctDropFiles(this));
    
    wxConfigBase * myConfig =  wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    bool bCheckStartup = myConfig->ReadBool("CHECK_AT_STARTUP", true);
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");
    
    if (bCheckStartup == false) {
        return;
    }
        
    long mySVNversion = wxNOT_FOUND;
    wxHgVersion::GetSoftNumber().ToLong(&mySVNversion);
    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    myUpdate->CheckNewVersion(mySVNversion, true, false, true);    
}



Frame::~Frame() {
	m_vrDisplay->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown),NULL, this);
	m_vrDisplay->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp),NULL, this);	

	// don't delete m_vrViewerLayerManager, will be deleted by the manager
	wxDELETE(m_vrLayerManager);

	delete wxLog::SetActiveTarget (NULL);

	uninitialize_images();
	vroomgis_clear_images();
}
