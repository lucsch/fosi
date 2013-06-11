/***************************************************************************
 createslbl_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _CREATE_SLBL_DLG_H_
#define _CREATE_SLBL_DLG_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include "core/demutilopts.h"


class CreateSLBL_DLG : public wxDialog
{	
public:
    
    CreateSLBL_DLG( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Create SLBL Raster"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
    ~CreateSLBL_DLG();
    
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
    
private:
    wxComboBox* m_InputListCtrl;
    wxButton* m_InputBrowseBtnCtrl;
    wxCheckBox* m_MaskUseCtrl;
    wxComboBox* m_MaskListCtrl;
    wxButton* m_MaskBrowseBtnCtrl;
    wxSpinCtrl* m_OptMaxIterCtrl;
    wxSpinCtrlDouble* m_OptMinDiffCtrl;
    ParFit m_Params;
    
    void _CreateControls();
};

#endif