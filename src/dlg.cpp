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
#include "wxgis/catalogui/processing.h"

#include <wx/statline.h>
#include <set>

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
    pParamSrcFClass->SetSelDomainValue(2); //select mid/mif

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
    
    // select geometry type
    wxGISGPParameter *pParamOutGeomType = new wxGISGPParameter(wxT("src_fclass_gt"), _("Select output geometry type"), enumGISGPParameterTypeRequired, enumGISGPParamDTIntegerChoice);
    pParamOutGeomType->SetDirection(enumGISGPParameterDirectionInput);
    pParamOutGeomType->AddDependency(wxT("src_fclass"));
    
    wxGISGPValueDomain* pDomain3 = new wxGISGPValueDomain();
    pParamOutGeomType->SetDomain(pDomain3);
    
    m_Parameters.Add(pParamOutGeomType);
    pParamOutGeomType->Advise(this);
    
    // check filter out invalid geometry
    wxGISGPParameter *pParamFilter = new wxGISGPParameter(wxT("src_fclass_invalid"), _("Check to filter invalid geometry"), enumGISGPParameterTypeRequired, enumGISGPParamDTBool);
    pParamFilter->SetDirection(enumGISGPParameterDirectionInput);
    pParamFilter->SetValue(false);
    
    m_Parameters.Add(pParamFilter);
    pParamFilter->Advise(this);
    
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
    
    wxGISDTChoice *pGeomTypeChoice = new wxGISDTChoice(m_Parameters, 4, this);    
    bSizer->Add( pGeomTypeChoice, 0, wxEXPAND, 5 );
    m_paControls.push_back(pGeomTypeChoice);
    
    wxGISDTBool *pGeomFilterCheck = new wxGISDTBool(m_Parameters, 5, this);    
    bSizer->Add( pGeomFilterCheck, 0, wxEXPAND, 5 );
    m_paControls.push_back(pGeomFilterCheck); 
   
    wxGISDTText* pInText = new wxGISDTText(m_Parameters, 6, this);    
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
        if(!m_Parameters[6]->GetAltered())
        {
            wxString sPath = event.GetParamValue();
            wxFileName Name(sPath);
            m_Parameters[6]->SetHasBeenValidated(false);   
            m_Parameters[6]->SetValue(wxVariant(Name.GetName(), wxT("dst_name")));
        }    
        
        //if(!m_Parameters[4]->GetAltered()) not check altered
        {
            wxString sPath = event.GetParamValue();
            wxGxCatalogBase* pCat = GetGxCatalog();
            wxGxDataset* pGxDSet = dynamic_cast<wxGxDataset*>(pCat->FindGxObject(sPath));
            if(pGxDSet)
            {
                wxGISFeatureDataset *pFeatureDataset = wxDynamicCast(pGxDSet->GetDataset(true), wxGISFeatureDataset);
                if(pFeatureDataset)
                {     
					wxGISPointerHolder holder(pFeatureDataset);             
                    if (!pFeatureDataset->IsOpened())
		            {
                        if (!pFeatureDataset->Open(0, false, true, true))
                        {
                            wsDELETE(pFeatureDataset);
                            return;
                        }
                    }
                    
                    // for cached datasources 
                    if (!pFeatureDataset->IsCached())
                    {
                        pFeatureDataset->Cache();
                    }
            
                    wxGISFeature Feature;
                    std::set<OGRwkbGeometryType> geomTypes;
                    
                    int adFeatureCount[10] = {0};
                    
                    pFeatureDataset->Reset();
                    while ((Feature = pFeatureDataset->Next()).IsOk())
		            {
		                wxGISGeometry geom = Feature.GetGeometry();
		                if(geom.IsOk())
		                {
		                    OGRwkbGeometryType eGeomType = geom.GetType();
		                    if(eGeomType < 1 && eGeomType > 6)
		                        continue;
		                        
		                    if(eGeomType > 3) // show not multi
		                        eGeomType = (OGRwkbGeometryType)(eGeomType - 3);
		                    geomTypes.insert(eGeomType);
		                    adFeatureCount[eGeomType]++;
		                }
		                
                    }    
                    
                    wxGISGPValueDomain* pDomain = m_Parameters[4]->GetDomain();
                    pDomain->Clear();
                    m_Parameters[4]->SetHasBeenValidated(false);   
                    //m_Parameters[4]->SetValid(true);                  
                       
                    for (std::set<OGRwkbGeometryType>::const_iterator it = geomTypes.begin(); it != geomTypes.end(); ++it)
                    {
                        OGRwkbGeometryType eGeomType = *it;
                        wxString sDomainName = wxString::Format(wxT("%s (%d)"), wxGetTranslation(OGRGeometryTypeToName(eGeomType)).c_str(), adFeatureCount[eGeomType]);
                        pDomain->AddValue((long)eGeomType, sDomainName);   
                    }      
                }
            }
        }       
    }

	    
    for (size_t i = 0; i < m_paControls.size(); ++i)
    {
        if (m_paControls[i])
        {
            m_paControls[i]->OnParamChanged(event);
        }
    }
    			
	if(event.GetId() ==  0)
	{
		//if(!m_Parameters[4]->GetAltered())
		{
			wxGISGPValueDomain *pDomain = m_Parameters[4]->GetDomain();
			if(NULL != pDomain)
			{
				int nPos = pDomain->GetPosByValue((long)m_dDefaultGeomType);
				m_Parameters[4]->SetSelDomainValue(nPos);
			}
		}
		
		//if(!m_Parameters[2]->GetAltered())
		{
			wxGISGPValueDomain *pDomain = m_Parameters[2]->GetDomain();
			if(NULL != pDomain)
			{
				int nPos = pDomain->GetPosByName(m_sMiFieldName);
				m_Parameters[2]->SetSelDomainValue(nPos);
			}
		}		
	}		
	
	if(event.GetId() ==  1)
	{
		//if(!m_Parameters[3]->GetAltered())
		{
			wxGISGPValueDomain *pDomain = m_Parameters[3]->GetDomain();
			if(NULL != pDomain)
			{
				int nPos = pDomain->GetPosByName(m_sCSVFieldName);
				m_Parameters[3]->SetSelDomainValue(nPos);
			}
		}
	}
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
    SerializeValues(true);

    wxString sInputFCPath = m_Parameters[0]->GetValue().GetString();
    wxString sInputTabPath = m_Parameters[1]->GetValue().GetString();
	int nPos = m_Parameters[2]->GetSelDomainValue();
	wxString sInputFCPathFieldName = m_Parameters[2]->GetDomain()->GetName(nPos);
	nPos = m_Parameters[3]->GetSelDomainValue();	
    wxString sInputTabPathFieldName = m_Parameters[3]->GetDomain()->GetName(nPos);
    OGRwkbGeometryType eGeomType = (OGRwkbGeometryType)(m_Parameters[4]->GetValue().GetLong() + 3);
    bool bFilterIvalidGeometry = m_Parameters[5]->GetValue().GetBool();
    wxString sOutputName = m_Parameters[6]->GetValue().GetString();
    wxGxCatalogBase* pCat = GetGxCatalog();
    if(pCat)
    {
        wxGISProgressDlg ProgressDlg(_("Form join feature dataset"), _("Begin operation..."), 100, this);
        
        wxGxDataset* pGxDSet = dynamic_cast<wxGxDataset*>(pCat->FindGxObject(sInputFCPath));
        if(!pGxDSet)
        {
            wxString sErr = wxString::Format(_("Failed to get feature dataset %s"), sInputFCPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }
        
        wxGISFeatureDataset *pFeatureDataset = wxDynamicCast(pGxDSet->GetDataset(true, &ProgressDlg), wxGISFeatureDataset);
        if(!pFeatureDataset)
        {
            wxString sErr = wxString::Format(_("Failed to get feature dataset %s"), sInputFCPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }
        
        if (!pFeatureDataset->IsOpened())
		{
            if (!pFeatureDataset->Open(0, false, true, true))
            {
                wsDELETE(pFeatureDataset);
                return;
            }
        }
		pFeatureDataset->SetEncoding(wxFONTENCODING_CP1251);
        
        // for cached datasources 
        if (!pFeatureDataset->IsCached())
        {
            pFeatureDataset->Cache(&ProgressDlg);
        }
        
               
        wxGISSpatialReference SpaRef;// = pFeatureDataset->GetSpatialReference();
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
        
        wxGISTable *pTable = wxDynamicCast(pGxTable->GetDataset(true, &ProgressDlg), wxGISTable);
        if(!pTable)
        {
            wxString sErr = wxString::Format(_("Failed to get table %s"), sInputTabPath.c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
            return;
        }  
        
        if (!pTable->IsOpened())
		{
            if (!pTable->Open(0, false, true, true))
            {
                wsDELETE(pTable);
                return;
            }
        } 
		
        // set UTF-8 but may be should be encoding list?
        pTable->SetEncoding(wxFONTENCODING_UTF8);
        
        // for cached datasources 
        if (!pTable->IsCached())
        {
            pTable->Cache(&ProgressDlg);
        }
        
    	// create temp memory dataset ready to upload to the NGW
    	
    	OGRCompatibleDriver* poMEMDrv = GetOGRCompatibleDriverByName("Memory");
        if (poMEMDrv == NULL)
        {
            CPLError(CE_Failure, CPLE_AppDefined, "Cannot load 'Memory' driver");
            return;
        }
        
        //OGRwkbGeometryType eGeomType = GetGeometryType(pFeatureDataset); 

        OGRCompatibleDataSource* poOutDS = poMEMDrv->CreateOGRCompatibleDataSource("OutDS", NULL);
        OGRLayer* poOutLayer = poOutDS->CreateLayer("output", SpaRef, eGeomType, NULL);
        
        // create fields based on two datasets
        wxArrayString saFieldNames;
        OGRFeatureDefn* poFields = pFeatureDataset->GetDefinition();
        int nFirstPartFieldsCount = poFields->GetFieldCount();
        for (size_t i = 0; i < nFirstPartFieldsCount; ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            OGRFieldDefn oFieldDefn(pField);
			wxString sFieldName(pField->GetNameRef(), wxCSConv(pFeatureDataset->GetEncoding()));
            oFieldDefn.SetName(Transliterate(sFieldName));
            if(saFieldNames.Index(wxString::FromUTF8(oFieldDefn.GetNameRef())) != wxNOT_FOUND)
            {
                wxString sAppend = wxString::Format(wxT("%.2d"), i + 1);
                wxString sNewFieldName = wxString::FromUTF8(oFieldDefn.GetNameRef()) + sAppend;
                oFieldDefn.SetName(sNewFieldName.ToUTF8());
            }            
                
            poOutLayer->CreateField(&oFieldDefn); 
            
            saFieldNames.Add(wxString::FromUTF8(oFieldDefn.GetNameRef()));               
        }
        
        poFields = pTable->GetDefinition();
        int nSecondPartFieldsCount = poFields->GetFieldCount();
        for (size_t i = 0; i < nSecondPartFieldsCount; ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            OGRFieldDefn oFieldDefn(pField);
            oFieldDefn.SetName(Transliterate(pField->GetNameRef()));
            if(saFieldNames.Index(wxString::FromUTF8(oFieldDefn.GetNameRef())) != wxNOT_FOUND)
            {
                wxString sAppend = wxString::Format(wxT("%.2d"), i + 1);
                wxString sNewFieldName = wxString::FromUTF8(oFieldDefn.GetNameRef()) + sAppend;
                oFieldDefn.SetName(sNewFieldName.ToUTF8());
            }            
                
            poOutLayer->CreateField(&oFieldDefn); 
            
            saFieldNames.Add(wxString::FromUTF8(oFieldDefn.GetNameRef()));               
        }
        
        // add style layer 
        OGRFieldDefn oStyleDefn("OGR_STYLE", OFTString);
        oStyleDefn.SetWidth(255);
        poOutLayer->CreateField(&oStyleDefn);

        wxGISFeatureDataset* pGISFeatureDataset = new wxGISFeatureDataset("", enumVecMem, poOutLayer, poOutDS);
        pGISFeatureDataset->SetEncoding(wxFONTENCODING_UTF8);
        wxGISPointerHolder holder(pGISFeatureDataset);

        int nCounter(0);   
        ProgressDlg.SetRange(pFeatureDataset->GetFeatureCount(true));     
        ProgressDlg.PutMessage(wxString::Format(_("Force geometry field to %s"), OGRGeometryTypeToName(eGeomType)), wxNOT_FOUND, enumGISMessageWarning);
        
		ProgressDlg.ShowProgress(true);

        OGRFeatureDefn* pDefn = pGISFeatureDataset->GetDefinition();
		
		wxGISFeature Feature;
		pFeatureDataset->Reset();
		while ((Feature = pFeatureDataset->Next()).IsOk())
		{    
			ProgressDlg.SetValue(nCounter++);
			if(!ProgressDlg.Continue())
			{
				return;
			}
			
			//join and fill values to new feature
			
			
			wxGISGeometry Geom = Feature.GetGeometry();
			if(!Geom.IsOk())
			{
			    //ProgressDlg.PutMessage(wxString::Format(_("Skip %ld feature"), Feature.GetFID()), wxNOT_FOUND, enumGISMessageWarning);
			    continue;
			}
			OGRwkbGeometryType eFeatureGeomType = Geom.GetType();
			
			if(eFeatureGeomType != eGeomType && eFeatureGeomType + 3 != eGeomType)
			{
			    //ProgressDlg.PutMessage(wxString::Format(_("Skip %ld feature"), Feature.GetFID()), wxNOT_FOUND, enumGISMessageWarning);
			    continue;
			}			    
			
			OGRGeometry *pNewGeom = NULL;
			if (eFeatureGeomType != eGeomType)
            {
                switch (eGeomType)
                {
                case wkbLineString:
                    pNewGeom = OGRGeometryFactory::forceToLineString(Geom.Copy());
                    break;
                case wkbPolygon:
                    pNewGeom = OGRGeometryFactory::forceToPolygon(Geom.Copy());
                    break;
                case wkbMultiPoint:
                    pNewGeom = OGRGeometryFactory::forceToMultiPoint(Geom.Copy());
                    break;
                case wkbMultiLineString:
                    pNewGeom = OGRGeometryFactory::forceToMultiLineString(Geom.Copy());
                    break;
                case wkbMultiPolygon:
                    pNewGeom = OGRGeometryFactory::forceToMultiPolygon(Geom.Copy());
                    break;
                case wkbPoint:
                default:
                    pNewGeom = Geom.Copy();
                    break;
                };

            }
            else
            {
                pNewGeom = Geom.Copy();
            }
			            
            wxGISFeature newFeature = pGISFeatureDataset->CreateFeature();
            
            // set geometry
            newFeature.SetGeometryDirectly(wxGISGeometry(pNewGeom, false));
            
            // set fields from feature class
            for(int i = 0; i < nFirstPartFieldsCount; ++i)
            {
                OGRFieldDefn* pField = pDefn->GetFieldDefn(i);
                SetField(newFeature, i, Feature, i, pField->GetType());
                //newFeature.SetField(i, Feature.GetRawField(i));    
            }
            
            // set style
            newFeature.SetField("OGR_STYLE", Feature.GetStyleString());
            //wxLogError("GetStyleString %s", Feature.GetStyleString().c_str())
            
            // set fields from table if any exist
            wxString sFieldValue = Feature.GetFieldAsString(sInputFCPathFieldName);
            wxGISFeature Row = FindRow(sInputTabPathFieldName, sFieldValue, pTable);
            if(Row.IsOk())
            {
                for(int i = 0; i < nSecondPartFieldsCount; ++i)
                {
                    OGRFieldDefn* pField = pDefn->GetFieldDefn(i + nFirstPartFieldsCount);
                    SetField(newFeature, i + nFirstPartFieldsCount, Row, i, pField->GetType());
                    //newFeature.SetField(i + nFirstPartFieldsCount, Row.GetRawField(i)); 
                }
            }            
            
            if( pGISFeatureDataset->StoreFeature( newFeature ) != OGRERR_NONE )
            {
		        const char* err = CPLGetLastErrorMsg();
                wxString sErr(err, wxConvUTF8);
                ProgressDlg.PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
	        }
		}
		
		// delete if exist layer with same name on server
		DeleteExistLayer(sOutputName);
		
		wxString sStyle(wxT("<map><layer><styleitem>OGR_STYLE</styleitem><class></class></layer></map>"));
		// upload to server
		if(!m_pResourceGroup->CreateVectorLayer(sOutputName, pGISFeatureDataset, wkbUnknown, sStyle, bFilterIvalidGeometry, &ProgressDlg))
		{
		    ProgressDlg.PutMessage(wxString(_("CreateVectorLayer failed")), wxNOT_FOUND, enumGISMessageError);
		    ShowMessageDialog(this, ProgressDlg.GetWarnings());
		    return;
		}
		
		ShowMessageDialog(this, ProgressDlg.GetWarnings());
		
		m_pResourceGroup->OnGetUpdates();
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

void wxGISBarnaulDataLoaderDlg::SetField(wxGISFeature& feature, int newIndex, const wxGISFeature &row, int index, OGRFieldType eType)
{
    switch (eType)
    {
    case OFTRealList:
        feature.SetField(newIndex, row.GetFieldAsDoubleList(index));
        break;
    case OFTIntegerList:
        feature.SetField(newIndex, row.GetFieldAsIntegerList(index));
        break;
    case OFTStringList:
        feature.SetField(newIndex, row.GetFieldAsStringList(index));
        break;
    case OFTDate:
    case OFTTime:
    case OFTDateTime:
    case OFTReal:
    case OFTInteger:
        feature.SetField(newIndex, row.GetRawField(index));
        break;
    case OFTString:
#ifdef CPL_RECODE_ICONV

#else
        if (bFastConv)
        {
            const char* pszStr = row.GetFieldAsChar(index);
            if (oEncConverter.Convert(pszStr, szMaxStr))
            {
                feature.SetField(newIndex, szMaxStr);
                break;
            }
        }
#endif //CPL_RECODE_ICONV
    default:
        feature.SetField(newIndex, row.GetFieldAsString(index));
        break;
    };
}

OGRwkbGeometryType wxGISBarnaulDataLoaderDlg::GetGeometryType(wxGISFeatureDataset * const pDSet)
{    
    OGRwkbGeometryType eType = pDSet->GetGeometryType();    
    OGRwkbGeometryType e2DType = wkbFlatten(eType);    
    if(e2DType > 0 && e2DType < 7)
    {
        if(e2DType < 4)
            return (OGRwkbGeometryType)(e2DType + 3); // to multi
        return e2DType;
    }

    
    wxGISFeature Feature;
    pDSet->Reset();
	while ((Feature = pDSet->Next()).IsOk())
	{	    
	    wxGISGeometry Geom = Feature.GetGeometry();
		if(Geom.IsOk())
		{
		    e2DType = wkbFlatten(Geom.GetType());
		    if(e2DType > 0 && e2DType < 7)
            {
                if(e2DType < 4)
                    e2DType = (OGRwkbGeometryType)(e2DType + 3); // to multi
                break;
            }
		}	
    }
    
    return e2DType;
}

void wxGISBarnaulDataLoaderDlg::DeleteExistLayer(const wxString& sLayerName)
{
    wxGxObjectList list = m_pResourceGroup->GetChildren();
    
    wxGxObjectList::const_iterator iter;
    for(iter = list.begin(); iter != list.end(); ++iter)
    {
        wxGxObject *current = *iter;
        IGxObjectEdit *pEdit = dynamic_cast<IGxObjectEdit*>(current);
        if(pEdit)
        {
            if(current->GetName().IsSameAs(sLayerName))  
            {
                pEdit->Delete();
                return;
            }  
        }
    }
}

wxGISFeature wxGISBarnaulDataLoaderDlg::FindRow(const wxString &sFieldName, 
                                                const wxString &sFieldValue, 
                                                wxGISTable * const pTable)
{
    wxGISFeature Feature;
    pTable->Reset();
	while ((Feature = pTable->Next()).IsOk())
	{
	    wxString sTestValue = Feature.GetFieldAsString(sFieldName);
	    if(sTestValue.IsSameAs(sFieldValue))
		{
	        break;
		}
    }	
    
    return Feature;
}

void wxGISBarnaulDataLoaderDlg::SerializeFramePos(bool bSave)
{
    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
        return;
		
	wxString sAppName = GetApplication()->GetAppName();	
	int x=0, y=0, w=400, h=300;    
            
    if (bSave)
    {
        GetPosition(&x, &y);
        GetClientSize(&w, &h);
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
        oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/first_run")), false);
    }
    else
    {
        if (oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/first_run")), true))
            return;

        //load
        bool bMaxi = oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/maxi")), false);
        if (!bMaxi)
        {
            x = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/xpos")), x);
            y = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/ypos")), y);
            w = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/width")), w);
            h = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/frame/height")), h);
            Move(x, y);
            SetClientSize(w, h);
        }
        else
        {
            Maximize();
        }

        SerializeValues(bSave);
    }
}

void wxGISBarnaulDataLoaderDlg::SerializeValues(bool bSave)
{
    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
        return;
    wxString sAppName = GetApplication()->GetAppName();

    if (bSave)
    {
        // save parameters
        wxGISGPValueDomain* pDomain = m_Parameters[2]->GetDomain();
        wxString sInputFCPathFieldName;
        if (NULL != pDomain)
        {
            int nPos = m_Parameters[2]->GetSelDomainValue();
            sInputFCPathFieldName = pDomain->GetName(nPos);
        }

        pDomain = m_Parameters[3]->GetDomain();
        wxString sInputTabPathFieldName;
        if (NULL != pDomain)
        {
            int nPos = m_Parameters[3]->GetSelDomainValue();
            sInputTabPathFieldName = pDomain->GetName(nPos);
        }

        OGRwkbGeometryType eGeomType = (OGRwkbGeometryType)(m_Parameters[4]->GetValue().GetLong());
        if (eGeomType > 3) // show not multi
            eGeomType = (OGRwkbGeometryType)(eGeomType - 3);
        oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/geom_type")), (int)eGeomType);
        oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/mi_fld_name")), sInputFCPathFieldName);
        oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/csv_fld_name")), sInputTabPathFieldName);
    }
    else
    {
        m_dDefaultGeomType = (OGRwkbGeometryType)oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/geom_type")), (int)wkbPolygon);
        m_sMiFieldName = oConfig.Read(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/mi_fld_name")), wxEmptyString);
        m_sCSVFieldName = oConfig.Read(enumGISHKCU, sAppName + wxString(wxT("/barnaul_dataloader/data/csv_fld_name")), wxEmptyString);
    }
}