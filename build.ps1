# Requires Python 3, Git, CMake, Ninja, native GCC/Clang, complete Arm GNU toolchain.
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot
python scripts/build.py @args
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
