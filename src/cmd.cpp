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
 
#include "cmd.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxngwconn.h"

#include "dlg.h"

#include "../art/strat_16.xpm"

//-----------------------------------------------------------------------------
// wxGISBarnaulDataloaderCmd
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISBarnaulDataloaderCmd, wxGISCommand)

wxGISBarnaulDataloaderCmd::wxGISBarnaulDataloaderCmd(void) : wxGISCommand()
{

}

wxGISBarnaulDataloaderCmd::~wxGISBarnaulDataloaderCmd(void)
{
}

wxIcon wxGISBarnaulDataloaderCmd::GetBitmap(void)
{
    switch(m_subtype)
	{
	    case 0:
		    if(!m_IconLoadTask.IsOk())
                m_IconLoadTask = wxIcon(strat_16_xpm);
		    return m_IconLoadTask;
		default:    
            return wxNullIcon;
    }
}

wxString wxGISBarnaulDataloaderCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Import spatial data"));
	    default:
		    return wxEmptyString;
	}
}

wxString wxGISBarnaulDataloaderCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
           return wxString(_("Geoprocessing"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISBarnaulDataloaderCmd::GetChecked(void)
{
	return false;
}

bool wxGISBarnaulDataloaderCmd::GetEnabled(void)
{
    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case 0:
		{
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                return pGxObject != NULL && pGxObject->IsKindOf(wxCLASSINFO(wxGxNGWResourceGroup));
            }
			return false;
		}
		
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISBarnaulDataloaderCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0:
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISBarnaulDataloaderCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Import spatial data to NGW"));		
		default:
			return wxEmptyString;
	}
}

void wxGISBarnaulDataloaderCmd::OnClick(void)
{
    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case 0:	
  			{
  			    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
  			    wxGxNGWResourceGroupUI* pResourceGroup = wxDynamicCast(pGxObject, wxGxNGWResourceGroupUI);
  			    if(NULL != pResourceGroup)
  			    {
      			    wxGISBarnaulDataLoaderDlg dlg(pResourceGroup, dynamic_cast<wxWindow*>(m_pApp));
                    dlg.ShowModal();
			    }
			}
			break;		
		
		default:
			return;
	}
}

bool wxGISBarnaulDataloaderCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = dynamic_cast<wxGxApplication*>(pApp);
	return true;
}

wxString wxGISBarnaulDataloaderCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Import spatial data to NGW"));		
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISBarnaulDataloaderCmd::GetCount(void)
{
	return 1;
}


