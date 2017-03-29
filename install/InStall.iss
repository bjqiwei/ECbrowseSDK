; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
; 注意: AppId 值用于唯一识别该应用程序。
; 禁止对其他应用程序的安装器使用相同的 AppId 值！
; (若要生成一个新的 GUID，请选择“工具 | 生成 GUID”。)
#define AppName "ECBrowserSDK"
#define AppVersion "1.1.12.27"
AppId={{E4294C38-FF95-4A35-84CD-7EA7847B999C}}
AppName=Cloopen{#AppName}
AppVersion={#AppVersion}
AppCopyright=Copyright (C)北京容联易通 Inc.
AppComments=北京容联易通
VersionInfoVersion={#AppVersion}
VersionInfoDescription={#AppName}安装包
AppPublisher=北京容联易通
AppPublisherURL=http://www.yuntongxun.com/
AppSupportURL=http://www.yuntongxun.com/
AppUpdatesURL=http://www.yuntongxun.com/
DefaultDirName={pf32}\yuntongxun\{#AppName}
DefaultGroupName=yuntongxun\{#AppName}
AllowNoIcons=yes
AlwaysShowDirOnReadyPage=yes
AlwaysShowGroupOnReadyPage=yes
DisableWelcomePage=no
OutputDir=.\
OutputBaseFilename=Cloopen{#AppName}.{#AppVersion}
Compression=lzma2/ultra
SolidCompression=yes
PrivilegesRequired=admin

ArchitecturesInstallIn64BitMode=x64 ia64

[Languages]
Name: "chinese"; MessagesFile: "ChineseSimplified.isl"

[Tasks]
;Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
;Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
;Source: "..\bin\CloopenAgent.htm"; DestDir: "{app}"; Flags: ignoreversion 32bit
Source: "..\build\Win32\bin\Register.bat"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\UnRegister.bat"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\npECBrowserPlugin.dll"; DestDir: "{app}\x86"; Flags: regserver replacesameversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\ECsdk.dll"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit ;Source: "..\bin\x86\avcodec-55.dll"; DestDir: "{app}\x86"; Flags: ignoreversion  32bit  ;Source: "..\bin\x86\avformat-55.dll"; DestDir: "{app}\x86"; Flags: ignoreversion 32bit 
;Source: "..\bin\x86\avutil-52.dll"; DestDir: "{app}\x86"; Flags: ignoreversion 32bit     Source: "..\build\Win32\bin\libx264-142.dll"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\ring.wav"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit 
Source: "..\build\Win32\bin\ringback.wav"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit  
Source: "..\build\Win32\bin\AppShareDll.dll"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\NetComm.dll"; DestDir: "{app}\x86"; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
;Source: "..\bin\x64\CloopenAgentOCXx64.dll"; DestDir: "{app}\x64"; Flags: regserver replacesameversion 64bit ;Check: Is64BitInstallMode
;Source: "..\bin\x64\avcodec-55.dll"; DestDir: "{app}\x64"; Flags: ignoreversion 64bit ;Check: Is64BitInstallMode
;Source: "..\bin\x64\avformat-55.dll"; DestDir: "{app}\x64"; Flags: ignoreversion 64bit  ;Check: Is64BitInstallMode
;Source: "..\bin\x64\avutil-52.dll"; DestDir: "{app}\x64"; Flags: ignoreversion 64bit  ;Check: Is64BitInstallMode
;Source: "..\bin\x64\swresample-0.dll"; DestDir: "{app}\x64"; Flags: ignoreversion 64bit  ;Check: Is64BitInstallMode
;Source: "..\bin\x64\CCPAppClientx64.dll"; DestDir: "{app}\x64"; Flags: replacesameversion 64bit  ;Check: Is64BitInstallMode
;Source: "..\bin\ring.wav"; DestDir: "{app}\x64"; Flags: ignoreversion 64bit  ;Check: Is64BitInstallMode
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
;Name: "{group}\WebPlugin"; Filename: "{app}\CloopenAgent.htm"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
;Name: "{commondesktop}\CloopenClientPlugin"; Filename: "{app}\ClientPlugin.htm"; Tasks: desktopicon
;Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\CloopenClientPlugin"; Filename: "{app}\ClientPlugin.htm"; Tasks: quicklaunchicon


[UninstallDelete]
Type:filesandordirs;Name:{app};
Type:dirifempty;Name:{pf32}\yuntongxun;
Type:dirifempty;Name:{group};

[Registry]
;Root: HKLM; Subkey: "SOFTWARE\Samwoo\AA\TIP"; ValueType: string; ValueName: "ConnectionString"; ValueData:"Provider=SQLOLEDB.1;Password=sa;Persist Security Info=True;User ID=sa;Initial Catalog=TF_CMS;Data Source=10.1.86.129"; Flags:createvalueifdoesntexist    uninsdeletekey

