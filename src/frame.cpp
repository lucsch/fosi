/***************************************************************************
 coltopgisframe.cpp
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#include "frame.h"
#include "log.h"	// for double logging process
#include <wx/splitter.h>
#include <wx/stdpaths.h>
#include "about_dlg.h"
#include "preference_dlg.h"
#include <wx/fileconf.h>
#include "dataframe.h"
#include "projectmanager.h"
#include "database.h"
#include "project.h"
#include "vrrender.h"
#include "vrlabel.h"
#include "datapanellayers.h"
#include "toc.h"
#include "vrlayerraster.h"
#include "vrprogress.h"
#include "securitydate.h"
#include "securitymessage.h"
#include "lsversion_core.h"
#include "vroomgis_bmp.h"




#include "coltop_plugin.h"


// bitmaps
#include "../art/coltop_bmp.h"



BEGIN_EVENT_TABLE( Frame, wxFrame )
	EVT_MENU( wxID_NEW, Frame::OnProjectNew)
	EVT_MENU( wxID_OPEN, Frame::OnProjectOpen)
	EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, Frame::OnProjectRecent)
	EVT_MENU( wxID_SAVE, Frame::OnProjectSave)
	EVT_MENU( wxID_SAVEAS, Frame::OnProjectSaveAs)
	EVT_MENU( wxID_EXIT, Frame::OnQuit )
	EVT_MENU( wxID_PREFERENCES, Frame::OnPreferences)
    EVT_MENU( wxID_HELP, Frame::OnUserManual)
    EVT_MENU( MENU_WEBSITE, Frame::OnWebSite)
	EVT_MENU( wxID_ABOUT, Frame::OnAbout)
    EVT_MENU( MENU_CHECK_UPDATE, Frame::OnCheckUpdates)
	EVT_MENU( MENU_WINDOW_LOG, 	Frame::OnLogWindow)
	EVT_MENU( MENU_WINDOW_DATA_MANAGER, Frame::OnDataManager)
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
	EVT_THREAD(COMMAND_SECURITY_THREAD_MESSAGE, Frame::OnDisplaySecurityMessage)
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
	m_vrTOC = new TOC( m_panel1, this, wxID_ANY);
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
	m_menu1->Append( new wxMenuItem( m_menu1, wxID_NEW) );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_menu1->AppendSeparator();
	
	m_menu1->Append( new wxMenuItem( m_menu1, wxID_OPEN));
	
	m_RecentMenu = new wxMenu();
	m_menu1->Append( wxID_ANY, _("Recent"), m_RecentMenu );
	
	wxMenuItem* m_separator2;
	m_separator2 = m_menu1->AppendSeparator();
	
	m_menu1->Append( new wxMenuItem( m_menu1, wxID_SAVE));
	m_menu1->Append( new wxMenuItem( m_menu1, wxID_SAVEAS, _("Save as...\tCtrl+Shift+S")));
	
	m_menu1->Append( new wxMenuItem( m_menu1, wxID_EXIT));
	m_menubar1->Append( m_menu1, _("Project") );

	
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
	m_menu2->Append(new wxMenuItem( m_menu2, MENU_WINDOW_DATA_MANAGER, _("Data Manager...\tCtrl+D") , wxEmptyString, wxITEM_NORMAL ) );
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
	wxString myDataManagerName = _("Data Manager");
	m_toolBar1->AddTool( MENU_WINDOW_DATA_MANAGER, myDataManagerName, wxBitmap(*_img_toolbar_datamanager), wxNullBitmap, wxITEM_NORMAL, myDataManagerName, wxEmptyString );
	m_toolBar1->Realize();
}



void Frame::_CreateAccelerators() {
	// ACCELERATORS
	/*wxAcceleratorEntry myEntries[3];
	myEntries[0].Set(wxACCEL_NORMAL, (int) 'V', MENU_FRAME_SELECT);
	myEntries[1].Set(wxACCEL_NORMAL, (int) 'Z', wxID_ZOOM_IN);
	myEntries[2].Set(wxACCEL_NORMAL, (int) 'H', MENU_FRAME_PAN);*/
	
	wxArrayAccelerators myAccels;
	wxAcceleratorEntry myEntry1 (wxACCEL_NORMAL, (int) 'V', MENU_FRAME_SELECT);
	wxAcceleratorEntry myEntry2 (wxACCEL_NORMAL, (int) 'Z', wxID_ZOOM_IN);
	wxAcceleratorEntry myEntry3 (wxACCEL_NORMAL, (int) 'H', MENU_FRAME_PAN);
	
	myAccels.Add(myEntry1);
	myAccels.Add(myEntry2);
	myAccels.Add(myEntry3);
	
	for (unsigned int i = 0; i<m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->GetAccelerators(myAccels);
	}
	
	wxAcceleratorEntry * myEntries = new wxAcceleratorEntry[myAccels.GetCount()];
	for (unsigned int i = 0; i< myAccels.GetCount(); i++) {
		*(myEntries+i) = myAccels.Item(i);
	}
		
	wxAcceleratorTable myAccelTable (myAccels.GetCount(), myEntries);
	wxASSERT(m_vrDisplay);
	m_vrDisplay->SetAcceleratorTable(myAccelTable);
	wxDELETEA(myEntries);
	
}



