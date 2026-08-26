@ECHO OFF
REM Usage: [buildbase.bat ..\vs2026\mysolution.sln 18]

SETLOCAL enabledelayedexpansion

SET solution=%1
SET version=%2
SET log=build_%version%.log
SET environment=
SET vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
IF NOT EXIST !vswhere! SET vswhere="%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
SET /A next_version=version + 1
SET version_range=[%version%.0,!next_version!.0)
SET required_components=Microsoft.VisualStudio.Component.VC.Tools.x86.x64
IF %version% GEQ 17 SET required_components=!required_components! Microsoft.VisualStudio.Component.VC.Tools.ARM64

IF %version% GEQ 15 (
  IF EXIST !vswhere! (
    FOR /F "usebackq tokens=*" %%I IN (`!vswhere! -latest -prerelease -products * -version !version_range! -requires !required_components! -property installationPath`) DO (
      SET environment="%%I\VC\Auxiliary\Build\vcvarsall.bat"
    )
  )
) ELSE (
  SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat
  SET environment="%ProgramFiles%\!tools!"
  IF NOT EXIST !environment! SET environment="%ProgramFiles(x86)%\!tools!"
)

IF NOT DEFINED environment GOTO no_tools
IF NOT EXIST !environment! GOTO no_tools

ECHO Environment: !environment!
ECHO Building: %solution%

CALL !environment! x86 > nul 2>&1
ECHO Platform=x86

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=Win32 %solution% >> %log%
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

ENDLOCAL & SET solution=%solution% & SET version=%version% & SET log=%log% & SET environment=%environment%
SETLOCAL enabledelayedexpansion

CALL !environment! x86_amd64 > nul 2>&1
ECHO Platform=x64

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=x64 %solution% >> %log%
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

@REM Build ARM64 packages only for Visual Studio 2022 and later
IF %version% GEQ 17 (
  ENDLOCAL & SET solution=%solution% & SET version=%version% & SET log=%log% & SET environment=%environment%
  SETLOCAL enabledelayedexpansion

  CALL !environment! amd64_arm64 > nul 2>&1
  ECHO Platform=ARM64

  ECHO Configuration=DynDebug
  msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
  ECHO Configuration=DynRelease
  msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
  ECHO Configuration=LtcgDebug
  msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
  ECHO Configuration=LtcgRelease
  msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
  ECHO Configuration=StaticDebug
  msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
  ECHO Configuration=StaticRelease
  msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=ARM64 %solution% >> %log%
  IF errorlevel 1 GOTO error
)

ECHO Complete: %solution%
EXIT /B 0

:error
ECHO *** ERROR, build terminated early, see: %log%
ECHO.
ECHO === Last errors from %log% ===
findstr /i /c:"error " /c:"error:" /c:"fatal error" %log%
ECHO.
EXIT /B 1

:no_tools
ECHO *** ERROR, Visual Studio %version% C++ build tools not found
EXIT /B 1
