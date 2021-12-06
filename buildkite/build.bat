%BUILD_NUGET% restore

REM Build Debug and Release (x64).
%BUILD_MSBUILD% cpvc-core.sln /t:Rebuild /p:Configuration=Release /p:Platform="x64"
%BUILD_MSBUILD% cpvc-core.sln /t:Rebuild /p:Configuration=Debug /p:Platform="x64"

call .\buildkite\backup.bat x64
