/******************************************************************************
 * Project:  ngm plugin
 * Purpose:  version functions
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
 ****************************************************************************/
#pragma once

#include "wx/version.h"

/*  NB: this file is parsed by automatic tools so don't change its format! */
#define MAJOR_VERSION      0
#define MINOR_VERSION      1
#define RELEASE_NUMBER     0
#define SUBRELEASE_NUMBER  0

/*  these are used by version.rc and should always be ASCII, not Unicode */
#define VERSION_NUM_STRING \
  wxMAKE_VERSION_STRING(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)
#define VERSION_NUM_DOT_STRING \
  wxMAKE_VERSION_DOT_STRING(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)

/* those are Unicode-friendly */
#define VERSION_NUM_STRING_T \
  wxMAKE_VERSION_STRING_T(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)
#define VERSION_NUM_DOT_STRING_T \
  wxMAKE_VERSION_DOT_STRING_T(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)

/*  check if the current version is at least major.minor.release */
#define CHECK_VERSION(major,minor,release) \
    (MAJOR_VERSION > (major) || \
    (MAJOR_VERSION == (major) && MINOR_VERSION > (minor)) || \
    (MAJOR_VERSION == (major) && MINOR_VERSION == (minor) && RELEASE_NUMBER >= (release)))

/* the same but check the subrelease also */
#define CHECK_VERSION_FULL(major,minor,release,subrel) \
    (wxCHECK_VERSION(major, minor, release) && \
        ((major) != MAJOR_VERSION || \
            (minor) != MINOR_VERSION || \
                (release) != RELEASE_NUMBER || \
                    (subrel) <= SUBRELEASE_NUMBER))


