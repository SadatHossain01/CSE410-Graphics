@echo off
setlocal enabledelayedexpansion

:: Delete existing output files
del "stage1.txt" "stage2.txt" "stage3.txt" "z_buffer.txt" "out.bmp" 2> nul

:: Check for command line parameter
if "%~1"=="" (
    echo Please provide the IOs directory path as a command line parameter.
    exit /b
)

:: Assign the IOs directory to a variable
set "IOsDir=%~1"

:: Compile the program
g++ -std=c++14 -O3 main.cpp -o main.exe vector.cpp matrix.cpp transform.cpp triangle.cpp

:: Check if compilation was successful
if not exist main.exe (
    echo Compilation failed.
    exit /b
)

:: Iterate over each subfolder in the IOs directory
for /d %%i in ("%IOsDir%\*") do (
    echo Processing folder: %%i

    :: Run the program with the scene and config files as input
    main.exe "%%i\scene.txt" "%%i\config.txt"

    :: Initialize a variable to track success
    set matched=true

    :: Compare each output file with the expected output
    for %%f in (stage1.txt stage2.txt stage3.txt z_buffer.txt out.bmp) do (
        if exist "%%i\%%f" (
            fc /W "%%f" "%%i\%%f" > nul || (
                echo Difference found in %%f:
                fc /W "%%f" "%%i\%%f"
                set matched=false
            )
        ) else (
            echo Expected output file %%f is missing in folder %%i.
            set matched=false
        )
    )

    :: Output the result
    if !matched! == true (
        echo Success: All files match.
    ) else (
        echo Fail: There were differences.
    )

    echo --------------------------------
)

echo Finished processing all folders.