void Frame::_InitPlugins() {
	m_Plugins.Clear();
#ifdef ctCOLTOP_PLUGIN_ENABLED
	ctPlugin * myColtopPlugin = new ctPlugin(ctCOLTOP_PLUGIN_NAME);
	m_Plugins.Add(myColtopPlugin);
#endif
	
	// iterate plugins for adding menu and toolbar items.
	for (unsigned int i = 0; i< m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->AddMenu(this);
	}
	GetToolBar()->Realize();
}



void Frame::_EndPlugins() {
	for (unsigned int i = 0; i<m_Plugins.GetCount(); i++){
		Plugin * myPlugin = m_Plugins.Item(0);
		m_Plugins.Detach(0);
		wxDELETE(myPlugin);
	}
	wxASSERT(m_Plugins.GetCount()==0);
}



bool Frame::_ProjectQuestion(const wxString & text) {
	wxString myProjName =  m_ProjectManager->GetProjectName().GetFullName();
	if (myProjName.IsEmpty()) {
		myProjName = _("untitled");
	}
	int myAnswer = wxMessageBox(wxString::Format(_("Project '%s' was modified, Save modifications before %s"), 
												 myProjName,
												 text),
								_("Project modified"),
								wxICON_EXCLAMATION | wxYES_NO | wxCANCEL,
								this);
	wxCommandEvent myUnusedEvt;
	switch (myAnswer) {
		case wxYES:
			_ProjectSave(m_ProjectManager->GetProjectName());
			return true;
			break;
			
		case wxNO:
			return true;
			break;
			
			
		default:
			return false;
			break;
	}
	return false;
}



bool Frame::ProjectOpen(const wxFileName & file) {
	// check if actual project is modified
	if (m_ProjectManager->IsModified() == true) {
		if (_ProjectQuestion(_("Opening project"))==false) {
			return false;
		}
	}
		
	if (file.IsOk()) {
		if (m_ProjectManager->Open(file) == true) {
			_ProjectAfterOpen();
			return true;
		}
		wxLogError(_("Unable to open '%s'"), file.GetFullName());
		return false;
	}
	
	// try to get a nice path
	wxString myPath = wxStandardPaths::Get().GetDocumentsDir();
	if (m_ProjectManager->GetProjectName().IsOk() == true) {
		myPath = m_ProjectManager->GetProjectName().GetPath();
	}
	
	wxFileDialog myDlg(this, _("Open ColtopGIS project"), myPath,"",
					   "ColtopGIS project (*.c2p)|*.c2p", wxFD_OPEN| wxFD_FILE_MUST_EXIST);
	
	if (myDlg.ShowModal() == wxID_CANCEL) {
		return false;
	}
	wxFileName myFileName (myDlg.GetPath());
	bool bOpen = m_ProjectManager->Open(myFileName);
	if (bOpen == false) {
		wxLogError(_("Opening '%s' failed"), myFileName.GetFullName());
	}
	else {
		_ProjectAfterOpen();
	}
	return bOpen;
}



bool Frame::_ProjectSave(const wxFileName & file) {
	if (file.IsOk()) {
		_ProjectBeforeSave(file);
		bool bResult = m_ProjectManager->Save(file);
		if (bResult == true) {
			_ProjectAfterSave();
		}
		return bResult;
	}
	
	// try to get a nice path
	wxString myPath = wxStandardPaths::Get().GetDocumentsDir();
	if (m_ProjectManager->GetProjectName().IsOk() == true) {
		myPath = m_ProjectManager->GetProjectName().GetPath();
	}
	
	
	wxFileDialog myDlg(this, _("Save ColtopGIS project"), myPath,"",
					   "ColtopGIS project (*.c2p)|*.c2p", wxFD_SAVE| wxFD_OVERWRITE_PROMPT);
	
	if (myDlg.ShowModal() == wxID_CANCEL) {
		return false;
	}
	
	wxFileName myFileName (myDlg.GetPath());
	myFileName.SetExt("c2p");
	_ProjectBeforeSave(myFileName);
	bool bSave = m_ProjectManager->Save(myFileName);
	if (bSave == false) {
		wxLogError(_("Saving '%s' failed"), myFileName.GetFullName());
	}else {
		_ProjectAfterSave();
	}
	return bSave;
}



