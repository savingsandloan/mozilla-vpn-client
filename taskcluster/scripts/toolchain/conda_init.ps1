# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


# This Script will install conda.
#
# Note: It does not ship QT right now. 

$REPO_ROOT_PATH =resolve-path "$PSScriptRoot/../../../"
$TASK_WORKDIR =resolve-path "$REPO_ROOT_PATH/../../"
$FETCHES_PATH =resolve-path "$TASK_WORKDIR/fetches"

New-Item -ItemType Directory -Force -Path "$TASK_WORKDIR/conda"
$CONDA_DIR =resolve-path "$TASK_WORKDIR/conda"
# Prep Env:

## Install MiniConda 
Start-Process "$FETCHES_PATH/miniconda_installer.exe" -Wait -ArgumentList @('/S',"/D=$CONDA_DIR")
# We don't have the ability to do conda init - as that need's admin rights.
# So let's just do that ourselves :3
$Env:CONDA_EXE = "$CONDA_DIR\Scripts\conda.exe"
$Env:_CE_M = ""
$Env:_CE_CONDA = ""
$Env:_CONDA_ROOT = "$CONDA_DIR"
$Env:_CONDA_EXE = "$CONDA_DIR\Scripts\conda.exe"
$CondaModuleArgs = @{ChangePs1 = $False}
Import-Module "$Env:_CONDA_ROOT\shell\condabin\Conda.psm1" -ArgumentList $CondaModuleArgs

$env:PATH ="$CONDA_DIR;$FETCHES_PATH;$QTPATH;$env:PATH"
