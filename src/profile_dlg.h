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

class vrViewerLayerManager;


class ProfileParams {
public:
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

public:
    
    Profile_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id = wxID_ANY, const wxString& title = _("Profile"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
    ~Profile_DLG();
    
    virtual bool TransferDataFromDialog();
    ProfileParams GetParams () {return m_Params;}

    
private:
    wxComboBox* m_InputDEMCtrl;
    wxComboBox* m_InputLineCtrl;
    wxRadioButton* m_FeaturesAllCtrl;
    wxRadioButton* m_FeaturesSelectedCtrl;
    wxNotebook* m_ExportNotebookCtrl;
    wxPanel* m_PanelText;
    wxRadioButton* m_DistanceHeightCtrl;
    wxRadioButton* m_XYZCtrl;
    wxRadioButton* m_ClipBoardCtrl;
    wxRadioButton* m_TextFileOptionCtrl;
    wxFilePickerCtrl* m_TextFilePickerCtrl;
    wxPanel* m_PanelVector;
    wxRadioButton* m_3DPointCtrl;
    wxRadioButton* m_3dLineCtrl;
    wxFilePickerCtrl* m_VectorFilePickerCtrl;
    wxButton* m_BtnOkCtrl;
    
    ProfileParams m_Params;
    
    void _CreateControls();
    
    void OnUpdateUITextFilePickerCtrl( wxUpdateUIEvent& event );
    void OnUpdateUITBtnOk( wxUpdateUIEvent& event );
	
};

#endif