void Frame::_ProjectBeforeSave(const wxFileName & project) {
	Database myDB;
	if(myDB.Open(m_ProjectManager->GetProjectNameTemp())==false){
		wxLogError(_("Unable to save opened file!"));
		return;
	}
	ProjectBase myBaseProj(&myDB);
	myBaseProj.ClearTOC();
	// save path as relative ?
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	wxASSERT(myConfig);
	myConfig->SetPath("GENERAL");
	bool myRelativeStatus = myConfig->ReadBool("SAVE_RELATIVE_PATH", true);
	myConfig->SetPath("..");
	
    int iNbItem = m_vrViewerLayerManager->GetCount();
    for (int i = 0; i< iNbItem; i++) {
		myBaseProj.SaveTOC(i, m_vrViewerLayerManager->GetRenderer(i), myRelativeStatus, project);
	}
	
	// save toc position for plugins items
	for (unsigned int j = 0; j< m_Plugins.GetCount(); j++) {
		m_Plugins.Item(j)->SaveTOC(m_vrViewerLayerManager, &myDB);
	}
}



void Frame::_ProjectAfterSave() {
	m_ProjectManager->SetModified(false);
	m_ProjectManager->SetHistoryMenu(m_RecentMenu);
}


void Frame::_ProjectAfterOpen() {
	m_ProjectManager->SetHistoryMenu(m_RecentMenu);
	m_vrDisplay->SetTool(new vrDisplayToolSelect (m_vrDisplay));
	
	// destroy dataframe
	DataFrame * myFrameData = (DataFrame*) FindWindowById(WINDOW_DATA_MANAGER);
	if (myFrameData) {
		myFrameData->Destroy();
	}
	
	// close plugin frames (stereonet, etc)
	for (unsigned int i = 0; i<m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->CloseWindow();
	}
	
	// empty toc
	m_vrViewerLayerManager->FreezeBegin();
	for (int j =  m_vrViewerLayerManager->GetCount() -1; j>= 0; j--) {
		_RemoveLayer(j);
	}
	
	// load toc
	Database myDB;
	if(myDB.Open(m_ProjectManager->GetProjectNameTemp())==false){
		wxLogError(_("Unable to load Table of content!"));
		return;
		m_vrViewerLayerManager->FreezeEnd();
	}
	
	ProjectBase myBaseProj(&myDB);
	long i = 0;
	long myIndex = wxNOT_FOUND;
	wxFileName myFile;
	bool myVisible = false;

	// load base project
	while (1) {
		vrRender * myRender = NULL;
		vrLabel * myLabel = NULL;
		if(myBaseProj.LoadTOC(i, myIndex, myFile, myVisible, &myRender, &myLabel, m_ProjectManager->GetProjectName())==false) {
			wxDELETE(myRender);
			wxDELETE(myLabel);
			break;
		}
		i++;
		wxLogMessage("Adding layer : %s @ position : %ld", myFile.GetFullName(), myIndex);
		AddLayer(myFile, myIndex, myRender, myLabel, myVisible);
	}
	
	// load project from plugins
	i = 0;
	for (unsigned int j = 0; j<m_Plugins.GetCount(); j++) {
		Project * myProj = m_Plugins.Item(j)->GetProject(&myDB);
		if (myProj == NULL) {
			continue;
		}
		while (1){
			vrRender * myRender = NULL;
			vrLabel * myLabel = NULL;
			bool bReadWrite = true;
			if(myProj->LoadTOC(i, myIndex, myFile, myVisible, &myRender, &myLabel, m_ProjectManager->GetProjectNameTemp())==false) {
				wxDELETE(myRender);
				wxDELETE(myLabel);
				break;
			}
			i++;
			AddLayer(myFile, myIndex, myRender, myLabel, myVisible, bReadWrite);			
		}
		wxDELETE(myProj);
        
        // post-loading processing for plugins
        m_Plugins.Item(j)->LoadTOCEnd(this, &myDB);
	}
	m_vrViewerLayerManager->FreezeEnd();
	m_ProjectManager->SetModified(false);

}



