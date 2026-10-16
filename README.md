# ChassisDcoder (Windows GUI)

Desktop GUI application that decodes a vehicle VIN via the NHTSA VPIC API and reports:
- Year / Make / Model
- ADAS and safety features present in NHTSA data
- Likely Autel-style chassis codes for selected makes (heuristic)

Supported chassis-code heuristics: BMW, MINI, Mercedes-Benz, Mazda, Toyota, Nissan, Subaru. Other makes return "no heuristic available".

## Running (distributed build)
- `dist/ChassisDcoder.exe` (statically linked; no separate runtime needed).
- Enter a VIN and click **Decode**.

## Build from source (Windows, MSYS2 UCRT64)

Install toolchain (if needed):
```
C:\msys64\usr\bin\pacman.exe -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make
```

Configure and build:
```
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe
cmake --build build
```

## Notes
- Requires network access to vpic.nhtsa.dot.gov.
- NHTSA may omit fields; an empty field does not prove a feature is absent.
- Chassis codes are estimates only; a VIN does not encode the Autel chassis code definitively.
- WinHTTP is used; no external JSON dependency.
