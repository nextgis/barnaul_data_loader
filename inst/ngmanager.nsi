; NextGIS Manager
;
; ngmanager.nsi - NSIS Script for NSIS 2.46
; Requires the ExecDos plugin (http://nsis.sourceforge.net/wiki/ExecDos)
;
; Copyright (c) 2008,2014 Bishop
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

; Use better compression
SetCompressor lzma

; Sections
!include "Sections.nsh"

; Modern User Interface
!include "MUI2.nsh"

;Enclude MUI_EXTRAPAGES header
; !include "MUI_EXTRAPAGES.nsh"

;!include "InstallOptions.nsh"
;!include "WordFunc.nsh"

;!insertmacro WordFind

;ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"

!define VERSION "0.6.8"
!define PRODUCT "NextGIS Manager ${VERSION}"
!define BUILD_PATH "D:/work/projects/wxGIS/build32"
!define SOURCE_PATH "D:/work/projects/wxGIS"
!define M_SOURCE_PATH "D:/work/projects/barnaul_data_loader"
!define REG_KEY "nextgis/manager"

; Use the new WIN32DIST build directory
!define PROJ_DIR "D:/work/projects/"
!define BIT "32"

; Place all temporary files used by the installer in their own subdirectory
; under $TEMP (makes the files easier to find)
;!define TEMPDIR "$TEMP\ngmanager_installer"

; --------------------------------
; Configuration

	; General
	Name "${PRODUCT}"
	OutFile "ngm-${VERSION}-setup.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\nextgis\manager"
	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\ngmanager" ""
	
	;Request application privileges for Windows Vista
	RequestExecutionLevel highest
	  
	SetDateSave on
	SetDatablockOptimize on
	CRCCheck on
	XPStyle on
	BrandingText "${PRODUCT}"
	SetOverwrite ifnewer
; --------------------------------
	AutoCloseWindow false
	ShowInstDetails show

; --------------------------------
; Interface Settings

	!define MUI_ABORTWARNING
	!define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\colorful.bmp"
	!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP "${SOURCE_PATH}\art\logo.bmp"
	!define MUI_HEADERIMAGE_UNBITMAP "${SOURCE_PATH}\art\logo.bmp"
	!define MUI_COMPONENTSPAGE_SMALLDESC
  
; --------------------------------
; Install Pages

	!insertmacro MUI_PAGE_WELCOME

	!insertmacro MUI_PAGE_LICENSE $(myLicenseData)	
	
; 	!insertmacro MUI_PAGE_README $(myVerData)
	
	!insertmacro MUI_PAGE_DIRECTORY

	!insertmacro MUI_PAGE_INSTFILES
  
	!insertmacro MUI_PAGE_FINISH 
; --------------------------------
; Uninstall Pages

	!insertmacro MUI_UNPAGE_CONFIRM
  
	!insertmacro MUI_UNPAGE_INSTFILES  

; --------------------------------	

; Languages
 
	!insertmacro MUI_LANGUAGE "Russian"
	!insertmacro MUI_LANGUAGE "English"
	
; --------------------------------
  ;Set up install lang strings for 1st lang
  ; ${ReadmeLanguage} "${LANG_ENGLISH}" \
          ; "Read Me" \
          ; "Please review the following important information." \
          ; "About $(^name):" \
          ; "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
  ; ;Set up uninstall lang strings for 1st lang
  ; ${Un.ReadmeLanguage} "${LANG_ENGLISH}" \
          ; "Change log" \
          ; "Please review the following important Uninstall information." \
          ; "About $(^name) Uninstall:" \
          ; "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
  ; ;set up install lang strings for second lang
  ; ${ReadmeLanguage} "${LANG_RUSSIAN}" \
          ; "Список изменений" \
          ; "Прочитайте внимательно следующую информацию." \
          ; "О $(^name):" \
          ; "$\n  Для просмотра всего текста используйте стрелку 'вниз' или клавишу 'Page Down'."
 
  ; ;set up uninstall lang strings for second lang
  ; ${Un.ReadmeLanguage} "${LANG_RUSSIAN}" \
          ; "Список изменений" \
          ; "Прочитайте внимательно следующую информацию." \
          ; "Oб удалении $(^name):" \
          ; "$\n  Для просмотра всего текста используйте стрелку 'вниз' или клавишу 'Page Down'."	

;--------------------------------
; License data
; Not exactly translated, but it shows what's needed
LicenseLangString myLicenseData ${LANG_ENGLISH} "${SOURCE_PATH}\docs\COPYING_EN"
LicenseLangString myLicenseData ${LANG_RUSSIAN} "${SOURCE_PATH}\docs\COPYING_RU"

; LicenseLangString myVerData ${LANG_ENGLISH} "${SOURCE_PATH}\build\release\ChangeLog_en.txt"
; LicenseLangString myVerData ${LANG_RUSSIAN} "${SOURCE_PATH}\build\release\ChangeLog_ru.txt"

;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Version Information

  VIProductVersion "${VERSION}.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "nextgis/manager"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "NextGIS Manager"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "NextGIS Manager"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2009-2014 Dmitry Baryshnikov, © 2014-2015 NextGIS"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}.0"
 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "nextgis/manager"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "Comments" "NextGIS Manager"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "NextGIS Manager"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "${VERSION}.0" 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "© 2009-2014 Dmitry Baryshnikov, © 2014-2015 NextGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductVersion" "${VERSION}.0"

