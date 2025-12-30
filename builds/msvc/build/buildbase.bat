@ECHO OFF
REM Usage: [buildbase.bat ..\vs2026\mysolution.sln 18]

SETLOCAL enabledelayedexpansion

SET solution=%1
SET version=%2
SET log=build_%version%.log
SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat

IF %version% == 18 (
  SET tools=Microsoft Visual Studio\2026\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
  SET environment="%programfiles%\!tools!"
  IF NOT EXIST !environment! (
    SET environment="%programfiles(x86)%\!tools!"
    IF NOT EXIST !environment! (
      SET tools=Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvarsall.bat
    )
  )
)

IF %version% == 17 (
  SET tools=Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
  SET environment="%programfiles%\!tools!"
  IF NOT EXIST !environment! (
    SET environment="%programfiles(x86)%\!tools!"
    IF NOT EXIST !environment! (
      SET tools=Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat
    )
  )
)

IF %version% == 16 (
  SET tools=Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
  SET environment="%programfiles%\!tools!"
  IF NOT EXIST !environment! (
    SET environment="%programfiles(x86)%\!tools!"
    IF NOT EXIST !environment! (
      SET tools=Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat
    )
  )
)

IF %version% == 15 (
  SET tools=Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
  SET environment="%programfiles%\!tools!"
  IF NOT EXIST !environment! (
    SET environment="%programfiles(x86)%\!tools!"
    IF NOT EXIST !environment! (
      SET tools=Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat
    )
  )
)
SET environment="%programfiles%\!tools!"
IF NOT EXIST !environment! SET environment="%programfiles(x86)%\!tools!"

ECHO Environment: !environment!

IF NOT EXIST !environment! GOTO no_tools

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

ENDLOCAL & SET solution=%solution% & SET version=%version% & SET log=%log% & SET tools=%tools% & SET environment=%environment%
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

@REM Build ARM64 packages only for Visual studio 2019 and later
IF %version% GEQ 16 (
  ENDLOCAL & SET solution=%solution% & SET version=%version% & SET log=%log% & SET tools=%tools% & SET environment=%environment%
  SETLOCAL enabledelayedexpansion

  CALL !environment! ARM64 > nul 2>&1
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
GOTO end

:error
ECHO *** ERROR, build terminated early, see: %log%
ECHO.
ECHO === Last errors from %log% ===
findstr /i /c:"error " /c:"error:" /c:"fatal error" %log%
ECHO.
GOTO end

:no_tools
ECHO *** ERROR, build tools not found: !tools!

:end

