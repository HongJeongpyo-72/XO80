rem @ECHO OFF
PUSHD .
DIR /ad /b > cleandir
FOR /F "usebackq" %%i IN (cleandir) DO (
    IF NOT %%i == codec (
        IF NOT %%i == lib (
            rd /s /q %%i
        )
    )
)
DEL cleandir

cd ../win32
DIR /ad /b > cleandir
FOR /F "usebackq" %%i IN (cleandir) DO (
    IF NOT %%i == lib (
        rd /s /q %%i
    )
)
DEL cleandir
POPD

PUSHD .
IF EXIST codec (
    CD codec
    DIR /a /b > cleandir
    FOR /F "usebackq" %%i IN (cleandir) DO (
        IF NOT %%i == lib (
            rd /s /q %%i
        )
        IF NOT %%i == lib (
            del /s /f /q %%i
        )
    )
    DEL cleandir
)
POPD

PUSHD .
IF EXIST codec\lib\sm32 (
    CD codec\lib\sm32
    DIR /a /b > cleandir
    FOR /F "usebackq" %%i IN (cleandir) DO (
        IF NOT %%i == sbc.hex (
            del /q /f %%i
        )
    )
    DEL cleandir
)
POPD

