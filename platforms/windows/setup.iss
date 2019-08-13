[Setup]
AppId={{DF79EFAF-3E5A-4651-B133-F342810D7A16}
AppName=${APPLICATION_DISPLAY_NAME}
AppVersion=${APPLICATION_VERSION}
AppPublisher=${APPLICATION_MAINTAINER}
AppPublisherURL=${APPLICATION_WEBSITE}
AppSupportURL=${APPLICATION_WEBSITE}
AppUpdatesURL=${APPLICATION_WEBSITE}
DefaultDirName=${ISS_PROGRAM_DIR}\${APPLICATION_DISPLAY_NAME}
DefaultGroupName=${APPLICATION_DISPLAY_NAME}
AllowNoIcons=yes
LicenseFile=${CMAKE_CURRENT_SOURCE_DIR}\license.rtf
SetupIconFile=${CMAKE_CURRENT_SOURCE_DIR}\icon.ico
Compression=lzma
SolidCompression=yes
UsePreviousTasks=no
DisableWelcomePage=no
DisableDirPage=no

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ${CMAKE_BINARY_DIR}\windows\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: {group}\${APPLICATION_DISPLAY_NAME}; Filename: {app}\${APPLICATION_NAME}.exe
Name: {commondesktop}\${APPLICATION_DISPLAY_NAME}; Filename: {app}\${APPLICATION_NAME}.exe; Tasks: DesktopIcon

[Tasks]
Name: DesktopIcon; Description: "Create a desktop shortcut";
Name: FileAssociation; Description: "Associate .bow files with ${APPLICATION_DISPLAY_NAME}";

[Registry]
Root: HKCR; Subkey: .bow; ValueType: string; ValueName: ""; ValueData: VirtualBowModelFile; Flags: uninsdeletevalue; Tasks: FileAssociation 
Root: HKCR; Subkey: VirtualBowModelFile; ValueType: string; ValueName: ""; ValueData: ${APPLICATION_DISPLAY_NAME} Model; Flags: uninsdeletekey; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowModelFile\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\${APPLICATION_NAME}.exe, 0"; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowModelFile\shell\open\command; ValueType: string; ValueName: ""; ValueData: "{app}\${APPLICATION_NAME}.exe %1"; Tasks: FileAssociation 

[Run]
Filename: {app}\${APPLICATION_NAME}.exe; Description: "Launch ${APPLICATION_DISPLAY_NAME}"; Flags: nowait postinstall skipifsilent
