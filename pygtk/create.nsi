; NSIS script to create an ASCEND binary installer.
; by John Pye, 2006.
;
; Based on example2.nsi from the NSIS distribution.
;
; This script is based on example1.nsi, but it remembers the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "ASCEND"

!include LogicLib.nsh

; The file to write
!ifdef OUTFILE
OutFile ${OUTFILE}.exe
!else
OutFile ascend-setup.exe
!endif


;SetCompressor /FINAL zlib
SetCompressor /SOLID lzma

; The default installation directory
InstallDir $PROGRAMFILES\ASCEND

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\ASCEND" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

Var /GLOBAL PYOK
Var /GLOBAL PYPATH
Var /GLOBAL GTKOK
Var /GLOBAL GTKPATH
Var /GLOBAL PYINSTALLED
Var /GLOBAL TCLOK
Var /GLOBAL TCLPATH
Var /GLOBAL TCLINSTALLED

Function .onInit
	StrCpy $PYINSTALLED ""
	StrCpy $TCLINSTALLED ""

	Call DetectPython
	Pop $PYOK
	Pop $PYPATH
	
	Call DetectGTK
	Pop $GTKOK
	Pop $GTKPATH
	
	Call DetectTcl
	Pop $TCLOK
	Pop $TCLPATH	
	
FunctionEnd


; The stuff to install
Section "ASCEND (required)"
  SectionIn RO

  DetailPrint "--- COMMON FILES ---"
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  File "..\ascend.dll"
  
  ; Model Library
  SetOutPath $INSTDIR\models
  File /r /x .svn "..\models\*.a4*"
  File /r /x .svn "..\models\*.bz2"
  File /r /x .svn "..\models\*.tcl"
  File /r /x .svn "..\models\*.dll" ; extension modules

  SetOutPath $INSTDIR
  File "Makefile.bt"
  File "ascend.syn"

  ; Set 'librarypath' in .ascend.ini
  WriteINIstr $APPDATA\.ascend.ini Directories librarypath "$DOCUMENTS\ascdata;$INSTDIR\models"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\ASCEND "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "DisplayName" "ASCEND Simulation Environment"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

;--------------------------------

Section "PyGTK GUI"

	StrCmp $PYOK "OK" pythonfound

	MessageBox MB_OK "PyGTK GUI can not be installed, because Python was not found on this system. If you do want to use the PyGTK GUI, please check the installation instructions ($PYPATH)"
	Goto pydone

pythonfound:

	StrCmp $GTKOK "OK" gtkfound
	MessageBox MB_OK "PyGTK GUI can not be installed, because GTK+ 2.0 was not found on this system. If you do want to use the PyGTK GUI, please check the installation instructions ($GTKPATH)"
	Goto pydone

gtkfound:

;	MessageBox MB_OK "Python: $PYPATH, GTK: $GTKPATH"

	DetailPrint "--- PYTHON INTERFACE ---"
	
	; Set output path to the installation directory.
	SetOutPath $INSTDIR

	; Python interface
	File "_ascpy.dll"
	File "*.py"
	SetOutPath $INSTDIR\glade
	File "glade\*.glade"
	File "glade\*.png"

	; Create 'ascend.bat' launcher for PyGTK interface
	ClearErrors
	FileOpen $0 $INSTDIR\ascend.bat w
	IfErrors pydone
	FileWrite $0 "@echo off"
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"
	FileWrite $0 "set PATH=$PYPATH;$GTKPATH"
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"
	FileWrite $0 "cd "
	FileWrite $0 $INSTDIR 
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"
	FileWrite $0 "$PYPATH\python gtkbrowser.py %1 %2 %3 %4 %5 %6 %7 %8"
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"

	FileClose $0
	
	StrCpy $PYINSTALLED "1"
	WriteRegDWORD HKLM "SOFTWARE\ASCEND" "Python" 1
pydone:

SectionEnd

;---------------------------------

Section "Tcl/Tk GUI"

${If} $TCLOK != 'OK'
	MessageBox MB_OK "Tck/Tk GUI can not be installed, because ActiveTcl was not found on this system. If do you want to use the Tcl/Tk GUI, please check the installation instructions ($TCLPATH)"
${Else}
	DetailPrint "--- TCL/TK INTERFACE ---"
	SetOutPath $INSTDIR\tcltk
	File /r /x .svn "..\tcltk\TK\*"
	SetOutPath $INSTDIR
	File "..\tcltk\generic\interface\ascendtcl.dll"
	File "..\tcltk\generic\interface\ascend4.exe"
	
	StrCpy $TCLINSTALLED "!"
	WriteRegDWORD HKLM "SOFTWARE\ASCEND" "TclTk" 1

