@echo off
echo ========================================
echo Compilando Sistema de Preenchimento ET/AET
echo ========================================
echo.

REM Remove o executável anterior se existir
if exist main.exe del main.exe

REM Compila o projeto
echo Compilando...
g++ -o main.exe main.cpp auxcode/*.h -lgdi32 -luser32 -lkernel32

REM Verifica se a compilação foi bem-sucedida
if exist main.exe (
    echo.
    echo ========================================
    echo COMPILACAO CONCLUIDA COM SUCESSO!
    echo ========================================
    echo.
    echo Executavel gerado: main.exe
    echo.
    echo Para executar, digite: main.exe
    echo ou clique duas vezes no arquivo main.exe
    echo.
    pause
) else (
    echo.
    echo ========================================
    echo ERRO NA COMPILACAO!
    echo ========================================
    echo.
    echo Verifique se o g++ esta instalado e configurado corretamente.
    echo.
    pause
)
