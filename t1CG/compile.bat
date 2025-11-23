@echo off
echo ========================================
echo Compilando com OpenGL + GLUT
echo ========================================
echo.

REM Verificar se g++ está disponível
where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] MinGW/g++ nao encontrado!
    echo Por favor, instale MinGW e adicione ao PATH.
    pause
    exit /b 1
)

echo [1/3] Limpando arquivos antigos...
if exist main.exe (
    del main.exe
    echo    - main.exe removido
)

echo.
echo [2/3] Compilando...
echo Comando: g++ -o main.exe main.cpp -Iinclude -Icore -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -std=c++17
echo.

g++ -o main.exe main.cpp -Iinclude -Icore -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -std=c++17

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo [ERRO] FALHA NA COMPILACAO!
    echo ========================================
    echo Codigo de erro: %ERRORLEVEL%
    echo.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Verificando executavel...
if exist main.exe (
    echo.
    echo ========================================
    echo COMPILACAO CONCLUIDA COM SUCESSO!
    echo ========================================
    echo.
    echo Executavel gerado: main.exe
    echo.
    echo Iniciando a aplicacao...
    echo.
    pause
    main.exe
) else (
    echo.
    echo [ERRO] O compilador nao reportou erro, mas main.exe nao foi criado.
    pause
)
