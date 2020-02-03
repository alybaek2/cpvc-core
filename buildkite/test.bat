call .\buildkite\restore.bat x64

REM Run C++ unit tests.
x64\Release\cpvc-core-test.exe
