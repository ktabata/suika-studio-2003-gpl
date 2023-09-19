# Microsoft Developer Studio Project File - Name="studio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=studio - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "studio.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "studio.mak" CFG="studio - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "studio - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "studio - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "studio - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "studio - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MDd /W4 /Gm /GX /Zi /Od /I ".\\" /I "..\\" /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ole32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "studio - Win32 Release"
# Name "studio - Win32 Debug"
# Begin Group "klib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\klib\alphatable.c
# End Source File
# Begin Source File

SOURCE=..\..\klib\array.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\config.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\edgescanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\graphic.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\graphiccontext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\image_draw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\image_ext_c.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\image_init.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\image_load.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\image_transfer.c
# End Source File
# Begin Source File

SOURCE=..\..\klib\io.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\io.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\keycode.h
# End Source File
# Begin Source File

SOURCE=..\..\klib\string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\klib\string.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Group "anime"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KAnimator.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\common\KRuntimeEnv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KRuntimeEnv.h
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneDoc.h
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView.h
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView_drag.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView_draw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView_edit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView_init.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\KSceneView_key.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\opconst.h
# End Source File
# End Group
# Begin Group "h"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\DebugToolbar.h
# End Source File
# Begin Source File

SOURCE=.\EditToolbar.h
# End Source File
# Begin Source File

SOURCE=..\player\KRuntimeEnvWin32.h
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\SceneView.h
# End Source File
# Begin Source File

SOURCE=.\SuikaButton.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\App.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\EditToolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\player\KRuntimeEnvWin32.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow_debug.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneView.cpp
# End Source File
# Begin Source File

SOURCE=.\SuikaButton.cpp
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\suikacommon\res\btninsert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\btninsertpush.bmp
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\debugend.bmp
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\debugendreturn.bmp
# End Source File
# Begin Source File

SOURCE=.\resource\dragcursor.cur
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\frame.bmp
# End Source File
# Begin Source File

SOURCE=.\resource\icon.ico
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\icons.bmp
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\line.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc

!IF  "$(CFG)" == "studio - Win32 Release"

!ELSEIF  "$(CFG)" == "studio - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource\SceneEdit.exe.manifest
# End Source File
# Begin Source File

SOURCE=..\..\suikacommon\res\sceneviewpad.bmp
# End Source File
# Begin Source File

SOURCE=.\resource\suika.ico
# End Source File
# End Group
# Begin Group "dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dialogs\EditCheckVarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\EditCheckVarDlg.h
# End Source File
# Begin Source File

SOURCE=.\dialogs\EditMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\EditMsgDlg.h
# End Source File
# End Group
# End Target
# End Project