;--------------------------------
;Installer Sections

Section SecCommon

  SetOutPath "$INSTDIR"
  SectionIn RO
  
  SetDetailsPrint textonly
  DetailPrint "Installing Core Files..."
  SetDetailsPrint listonly  
  
  ;system libs
  SetOutPath "$INSTDIR\bin"
  File /r "${PROJ_DIR}\wxGIS\inst\dll${BIT}\*.*"  
  ;wxWidgets 32 bit libs
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxbase30u_net_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxbase30u_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxbase30u_xml_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxmsw30u_adv_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxmsw30u_aui_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxmsw30u_core_vc_custom.dll"  
  File "${PROJ_DIR}\wxWidgets\lib\vc_dll\wxmsw30u_propgrid_vc_custom.dll"  
  ;gdal libs
  File "${PROJ_DIR}\gdal-nextgis-git\build${BIT}\Release\gdal20.dll"  
  ;proj lib
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\proj4.dll"  
  ;geos lib
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\geos_c.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\geos.dll"
  ;curl lib
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libcurl.dll"
  ;
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\cairo.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libeay32.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libexpat.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libiconv.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libjpeg.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libpng16.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\libtiff.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\ssleay32.dll"
  File "${PROJ_DIR}\wxGIS\build${BIT}\Debug\zlib.dll"
  
  File "${BUILD_PATH}\Release\wxgiscarto06.dll"
  File "${BUILD_PATH}\Release\wxgiscartoui06.dll"
  File "${BUILD_PATH}\Release\wxgiscatalog06.dll"
  File "${BUILD_PATH}\Release\wxgiscatalogui06.dll"
  File "${BUILD_PATH}\Release\wxgiscore06.dll"
  File "${BUILD_PATH}\Release\wxgisdatasource06.dll"
  File "${BUILD_PATH}\Release\wxgisdisplay06.dll"
  File "${BUILD_PATH}\Release\wxgisframework06.dll"
  File "${BUILD_PATH}\Release\wxgisfreechart06.dll"
  File "${BUILD_PATH}\Release\wxgisgeoprocessing06.dll"
  File "${BUILD_PATH}\Release\wxgisgeoprocessingui06.dll"
  File "${BUILD_PATH}\Release\wxgisnet06.dll"
  
  File "${BUILD_PATH}\Release\wxgiscat.exe"
  
  File "${BUILD_PATH}\Release\wxgistaskmanager.exe"
  File "${BUILD_PATH}\Release\wxgisconf.exe"
  
  File "${M_SOURCE_PATH}\build${BIT}\Release\ngm_barnaul_dataloader01.dll"
   
  ;sys dir
  SetOutPath "$INSTDIR\sys\gdal"
  File /r "${SOURCE_PATH}\opt\sys\gdal\*.*"
  SetOutPath "$INSTDIR\sys"
  File "${SOURCE_PATH}\opt\sys\cs.zip"
  
  ;log dir will create by app in it folder
  
  ;conf dir
  SetOutPath "$INSTDIR\config"
  File "${SOURCE_PATH}\opt\config\wxGISContDialog.xml"
  File "${SOURCE_PATH}\opt\config\wxGISObjDialog.xml"
  File "${SOURCE_PATH}\opt\config\wxTranslit.xml"
  File "${M_SOURCE_PATH}\opt\config\wxGISCommon.xml"
  File "${M_SOURCE_PATH}\opt\config\wxGISCatalog.xml"
  File "${SOURCE_PATH}\opt\config\wxCatalog.xml"
  File "${SOURCE_PATH}\opt\config\wxCatalogUI.xml"
  
  File "${SOURCE_PATH}\opt\config\wxCatalog.xml" 
  File "${SOURCE_PATH}\opt\config\wxCatalogUI.xml" 
  
  ;locale dir
  SetOutPath "$INSTDIR\locale"
  File /r "${BUILD_PATH}\opt\loc\"
  File /r "${M_SOURCE_PATH}\build${BIT}\opt\loc\"
  File "${SOURCE_PATH}\opt\loc\*.txt"
  
  ;license
  SetOutPath "$INSTDIR\docs"
  File "${SOURCE_PATH}\doc\COPYING_*"
  File "${SOURCE_PATH}\doc\ChangeLog_*"
  
  SetOutPath "$INSTDIR"
  
  ;Store installation folder
  WriteRegStr HKLM "Software\${REG_KEY}" "" $INSTDIR
 
	; Store uninstall information
	;Delete previous uninstal info
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
	;Create uninstal info
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayIcon" '"$INSTDIR\Uninstall.exe"'  
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  SetShellVarContext current
  CreateShortCut "$DESKTOP\nextgismanager.lnk" "$INSTDIR\bin\wxgiscat.exe"
  CreateShortCut "$QUICKLAUNCH\nextgismanager.lnk" "$INSTDIR\bin\wxgiscat.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC
