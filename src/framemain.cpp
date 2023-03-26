/***************************************************************************
 frame.cpp
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "framemain.h"

#include <wx/fileconf.h>
#include <wx/splitter.h>
#include <wx/stdpaths.h>

#include "accelerators.h"
#include "log.h"  // for double logging process
#include "preference_dlg.h"
#include "vrlabel.h"
#include "vrlayerraster.h"
#include "vrlayervector.h"
#include "vroomgis_bmp.h"
#include "vroomgis_toolbmp.h"
#include "vrprogress.h"
#include "vrrender.h"
#include "vrshapeeditor.h"
// #include "wxhgversion_dlg.h"
// #include "wxhgversion_core.h"
#include <sentry.h>

#include "core/demutil.h"
#include "createslbl_dlg.h"
#include "frameabout.h"
#include "plint_dlg.h"
#include "profile_dlg.h"
#include "profileview_dlg.h"
#include "toolbarbitmaps.h"
#include "version.h"

BEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_MENU(MENU_FRAME_CREATE_SLBL, Frame::OnCreateSLBL)
EVT_MENU(MENU_FRAME_PLINT_DIALOG, Frame::OnPlaneIntersectionDialog)
EVT_MENU(MENU_FRAME_PROFILE_DIALOG, Frame::OnProfileDialog)
EVT_MENU(MENU_FRAME_PROFILE_VIEW, Frame::OnProfileView)
EVT_MENU(wxID_EXIT, Frame::OnQuit)
EVT_MENU(wxID_PREFERENCES, Frame::OnPreferences)
EVT_MENU(wxID_HELP, Frame::OnUserManual)
EVT_MENU(MENU_WEBSITE, Frame::OnWebSite)
EVT_MENU(wxID_ABOUT, Frame::OnAbout)
EVT_MENU(MENU_CHECK_UPDATE, Frame::OnCheckUpdates)
EVT_MENU(MENU_WINDOW_LOG, Frame::OnLogWindow)
EVT_MENU(MENU_DATA_ADD, Frame::OnLayerAdd)
EVT_MENU(MENU_DATA_MEMORY_ADD, Frame::OnLayerMemoryAdd)
EVT_MENU(MENU_DATA_REMOVE, Frame::OnLayerRemove)
EVT_MENU(MENU_EDITION_START, Frame::OnEditionStart)
EVT_MENU(MENU_EDITION_STOP, Frame::OnEditionStop)
EVT_IDLE(Frame::OnUpdateIdle)
EVT_CLOSE(Frame::OnClose)
EVT_MENU(MENU_FRAME_SELECT, Frame::OnToolSelect)
EVT_MENU(MENU_FRAME_CLEAR_SELECTION, Frame::OnToolClearSelection)
EVT_MENU(wxID_ZOOM_IN, Frame::OnToolZoom)
EVT_MENU(wxID_ZOOM_FIT, Frame::OnToolZoomToFit)
EVT_MENU(MENU_FRAME_PAN, Frame::OnToolPan)
EVT_MENU(MENU_TOOL_DRAW, Frame::OnToolDraw)
EVT_MENU(MENU_TOOL_MODFIY, Frame::OnToolModify)
EVT_MENU(MENU_FEATURE_DELETE, Frame::OnDeleteFeature)
EVT_MENU(MENU_DATA_SAVE_SELECTED, Frame::OnSaveSelectedLayer)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, Frame::OnToolZoomAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOMOUT, Frame::OnToolZoomAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, Frame::OnToolPanAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, Frame::OnToolSelectAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_EDIT, Frame::OnToolDrawAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_EDIT_FINISHED, Frame::OnToolDrawAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_MODIFY, Frame::OnToolModifySearch)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_MODIFY_FINISHED, Frame::OnToolModifyUpdate)

EVT_KEY_DOWN(Frame::OnKeyDown)
EVT_KEY_UP(Frame::OnKeyUp)
EVT_UPDATE_UI(MENU_FRAME_CLEAR_SELECTION, Frame::OnUpdateUIToolClearSelection)
EVT_UPDATE_UI(MENU_DATA_REMOVE, Frame::OnUpdateUIRemoveLayer)
EVT_UPDATE_UI(MENU_EDITION_START, Frame::OnUpdateEditionStart)
EVT_UPDATE_UI(MENU_EDITION_STOP, Frame::OnUpdateEditionPossible)
EVT_UPDATE_UI(MENU_TOOL_DRAW, Frame::OnUpdateEditionPossible)
EVT_UPDATE_UI(MENU_TOOL_MODFIY, Frame::OnUpdateEditionPossible)
EVT_UPDATE_UI(MENU_FEATURE_DELETE, Frame::OnUpdateDeletePossible)
EVT_UPDATE_UI(MENU_DATA_SAVE_SELECTED, Frame::OnUpdateUISaveSelectedLayer)
EVT_UPDATE_UI(MENU_FRAME_PLINT_DIALOG, Frame::OnUpdateUIPlaneIntersection)
END_EVENT_TABLE()

ctDropFiles::ctDropFiles(Frame* parent) {
    wxASSERT(parent);
    m_Frame = parent;
}

bool ctDropFiles::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    if (filenames.GetCount() == 0) {
        return false;
    }

    m_Frame->AddLayers(filenames);
    return true;
}

void Frame::_CreateControls() {
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    wxSplitterWindow* m_splitter1;
    m_splitter1 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       wxSP_LIVE_UPDATE | wxSP_THIN_SASH);

    wxPanel* m_panel1;
    m_panel1 = new wxPanel(m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);
    m_vrTOC = new vrViewerTOCTree(m_panel1, wxID_ANY);
    bSizer2->Add(m_vrTOC->GetControl(), 1, wxEXPAND, 5);
    m_panel1->SetSizer(bSizer2);
    m_splitter1->Initialize(m_panel1);

    wxPanel* m_panel2;
    m_panel2 = new wxPanel(m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer(wxVERTICAL);

    m_vrDisplay = new vrViewerDisplay(m_panel2, wxID_ANY);
    bSizer3->Add(m_vrDisplay, 1, wxEXPAND, 5);
    m_panel2->SetSizer(bSizer3);
    m_splitter1->Initialize(m_panel2);

    m_splitter1->SplitVertically(m_panel1, m_panel2, 230);
    bSizer1->Add(m_splitter1, 1, wxEXPAND, 5);

    m_InfoBar = new WebUpdateInformationBar(this);
    bSizer1->Add(m_InfoBar, wxSizerFlags().Expand());

    this->SetSizer(bSizer1);
    bSizer1->SetSizeHints(this);

    m_vrDisplay->SetStatusCoordinates(GetStatusBar(), 0, _("Coordinates not available"));
    // this->SetSizer( bSizer1 );
    // this->Layout();
}

void Frame::_CreateMenus() {
    wxMenuBar* m_menubar1;
    m_menubar1 = new wxMenuBar(0);

    // PROJECT
#ifndef __WXMAC__
    wxMenu* m_menu1 = new wxMenu();
    m_menu1->Append(new wxMenuItem(m_menu1, wxID_EXIT));
    m_menubar1->Append(m_menu1, _("Project"));
#endif

    // DATA
    wxMenu* m_menu5 = new wxMenu();
    m_menu5->Append(new wxMenuItem(m_menu5, MENU_DATA_ADD, _("Add layer...\tCtrl+O"), wxEmptyString, wxITEM_NORMAL));
    m_menu5->Append(new wxMenuItem(m_menu5, MENU_DATA_MEMORY_ADD, _("Add memory layer...")));
    m_menu5->AppendSeparator();
    m_menu5->Append(
        new wxMenuItem(m_menu5, MENU_DATA_SAVE_SELECTED, _("Save selected layer..."), wxEmptyString, wxITEM_NORMAL));
    m_menu5->AppendSeparator();
    m_menu5->Append(
        new wxMenuItem(m_menu5, MENU_DATA_REMOVE, _("Remove layer...\tCtrl+W"), wxEmptyString, wxITEM_NORMAL));

    m_menubar1->Append(m_menu5, _("Data"));

    // SELECTION
    wxMenu* m_menu51;
    m_menu51 = new wxMenu();
    m_menu51->Append(new wxMenuItem(m_menu51, MENU_FRAME_SELECT, _("Select tool\tV"), wxEmptyString, wxITEM_NORMAL));
    m_menu51->Append(
        new wxMenuItem(m_menu51, MENU_FRAME_CLEAR_SELECTION, _("Clear selection"), wxEmptyString, wxITEM_NORMAL));
    m_menubar1->Append(m_menu51, _("Selection"));

    // EDITION
    wxMenu* m_menu52;
    m_menu52 = new wxMenu();
    m_menu52->Append(new wxMenuItem(m_menu52, MENU_EDITION_START, _("Start Edition"), wxEmptyString, wxITEM_NORMAL));
    m_menu52->Append(new wxMenuItem(m_menu52, MENU_EDITION_STOP, _("Stop Edition"), wxEmptyString, wxITEM_NORMAL));
    m_menu52->AppendSeparator();
    m_menu52->Append(new wxMenuItem(m_menu52, MENU_TOOL_DRAW, _("Draw\tD"), wxEmptyString, wxITEM_NORMAL));
    m_menu52->Append(new wxMenuItem(m_menu52, MENU_TOOL_MODFIY, _("Modify\tM"), wxEmptyString, wxITEM_NORMAL));
    m_menu52->AppendSeparator();
    wxString myDelShortcut = "\tDel";
#ifdef __WXMAC__
    myDelShortcut = "\tBack";
#endif
    m_menu52->Append(new wxMenuItem(m_menu52, MENU_FEATURE_DELETE, _("Delete selected features") + myDelShortcut,
                                    wxEmptyString, wxITEM_NORMAL));
    m_menubar1->Append(m_menu52, _("Edition"));

    // VIEW
    wxMenu* m_menu6;
    m_menu6 = new wxMenu();
    m_menu6->Append(new wxMenuItem(m_menu6, wxID_ZOOM_IN, _("Zoom tool\tZ"), wxEmptyString, wxITEM_NORMAL));
    m_menu6->Append(new wxMenuItem(m_menu6, MENU_FRAME_PAN, _("Pan tool\tH"), wxEmptyString, wxITEM_NORMAL));
    m_menu6->AppendSeparator();
    m_menu6->Append(new wxMenuItem(m_menu6, wxID_ZOOM_FIT, _("Zoom to Fit\tCtrl+0"), wxEmptyString, wxITEM_NORMAL));
    m_menubar1->Append(m_menu6, _("View"));

    // STRUCTURAL ANALYSIS
    wxMenu* myStructMenu = new wxMenu();

    wxMenu* mySlblMenu = new wxMenu();
    mySlblMenu->Append(new wxMenuItem(mySlblMenu, MENU_FRAME_CREATE_SLBL, _("Create SLBL...\tCtrl+N")));
    myStructMenu->AppendSubMenu(mySlblMenu, _("SLBL"));

    wxMenu* myPlintMenu = new wxMenu();
    myPlintMenu->Append(MENU_FRAME_PLINT_DIALOG, _("Plane Intersection Dialog...\tCtrl+I"), wxEmptyString, true);
    myStructMenu->AppendSubMenu(myPlintMenu, _("Plane Intersection"));

    wxMenu* myProfileMenu = new wxMenu();
    myProfileMenu->Append(MENU_FRAME_PROFILE_DIALOG, _("Export profile...\tCtrl+Alt+V"), wxEmptyString);
    myProfileMenu->Append(MENU_FRAME_PROFILE_VIEW, _("View profile...\tCtrl+V"), wxEmptyString);
    myStructMenu->AppendSubMenu(myProfileMenu, _("Profile"));

    m_menubar1->Append(myStructMenu, _("Structural Analysis"));

    // WINDOW
    wxMenu* m_menu2 = new wxMenu();
    m_menu2->Append(new wxMenuItem(m_menu2, MENU_WINDOW_LOG, _("Log window...\tCtrl+L"), wxEmptyString, wxITEM_NORMAL));

    // Bug #42, append preference into the Window menu, except for Mac where it goes into Application menu
    m_menu2->Append(new wxMenuItem(m_menu2, wxID_PREFERENCES));
    m_menubar1->Append(m_menu2, _("Window"));

    // HELP
    wxMenu* m_menu3 = new wxMenu();
    m_menu3->Append(new wxMenuItem(m_menu3, wxID_ABOUT));
    m_menu3->Append(new wxMenuItem(m_menu3, wxID_HELP, _("User Manual"), wxEmptyString, wxITEM_NORMAL));
    m_menu3->Append(new wxMenuItem(m_menu3, MENU_WEBSITE, _("Website"), wxEmptyString, wxITEM_NORMAL));
    m_menu3->AppendSeparator();
    m_menu3->Append(new wxMenuItem(m_menu3, MENU_CHECK_UPDATE, _("Check for updates"), wxEmptyString, wxITEM_NORMAL));
    m_menubar1->Append(m_menu3, _("Help"));
    this->SetMenuBar(m_menubar1);

    // ACCELERATORS
    wxAcceleratorEntry myEntries[3];
    myEntries[0].Set(wxACCEL_NORMAL, (int)'V', MENU_FRAME_SELECT);
    myEntries[1].Set(wxACCEL_NORMAL, (int)'Z', wxID_ZOOM_IN);
    myEntries[2].Set(wxACCEL_NORMAL, (int)'H', MENU_FRAME_PAN);
    wxAcceleratorTable myAccelTable(3, myEntries);
    wxASSERT(m_vrDisplay);
    m_vrDisplay->SetAcceleratorTable(myAccelTable);
}

void Frame::_CreateToolbar() {
    long my_toolbar_style = wxTB_DEFAULT_STYLE;
#ifdef __WXOSX__
    my_toolbar_style = my_toolbar_style | wxTB_TEXT;
#endif
    wxToolBar* my_toolbar = this->CreateToolBar(my_toolbar_style);
    wxASSERT(my_toolbar);

    // support for dark theme
    wxString str_color = "black";
    wxSystemAppearance s = wxSystemSettings::GetAppearance();
    if (s.IsDark()) {
        str_color = "white";
    }

    wxString labels[] = {_("Select"),        _("Zoom to fit"), _("Zoom"),  _("Pan"),
                         _("Start edition"), _("Draw"),        _("Modify")};
    my_toolbar->AddTool(MENU_FRAME_SELECT, labels[0],
                        Bitmaps::GetBitmap(Bitmaps::ID::SELECT, str_color, wxSize(24, 24)), labels[0]);
    my_toolbar->AddTool(wxID_ZOOM_FIT, labels[1], Bitmaps::GetBitmap(Bitmaps::ID::ZOOM_FIT, str_color, wxSize(24, 24)),
                        labels[1]);
    my_toolbar->AddTool(wxID_ZOOM_IN, labels[2], Bitmaps::GetBitmap(Bitmaps::ID::ZOOM, str_color, wxSize(24, 24)),
                        labels[2]);
    my_toolbar->AddTool(MENU_FRAME_PAN, labels[3], Bitmaps::GetBitmap(Bitmaps::ID::PAN, str_color, wxSize(24, 24)),
                        labels[3]);
    my_toolbar->AddTool(MENU_EDITION_START, labels[4],
                        Bitmaps::GetBitmap(Bitmaps::ID::EDITION_START, str_color, wxSize(24, 24)), labels[4]);
    my_toolbar->AddTool(MENU_TOOL_DRAW, labels[5],
                        Bitmaps::GetBitmap(Bitmaps::ID::EDITION_DRAW, str_color, wxSize(24, 24)), labels[4]);
    my_toolbar->AddTool(MENU_TOOL_MODFIY, labels[6],
                        Bitmaps::GetBitmap(Bitmaps::ID::EDITION_MODIFY, str_color, wxSize(24, 24)), labels[4]);
    my_toolbar->Realize();
}

void Frame::_CreateAccelerators() {
    wxArrayAccelerators myAccels;
    wxAcceleratorEntry myEntry1(wxACCEL_NORMAL, (int)'V', MENU_FRAME_SELECT);
    wxAcceleratorEntry myEntry2(wxACCEL_NORMAL, (int)'Z', wxID_ZOOM_IN);
    wxAcceleratorEntry myEntry3(wxACCEL_NORMAL, (int)'H', MENU_FRAME_PAN);
    wxAcceleratorEntry myEntry4(wxACCEL_NORMAL, (int)'D', MENU_TOOL_DRAW);
    wxAcceleratorEntry myEntry5(wxACCEL_NORMAL, (int)'M', MENU_TOOL_MODFIY);

    myAccels.Add(myEntry1);
    myAccels.Add(myEntry2);
    myAccels.Add(myEntry3);
    myAccels.Add(myEntry4);
    myAccels.Add(myEntry5);

    wxAcceleratorEntry* myEntries = new wxAcceleratorEntry[myAccels.GetCount()];
    for (unsigned int i = 0; i < myAccels.GetCount(); i++) {
        *(myEntries + i) = myAccels.Item(i);
    }

    wxAcceleratorTable myAccelTable(myAccels.GetCount(), myEntries);
    wxASSERT(m_vrDisplay);
    m_vrDisplay->SetAcceleratorTable(myAccelTable);
    wxDELETEA(myEntries);
}

void Frame::_SetSelectedFeature() {
    if (m_vrViewerLayerManager == NULL) {
        return;
    }

    int mySel = m_vrViewerLayerManager->GetSelectionCount();
    GetStatusBar()->SetStatusText(wxString::Format(_("%d Feature(s) selected"), mySel), 1);
}

bool Frame::AddLayer(const wxFileName& file, long position, vrRender* render, vrLabel* label, bool visible,
                     bool readwrite) {
    if (m_vrLayerManager->Open(file, readwrite) == false) {
        wxLogError(_("File %s not found, we will implement a search function later on"), file.GetFullName());
        return false;
    }

    vrLayer* myLayer = m_vrLayerManager->GetLayer(file);
    wxASSERT(myLayer);

    // build overviews if needed.
    wxConfigBase* myConfig = wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("GENERAL");
    bool myBuildOverviews = myConfig->ReadBool("BUILD_OVERVIEW", true);
    myConfig->SetPath("..");
    if (myBuildOverviews == true) {
        if (myLayer->GetType() == vrDRIVER_RASTER_C2D || myLayer->GetType() == vrDRIVER_RASTER_TIFF ||
            myLayer->GetType() == vrDRIVER_RASTER_JPEG || myLayer->GetType() == vrDRIVER_RASTER_ESRIGRID) {
            vrLayerRasterGDAL* myRaster = (vrLayerRasterGDAL*)myLayer;
            if (myRaster->HasOverviews() == false) {
                wxArrayInt myFactors;
                myFactors.Add(4);
                myFactors.Add(8);
                myFactors.Add(16);
                myFactors.Add(32);

                vrProgressSimple myProgressData(
                    this, _("Building Overviews (Pyramids)"),
                    wxString::Format(_("Building Overviews: '%s'"), myRaster->GetDisplayName().GetFullName()));
                myRaster->BuildOverviews(myFactors, vrOVERVIEW_NEAREST, GDALUpdateSimple, &myProgressData);
            }
        }
    }

    // add files to the viewer
    m_vrViewerLayerManager->Add(position, myLayer, render, label, visible);
    return true;
}

bool Frame::_RemoveLayer(long position) {
    wxASSERT(m_vrViewerLayerManager);
    vrRenderer* myRenderer = m_vrViewerLayerManager->GetRenderer(position);
    if (myRenderer == NULL) {
        wxLogError(_("Unable to found renderer at position : %ld"), position);
        return false;
    }

    vrLayer* myLayer = myRenderer->GetLayer();
    wxASSERT(myLayer);
    wxFileName myFile = myRenderer->GetLayer()->GetDisplayName();
    m_vrViewerLayerManager->Remove(myRenderer);

    // close layer (not used anymore);
    m_vrLayerManager->Close(myLayer);
    return true;
}

void Frame::_PreferenceChanged(bool refresh) {
    // update main panel color
    wxConfigBase* myConfig = wxConfigBase::Get(false);
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
        for (int i = 0; i < m_vrViewerLayerManager->GetCount(); i++) {
            m_vrViewerLayerManager->GetRenderer(i)->GetRender()->SetSelectionColour(mySelColour);
        }
    }

    if (refresh == true) {
        m_vrViewerLayerManager->Reload();
    }
}

void Frame::OnAbout(wxCommandEvent& event) {
    sentry_capture_event(sentry_value_new_message_event(
        /*   level */ SENTRY_LEVEL_INFO,
        /*  logger */ "custom",
        /* message */ "It works!"));
    FrameAbout myAboutDlg(this);
    myAboutDlg.ShowModal();
}

