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
#include "wxgis/core/format.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/table.h"

#include <wx/statline.h>

#include "../art/strat_16.xpm"

//---------------------------------------------------------------------------
// wxGISBarnaulDataLoaderDlg
//---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISBarnaulDataLoaderDlg, wxDialog)
	EVT_UPDATE_UI(wxID_OK, wxGISBarnaulDataLoaderDlg::OnOKUI)
	EVT_BUTTON( wxID_OK, wxGISBarnaulDataLoaderDlg::OnOk )
	EVT_GPPARAM_CHANGED( wxGISBarnaulDataLoaderDlg::OnParamChanged )
END_EVENT_TABLE()

wxGISBarnaulDataLoaderDlg::wxGISBarnaulDataLoaderDlg( wxGxNGWResourceGroupUI *pResourceGroup, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    m_pResourceGroup = pResourceGroup;
    SetIcon(wxIcon(strat_16_xpm));

	//this->SetSizeHints( wxSize( 400, 300 ));
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

    //create and fill parameters array
    
    // select input feature class
    wxGISGPParameter *pParamSrcFClass = new wxGISGPParameter(wxT("src_fclass"), _("Set input feature class"), enumGISGPParameterTypeRequired, enumGISGPParamDTPath);
    pParamSrcFClass->SetDirection(enumGISGPParameterDirectionInput);

    wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
    AddAllVectorFileFilters(pDomain1);
    pParamSrcFClass->SetDomain(pDomain1);

    m_Parameters.Add(pParamSrcFClass);
    pParamSrcFClass->Advise(this);
    
    // select input csv or xls
    wxGISGPParameter *pParamSrcTable = new wxGISGPParameter(wxT("src_table"), _("Set input table"), enumGISGPParameterTypeRequired, enumGISGPParamDTPath);
    pParamSrcTable->SetDirection(enumGISGPParameterDirectionInput);

    wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
    pDomain2->AddFilter(new wxGxTableFilter(enumTableCSV));
    pDomain2->AddFilter(new wxGxTableFilter(enumTableXLSX));
    pParamSrcTable->SetDomain(pDomain2);

    m_Parameters.Add(pParamSrcTable);
    pParamSrcTable->Advise(this);
    
    // select filed from feature class to join
    wxGISGPParameter *pParamSrcFClassJoinField = new wxGISGPParameter(wxT("src_fclass_jf"), _("Set input feature class field to join"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pParamSrcFClassJoinField->SetDirection(enumGISGPParameterDirectionInput);
    pParamSrcFClassJoinField->AddDependency(wxT("src_fclass"));
    
    m_Parameters.Add(pParamSrcFClassJoinField);
    pParamSrcFClassJoinField->Advise(this);
        
    // select filed from csv or xls to join
    wxGISGPParameter *pParamSrcTableJoinField = new wxGISGPParameter(wxT("src_table_jf"), _("Set input table field to join"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pParamSrcTableJoinField->SetDirection(enumGISGPParameterDirectionInput);
    pParamSrcTableJoinField->AddDependency(wxT("src_table"));
    
    m_Parameters.Add(pParamSrcTableJoinField);
    pParamSrcTableJoinField->Advise(this);
    
    // select output name
    wxGISGPParameter *pOutputName = new wxGISGPParameter(wxT("dst_name"), _("Set outpul layer name"), enumGISGPParameterTypeRequired, enumGISGPParamDTFieldAnyChoice);
    pOutputName->SetDirection(enumGISGPParameterDirectionOutput);
    
    m_Parameters.Add(pOutputName);
    pOutputName->Advise(this);
    
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

    //m_pDS = NULL;
    
    SerializeFramePos(false);
}

wxGISBarnaulDataLoaderDlg::~wxGISBarnaulDataLoaderDlg()
{
    SerializeFramePos(true);
    
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

void wxGISBarnaulDataLoaderDlg::OnParamChanged(wxGISGPParamEvent& event)
{
    if(event.GetId() == 0)
    {
        if(!m_Parameters[4]->GetAltered())
        {
            wxString sPath = event.GetParamValue();
            wxFileName Name(sPath);
            m_Parameters[4]->SetValue(wxVariant(Name.GetName(), wxT("dst_name")));
            m_Parameters[4]->SetAltered(true);
        }       
    }
    
    for(size_t i = 0; i < m_paControls.size(); ++i)
        if(m_paControls[i])
            m_paControls[i]->OnParamChanged(event);
}


bool wxGISBarnaulDataLoaderDlg::IsValid(void)
{
    for(size_t i = 0; i < m_Parameters.GetCount(); ++i)
        if(!m_Parameters[i]->IsValid())
            return false;
    //addition checks
    //check fields of input file
    return true;
}

void wxGISBarnaulDataLoaderDlg::OnOk(wxCommandEvent & event)
{
    wxString sInputFCPath = m_Parameters[0]->GetValue().GetString();
    wxString sInputTabPath = m_Parameters[1]->GetValue().GetString();
    wxString sInputFCPathFieldName = m_Parameters[2]->GetValue().GetString();
    wxString sInputTabPathFieldName = m_Parameters[3]->GetValue().GetString();
    wxString sOutputName = m_Parameters[4]->GetValue().GetString();
    wxGxCatalogBase* pCat = GetGxCatalog();
    if(pCat)
    {
        wxGxDataset* pGxDSet = dynamic_cast<wxGxDataset*>(pCat->FindGxObject(sInputFCPath));
        if(!pGxDSet)
        {
            wxString sErr = wxString::Format(_("Failed to get feature dataset %s"), sInputFCPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }
        
        wxGISFeatureDataset *pFeatureDataset = wxDynamicCast(pGxDSet->GetDataset(false), wxGISFeatureDataset);
        if(!pFeatureDataset)
        {
            wxString sErr = wxString::Format(_("Failed to get feature dataset %s"), sInputFCPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }   
        
        wxGISSpatialReference SpaRef = pFeatureDataset->GetSpatialReference();
        if(!SpaRef.IsOk())
        {
            SpaRef = wxGISSpatialReference(wxT("PROJCS[\"unnamed\",GEOGCS[\"Krassovsky, 1942\",DATUM[\"unknown\",SPHEROID[\"krass\",6378245,298.3],TOWGS84[23.92,-141.27,-80.9,-0,0.35,0.82,-0.12]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Hotine_Oblique_Mercator_Azimuth_Center\"],PARAMETER[\"latitude_of_center\",53.3090998192],PARAMETER[\"longitude_of_center\",82.466678914],PARAMETER[\"azimuth\",-1.2302179328],PARAMETER[\"rectified_grid_angle\",0],PARAMETER[\"scale_factor\",0.9999265173],PARAMETER[\"false_easting\",-77391.44014],PARAMETER[\"false_northing\",10469.46443],UNIT[\"Meter\",1]]"));
        }
        
        wxGxDataset* pGxTable = dynamic_cast<wxGxDataset*>(pCat->FindGxObject(sInputTabPath));
        if(!pGxTable)
        {
            wxString sErr = wxString::Format(_("Failed to get table %s"), sInputTabPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }
        
        wxGISTable *pTable = wxDynamicCast(pGxTable->GetDataset(false), wxGISTable);
        if(!pTable)
        {
            wxString sErr = wxString::Format(_("Failed to get table %s"), sInputTabPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }   
        
    	// create temp memory dataset ready to upload to the NGW
    	
    	OGRCompatibleDriver* poMEMDrv = GetOGRCompatibleDriverByName("Memory");
        if (poMEMDrv == NULL)
        {
            CPLError(CE_Failure, CPLE_AppDefined, "Cannot load 'Memory' driver");
            return;
        }

        OGRCompatibleDataSource* poOutDS = poMEMDrv->CreateOGRCompatibleDataSource("OutDS", NULL);
        OGRLayer* poOutLayer = poOutDS->CreateLayer("output", SpaRef, pFeatureDataset->GetGeometryType(), NULL);
        
        // create fields based on two datasets
        wxArrayString saFieldNames;
        OGRFeatureDefn* poFields = pFeatureDataset->GetDefinition();
        int nFirstPartFieldsCount = poFields->GetFieldCount();
        for (size_t i = 0; i < nFirstPartFieldsCount; ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            OGRFieldDefn oFieldDefn(pField);
            oFieldDefn.SetName(Transliterate(pField->GetNameRef()));
            if(saFieldNames.Index(wxString::FromUTF8(oFieldDefn.GetName()) != wxNOT_FOUND)
            {
                wxString sAppend = wxString::Format(wxT("%.2d"), i + 1);
                wxString sNewFieldName = wxString::FromUTF8(oFieldDefn.GetName()) + sAppend;
                oFieldDefn.SetName(sNewFieldName.ToUTF8());
            }            
                
            poOutLayer->CreateField(&oFieldDefn); 
            
            saFieldNames.Add(wxString::FromUTF8(oFieldDefn.GetName()));               
        }
        
        poFields = pTable->GetDefinition();
        for (size_t i = 0; i < poFields->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            OGRFieldDefn oFieldDefn(pField);
            oFieldDefn.SetName(Transliterate(pField->GetNameRef()));
            if(saFieldNames.Index(wxString::FromUTF8(oFieldDefn.GetName()) != wxNOT_FOUND)
            {
                wxString sAppend = wxString::Format(wxT("%.2d"), i + 1);
                wxString sNewFieldName = wxString::FromUTF8(oFieldDefn.GetName()) + sAppend;
                oFieldDefn.SetName(sNewFieldName.ToUTF8());
            }            
                
            poOutLayer->CreateField(&oFieldDefn); 
            
            saFieldNames.Add(wxString::FromUTF8(oFieldDefn.GetName()));               
        }
        
        // add style layer 

        wxGISFeatureDataset* pGISFeatureDataset = new wxGISFeatureDataset("", enumVecMem, poOutLayer, poOutDS);

        int nCounter(0);
        wxGISProgressDlg ProgressDlg(_("Form join feature dataset"), _("Begin operation..."), pFeatureDataset->GetFeatureCount(true), this);
		ProgressDlg.ShowProgress(true);
		
		wxGISFeature Feature;
		while ((Feature = pFeatureDataset->Next()).IsOk())
		{    
			ProgressDlg.SetValue(nCounter++);
			if(!ProgressDlg.Continue())
			{
				return;
			}
			
			//join and fill values to new feature
			
			/*wxGISFeature Feature = pGISFeatureDataset->CreateFeature();
            Feature.SetFID((*iter)->GetId());
            
            if( pGISFeatureDataset->StoreFeature( Feature ) != OGRERR_NONE )
            {
		        const char* err = CPLGetLastErrorMsg();
                wxString sErr(err, wxConvUTF8);
                wxLogError(sErr);
	        }*/
		}
    }


    if ( IsModal() )
    {
        EndModal(wxID_OK);
    }
    else
    {
        SetReturnCode(wxID_OK);
        this->Show(false);
    }
}

void wxGISBarnaulDataLoaderDlg::SerializeFramePos(bool bSave)
{
    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
        return;
		
	wxString sAppName = GetApplication()->GetAppName();	
	int x, y, w, h;
    GetClientSize(&w, &h);
    GetPosition(&x, &y);
            
    if (bSave)
    {
        if (IsMaximized())
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/maxi")), true);
        else
        {
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/maxi")), false);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/width")), w);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/height")), h);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/xpos")), x);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/ypos")), y);
        }
    }
    else
    {
        //load
        bool bMaxi = oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/maxi")), false);
        if (!bMaxi)
        {
            int x = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/xpos")), x);
            int y = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/ypos")), y);
            int w = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/width")), w);
            int h = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/height")), h);
            Move(x, y);
            SetClientSize(w, h);
        }
        else
        {
            Maximize();
        }
    }
}


/*

wxGISFeatureDataset *wxGISBarnaulDataLoaderDlg::GetInputDataset() const
{
    return m_pDS;
}

*/

