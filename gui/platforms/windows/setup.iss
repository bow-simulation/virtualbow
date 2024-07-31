[Setup]
AppId=DF79EFAF-3E5A-4651-B133-F342810D7A16
AppName=${APPLICATION_NAME}
AppVersion=${APPLICATION_VERSION}
AppPublisher=${APPLICATION_MAINTAINER}
AppPublisherURL=${APPLICATION_WEBSITE}
AppSupportURL=${APPLICATION_WEBSITE}
AppUpdatesURL=${APPLICATION_WEBSITE}
DefaultDirName={commonpf64}\${APPLICATION_NAME}
DefaultGroupName=${APPLICATION_NAME}
AllowNoIcons=yes
LicenseFile=${CMAKE_SOURCE_DIR}\..\LICENSE.md
Compression=lzma
SolidCompression=yes
UsePreviousTasks=no
DisableWelcomePage=no
DisableDirPage=no
ChangesAssociations = yes

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ${CMAKE_BINARY_DIR}\application\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: {group}\VirtualBow; Filename: {app}\virtualbow-gui.exe
Name: {group}\VirtualBow Post; Filename: {app}\virtualbow-post.exe
Name: {commondesktop}\VirtualBow; Filename: {app}\virtualbow-gui.exe; Tasks: DesktopIcons
Name: {commondesktop}\VirtualBow Post; Filename: {app}\virtualbow-post.exe; Tasks: DesktopIcons

[Tasks]
Name: DesktopIcons; Description: "Create desktop shortcuts";
Name: FileAssociation; Description: "Associate .bow and .res files with VirtualBow";
Name: AddToPath; Description: "Add VirtualBow to PATH";

[Registry]
Root: HKCR; Subkey: .bow; ValueType: string; ValueName: ""; ValueData: VirtualBowModelFile; Flags: uninsdeletevalue; Tasks: FileAssociation 
Root: HKCR; Subkey: VirtualBowModelFile; ValueType: string; ValueName: ""; ValueData: VirtualBow Model; Flags: uninsdeletekey; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowModelFile\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-gui.exe,-2"; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowModelFile\shell\open\command; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-gui.exe %1"; Tasks: FileAssociation 

Root: HKCR; Subkey: .res; ValueType: string; ValueName: ""; ValueData: VirtualBowResultFile; Flags: uninsdeletevalue; Tasks: FileAssociation 
Root: HKCR; Subkey: VirtualBowResultFile; ValueType: string; ValueName: ""; ValueData: VirtualBow Result; Flags: uninsdeletekey; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowResultFile\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-post.exe,-2"; Tasks: FileAssociation
Root: HKCR; Subkey: VirtualBowResultFile\shell\open\command; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-post.exe %1"; Tasks: FileAssociation

Root: HKCU; Subkey: Environment; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Tasks: AddToPath; Check: NotInPath(ExpandConstant('{app}'))

[Run]
Filename: {app}\virtualbow-gui.exe; Description: "Launch VirtualBow"; Flags: nowait postinstall skipifsilent

[Code]
{ https://stackoverflow.com/a/3431379 }
function NotInPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  { look for the path with leading and trailing semicolon }
  { Pos() returns 0 if not found }
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;