void Frame::OnCheckUpdates(wxCommandEvent& event) {
    // crash
    wxConfigBase* myConfig = wxConfigBase::Get(false);
    myConfig = nullptr;
    myConfig->SetPath("INTERNET");

    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");

    //    long mySVNversion = wxNOT_FOUND;
    //    wxHgVersion::GetSoftNumber().ToLong(&mySVNversion);
    //    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    //    myUpdate->CheckNewVersion(mySVNversion, true, true, true);
}

void Frame::OnPreferences(wxCommandEvent& event) {
    PreferenceDlg myPrefDlg(this);
    myPrefDlg.CenterOnParent(wxBOTH);
    int myValue = myPrefDlg.ShowModal();
    if (myValue == wxID_OK) {
        _PreferenceChanged();
    }
}

void Frame::OnUserManual(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("http://www.google.ch");
}

void Frame::OnWebSite(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("http://www.terranum.ch");
}

void Frame::OnQuit(wxCommandEvent& event) {
    Close(true);
}

void Frame::OnLogWindow(wxCommandEvent& event) {
    m_LogWnd->Show(true);
    m_LogWnd->GetFrame()->Raise();
}

void Frame::OnLayerAdd(wxCommandEvent& event) {
    vrDrivers myDrivers;
    wxFileDialog myFileDlg(this, "Select GIS Layers", wxEmptyString, wxEmptyString,
                           myDrivers.GetWildcards(vrDRIVERS_FLAG_ALL),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);

    wxArrayString myPathsFileName;

    if (myFileDlg.ShowModal() == wxID_OK) {
        // try to open files
        myFileDlg.GetPaths(myPathsFileName);
        wxASSERT(myPathsFileName.GetCount() > 0);
        AddLayers(myPathsFileName);
    }
}

