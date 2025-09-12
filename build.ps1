$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location "$root\src"

# Clean
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path build | Out-Null

# Build with make if available, else direct gcc
if (Get-Command make -ErrorAction SilentlyContinue) {
    make clean
    make
} else {
    gcc -g -c main.c utils.c leftFactoring.c leftRecursion.c LL1Parser.c parsingStack.c stringParser.c
    gcc -g -o build/parser main.o utils.o leftFactoring.o leftRecursion.o LL1Parser.o parsingStack.o stringParser.o
}

# Run
if (Test-Path .\build\parser.exe) { .\build\parser.exe input.txt }
elseif (Test-Path .\build\parser) { ./build/parser input.txt }
else { Write-Host "Build failed: parser not found" -ForegroundColor Red }
