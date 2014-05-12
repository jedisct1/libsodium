@ECHO OFF
REM Usage: [buildbase.bat ..\vs2013\libzmq.sln 12]

SET solution=%1
SET version=%2

SET log=build_%version%.log

REM This breaks if visual studio is not installed or is installed to a non-default location.
SET environment="C:\Program Files (x86)\Microsoft Visual Studio %version%.0\VC\vcvarsall.bat"

ECHO Building: %solution%

CALL %environment% x86 > nul
ECHO Platform=x86

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=Win32 %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error

CALL %environment% x86_amd64 > nul
ECHO Platform=x64

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=x64 %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error

ECHO Complete: %solution%
GOTO end

:error
ECHO *** ERROR, build terminated, see: %log%

:end