void Frame::OnLayerMemoryAdd(wxCommandEvent& event) {
    // ask for spatial types
    const wxString myNames[] = {_("Points"), _("Lines"), _("Polygons")};
    wxSingleChoiceDialog myTypDlg(this, _("Select layer spatial type"), _("Memory layer"),
                                  sizeof(myNames) / sizeof(wxString), &myNames[0]);
    if (myTypDlg.ShowModal() != wxID_OK) {
        return;
    }

    // ask for layer name
    wxString myName = _GetMemoryLayerNameFromUser(myNames[myTypDlg.GetSelection()]);
    if (myName == wxEmptyString) {
        return;
    }
    wxFileName myMemoryLayerName("", myName, "memory");

    m_vrViewerLayerManager->FreezeBegin();
    vrLayerVectorOGR* myLayer = new vrLayerVectorOGR();
    int mySpatialTypes[] = {wkbPoint, wkbLineString, wkbPolygon};
    if (myLayer->Create(myMemoryLayerName, mySpatialTypes[myTypDlg.GetSelection()]) == false) {
        wxFAIL;
    }
    m_vrLayerManager->Add(myLayer);
    m_vrViewerLayerManager->Add(-1, myLayer);
    m_vrViewerLayerManager->FreezeEnd();
}

void Frame::OnEditionStart(wxCommandEvent& event) {
    vrRenderer* mySelectedRenderer = m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection());
    if (mySelectedRenderer == NULL) {
        return;
    }
    m_vrViewerLayerManager->StopEdition();
    m_vrViewerLayerManager->StartEdition(mySelectedRenderer);
}

