# Microsoft Developer Studio Project File - Name="player" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=player - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "player.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "player.mak" CFG="player - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "player - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "player - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "player - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "player - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Za /W4 /Gm /GX /Zi /Od /I "..\src" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "player - Win32 Release"
# Name "player - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Group "baselib"

# PROP Default_Filter ""
# Begin Group "image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\baselib\image\alphatable.c
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KEdgeBuffer.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KEdgeScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KEdgeScanner.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KGraphicContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KGraphicContext.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImage.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImage_draw.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImage_drawopt.c

!IF  "$(CFG)" == "player - Win32 Release"

!ELSEIF  "$(CFG)" == "player - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\src\common\baselib\image\KImage_drawopt.c

"..\src\common\baselib\image\KImage_drawopt.obj" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
	dummy.bat

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImage_load.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\image\KImageData.h
# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\baselib\io\KFileInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KFileInputStream.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KFileOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KFileOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KInputStream.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KTypedInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KTypedInputStream.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KTypedOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\io\KTypedOutputStream.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\common\baselib\config.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\debug.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\keycode.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KString.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KString.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KStringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KStringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KTable.h
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\baselib\KTreeNode.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\common\KSceneDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\KSceneDoc.h
# End Source File
# Begin Source File

SOURCE=..\src\common\KSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\KSceneNode.h
# End Source File
# End Group
# Begin Group "player"

# PROP Default_Filter ""
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\player\Win32\KRuntimeEnvWin32.cpp
# End Source File
# Begin Source File

SOURCE=..\src\player\Win32\KRuntimeEnvWin32.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\player\Execute.cpp
# End Source File
# Begin Source File

SOURCE=..\src\player\KRuntimeEnv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\player\KRuntimeEnv.h
# End Source File
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource\icon.ico
# End Source File
# End Target
# End Project
