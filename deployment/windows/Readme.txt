// Installer Todo:
- Make file associations optional

Release Build and Deployment on Windows
=======================================

1. Build a static release version of Qt with MinGW using the instructions in windows-qt-static.html and the shell script windows-qt-static.ps1 (from the Qt wiki, script modified to only perform release build).

2. Open Qt-Creator, create a new static kit and do a release build of the application.

3. Open the .exe with Resource Hacker and change the icon to '\resources\icons\logo.ico'.

4. Gather the following files:
    - BowSimulator.exe
    - License.rtf

5. Open installer.aip with Advanced Installer (www.advancedinstaller.com) and build.

6. Name the installer 'bow-simulator-[major].[minor].[patch]-win32' according to the current release

7. Test the installer and the software on different physical and virtual machines