void Frame::OnEditionStop(wxCommandEvent& event) {
    m_vrViewerLayerManager->StopEdition();
}

void Frame::OnDeleteFeature(wxCommandEvent& event) {
    vrLayerVector* myLayer = static_cast<vrLayerVector*>(m_vrViewerLayerManager->GetEditionRenderer()->GetLayer());
    if (myLayer == NULL) {
        return;
    }
    for (unsigned int i = 0; i < myLayer->GetSelectedIDs()->GetCount(); i++) {
        myLayer->DeleteFeature(myLayer->GetSelectedIDs()->Item(i));
    }
    m_vrViewerLayerManager->Reload();
}

void Frame::OnSaveSelectedLayer(wxCommandEvent& event) {
    vrLayerVectorOGR* myMemoryLayer = static_cast<vrLayerVectorOGR*>(
        m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection())->GetLayer());
    wxASSERT(myMemoryLayer);

    wxString mySaveFileName = wxFileSelector(_("Choose a filename for the shapefile"), wxEmptyString, wxEmptyString,
                                             _T("shp"), _T("ESRI Shapefiles (*.shp)|*.shp"), wxFD_SAVE, this);
    if (mySaveFileName.IsEmpty() == true) {
        wxLogMessage(_("Saving canceled!"));
        return;
    }

    wxFileName mySaveFile(mySaveFileName);
    mySaveFile.SetExt(_T("shp"));

    vrLayerVectorOGR myShapefile;
    if (myShapefile.Create(mySaveFile, myMemoryLayer->GetGeometryType()) == false) {
        wxLogError(_("Creating '%s' failed!"), mySaveFile.GetFullPath());
        return;
    }

    bool restart = false;
    myMemoryLayer->ClearSpatialFilter();
    for (long i = 0; i < myMemoryLayer->GetFeatureCount(); i++) {
        restart = false;
        if (i == 0) {
            restart = true;
        }
        OGRFeature* myFeature = myMemoryLayer->GetNextFeature(restart);
        wxASSERT(myFeature);
        myShapefile.AddFeature(myFeature->GetGeometryRef());
        OGRFeature::DestroyFeature(myFeature);
    }
    wxLogMessage(_("'%s' saved as: '%s'"), myMemoryLayer->GetDisplayName().GetName(), mySaveFile.GetFullName());
}

