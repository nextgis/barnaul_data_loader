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

#include "dlg.h"

#include <wx/statline.h>

#include "../art/strat_16.xpm"

//---------------------------------------------------------------------------
// wxGISBarnaulDataLoaderDlg
//---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISBarnaulDataLoaderDlg, wxDialog)
	EVT_UPDATE_UI(wxID_OK, wxGISBarnaulDataLoaderDlg::OnOKUI)
	EVT_BUTTON( wxID_OK, wxGISBarnaulDataLoaderDlg::OnOk )
END_EVENT_TABLE()

wxGISBarnaulDataLoaderDlg::wxGISBarnaulDataLoaderDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    SetIcon(wxIcon(strat_16_xpm));

	this->SetSizeHints( wxSize( 400, 300 ));
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

    //create and fill parameters array
    
    // select input feature class
    wxGISGPParameter *pParamSrcFClass = new wxGISGPParameter(wxT("src_fclass"), _("Set input feature class"), enumGISGPParameterTypeRequired, enumGISGPParamDTPath);
    pParamSrcFClass->SetDirection(enumGISGPParameterDirectionInput);

    wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
    AddAllVectorFilters(pDomain1);
    pParamSrcFClass->SetDomain(pDomain1);

    m_Parameters.Add(pParamSrcFClass);
    
    // select input csv or xls
    wxGISGPParameter *pParamSrcTable = new wxGISGPParameter(wxT("src_table"), _("Set input table"), enumGISGPParameterTypeRequired, enumGISGPParamDTPath);
    pParamSrcTable->SetDirection(enumGISGPParameterDirectionInput);

    wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
    pDomain2->AddFilter(new wxGxTableFilter(enumTableCSV));
    pDomain2->AddFilter(new wxGxTableFilter(enumTableXLSX));
    pParamSrcTable->SetDomain(pDomain2);

    m_Parameters.Add(pParamSrcTable);
    
    // select filed from feature class to join
    wxGISGPParameter *pParamSrcFClassJoinField = new wxGISGPParameter(wxT("src_fclass_jf"), _("Set input feature class field to join"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pParamSrcFClassJoinField->SetDirection(enumGISGPParameterDirectionInput);
    
    m_Parameters.Add(pParamSrcFClassJoinField);
        
    // select filed from csv or xls to join
    wxGISGPParameter *pParamSrcTableJoinField = new wxGISGPParameter(wxT("src_table_jf"), _("Set input table field to join"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pParamSrcTableJoinField->SetDirection(enumGISGPParameterDirectionInput);
    
    m_Parameters.Add(pParamSrcTableJoinField);
    
    // select output name
    wxGISGPParameter *pOutputName = new wxGISGPParameter(wxT("dst_name"), _("Set outpul layer name"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pOutputName->SetDirection(enumGISGPParameterDirectionOutput);
    
    m_Parameters.Add(pOutputName);
    
    //TODO: Coordinates or BBox

    //create gpcontrols
    wxGISDTPath* pInPath1 = new wxGISDTPath(m_Parameters, 0, this);
    bSizer->Add( pInPath1, 0, wxEXPAND, 5 );
    m_paControls.push_back(pInPath1);

    wxGISDTPath* pInPath2 = new wxGISDTPath(m_Parameters, 1, this);
    bSizer->Add( pInPath2, 0, wxEXPAND, 5 );
    m_paControls.push_back(pInPath2);
    
    wxGISDTFieldChoice* pInFldChoice1 = new wxGISDTFieldChoice(m_Parameters, 2, this);
    bSizer->Add( pInFldChoice1, 0, wxEXPAND, 5 );
    m_paControls.push_back(pInFldChoice1);
    
    wxGISDTFieldChoice* pInFldChoice2 = new wxGISDTFieldChoice(m_Parameters, 3, this);
    bSizer->Add( pInFldChoice2, 0, wxEXPAND, 5 );
    m_paControls.push_back(pInFldChoice2);
    
    wxGISDTText* pInText = new wxGISDTText(m_Parameters, 4, this);    
    bSizer->Add( pInText, 0, wxEXPAND, 5 );
    m_paControls.push_back(pInText);
    
    //
    wxStaticLine *pStatLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer->Add( pStatLine, 0, wxEXPAND|wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	wxButton *sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( sdbSizerOK );
	sdbSizerOK->Disable();
	wxButton *sdbSizerCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	m_sdbSizer->AddButton( sdbSizerCancel );
	m_sdbSizer->Realize();
	bSizer->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );
	
	this->SetSizerAndFit( bSizer );
	this->Layout();
	
	this->Centre( wxBOTH );

    //m_pFilter = NULL;
    //m_pDS = NULL;
}

wxGISBarnaulDataLoaderDlg::~wxGISBarnaulDataLoaderDlg()
{
    for(size_t i = 0; i < m_paControls.size(); ++i)
    {
        if(m_paControls[i])
            m_paControls[i]->OnDelete();
    }
    //wsDELETE(m_pDS);
    for(size_t i = 0; i < m_Parameters.size(); ++i)
    {
        wxDELETE(m_Parameters[i]);
    }
}

void wxGISBarnaulDataLoaderDlg::OnOKUI(wxUpdateUIEvent & event)
{
    event.Enable(IsValid());
}

bool wxGISBarnaulDataLoaderDlg::IsValid(void)
{
    /*if(!m_Parameters[1]->GetAltered())
    {
        if(m_Parameters[0]->IsValid())
        {
            //generate temp name
            wxString sPath = m_Parameters[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_calibrate"));
            m_Parameters[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("dst_csv")));
            m_Parameters[1]->SetAltered(true);//??
        }
    }*/

    for(size_t i = 0; i < m_Parameters.GetCount(); ++i)
        if(!m_Parameters[i]->IsValid())
            return false;
    //addition checks
    //check fields of input file
    return true;
}

void wxGISBarnaulDataLoaderDlg::OnOk(wxCommandEvent & event)
{
    wxString sOutputPath = m_Parameters[1]->GetValue().GetString();
    wxString sInputPath = m_Parameters[0]->GetValue().GetString();
    wxGxCatalogBase* pCat = GetGxCatalog();
    if(pCat)
    {
        /*wxGxDataset* pGxDSet = dynamic_cast<wxGxDataset*>(pCat->FindGxObject(sInputPath));
        if(pGxDSet)
            m_pDS = wxDynamicCast(pGxDSet->GetDataset(false), wxGISFeatureDataset);


    	//check overwrite & do it!
        wxGxObject* pGxObj = pCat->FindGxObject(sOutputPath);
	    if(pGxObj && !OverWriteGxObject(pGxObj))
        {
            wxMessageBox(_("Overwrite failed"), _("Error"), wxOK | wxICON_ERROR);
		    return;
        }

	    wxGxObject* pGxDstObject = GetParentGxObjectFromPath(sOutputPath);
        if(!pGxDstObject)
        {
            wxMessageBox(_("No path exist"), _("Error"), wxOK | wxICON_ERROR);
		    return;
        }

        wxFileName sDstFileName(sOutputPath);
        wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_Parameters[1]->GetDomain());
	    m_pFilter = pDomain->GetFilter(m_Parameters[1]->GetSelDomainValue());
        if(m_pFilter && !sDstFileName.HasExt())
        {
            m_soOutPath = CPLFormFilename(pGxDstObject->GetPath(), sDstFileName.GetName().mb_str(wxConvUTF8), m_pFilter->GetExt());
        }
        else
        {
            m_soOutPath = CPLFormFilename(pGxDstObject->GetPath(), sDstFileName.GetFullName().mb_str(wxConvUTF8), "");
        }*/
    }


    if ( IsModal() )
        EndModal(wxID_OK);
    else
    {
        SetReturnCode(wxID_OK);
        this->Show(false);
    }
}
/*
CPLString wxGISBarnaulDataLoaderDlg::GetOutputPath() const
{
    return m_soOutPath;
}

wxGISFeatureDataset *wxGISBarnaulDataLoaderDlg::GetInputDataset() const
{
    return m_pDS;
}

wxGxObjectFilter* wxGISBarnaulDataLoaderDlg::GetFilter() const
{
    return m_pFilter;
}
*/

