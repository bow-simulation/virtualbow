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
ChangesAssociations=yes
UninstallDisplayIcon={app}\virtualbow-gui.exe

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ${CMAKE_BINARY_DIR}\application\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: {group}\VirtualBow; Filename: {app}\virtualbow-gui.exe
Name: {commondesktop}\VirtualBow; Filename: {app}\virtualbow-gui.exe; Tasks: DesktopIcons

[Tasks]
Name: DesktopIcons; Description: "Create desktop shortcut";
Name: FileAssociation; Description: "Associate *.bow and *.res files with VirtualBow";
Name: AddToPath; Description: "Add VirtualBow to system PATH"; Flags: unchecked

[Registry]
Root: HKCR; Subkey: .bow; ValueType: string; ValueName: ""; ValueData: VirtualBowModelFile; Tasks: FileAssociation; Flags: uninsdeletevalue
Root: HKCR; Subkey: VirtualBowModelFile; ValueType: string; ValueName: ""; ValueData: VirtualBow Model; Tasks: FileAssociation; Flags: uninsdeletekey
Root: HKCR; Subkey: VirtualBowModelFile\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-gui.exe,-2"; Tasks: FileAssociation; Flags: uninsdeletekey
Root: HKCR; Subkey: VirtualBowModelFile\shell\open\command; ValueType: string; ValueName: ""; ValueData: "{app}\virtualbow-gui.exe %1"; Tasks: FileAssociation; Flags: uninsdeletekey

Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Tasks: AddToPath; Check: NotInPath(ExpandConstant('{app}'))

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
