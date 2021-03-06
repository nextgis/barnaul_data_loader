/******************************************************************************
* Project:  nextgis manager plugin, Barnaul data loader 
* Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nextgis.com
*******************************************************************************
*   Copyright (C) 2015 NextGIS, info@nextgis.com
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "exports.h"
#include "wxgis/geoprocessingui/gpcontrols.h"
#include "wxgis/geoprocessingui/gptooldlg.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalogui/gxngwconnui.h"

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

//---------------------------------------------------------------------------------------
// wxGISBarnaulDataLoaderDlg
//---------------------------------------------------------------------------------------

class WXDLLIMPEXP_GIS_BDL wxGISBarnaulDataLoaderDlg : public wxGISToolGenericDlg
{
public:
	wxGISBarnaulDataLoaderDlg( wxGxNGWResourceGroupUI *pResourceGroup, 
	                           wxWindow* parent, wxWindowID id = wxID_ANY, 
	                           const wxString& title = _("Import spatial data"), 
	                           const wxPoint& pos = wxDefaultPosition, 
	                           const wxSize& size = wxDefaultSize, 
	                           long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLIP_CHILDREN );
    wxGISBarnaulDataLoaderDlg( wxGxNGWLayerUI *pLayer,
        wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = _("Reload spatial data"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN );
    
    virtual ~wxGISBarnaulDataLoaderDlg();
    //events
    virtual void OnOk(wxCommandEvent & event);
    virtual void OnParamChanged(wxGISGPParamEvent& event);
protected:
    virtual wxString GetDialogSettingsName() const;
    void SerializeValues(bool bSave);
    wxGISFeature FindRow(const wxString &sFieldName, const wxString &sFieldValue, 
                         wxGISTable * const pTable);
    void DeleteExistLayer(const wxString& sLayerName);
    OGRwkbGeometryType GetGeometryType(wxGISFeatureDataset * const pDSet);
    void SetField(wxGISFeature& feature, int newIndex, const wxGISFeature &row, int index, OGRFieldType eType);
    void Load();
    void Reload();
    wxGISFeatureDataset* PrepareDataset(OGRwkbGeometryType eGeomType, bool bFilterIvalidGeometry, ITrackCancel* const pTrackCancel);
    bool IsFieldNameForbidden(const wxString& sTestFieldName) const;
protected:
    CPLString m_soOutPath;
    wxGxNGWResourceGroupUI *m_pResourceGroup; 
    wxGxNGWLayerUI *m_pLayer;
	wxString m_sMiFieldName, m_sCSVFieldName;
	OGRwkbGeometryType m_dDefaultGeomType;
    bool m_bUpdateMode;
};


//---------------------------------------------------------------------------------------
// wxGISBarnaulSimpleDataLoaderDlg
//---------------------------------------------------------------------------------------

class WXDLLIMPEXP_GIS_BDL wxGISBarnaulSimpleDataLoaderDlg : public wxGISToolGenericDlg
{
public:
    wxGISBarnaulSimpleDataLoaderDlg(wxGxNGWResourceGroupUI *pResourceGroup,
        wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = _("Import spatial data"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);
    wxGISBarnaulSimpleDataLoaderDlg(wxGxNGWLayerUI *pLayer,
        wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = _("Reload spatial data"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

    virtual ~wxGISBarnaulSimpleDataLoaderDlg();
    //events
    virtual void OnOk(wxCommandEvent & event);
    virtual void OnParamChanged(wxGISGPParamEvent& event);
protected:
    void SetField(wxGISFeature& feature, int newIndex, const wxGISFeature &row, int index, OGRFieldType eType);
    void DeleteExistLayer(const wxString& sLayerName);
    OGRwkbGeometryType GetGeometryType(wxGISFeatureDataset * const pDSet);
    virtual wxString GetDialogSettingsName() const;
    void SerializeValues(bool bSave);    
    void Load();
    void Reload();
    wxGISFeatureDataset* PrepareDataset(OGRwkbGeometryType eGeomType, bool bFilterIvalidGeometry, ITrackCancel* const pTrackCancel);
    bool IsFieldNameForbidden(const wxString& sTestFieldName) const;
protected:
    CPLString m_soOutPath;
    wxGxNGWResourceGroupUI *m_pResourceGroup;
    wxGxNGWLayerUI *m_pLayer;
    wxString m_sMiFieldName, m_sCSVFieldName;
    OGRwkbGeometryType m_dDefaultGeomType;
    bool m_bUpdateMode;
};