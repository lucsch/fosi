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
#include "Excel.h"




Profile_DLG::Profile_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ){
    _CreateControls();
    
    ProfileOperation myOperation (viewermanager, NULL);
    wxArrayString myDisplayNames;
    myOperation.GetListRasters(m_AllRaster, myDisplayNames);
    
    if (myDisplayNames.GetCount() != 0) {
        m_RasterList->InsertItems(myDisplayNames, 0);
    }
    m_BtnOkCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );
    
    SetMinSize(GetSize());
}



Profile_DLG::~Profile_DLG(){
    m_BtnOkCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );
}



void Profile_DLG::OnUpdateUITBtnOk( wxUpdateUIEvent& event ){
    wxArrayInt myCheckedItems;
    m_RasterList->GetCheckedItems(myCheckedItems);
    if (myCheckedItems.GetCount() == 0) {
        event.Enable(false);
        return;
    }
    
    event.Enable( m_TextFilePickerCtrl->GetFileName().IsOk());
}


bool Profile_DLG::TransferDataFromWindow(){
    m_OutFile = m_TextFilePickerCtrl->GetFileName();
    m_SelectedRasterNames.Clear();
    
    wxArrayInt mySelectedIndex;
    m_RasterList->GetCheckedItems(mySelectedIndex);
    for (unsigned int i = 0; i< mySelectedIndex.GetCount(); i++) {
        int myIndex = mySelectedIndex.Item(i);
        m_SelectedRasterNames.Add( m_AllRaster.Item(myIndex) );
    }
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
	
	m_TextFilePickerCtrl = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), _("Excel files (*.xls)|*.xls"), wxDefaultPosition, wxSize( -1,-1 ), wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
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



bool ProfileOperation::GetListRasters(wxArrayString & fullnames, wxArrayString & displayname){
    wxASSERT(m_ViewerManager);
    
    fullnames.Clear();
    displayname.Clear();
    
    for (unsigned int i = 0; i< m_ViewerManager->GetCount(); i++) {
        vrLayer * myLayer =  m_ViewerManager->GetRenderer(i)->GetLayer();
        wxASSERT(myLayer);
        
        if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7){
            displayname.Add(myLayer->GetDisplayName().GetFullName());
            fullnames.Add(myLayer->GetFileName().GetFullPath());
        }
    }

    if (fullnames.GetCount() == 0) {
        return false;
    }
    return true;
}


OGRGeometry * ProfileOperation::GetSelectedProfileGeometry (vrViewerTOC * toc){
    wxASSERT(toc);
    
    int mySelLayer = toc->GetSelection();
    if (mySelLayer == wxNOT_FOUND) {
        wxLogWarning(_("No layer Selected! Select a layer!"));
        return NULL;
    }
    
    vrRenderer * myRenderer = m_ViewerManager->GetRenderer(mySelLayer);
    if (myRenderer == NULL || myRenderer->GetLayer()->GetType() > vrDRIVER_VECTOR_MEMORY) {
        wxLogWarning(_("Incorrect layer type selected! Select a vector layer"));
        return NULL;
    }
    
    vrLayerVectorOGR * myLayer = static_cast<vrLayerVectorOGR*>(myRenderer->GetLayer());
    if (myLayer->GetSelectedIDs()->GetCount() != 1) {
        wxLogWarning(_("Select one feature to create profile on it!"));
        return NULL;
    }
    if (wkbFlatten( myLayer->GetGeometryType() ) != wkbLineString) {
        wxLogWarning(_("Incorrect geometry: profile is only working with line layers!"));
        return NULL;
    }
    
    OGRFeature * myFeature = myLayer->GetFeature(myLayer->GetSelectedIDs()->Item(0));
    if (myFeature == NULL) {
        wxLogError(_("Unable to get feature: %ld"), myLayer->GetSelectedIDs()->Item(0));
        return NULL;
    }
    
    OGRGeometry * myGeom = myFeature->GetGeometryRef()->clone();
    OGRFeature::DestroyFeature(myFeature);
    return myGeom;
}


bool ProfileOperation::DoExportText (OGRGeometry * profile, const wxArrayString & rasternames, const wxFileName & destination){
    Excel myExcel;
    if(myExcel.Open((const char *) destination.GetFullPath().mb_str(wxConvUTF8))==false){
        wxLogError(_("Unable to open: '%s'"), destination.GetFullName());
        return false;
    }
    
    myExcel.Begin();
    /* file structure is:
    rastername
    distance | X | Y | Z
     0 | 1 | 2 | 3
     ...
    */
    for (unsigned int i = 0; i < rasternames.GetCount(); i++) {
        vrRenderer * myRenderer = m_ViewerManager->GetRenderer(rasternames.Item(i));
        if (myRenderer == NULL) {
            wxLogError(_("Error getting raster: %s"), rasternames.Item(i));
            continue;
        }
        
        vrOperationVectorProfiler myProfiler (profile, static_cast<vrLayerRasterGDAL*>( myRenderer->GetLayer()));
        if( myProfiler.DoProfile() == false ) {
            wxLogError(_("Error computing profile on: %s"), rasternames.Item(i));
            continue;
        }
        
        // write xls header
        wxFileName myRasterName (rasternames.Item(i));
        myExcel.Write(0, i * 4, myRasterName.GetFullName().mb_str(wxConvUTF8));
        myExcel.Write(1, i * 4, "DISTANCE");
        myExcel.Write(1, (i * 4) + 1, "X");
        myExcel.Write(1, (i * 4) + 2, "Y");
        myExcel.Write(1, (i * 4) + 3, "Z");
        
        // write xls data
        double myDist = myProfiler.GetIncrementDistance();
        double myXStep = myProfiler.GetIncrementX();
        double myYStep = myProfiler.GetIncrementY();
        
        OGRPoint myOriginPt;
        static_cast<OGRLineString*>(profile)->getPoint(0, &myOriginPt);
        wxArrayDouble * myResults = myProfiler.GetResultRef();
        wxASSERT(myResults);
        
        double myD = 0.0;
        double myX = myOriginPt.getX();
        double myY = myOriginPt.getY();
        
        for (unsigned int d = 0; d < myResults->GetCount(); d++) {
            myExcel.Write(2 + d, i * 4, myD);
            myExcel.Write(2 + d, (i * 4) + 1, myX);
            myExcel.Write(2 + d, (i * 4) + 2, myY);
            myExcel.Write(2 + d, (i * 4) + 3, myResults->Item(d));
            
            myD += myDist;
            myX += myXStep;
            myY += myYStep;
        }
        
    }
    
    myExcel.End();
    return true;
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




