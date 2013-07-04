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


const wxString PLINT_DIALOG_NAME = _T("PLINT_DLG");
class PlInt_DLG : public wxFrame
{
public:
    
    PlInt_DLG( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Plane Intersection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxTINY_CAPTION , const wxString & name = PLINT_DIALOG_NAME);
    ~PlInt_DLG();
    
private:
    wxComboBox* m_DemListCtrl;
    wxComboBox* m_VectorListCtrl;
    wxRadioButton* m_3PointsCtrl;
    wxRadioButton* m_2PointsCtrl;
    wxSpinCtrl* m_DipCtrl;
    wxCheckBox* m_LivePreviewCtrl;
    wxToggleButton* m_EditPtsBtn;
    wxStaticText* m_DirTxtCtrl;
    wxStaticText* m_DipTxtCtrl;
    wxStaticText* m_PtsTxtCtrl;
    
    void _CreateControls();
    
    void OnClose( wxCloseEvent& event );
    void OnUpdateDipCtrl( wxUpdateUIEvent& event ) ;
    void OnEditPoints( wxCommandEvent& event ) ;
    void OnUpdateUIEditPoints( wxUpdateUIEvent& event ) ;
};


#endif