wxString Frame::_GetMemoryLayerNameFromUser(const wxString& name) {
    wxTextEntryDialog myNameDlg(this, _("Layer Names"));
    myNameDlg.SetValue(name);
    if (myNameDlg.ShowModal() != wxID_OK) {
        return wxEmptyString;
    }

    wxFileName myFileName("", myNameDlg.GetValue(), "memory");
    if (m_vrLayerManager->GetLayer(myFileName) != NULL) {
        wxLogError(_("Name '%s' allready exists, please choose another name!"), myNameDlg.GetValue());
        return _GetMemoryLayerNameFromUser(name);
    }

    return myNameDlg.GetValue();
}

bool Frame::AddLayers(const wxArrayString& names) {
    m_vrViewerLayerManager->FreezeBegin();
    for (unsigned int i = 0; i < names.GetCount(); i++) {
        AddLayer(names.Item(i));
    }
    m_vrViewerLayerManager->FreezeEnd();
    return true;
}

void Frame::OnLayerRemove(wxCommandEvent& event) {
    wxArrayString myLayersName;
    int myDipsIndex = wxNOT_FOUND;
    for (int i = 0; i < m_vrViewerLayerManager->GetCount(); i++) {
        vrRenderer* myRenderer = m_vrViewerLayerManager->GetRenderer(i);
        wxASSERT(myRenderer);
        if (myRenderer->GetLayer()->GetType() != vrDRIVER_VECTOR_C2P) {
            myLayersName.Add(myRenderer->GetLayer()->GetDisplayName().GetFullName());
        } else {
            myDipsIndex = i;
        }
    }

    if (myLayersName.IsEmpty()) {
        wxLogError("No layer opened, nothing to close");
        return;
    }

    wxMultiChoiceDialog myChoiceDlg(this, "Select Layer(s) to close", "Close layer(s)", myLayersName);
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
    for (int j = (signed)myLayerToRemoveIndex.GetCount() - 1; j >= 0; j--) {
        long myIndexToRemove = myLayerToRemoveIndex.Item(j);
        if (myDipsIndex != wxNOT_FOUND && myDipsIndex <= myIndexToRemove) {
            myIndexToRemove = myIndexToRemove + 1;
        }
        _RemoveLayer(myIndexToRemove);
    }

    m_vrViewerLayerManager->FreezeEnd();
}

void Frame::OnUpdateIdle(wxIdleEvent& event) {
    _SetSelectedFeature();
}

void Frame::OnClose(wxCloseEvent& event) {
    event.Skip();
}

void Frame::OnKeyDown(wxKeyEvent& event) {
    m_KeyBoardState = wxKeyboardState(event.ControlDown(), event.ShiftDown(), event.AltDown(), event.MetaDown());
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool* myTool = m_vrDisplay->GetTool();
    if (myTool == NULL) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_IN) {
        m_vrDisplay->SetToolZoomOut();
    }
    event.Skip();
}

void Frame::OnKeyUp(wxKeyEvent& event) {
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool* myTool = m_vrDisplay->GetTool();
    if (myTool == NULL) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_OUT || myTool->GetID() == wxID_ZOOM_IN) {
        m_vrDisplay->SetToolZoom();
    }
    event.Skip();
}

