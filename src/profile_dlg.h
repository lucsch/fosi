/***************************************************************************
 profile_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PROFILE_DLG_H_
#define _PROFILE_DLG_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/clipbrd.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>


class vrViewerLayerManager;
class vrLayerManager;
class vrLayerVectorOGR;
class vrLayerRasterGDAL;



class ProfileParams {
public:
    wxFileName m_FileInputDEM;
    wxFileName m_FileInputVector;
    wxFileName m_FileOutputText;
    wxFileName m_FileOutputVector;
    bool m_AllValues;
    enum  {PROFILE_EXPORT_TEXT = 0, PROFILE_EXPORT_VECTOR = 1 } m_ExportTypeFormat;
    enum  {PROFILE_TEXT_CLIPBOARD, PROFILE_TEXT_FILE} m_ExportTypeTextFormat;
    enum  {PROFILE_TEXT_DISTANCE_HEIGHT, PROFILE_TEXT_XYZ } m_ExportTypeText;
    enum  {PROFILE_VECTOR_POINT, PROFILE_VECTOR_LINE } m_ExportTypeVector;
    ProfileParams () {
        m_AllValues = true;
        m_ExportTypeFormat = PROFILE_EXPORT_TEXT;
        m_ExportTypeTextFormat = PROFILE_TEXT_CLIPBOARD;
        m_ExportTypeText = PROFILE_TEXT_DISTANCE_HEIGHT;
        m_ExportTypeVector = PROFILE_VECTOR_POINT;
    }
};




class Profile_DLG : public wxDialog
{
private:
    wxFilePickerCtrl* m_TextFilePickerCtrl;
    wxCheckListBox* m_RasterList;
    wxButton* m_BtnOkCtrl;
    
    wxFileName m_OutFile;
    wxArrayString m_AllRaster;
    wxArrayString m_SelectedRasterNames;
    
    void _CreateControls();
    
    void OnUpdateUITBtnOk( wxUpdateUIEvent& event );
    
public:
    Profile_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id = wxID_ANY, const wxString& title = _("Profile"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    ~Profile_DLG();
    
    wxFileName GetDestinationFileName() { return m_OutFile;}
    wxArrayString GetSelectedRasterName () {return m_SelectedRasterNames;}
    
    virtual bool TransferDataFromWindow();    
};



class OGRGeometry;
class vrViewerTOC;
class ProfileOperation {
private:
    vrViewerLayerManager * m_ViewerManager;
    vrLayerManager * m_LayerManager;

    
public:
    ProfileOperation (vrViewerLayerManager * viewermanager, vrLayerManager * layermanager);
    ~ProfileOperation ();
    bool DoExport();
    
    bool GetListRasters(wxArrayString & fullnames, wxArrayString & displayname);
    OGRGeometry * GetSelectedProfileGeometry (vrViewerTOC * toc);
    
    bool DoExportText (OGRGeometry * profile, const wxArrayString & rasternames, const wxFileName & destination);
};




#endif