/***************************************************************************
 preferencepanel.h
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _PREFERENCEPANEL_H_
#define _PREFERENCEPANEL_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/config.h>
#include <wx/clrpicker.h>


class PreferenceDlg;

class PreferencePanel : public wxPanel {
private:
    wxWindow * m_Parent;
	
protected:
    virtual void _LoadSettings(){;}
	
public:
    PreferencePanel(wxWindow * parent);
    virtual ~PreferencePanel();
	
    virtual bool SaveSettings();
    inline const wxWindow * GetPanelParent() const;
	
};

inline const wxWindow * PreferencePanel::GetPanelParent() const {
	return m_Parent;
}





class PreferencePanelMain : public PreferencePanel {
private:
	wxColourPickerCtrl * m_BackgroundColourCtrl;
	wxColourPickerCtrl * m_SelectionColourCtrl;
	wxChoice * m_UnitChoiceCtrl;
	wxCheckBox* m_RelativePathCtrl;
	wxCheckBox* m_BuildOverviewCtrl;


	
protected:
    virtual void _LoadSettings();
	
public:
    PreferencePanelMain(wxWindow * parent);
    virtual ~PreferencePanelMain();
	
    virtual bool SaveSettings();
	
};



class PreferencePanelInternet : public PreferencePanel {
private:
	wxCheckBox* m_CheckAtStartup;
	wxTextCtrl* m_ProxyInfo;
	
protected:
    virtual void _LoadSettings();
	
public:
    PreferencePanelInternet(wxWindow * parent);
    virtual ~PreferencePanelInternet();
	
    virtual bool SaveSettings();
	
};

#endif