void Frame::_SetFrameName() {
	wxASSERT(m_ProjectManager);

	// create project name
	wxString myProjName = m_ProjectManager->GetProjectName().GetFullName();
	if (myProjName == wxEmptyString) {
		myProjName = _("untitled");
	}
	
	// show modified status
	if (m_ProjectManager->IsModified() == true) {
		myProjName.Prepend(_T("*"));
	}
	OSXSetModified(m_ProjectManager->IsModified());
	// display title
	SetTitle(g_ProgName + _T(" - ") + myProjName);
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
	
	//inform plugins that layer where added
	for (unsigned int i = 0; i<m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->LayerAdded(file);
	}
	return true;
}



bool Frame::_RemoveLayer(long position) {
	DataFrame * myFrame = (DataFrame*) FindWindowById(WINDOW_DATA_MANAGER);
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
	
	if (myFrame) {
		myFrame->EditPanel(DATAPANEL_TYPE_LAYERS, DP_REMOVE);
	}
	
	//inform plugins that layer where removed
	for (unsigned int i = 0; i<m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->LayerRemoved(myFile);
	}
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
	
	// update background colour for empty panel
	DataFrame * myFrameData = (DataFrame*) FindWindowById(WINDOW_DATA_MANAGER);
	if (myFrameData && myFrameData->GetActualPanel()->GetPanelType() == DATAPANEL_TYPE_EMPTY){
		myFrameData->GetActualPanel()->EditData();
	}
	
}


bool Frame::_CheckSecurity(){
	SecurityDate mySecurity;
	SecurityMessage myMessage (this);
	
	// get last start
	wxDateTime myLastStartDate;
	bool bGetLastStart = mySecurity.GetLastStart(myLastStartDate, wxConfigBase::Get(false));
	if (bGetLastStart == false) {
		wxLogMessage(_("Last start time not found (or not correct)! First start ?"));
		// TODO get actual date from internet (Beta 13)
	}
	else {
		wxLogMessage(_("Program started last time: %s"), myLastStartDate.FormatISOCombined(' '));
	}

	
	// get local actual date
	wxDateTime myActualDateLocal = mySecurity.GetActualDate();
	wxASSERT(myActualDateLocal.IsValid());
	
	// check local date validity
	bool bIsActualDateLocalValid = (mySecurity.IsDateValid(myActualDateLocal));
	if (bIsActualDateLocalValid == false) {
 		myMessage.SendMessageBetaExpired();
		return false;
	}
	
	if (bGetLastStart == true) {
		if (myActualDateLocal.IsEarlierThan(myLastStartDate)) {
             myMessage.SendMessageTimeIncorrect();
			return false;
		}
	}
	return true;
}



void Frame::OnDisplaySecurityMessage(wxThreadEvent & event){
	bool myQuit = true;
    if (event.GetInt() == 0){
        myQuit = false;
    }
     
    wxMessageDialog myDlg(this,
                          event.GetString(),
                          _("Security Alert"),
                          wxICON_ERROR | wxCENTRE | wxOK | wxCANCEL | wxOK_DEFAULT);
    myDlg.SetOKCancelLabels(_("ColtopGIS website"), _("Close"));
    if(myDlg.ShowModal() == wxID_OK){
        wxLaunchDefaultBrowser("http://www.crealp.ch/index.php?option=com_content&task=view&id=379&Itemid=383");
    }
 
	if(myQuit == true){
		Close(true);
	}
}


void Frame::OnAbout(wxCommandEvent & event) {
	AboutDLG myAbout(this);
	myAbout.ShowModal();
}



void Frame::OnCheckUpdates(wxCommandEvent & event){
    wxConfigBase * myConfig =  wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");
        
    long mySVNversion = wxNOT_FOUND;
    lsVersion::GetSoftSVN().ToLong(&mySVNversion);
    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    myUpdate->CheckNewVersion(mySVNversion, true, true, true); 
    
}