void Frame::OnCreateSLBL(wxCommandEvent& event) {
    CreateSLBL_DLG myDlg(this, m_vrViewerLayerManager, m_vrLayerManager);
    if (myDlg.ShowModal() != wxID_OK) {
        return;
    }

    vrLayerRaster* myRasterInp = myDlg.GetInputRaster();
    if (myRasterInp == NULL) {
        wxLogError(_("Getting Input raster failed!"));
        return;
    }

    vrLayerRaster* myRasterMask = myDlg.GetMaskRaster();
    vrLayerRaster* myRasterOut = myDlg.GetOutputRaster();
    if (myRasterOut == NULL) {
        wxLogError(_("Getting Output raster failed!"));
        return;
    }

    ParFit myParam = myDlg.GetParameters();
    wxProgressDialog* myProgressDlg = new wxProgressDialog(_("Creating SLBL"), _("Please wait"), 100, this,
                                                           wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
    myParam.reportProgressDlg = myProgressDlg;
    wxArrayDouble myGeoTransformArray;
    myRasterInp->GetGeoTransform(myGeoTransformArray);
    int myRetVal = demFit(SLBL, myRasterOut->GetDatasetRef()->GetRasterBand(1),
                          myRasterInp->GetDatasetRef()->GetRasterBand(1),
                          myRasterMask ? myRasterMask->GetDatasetRef()->GetRasterBand(1) : NULL,
                          myGeoTransformArray.Item(1), myGeoTransformArray.Item(5), myParam,
                          NULL,   // DEM BL
                          NULL);  // DEM DIFF
    wxLogMessage(_("Creating SLBL, returns: %d"), myRetVal);
    wxDELETE(myProgressDlg);

    // remove temporary maks raster if needed
    if (myRasterMask != NULL) {
        wxFileName myRasterMaskName(myRasterMask->GetFileName());
        m_vrLayerManager->Close(myRasterMask);
        m_vrLayerManager->Erase(myRasterMaskName);

        myRasterMaskName.SetExt(_T(""));
        if (myRasterMaskName.Exists()) {
            wxRemoveFile(myRasterMaskName.GetFullPath());
        }
    }

    if (myDlg.DoAddResultToDisplay() == true) {
        AddLayer(myRasterOut->GetFileName());
    }
}

void Frame::OnPlaneIntersectionDialog(wxCommandEvent& event) {
    PlInt_DLG* myDlg = static_cast<PlInt_DLG*>(FindWindowByName(PLINT_DIALOG_NAME));
    if (myDlg != NULL) {
        myDlg->Raise();
        return;
    }

    myDlg = new PlInt_DLG(this, m_vrViewerLayerManager);
    myDlg->Show();
}

void Frame::OnProfileDialog(wxCommandEvent& event) {
    ProfileOperation myProfileOp(m_vrViewerLayerManager, m_vrLayerManager);
    OGRGeometry* myGeom = myProfileOp.GetSelectedProfileGeometry(m_vrTOC);
    if (myGeom == NULL) {
        return;
    }

    Profile_DLG myDlg(this, m_vrViewerLayerManager);
    if (myDlg.ShowModal() != wxID_OK) {
        OGRGeometryFactory::destroyGeometry(myGeom);
        return;
    }

    if (myProfileOp.DoExportText(myGeom, myDlg.GetSelectedRasterName(), myDlg.GetDestinationFileName()) == true) {
        wxMessageBox(_("Profil exported correctly!"));
    }
    OGRGeometryFactory::destroyGeometry(myGeom);
}

void Frame::OnProfileView(wxCommandEvent& event) {
    wxArrayString myPaths;
    wxArrayString myNames;
    wxArrayString myDisplayNames;

    for (unsigned int i = 0; i < m_vrViewerLayerManager->GetCount(); i++) {
        vrLayer* myLayer = m_vrViewerLayerManager->GetRenderer(i)->GetLayer();
        wxASSERT(myLayer);

        if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7) {
            myDisplayNames.Add(myLayer->GetDisplayName().GetFullName());
            myNames.Add(myLayer->GetFileName().GetFullName());
            myPaths.Add(myLayer->GetFileName().GetPath());
        }
    }

    if (myDisplayNames.GetCount() == 0) {
        wxLogWarning(_("No Data, making profile not possible"));
        return;
    }

    int mySelLayer = m_vrTOC->GetSelection();
    if (mySelLayer == wxNOT_FOUND) {
        wxLogWarning(_("No layer Selected! Select a layer!"));
        return;
    }
    vrRenderer* myRenderer = m_vrViewerLayerManager->GetRenderer(mySelLayer);
    if (myRenderer == NULL || myRenderer->GetLayer()->GetType() > vrDRIVER_VECTOR_MEMORY) {
        wxLogWarning(_("Incorrect layer type selected! Select a vector layer"));
        return;
    }

    vrLayerVectorOGR* myLayer = static_cast<vrLayerVectorOGR*>(myRenderer->GetLayer());
    if (myLayer->GetSelectedIDs()->GetCount() != 1) {
        wxLogWarning(_("Select one feature to create profile on it!"));
        return;
    }
    if (wkbFlatten(myLayer->GetGeometryType()) != wkbLineString) {
        wxLogWarning(_("Incorrect geometry: profile is only working with line layers!"));
        return;
    }

    wxMultiChoiceDialog myChoiceDlg(this, _("Select Raster layer"), _("Profile data"), myDisplayNames);
    if (myChoiceDlg.ShowModal() != wxID_OK) {
        return;
    }
    wxArrayInt mySelectedRasterIndex = myChoiceDlg.GetSelections();
    if (mySelectedRasterIndex.GetCount() == 0) {
        return;
    }

    vrArrayLayer myRasterLayers;
    for (unsigned int i = 0; i < mySelectedRasterIndex.GetCount(); i++) {
        int index = mySelectedRasterIndex.Item(i);
        wxFileName myFileName(myPaths.Item(index), myNames.Item(index));
        vrLayer* myLayer = m_vrViewerLayerManager->GetRenderer(myFileName.GetFullPath())->GetLayer();
        wxASSERT(myLayer);
        myRasterLayers.Add(myLayer);
    }

    ProfileView_DLG* myDlg = new ProfileView_DLG(this, m_vrLayerManager, myLayer, myRasterLayers);
    myDlg->Show();
}

void Frame::OnToolZoomToFit(wxCommandEvent& event) {
    m_vrViewerLayerManager->ZoomToFit(true);
    m_vrViewerLayerManager->Reload();
}

void Frame::OnToolSelect(wxCommandEvent& event) {
    m_vrDisplay->SetTool(new vrDisplayToolSelect(m_vrDisplay));
}

void Frame::OnToolSelectAction(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    m_vrViewerLayerManager->ClearSelection();
    if (m_vrViewerLayerManager->Select(myMsg->m_rect, m_vrViewerLayerManager->GetEditionRenderer()) == wxNOT_FOUND) {
        wxDELETE(myMsg);
        return;
    }

    m_vrViewerLayerManager->Reload();
    wxDELETE(myMsg);
}

void Frame::OnToolClearSelection(wxCommandEvent& event) {
    wxASSERT(m_vrViewerLayerManager);
    m_vrViewerLayerManager->ClearSelection();
    m_vrViewerLayerManager->Reload();
}

void Frame::OnToolZoom(wxCommandEvent& event) {
    m_vrDisplay->SetToolZoom();
}

void Frame::OnToolZoomAction(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);
    // getting rectangle
    vrCoordinate* myCoord = m_vrViewerLayerManager->GetDisplay()->GetCoordinate();
    wxASSERT(myCoord);

    // get real rectangle
    vrRealRect myRealRect;
    bool bSuccess = myCoord->ConvertFromPixels(myMsg->m_rect, myRealRect);
    wxASSERT(bSuccess == true);

    // get fitted rectangle
    vrRealRect myFittedRect = myCoord->GetRectFitted(myRealRect);
    wxASSERT(myFittedRect.IsOk());

    // zoom out
    if (myMsg->m_evtType == vrEVT_TOOL_ZOOM) {
        m_vrViewerLayerManager->Zoom(myFittedRect);
    } else {
        m_vrViewerLayerManager->ZoomOut(myFittedRect);
    }
    wxDELETE(myMsg);
}

void Frame::OnToolPan(wxCommandEvent& event) {
    m_vrDisplay->SetToolPan();
}

void Frame::OnToolPanAction(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrCoordinate* myCoord = m_vrViewerLayerManager->GetDisplay()->GetCoordinate();
    wxASSERT(myCoord);

    wxPoint myMovedPos = myMsg->m_position;
    wxPoint2DDouble myMovedRealPt;
    if (myCoord->ConvertFromPixels(myMovedPos, myMovedRealPt) == false) {
        wxLogError("Error converting point : %d, %d to real coordinate", myMovedPos.x, myMovedPos.y);
        wxDELETE(myMsg);
        return;
    }

    vrRealRect myActExtent = myCoord->GetExtent();
    myActExtent.MoveLeftTopTo(myMovedRealPt);
    myCoord->SetExtent(myActExtent);
    m_vrViewerLayerManager->Reload();
    wxDELETE(myMsg);
}

