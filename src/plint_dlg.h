/***************************************************************************
 plint_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PLINT_DLG_H_
#define _PLINT_DLG_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include <wx/tglbtn.h>

class vrViewerLayerManager;
class PlIntViewerOverlay;
class PlIntOperation;

const wxString PLINT_DIALOG_NAME = _T("PLINT_DLG");
const wxString PLINT_DIALOG_TOOL_NAME = _T("PLING_TOOL_DLG");

class PlInt_DLG : public wxFrame
{
public:
    
    PlInt_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id = wxID_ANY, const wxString& title = _("Plane Intersection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxTINY_CAPTION , const wxString & name = PLINT_DIALOG_NAME);
    ~PlInt_DLG();
    
private:
    wxComboBox* m_DemListCtrl;
    wxComboBox* m_VectorListCtrl;
    wxRadioButton* m_3PointsCtrl;
    wxRadioButton* m_2PointsCtrl;
    wxButton* m_EditPtsBtn;
    
    vrViewerLayerManager * m_ViewerLayerManager;
    
    wxString m_DemListCtrlSelectedText;
    wxString m_VectorListCtrlSelectedText;
    
    void _CreateControls();
    
    void OnClose( wxCloseEvent& event );
    void OnWindowFocus( wxActivateEvent& event ) ;
    void OnCreatePlaneIntersection( wxCommandEvent& event );
    void OnUpdateUICreateIntersection( wxUpdateUIEvent& event ) ;
    
    friend class PlInt_Tool_DLG;
};








class PlInt_Tool_DLG : public wxFrame
{
public:
    
    PlInt_Tool_DLG( PlInt_DLG * parent, wxWindowID id = wxID_ANY, const wxString& title = _("Plane Intersection Tools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxTAB_TRAVERSAL, const wxString & name = PLINT_DIALOG_TOOL_NAME);
    ~PlInt_Tool_DLG();
    
private:
    wxStaticText* m_DirTxtCtrl;
    wxSpinCtrl* m_DipCtrl;
    wxStaticText* m_PtsTxtCtrl;
    wxCheckBox* m_LivePreviewCtrl;
    wxButton* m_EditPtsBtn;
    wxButton* m_ClearPtsBtn;
    
    PlInt_DLG * m_ParentDlg;
    PlIntViewerOverlay * m_Overlay;
    PlIntOperation * m_Operation;
    
    
    void _CreateControls ();
    
    void OnClose( wxCloseEvent& event ) ;
    void OnIdleProcess( wxIdleEvent& event ) ;
    void OnEditPoints( wxCommandEvent& event ) ;
    void OnClearPoints( wxCommandEvent& event ) ;
	
};




#endif