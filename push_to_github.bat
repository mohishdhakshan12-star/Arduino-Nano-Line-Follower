@echo off
cls
echo ======================================================================
echo       ARDUINO NANO LINE FOLLOWER ROBOT - GITHUB PUSHER
echo ======================================================================
echo.
echo Remote URL: https://github.com/mohishdhakshan12-star/Arduino-Nano-Line-Follower.git
echo.
echo 1. If you haven't created the repository on GitHub yet:
echo    - Open your browser to: https://github.com/new
echo    - Enter Repository name: Arduino-Nano-Line-Follower
echo    - Leave 'Add a README file' UNCHECKED
echo    - Click 'Create repository'
echo.
echo 2. Pushing local commits to GitHub...
echo.
git push -u origin main

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ======================================================================
    echo [SUCCESS] Code and complete PDF details uploaded successfully!
    echo View it at: https://github.com/mohishdhakshan12-star/Arduino-Nano-Line-Follower
    echo ======================================================================
) else (
    echo.
    echo ======================================================================
    echo [INFO] Push encountered an authentication or remote check.
    echo If your account name is mohishdhakshan12-start instead of mohishdhakshan12-star,
    echo run:
    echo   git remote set-url origin https://github.com/mohishdhakshan12-start/Arduino-Nano-Line-Follower.git
    echo   git push -u origin main
    echo ======================================================================
)

echo.
pause