void Frame::OnToolDraw(wxCommandEvent& event) {
    vrRenderer* myRendererEdit = m_vrViewerLayerManager->GetEditionRenderer();
    if (myRendererEdit == NULL) {
        return;
    }

    vrLayerVector* myLayerVect = static_cast<vrLayerVector*>(myRendererEdit->GetLayer());
    OGRwkbGeometryType myGeomType = myLayerVect->GetGeometryType();
    switch (wkbFlatten(myGeomType)) {
        case wkbPoint:
        case wkbMultiPoint:
            m_vrDisplay->SetTool(new vrDisplayToolEdit(m_vrDisplay));
            break;

        case wkbLineString:
        case wkbMultiLineString:
        case wkbPolygon:
        case wkbMultiPolygon:
            m_vrDisplay->SetTool(new vrDisplayToolEditLine(m_vrDisplay));
            break;

        default:
            wxLogError(_("Geometry not supported for edition: %d"), wkbFlatten(myGeomType));
            break;
    }

    if (m_Editor) {
        // Editor exists, assume view has changed
        m_Editor->ViewChanged();
        m_Editor->DrawShapeEdit(m_vrViewerLayerManager->GetEditionRenderer()->GetRender());
    }
}

void Frame::OnToolDrawAction(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrRenderer* myRendererEdit = m_vrViewerLayerManager->GetEditionRenderer();
    if (myRendererEdit == NULL) {
        wxDELETE(myMsg);
        return;
    }
    vrLayerVector* myLayerVect = static_cast<vrLayerVector*>(myRendererEdit->GetLayer());

    // create editor if not exists
    if (m_Editor == NULL) {
        OGRwkbGeometryType myGeomType = myLayerVect->GetGeometryType();
        switch (wkbFlatten(myGeomType)) {
            case wkbPoint:
            case wkbMultiPoint:
                m_Editor = new vrShapeEditorPoint(m_vrDisplay);
                break;

            case wkbLineString:
            case wkbMultiLineString:
                m_Editor = new vrShapeEditorLine(m_vrDisplay);
                break;

            case wkbPolygon:
            case wkbMultiPolygon:
                m_Editor = new vrShapeEditorPolygon(m_vrDisplay);
                break;

            default:
                wxLogError(_("Geometry not supported for edition: %d"), wkbFlatten(myGeomType));
                wxDELETE(myMsg);
                return;
        }
    }

    wxPoint2DDouble myRealPt(0, 0);
    m_vrDisplay->GetCoordinate()->ConvertFromPixels(myMsg->m_position, myRealPt);
    wxASSERT(m_Editor);
    m_Editor->AddVertex(myRealPt);

    if (myMsg->m_evtType == vrEVT_TOOL_EDIT) {
        m_Editor->DrawShapeEdit(myRendererEdit->GetRender());
    } else if (myMsg->m_evtType == vrEVT_TOOL_EDIT_FINISHED) {
        vrLayerVectorOGR* myMemoryLayer = static_cast<vrLayerVectorOGR*>(myRendererEdit->GetLayer());
        long myAddedId = myMemoryLayer->AddFeature(m_Editor->GetGeometryRef());
        myMemoryLayer->SetSelectedID(myAddedId);
        wxDELETE(m_Editor);
        m_vrViewerLayerManager->Reload();
    }
    wxDELETE(myMsg);
}

void Frame::OnToolModify(wxCommandEvent& event) {
    m_vrDisplay->SetTool(new vrDisplayToolModify(m_vrDisplay));
}

void Frame::OnToolModifySearch(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrRenderer* myMemoryRenderer = m_vrViewerLayerManager->GetEditionRenderer();
    wxASSERT(myMemoryRenderer);
    vrLayerVectorOGR* myMemoryLayer = static_cast<vrLayerVectorOGR*>(myMemoryRenderer->GetLayer());
    wxASSERT(myMemoryLayer);

    vrRealRect myRealRect;
    m_vrDisplay->GetCoordinate()->ConvertFromPixels(myMsg->m_rect, myRealRect);
    wxDELETE(myMsg);
    myMemoryLayer->Select(myRealRect);
    wxArrayLong* mySelectedIDs = myMemoryLayer->GetSelectedIDs();
    wxASSERT(mySelectedIDs);
    if (mySelectedIDs->GetCount() != 0) {
        wxLogMessage(_T("Selected Geometries ID: %ld (number: %ld)"), mySelectedIDs->Item(0),
                     mySelectedIDs->GetCount());
        // copy geometry vertex to tool
        OGRFeature* myFeature = myMemoryLayer->GetFeature(mySelectedIDs->Item(0));
        vrDisplayToolModify* myModifyTool = (vrDisplayToolModify*)m_vrDisplay->GetTool();
        wxASSERT(myModifyTool);
        myModifyTool->SetActiveGeometry(myFeature->GetGeometryRef(), myMemoryLayer->GetGeometryType(),
                                        m_vrDisplay->GetCoordinate());
        OGRFeature::DestroyFeature(myFeature);
        m_vrViewerLayerManager->Reload();
    }
}

void Frame::OnToolModifyUpdate(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrRenderer* myMemoryRenderer = m_vrViewerLayerManager->GetEditionRenderer();
    wxASSERT(myMemoryRenderer);
    vrLayerVectorOGR* myMemoryLayer = (vrLayerVectorOGR*)myMemoryRenderer->GetLayer();
    wxASSERT(myMemoryLayer);

    wxPoint2DDouble myRealPt;
    m_vrDisplay->GetCoordinate()->ConvertFromPixels(myMsg->m_position, myRealPt);
    int myVertexIndex = myMsg->m_longData;
    wxDELETE(myMsg);

    wxArrayLong* mySelectedIDs = myMemoryLayer->GetSelectedIDs();
    wxASSERT(mySelectedIDs);
    wxASSERT(mySelectedIDs->GetCount() > 0);

    // update geometry and send new geometry to the modify tool
    OGRFeature* myFeature = myMemoryLayer->GetFeature(mySelectedIDs->Item(0));
    switch (myMemoryLayer->GetGeometryType()) {
        case wkbLineString:  // line
        {
            OGRLineString* myLine = (OGRLineString*)myFeature->GetGeometryRef();
            myLine->setPoint(myVertexIndex, myRealPt.m_x, myRealPt.m_y);
        } break;

        case wkbPolygon: {
            OGRPolygon* myPolygon = (OGRPolygon*)myFeature->GetGeometryRef();
            OGRLineString* myLine = (OGRLineString*)myPolygon->getExteriorRing();
            myLine->setPoint(myVertexIndex, myRealPt.m_x, myRealPt.m_y);
        } break;

        case wkbPoint: {
            OGRPoint* myPt = (OGRPoint*)myFeature->GetGeometryRef();
            myPt->setX(myRealPt.m_x);
            myPt->setY(myRealPt.m_y);
        } break;

        default:
            wxLogError(_T("Modification of geometry type: %d isn't supported!"), myMemoryLayer->GetGeometryType());
            break;
    }
    myMemoryLayer->SetFeature(myFeature);
    vrDisplayToolModify* myModifyTool = (vrDisplayToolModify*)m_vrDisplay->GetTool();
    myModifyTool->SetActiveGeometry(myFeature->GetGeometryRef(), myMemoryLayer->GetGeometryType(),
                                    m_vrDisplay->GetCoordinate());
    OGRFeature::DestroyFeature(myFeature);
    m_vrViewerLayerManager->Reload();
}

