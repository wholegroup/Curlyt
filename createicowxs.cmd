@ECHO OFF
@ECHO ^<Include^> > Icons.wxs
@FOR /F %%I IN ('dir /b Sources\Icons\*.ico') DO (
	@ECHO 	^<File Id="%%I" Name='%%I' Source="Sources/Icons/%%I" /^> >> Icons.wxs
)
@ECHO ^</Include^> >> Icons.wxs
