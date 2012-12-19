IDI_ICON1               ICON    DISCARDABLE     "application.ico"
1 VERSIONINFO
    FILEVERSION @RC_VERSION@
    PRODUCTVERSION @RC_VERSION@
#ifdef _DEBUG
    FILEFLAGS 0x1L
#else
    FILEFLAGS 0x0L
#endif
    FILEOS 0x4l
    FILETYPE 0x1
    FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "CompanyName", "Tycho Softworks"
            VALUE "FileDescription", "Coastal Notification Manager"
            VALUE "FileVersion", "@VERSION@"
            VALUE "InternalName", "coastal-notify"
            VALUE "LegalCopyright", "(C) 2012 David Sugar, Tycho Softworks"
            VALUE "OriginalFilename", "coastal-notify.exe"
            VALUE "ProductName", "Coastal Notifications"
            VALUE "ProductVersion", "@VERSION@"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1252
    END
END

