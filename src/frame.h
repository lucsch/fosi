/***************************************************************************
 coltopgisframe.h
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/
#ifndef _COLTOPGISFRAME_H
#define _COLTOPGISFRAME_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/infobar.h>
#include <wx/dnd.h>

#include "vroomgis.h"
#include "update.h"

class TOC;
class vrLayerManager;
class vrViewerLayerManager;
class vrViewerDisplay;
class ProjectManager;

// Identifier
const int FRAME_WINDOW = wxID_HIGHEST + 1;
const int MENU_FRAME_SELECT = wxID_HIGHEST + 2;
const int MENU_FRAME_PAN = wxID_HIGHEST + 3;
const int MENU_WINDOW_LOG = wxID_HIGHEST + 4;
const int MENU_WINDOW_DATA_MANAGER = wxID_HIGHEST + 5;
const int WINDOW_DATA_MANAGER = wxID_HIGHEST + 6;
const int MENU_DATA_ADD = wxID_HIGHEST + 7;
const int MENU_DATA_REMOVE = wxID_HIGHEST + 8;
const int MENU_CHECK_UPDATE = wxID_HIGHEST + 23;
const int MENU_WEBSITE = wxID_HIGHEST + 24;
const int MENU_FRAME_CLEAR_SELECTION = wxID_HIGHEST + 26;

const wxString g_ProgName = "ColtopGIS";



// DND support
class Frame;
class ctDropFiles : public wxFileDropTarget {
private:
	Frame * m_Frame;
	
public:
	ctDropFiles(Frame * parent);
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filenames);
};



class Frame : public wxFrame {
private:
	
	// vroomgis engine members
    TOC * m_vrTOC;
    vrLayerManager * m_vrLayerManager;
    vrViewerLayerManager * m_vrViewerLayerManager;
    vrViewerDisplay * m_vrDisplay;
	
	ArrayPlugin m_Plugins;
	wxLogWindow * m_LogWnd;
	ProjectManager * m_ProjectManager;
	wxMenu * m_RecentMenu;
	wxKeyboardState m_KeyBoardState;
    WebUpdateInformationBar * m_InfoBar;
	
	
	void _CreateControls();
    void _CreateMenus();
    void _CreateToolbar();
	void _CreateAccelerators();
	void _InitPlugins();
    void _EndPlugins();
	bool _ProjectQuestion(const wxString & text);
    
    bool _ProjectSave(const wxFileName & file);
	void _ProjectBeforeSave(const wxFileName & project);
	void _ProjectAfterSave();
    void _ProjectAfterOpen();
	void _SetFrameName();
	void _SetSelectedFeature();
    bool _RemoveLayer(long position);
	void _PreferenceChanged(bool refresh = true);
	bool _CheckSecurity();
	void OnDisplaySecurityMessage(wxThreadEvent & event);


	
    void OnAbout(wxCommandEvent & event);
    void OnCheckUpdates(wxCommandEvent & event);
	void OnPreferences(wxCommandEvent & event);
    void OnUserManual(wxCommandEvent & event);
    void OnWebSite (wxCommandEvent & event);
    void OnQuit(wxCommandEvent & event);
	void OnLogWindow(wxCommandEvent & event);
	void OnDataManager(wxCommandEvent & event);
	void OnProjectNew(wxCommandEvent & event);
    void OnProjectOpen(wxCommandEvent & event);
	void OnProjectRecent(wxCommandEvent & event);
    void OnProjectSave(wxCommandEvent & event);
    void OnProjectSaveAs(wxCommandEvent & event);
	void OnLayerAdd(wxCommandEvent & event);
    void OnLayerRemove(wxCommandEvent & event);
	void OnUpdateIdle(wxIdleEvent & event);
	void OnClose(wxCloseEvent & event);
	void OnKeyDown(wxKeyEvent & event);
    void OnKeyUp(wxKeyEvent & event);
	
	void OnToolZoomToFit(wxCommandEvent & event);
    void OnToolSelect(wxCommandEvent & event);
    void OnToolSelectAction(wxCommandEvent & event);
    void OnToolClearSelection(wxCommandEvent & event);
    void OnToolZoom(wxCommandEvent & event);
    void OnToolZoomAction(wxCommandEvent & event);
    void OnToolPan(wxCommandEvent & event);
    void OnToolPanAction(wxCommandEvent & event);
    
    // Update UI functions
    void OnUpdateUIToolClearSelection(wxUpdateUIEvent & event);
	
	DECLARE_EVENT_TABLE();
public:
    Frame(const wxString & title);
    virtual ~Frame();
	
	bool ProjectOpen(const wxFileName & file);
	bool AddLayers (const wxArrayString & names);
    bool AddLayer(const wxFileName & file, long position = -1, vrRender * render = NULL,
				  vrLabel * label = NULL, bool visible = true, bool readwrite = false);
	inline  ProjectManager * GetProjectManager() const;
	inline vrViewerLayerManager * GetViewerLayerManager() const;
	inline vrLayerManager * GetLayerManager() const;
    inline ArrayPlugin * GetPlugins();


};

inline  ProjectManager * Frame::GetProjectManager() const {
	return m_ProjectManager;
}

inline  vrLayerManager * Frame::GetLayerManager() const {
	return m_vrLayerManager;
}

inline  vrViewerLayerManager * Frame::GetViewerLayerManager() const {
	return m_vrViewerLayerManager;
}

inline ArrayPlugin * Frame::GetPlugins() {
	return &m_Plugins;
}

#endif