void Frame::OnPreferences(wxCommandEvent & event){
	PreferenceDlg myPrefDlg (this, &m_Plugins);
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


void Frame::OnDataManager(wxCommandEvent & event) {
	DataFrame * myFrameData = (DataFrame*) FindWindowById(WINDOW_DATA_MANAGER);
	if (myFrameData == NULL) {
		myFrameData = new DataFrame(this, &m_Plugins, WINDOW_DATA_MANAGER);
		myFrameData->SetSize(wxSize(800,600));
		myFrameData->Show();
	}
	myFrameData->Raise();
}



void Frame::OnProjectNew(wxCommandEvent & event) {
	if (m_ProjectManager->IsModified() == false) {
		m_ProjectManager->New();
		_ProjectAfterOpen();
		return;
	}
	
	if (_ProjectQuestion(_("creating new project"))==true) {
		m_ProjectManager->New();
		_ProjectAfterOpen();
	}
	
	
}



void Frame::OnProjectOpen(wxCommandEvent & event) {
	wxFileName myEmpytfileName;
	ProjectOpen(myEmpytfileName);
}


void Frame::OnProjectRecent(wxCommandEvent & event) {
	wxLogMessage("Opening recent : %d", event.GetId() - wxID_FILE); 
	ProjectOpen(m_ProjectManager->GetHistoryFile(event.GetId()));
}


void Frame::OnProjectSave(wxCommandEvent & event) {
	_ProjectSave(m_ProjectManager->GetProjectName());
}



void Frame::OnProjectSaveAs(wxCommandEvent & event) {
	wxFileName myEmptyFile;
	_ProjectSave(myEmptyFile);
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
	DataFrame * myDataFrame = (DataFrame*) FindWindowById(WINDOW_DATA_MANAGER);

	m_vrViewerLayerManager->FreezeBegin();
	for (unsigned int i = 0; i< names.GetCount(); i++) {
		AddLayer(names.Item(i));
		if (myDataFrame) {
			myDataFrame->EditPanel(DATAPANEL_TYPE_LAYERS, DP_ADD);
		}
		
	}
	m_vrViewerLayerManager->FreezeEnd();
	m_ProjectManager->SetModified(true);
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
	m_ProjectManager->SetModified(true);
}




void Frame::OnUpdateIdle(wxIdleEvent & event) {
	_SetFrameName();
	_SetSelectedFeature();
}



void Frame::OnClose(wxCloseEvent & event) {
	if ( event.CanVeto() && m_ProjectManager->IsModified()){
		if(_ProjectQuestion(_("closing")) == false){
			event.Veto();
			return;
		}
	}
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
	
    vrRenderer * myActualSelectedRender = m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection());
	// inform plugins that selection is done.
	for (unsigned int i = 0; i< m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->SelectionChanged(myActualSelectedRender);
	}
	
}


void Frame::OnToolClearSelection(wxCommandEvent & event) {
    wxASSERT(m_vrViewerLayerManager);
    m_vrViewerLayerManager->ClearSelection();
    m_vrViewerLayerManager->Reload();
    
    // inform plugins that selection is done.
    vrRenderer * myActualSelectedRender = m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection());
	for (unsigned int i = 0; i< m_Plugins.GetCount(); i++) {
		m_Plugins.Item(i)->SelectionChanged(myActualSelectedRender);
	}
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
	m_LogWnd = new wxLogWindow(this, "ColtopGIS Log", false, true);
    
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
    
	_CreateControls();
	_CreateMenus();
	_CreateToolbar();

	// add icon (windows / linux)
	wxIcon myColtopIcon;
	myColtopIcon.CopyFromBitmap(*_img_coltopgis_sml);
	SetIcon(myColtopIcon);


	// vroomGIS
	m_vrLayerManager = new vrLayerManager();
	m_vrViewerLayerManager = new vrViewerLayerManager(m_vrLayerManager, this, m_vrDisplay , m_vrTOC);
		
	// plugins
	_InitPlugins();
	_CreateAccelerators();
	m_ProjectManager = new ProjectManager(&m_Plugins);
	wxCommandEvent myUnusedEvent;
	OnProjectNew(myUnusedEvent);
	
	
	m_vrDisplay->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown),NULL, this);
	m_vrDisplay->Connect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp),NULL, this);

	m_vrDisplay->SetFocus();
	_PreferenceChanged(false);
	
    
    // check security and updates
    if(_CheckSecurity()==true){
        SecurityDate myDate;
        myDate.SetLastStart(myDate.GetActualDate(), wxConfigBase::Get(false));
    }
	
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
    lsVersion::GetSoftSVN().ToLong(&mySVNversion);
    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    myUpdate->CheckNewVersion(mySVNversion, true, false, true);    
}



Frame::~Frame() {
	m_vrDisplay->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown),NULL, this);
	m_vrDisplay->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp),NULL, this);	

	// don't delete m_vrViewerLayerManager, will be deleted by the manager
	wxDELETE(m_vrLayerManager);
	wxDELETE(m_ProjectManager);

	delete wxLog::SetActiveTarget (NULL);

	uninitialize_images();
	vroomgis_clear_images();
	_EndPlugins();
	

}

