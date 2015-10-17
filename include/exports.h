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

#pragma once

#include "wxgis/base.h"

#ifdef WXMAKINGDLL_GIS_BDL
#    define WXDLLIMPEXP_GIS_BDL WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_BDL(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_BDL WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_BDL(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_BDL
#    define WXDLLIMPEXP_DATA_GIS_BDL(type) type
#endif

#if defined(HAVE_VISIBILITY) || (defined(__WINDOWS__) && defined(__GNUC__))
    #define WXDLLIMPEXP_FWD_GIS_BDL
#else
    #define WXDLLIMPEXP_FWD_GIS_BDL	WXDLLIMPEXP_GIS_BDL
#endif