void Frame::OnUpdateUIToolClearSelection(wxUpdateUIEvent& event) {
    if (m_vrViewerLayerManager && m_vrViewerLayerManager->GetSelectionCount() > 0) {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}

void Frame::OnUpdateUIRemoveLayer(wxUpdateUIEvent& event) {
    if (m_vrViewerLayerManager && m_vrViewerLayerManager->GetCount() > 0) {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}

void Frame::OnUpdateEditionStart(wxUpdateUIEvent& event) {
    if (m_vrTOC && m_vrTOC->GetSelection() != wxNOT_FOUND) {
        if (m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection()) !=
            m_vrViewerLayerManager->GetEditionRenderer()) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}

void Frame::OnUpdateEditionPossible(wxUpdateUIEvent& event) {
    if (m_vrViewerLayerManager->GetEditionRenderer() == NULL) {
        event.Enable(false);
        return;
    }
    event.Enable(true);
}

void Frame::OnUpdateDeletePossible(wxUpdateUIEvent& event) {
    if (m_vrViewerLayerManager->GetEditionRenderer() != NULL) {
        vrLayerVector* myLayer = static_cast<vrLayerVector*>(m_vrViewerLayerManager->GetEditionRenderer()->GetLayer());
        wxASSERT(myLayer);
        if (myLayer->GetSelectedIDs()->GetCount() > 0) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}

void Frame::OnUpdateUISaveSelectedLayer(wxUpdateUIEvent& event) {
    if (m_vrTOC && m_vrTOC->GetSelection() != wxNOT_FOUND) {
        vrLayerVector* myLayer = static_cast<vrLayerVector*>(
            m_vrViewerLayerManager->GetRenderer(m_vrTOC->GetSelection())->GetLayer());

        if (myLayer != NULL && myLayer->GetType() >= vrDRIVER_VECTOR_SHP &&
            myLayer->GetType() <= vrDRIVER_VECTOR_MEMORY) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
    return;
}

void Frame::OnUpdateUIPlaneIntersection(wxUpdateUIEvent& event) {
    bool bCheck = false;
    if (FindWindowByName(PLINT_DIALOG_NAME) != NULL) {
        bCheck = true;
    }
    event.Check(bCheck);
}

Frame::Frame(const wxString& title)
    : wxFrame(nullptr, FRAME_WINDOW, title) {
    // crash logging
    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(
        options, "https://7ef21540a4d44fdd9c50f64b3e36c536@o4504901722177536.ingest.sentry.io/4504901725519872");
    // This is also the default-path. For further information and recommendations:
    // https://docs.sentry.io/platforms/native/configuration/options/#database-path

    sentry_options_set_database_path(options, ".sentry-native");
    wxString my_release_version(SOFTWARE_NAME);
    my_release_version << "@" << Fosi_MAJOR_VERSION << "." << Fosi_MINOR_VERSION << "." << GIT_NUMBER;
    sentry_options_set_release(options, my_release_version);
    sentry_options_set_debug(options, 1);
    sentry_init(options);

    // Logging
    wxLog::SetLogLevel(wxLOG_Info);
    wxLog* myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
    delete wxLog::SetActiveTarget(myDlgLog);
    m_LogWnd = new wxLogWindow(this, "Fosi Log", false, true);

    // setlocale(LC_NUMERIC, "C");

    // preferences
    wxConfigBase::Set(new wxFileConfig(SOFTWARE_NAME));
    m_vrViewerLayerManager = nullptr;
    m_vrDisplay = nullptr;
    m_vrLayerManager = nullptr;
    m_Editor = nullptr;

    // Design interface
    vroomgis_initialize_images();
    vroomgis_initialize_images_toolbar();
    CreateStatusBar(3);
    // disable help menu into statusbar
    SetStatusBarPane(-1);
    wxString version_txt = "v";
    version_txt << Fosi_MAJOR_VERSION << "." << Fosi_MINOR_VERSION << "." << GIT_NUMBER << " (";
    version_txt << GIT_REV << ")";
    SetStatusText(version_txt, 2);

    _CreateControls();
    _CreateMenus();
    _CreateToolbar();

    // add icon (windows / linux)
    wxIcon myIcon;
    myIcon.CopyFromBitmap(Bitmaps::GetLogo());
    SetIcon(myIcon);

    // vroomGIS
    m_vrLayerManager = new vrLayerManager();
    m_vrViewerLayerManager = new vrViewerLayerManager(m_vrLayerManager, this, m_vrDisplay, m_vrTOC);

    _CreateAccelerators();

    m_vrDisplay->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown), NULL, this);
    m_vrDisplay->Connect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp), NULL, this);

    m_vrDisplay->SetFocus();
    _PreferenceChanged(false);

    // DND
    m_vrTOC->GetControl()->SetDropTarget(new ctDropFiles(this));

    wxConfigBase* myConfig = wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    bool bCheckStartup = myConfig->ReadBool("CHECK_AT_STARTUP", true);
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");

    if (bCheckStartup == false) {
        return;
    }

    //    long mySVNversion = wxNOT_FOUND;
    //    wxHgVersion::GetSoftNumber().ToLong(&mySVNversion);
    //    WebUpdateThread * myUpdate = new WebUpdateThread(m_InfoBar, myProxyInfo);
    //    myUpdate->CheckNewVersion(mySVNversion, true, false, true);
}

Frame::~Frame() {
    m_vrDisplay->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Frame::OnKeyDown), NULL, this);
    m_vrDisplay->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(Frame::OnKeyUp), NULL, this);

    // don't delete m_vrViewerLayerManager, will be deleted by the manager
    wxDELETE(m_vrLayerManager);

    delete wxLog::SetActiveTarget(NULL);

    vroomgis_clear_images();
    vroomgis_uninitialize_images_toolbar();
    sentry_close();
}
