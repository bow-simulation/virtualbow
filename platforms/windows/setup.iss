#define APP_NAME "Bow Simulator"
#define EXE_NAME "bow-simulator.exe"
#define VERSION "0.2"
#define PUBLISHER "Stefan Pfeifer"
#define WEBSITE "https://bow-simulator.gitlab.io/"
#define BUILD_DIR

[Setup]
AppId={{DF79EFAF-3E5A-4651-B133-F342810D7A16}
AppName={#APP_NAME}
AppVersion={#VERSION}
AppPublisher={#PUBLISHER}
AppPublisherURL={#WEBSITE}
AppSupportURL={#WEBSITE}
AppUpdatesURL={#WEBSITE}
DefaultDirName={pf}\{#APP_NAME}
DefaultGroupName={#APP_NAME}
AllowNoIcons=yes
LicenseFile=license.rtf
SetupIconFile=logo.ico
Compression=lzma
SolidCompression=yes
UsePreviousTasks=no

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: "{#BINARY_DIR}/*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: {group}\{#APP_NAME}; Filename: {app}\{#EXE_NAME}
Name: {commondesktop}\{#APP_NAME}; Filename: {app}\{#EXE_NAME}; Tasks: desktopicon

[Tasks]
Name: DesktopIcon; Description: "Create a desktop shortcut"; Flags: unchecked
Name: FileAssociation; Description: "Associate .bow files with Bow Simulator";

[Registry]
Root: HKCR; Subkey: .bow; ValueType: string; ValueName: ""; ValueData: BowSimulatorModelFile; Flags: uninsdeletevalue; Tasks: FileAssociation 
Root: HKCR; Subkey: BowSimulatorModelFile; ValueType: string; ValueName: ""; ValueData: Bow Simulator Model; Flags: uninsdeletekey; Tasks: FileAssociation
Root: HKCR; Subkey: BowSimulatorModelFile\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\{#EXE_NAME}, 0"; Tasks: FileAssociation
Root: HKCR; Subkey: BowSimulatorModelFile\shell\open\command; ValueType: string; ValueName: ""; ValueData: "{app}\{#EXE_NAME} %1"; Tasks: FileAssociation 

[Run]
Filename: {app}\{#EXE_NAME}; Description: "Launch Bow Simulator"; Flags: nowait postinstall skipifsilent
