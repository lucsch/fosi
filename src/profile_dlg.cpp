/***************************************************************************
 profile_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "profile_dlg.h"
#include "vroomgis.h"
#include "vrlayervector.h"
#include "vrlayerraster.h"
#include "vroperationvectorprofiler.h"





Profile_DLG::Profile_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ){
    _CreateControls();
    
    ProfileOperation myOperation (viewermanager, NULL);
    wxArrayString myPaths;
    wxArrayString myNames;
    wxArrayString myDisplayNames;
    myOperation.GetListRasters(myPaths, myNames, myDisplayNames);
    
    m_RasterList->InsertItems(myDisplayNames, 0);

    m_BtnOkCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );
    
    SetMinSize(GetSize());
}



Profile_DLG::~Profile_DLG(){
    m_BtnOkCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );
}


void Profile_DLG::OnUpdateUITBtnOk( wxUpdateUIEvent& event ){
    
}





bool Profile_DLG::TransferDataFromWindow(){
    return true;
}



void Profile_DLG::_CreateControls(){
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Source") ), wxVERTICAL );
	
	wxArrayString m_RasterListChoices;
	m_RasterList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 300,150 ), m_RasterListChoices, 0 );
	sbSizer1->Add( m_RasterList, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer8->Add( sbSizer1, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Output") ), wxVERTICAL );
	
	m_TextFilePickerCtrl = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxSize( -1,-1 ), wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	sbSizer13->Add( m_TextFilePickerCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer8->Add( sbSizer13, 0, wxEXPAND|wxALL, 5 );
	
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1Cancel;
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_BtnOkCtrl = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_BtnOkCtrl );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer8->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer8 );
	this->Layout();
	bSizer8->Fit( this );
	
	this->Centre( wxBOTH );
}





ProfileOperation::ProfileOperation (vrViewerLayerManager * viewermanager, vrLayerManager * layermanager){
    m_ViewerManager = viewermanager;
    m_LayerManager = layermanager;
}


ProfileOperation::~ProfileOperation (){
   }



bool ProfileOperation::GetListRasters(wxArrayString & paths, wxArrayString & names, wxArrayString & displayname){
    wxASSERT(m_ViewerManager);
    
    paths.Clear();
    names.Clear();
    displayname.Clear();
    
    for (unsigned int i = 0; i< m_ViewerManager->GetCount(); i++) {
        vrLayer * myLayer =  m_ViewerManager->GetRenderer(i)->GetLayer();
        wxASSERT(myLayer);
        
        if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7){
            displayname.Add(myLayer->GetDisplayName().GetFullName());
            names.Add(myLayer->GetFileName().GetFullName());
            paths.Add(myLayer->GetFileName().GetPath());
        }
    }

    if (names.GetCount() == 0) {
        return false;
    }
    return true;
}


OGRGeometry * ProfileOperation::GetSelectedProfileGeometry (vrViewerTOC * toc){
    
}



bool ProfileOperation::DoExport(){
    /*
    if (IsOK() == false) {
        return false;
    }
    
    
    long myTotalData = 0;
    bool bRestart = true;
    wxString myClipboardData = wxEmptyString;
    
    for (unsigned int i = 0; i< m_LayerVector->GetFeatureCount(); i++) {
        OGRFeature * myFeat = m_LayerVector->GetNextFeature(bRestart);
        bRestart = false;
        if (myFeat == NULL || myFeat->GetGeometryRef() == NULL) {
            continue;
        }
        
        if (m_Param.m_AllValues == false && m_LayerVector->IsFeatureSelected(myFeat->GetFID()) == false ) {
            OGRFeature::DestroyFeature(myFeat);
            continue;
        }
        
        vrOperationVectorProfiler myProfiler (myFeat->GetGeometryRef(), m_LayerRaster);
        if (myProfiler.IsOk() == false) {
            continue;
        }
        
        myProfiler.DoProfile();
        if (myProfiler.GetResultRef()->GetCount() == 0) {
            wxLogWarning (_("No results when profiling: '%s', feature: %ld"), m_Param.m_FileInputVector.GetFullName(), myFeat->GetFID());
            continue;
        }
        myTotalData += myProfiler.GetResultRef()->GetCount();
        
        // export text
        if (m_Param.m_ExportTypeFormat == ProfileParams::PROFILE_EXPORT_TEXT) {
            double myDistance = myProfiler.GetIncrementDistance();
            if (m_Param.m_ExportTypeTextFormat == ProfileParams::PROFILE_TEXT_CLIPBOARD) {
                for (unsigned l = 0; l< myProfiler.GetResultRef()->GetCount(); l++) {
                    if (m_Param.m_ExportTypeText == ProfileParams::PROFILE_TEXT_DISTANCE_HEIGHT) {
                        myClipboardData.Append(wxString::FromDouble(l * myDistance) + _T("\t") + wxString::FromDouble(myProfiler.GetResultRef()->Item(l)) + _T("\n"));
                    }
                    else {
                        OGRPoint myP1;
                        myProfiler.GetResultPoint(l, &myP1);
                        myClipboardData.Append(wxString::Format(_T("%s\t%s\t%s\n"),
                                                                wxString::FromDouble(myP1.getX()),
                                                                wxString::FromDouble(myP1.getY()),
                                                                wxString::FromDouble(myP1.getZ())));
                    }
                    myClipboardData.Append(_T("\n"));
                }
            }
            else { // export to text file
                wxASSERT(m_TextFile && m_TextFile->IsOpened()==true);
                wxString myHeader = _("DISTANCE\tHEIGHT\n");
                if (m_Param.m_ExportTypeText == ProfileParams::PROFILE_TEXT_XYZ) {
                    myHeader = _("X\tY\tZ\n");
                }
                m_TextFile->Write(myHeader);
                for (unsigned l = 0; l< myProfiler.GetResultRef()->GetCount(); l++) {
                    if (m_Param.m_ExportTypeText == ProfileParams::PROFILE_TEXT_DISTANCE_HEIGHT) {
                        m_TextFile->Write(wxString::FromDouble(l * myDistance) + _T("\t") + wxString::FromDouble(myProfiler.GetResultRef()->Item(l)) + _T("\n"));
                    }
                    else {
                        OGRPoint myP1;
                        myProfiler.GetResultPoint(l, &myP1);
                        m_TextFile->Write(wxString::Format(_T("%s\t%s\t%s\n"),
                                                          wxString::FromDouble(myP1.getX()),
                                                          wxString::FromDouble(myP1.getY()),
                                                          wxString::FromDouble(myP1.getZ())));
                    }
                    myClipboardData.Append(_T("\n"));
                }
            }
            
        }
        else { // export vector
            
        }
        OGRFeature::DestroyFeature(myFeat);
    }
    
    if (m_Param.m_ExportTypeFormat == ProfileParams::PROFILE_EXPORT_TEXT &&
        m_Param.m_ExportTypeTextFormat == ProfileParams::PROFILE_TEXT_CLIPBOARD) {
        if (wxTheClipboard->Open()){
            wxTheClipboard->SetData( new wxTextDataObject(myClipboardData) );
            wxTheClipboard->Close();
        }
    }
    
    wxMessageBox(wxString::Format(_("%ld Data processed!"), myTotalData));*/
    return false;
     
}




