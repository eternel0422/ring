cls
call ..\..\..\language\build\locatevc.bat x64         
cl /c /DEBUG mylibMandel.c -I"..\..\..\language\include"
link /DEBUG mylibMandel.obj ..\..\..\lib\ring.lib /DLL /OUT:mylibMandel.dll
del mylibMandel.obj