;--------------------------------
!include "LangFile.nsh"

LangString UninstQuiestion ${LANG_ENGLISH} "There is the previous installation in '$INSTDIR'.$\r$\nUninstall?"
LangString UninstQuiestion ${LANG_RUSSIAN} "Обнаружена предыдущая установка в папке '$INSTDIR'.$\r$\nДеинсталировать?"
LangString UninstSettings ${LANG_ENGLISH} "Delete user settings?"
LangString UninstSettings ${LANG_RUSSIAN} "Удалить пользовательские настройки?"
;LangString UninstSettings ${PRODUCT_NAME_ENGLISH} "NextGIS Manager"
;LangString UninstSettings ${PRODUCT_NAME_RUSSIAN} "NextGIS Менеджер"

LangString Language ${LANG_ENGLISH} "en"
LangString Language ${LANG_RUSSIAN} "ru"

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  SetDetailsPrint textonly
  DetailPrint "Uninstalling NextGIS Manager..."
  SetDetailsPrint listonly

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r /REBOOTOK "$INSTDIR"

  DeleteRegKey HKLM "Software\${REG_KEY}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
  
  RMDir /r "$SMPROGRAMS\nextgis\manager"
  Delete "$DESKTOP\nextgismanager.lnk"
  Delete "$QUICKLAUNCH\nextgismanager.lnk"
  
  SetShellVarContext all
  RMDir /r "$APPDATA\wxGIS"
  MessageBox MB_YESNO $(UninstSettings) IDNO ngm_uninstalled
	SetShellVarContext current
	RMDir /r "$APPDATA\wxGIS" 
ngm_uninstalled:
SectionEnd

;
; Miscellaneous functions
;

Function un.onInit

FunctionEnd

Function .onInit
	ReadRegStr $0 HKLM "Software\${REG_KEY}" ""
	
	IfFileExists $0\Uninstall.exe 0 ngm_notinstalled
	;remove HKLM settings
	  SetShellVarContext all
	  RMDir /r "$APPDATA\wxGIS"
		MessageBox MB_YESNO $(UninstQuiestion) IDYES ngm_installed
		Abort "Отменено пользователем"
ngm_installed:
		ExecWait "$0\Uninstall.exe"
ngm_notinstalled:
FunctionEnd