${EndIf}

SectionEnd

;---------------------------------

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  
  WriteRegDWORD HKLM "SOFTWARE\ASCEND" "StartMenu" 1

  CreateDirectory "$SMPROGRAMS\ASCEND"
  CreateShortCut "$SMPROGRAMS\ASCEND\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\ASCEND\ASCEND Tcl/Tk.lnk" "$INSTDIR\ascend4.exe" "" "$INSTDIR\ascend4.exe" 0

  ; Link to PyGTK GUI
  StrCmp $PYINSTALLED "" smdone smpython
smpython:
  CreateShortCut "$SMPROGRAMS\ASCEND\ASCEND.lnk" "$INSTDIR\ascend.bat" "" "$INSTDIR\ascend.bat" 0
smdone:
  
SectionEnd

;------------------------------------------------------------------
; UNINSTALLER

Section "Uninstall"

;--- python components ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "Python"
	IntCmp $0 0 unnopython unpython
  
unpython:
	DetailPrint "--- REMOVING PYTHON COMPONENTS ---"
	Delete $INSTDIR\_ascpy.dll
	Delete $INSTDIR\ascend.bat
	Delete $INSTDIR\*.py
	Delete $INSTDIR\glade\*.glade
	Delete $INSTDIR\glade\*.png
	RmDir $INSTDIR\glade

unnopython:

;--- tcl/tk components ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "TclTk"
	${If} $0 != 0
		DetailPrint "--- REMOVING TCL/TK COMPONENTS ---"
		Delete $INSTDIR\ascendtcl.dll
		RMDir /r $INSTDIR\tcltk
	${EndIf}

;--- start menu ---

	ReadRegDWORD $1 HKLM "SOFTWARE\ASCEND" "StartMenu"
	IntCmp $1 0 unnostart unstart 
unstart:
	; Remove shortcuts, if any
	DetailPrint "--- REMOVING START MENU SHORTCUTS ---"
	RmDir /r "$SMPROGRAMS\ASCEND"

unnostart:

;--- common components ---

	DetailPrint "--- REMOVING COMMON COMPONENTS ---"
	; Remove registry keys

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND"
	DeleteRegKey HKLM "SOFTWARE\ASCEND"

	; Remove files and uninstaller

	Delete $INSTDIR\ascend.dll
	Delete $INSTDIR\Makefile.bt
	Delete $INSTDIR\ascend.syn
	RMDir /r $INSTDIR\models

	; Remove directories used
	RMDir "$INSTDIR"

SectionEnd

;---------------------------------------------------------------------
; UTILITY ROUTINES

Function DetectPython
  ReadRegStr $R6 HKCU "SOFTWARE\Python\PythonCore\2.4\InstallPath" ""
  StrCmp $R6 "" NoFound1

  IfFileExists "$R6\python.exe" 0 NoFound
  Push "$R6"
  Push "OK"
  Return
 
NoFound:
  Push "No python.exe found"
  Push "NOK"
  Return

NoFound1:
  Push "No registry key found"
  Push "NOK"
  Return
  
FunctionEnd

;--------------------------------------------------------------------

Function DetectGTK
  ReadRegStr $R6 HKLM "SOFTWARE\GTK\2.0" "DllPath"
  StrCmp $R6 "" GtkNoFound
  IfFileExists "$R6\pkg-config.exe" 0 GtkNoFound1
  Push "$R6"
  Push "OK"
  Return

GtkNoFound:
  Push "No registry key"
  Push "NOK"
  Return
  
GtkNoFound1:
  Push "No pkg-config.exe found"
  Push "NOK"
  Return

FunctionEnd

;--------------------------------------------------------------------

Function DetectTcl
  ReadRegStr $R6 HKLM "SOFTWARE\ActiveState\ActiveTcl" "CurrentVersion"
  ${If} $R6 == ''
  	Push "No 'CurrentVersion' registry key"
  	Push "NOK"
  ${Else}
  	StrCpy $R7 "SOFTWARE\ActiveState\ActiveTcl\$R6"
  	ReadRegStr $R8 HKLM $R7 ""
  	${If} $R8 == ''
  		Push "No value for $R7"
  		Push "NOK"
  	${Else}
  	 	Push "$R8\bin"
  		Push "OK"
  	${EndIf}
  ${EndIf}
FunctionEnd
