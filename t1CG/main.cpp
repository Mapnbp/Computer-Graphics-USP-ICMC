/**
 * @file main_modular.cpp
 * @brief Sistema de Preenchimento de Polígonos - Versão Modular
 * @brief Implementa o algoritmo de preenchimento ET/AET com arquitetura modular
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#include "auxcode/data_structures.h"
#include "auxcode/polygon_manager.h"
#include "auxcode/graphics_renderer.h"
#include "auxcode/event_handler.h"
#include "auxcode/polygon_fill_algorithm.h"
#include <windows.h>
#include <iostream>

HWND mainWindowHandle;
PolygonManager polygonManager;
GraphicsRenderer* graphicsRenderer;
EventHandler* eventHandler;
ApplicationState currentApplicationState;
WindowDimensions* currentWindowDimensions;

/**
 * @brief Callback da janela para processamento de mensagens do Windows
 * @param windowHandle Handle da janela
 * @param message Mensagem recebida
 * @param wParam Parâmetro W da mensagem
 * @param lParam Parâmetro L da mensagem
 * @return Resultado do processamento da mensagem
 */
LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT paintStructure;
            HDC deviceContext = BeginPaint(windowHandle, &paintStructure);
            
            RECT clientRect;
            GetClientRect(windowHandle, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            
            HDC memoryDC = CreateCompatibleDC(deviceContext);
            HBITMAP memoryBitmap = CreateCompatibleBitmap(deviceContext, width, height);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memoryDC, memoryBitmap);
            
            HBRUSH backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(memoryDC, &clientRect, backgroundBrush);
            DeleteObject(backgroundBrush);
            
            eventHandler->handlePaintEvent(windowHandle, memoryDC);
            
            BitBlt(deviceContext, paintStructure.rcPaint.left, paintStructure.rcPaint.top,
                paintStructure.rcPaint.right - paintStructure.rcPaint.left,
                paintStructure.rcPaint.bottom - paintStructure.rcPaint.top,
                memoryDC, paintStructure.rcPaint.left, paintStructure.rcPaint.top, SRCCOPY);
            
            SelectObject(memoryDC, oldBitmap);
            DeleteObject(memoryBitmap);
            DeleteDC(memoryDC);
            
            EndPaint(windowHandle, &paintStructure);
            return 0;
        }
            
        case WM_LBUTTONDOWN: {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            eventHandler->handleMouseClick(mouseX, mouseY, false);
            return 0;
        }
            
        case WM_RBUTTONDOWN: {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            eventHandler->handleMouseClick(mouseX, mouseY, true);
            return 0;
        }
            
        case WM_MOUSEMOVE: {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            eventHandler->handleMouseMove(mouseX, mouseY);
            
            eventHandler->updateMouseCursor(mouseX, mouseY);
            return 0;
        }
            
        case WM_KEYDOWN: {
            int keyCode = static_cast<int>(wParam);
            
            if (keyCode == VK_ADD || keyCode == VK_OEM_PLUS) {
                eventHandler->handleKeyboardInput('+');
            } else if (keyCode == VK_SUBTRACT || keyCode == VK_OEM_MINUS) {
                eventHandler->handleKeyboardInput('-');
            } else {
                char keyPressed = static_cast<char>(wParam);
                eventHandler->handleKeyboardInput(keyPressed);
            }
            return 0;
        }
            
        case WM_SIZE: {
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);
            
            if (currentWindowDimensions) {
                delete currentWindowDimensions;
            }
            currentWindowDimensions = new WindowDimensions(newWidth, newHeight);
            
            if (eventHandler) {
                eventHandler->updateWindowDimensions(currentWindowDimensions);
            }
            
            InvalidateRect(windowHandle, NULL, TRUE);
            return 0;
        }
            
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(windowHandle, message, wParam, lParam);
}

/**
 * @brief Inicializa o sistema e cria a janela principal
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 */
bool initializeApplication() {
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.lpszClassName = "PolygonFillSystem";
    windowClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    
    if (!RegisterClass(&windowClass)) {
        std::cerr << "Erro ao registrar classe da janela" << std::endl;
        return false;
    }
    
    mainWindowHandle = CreateWindowEx(
        0,
        "PolygonFillSystem",
        "Sistema de Preenchimento de Poligonos - ET/AET",
        WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL,
        GetModuleHandle(NULL), NULL
    );
    
    if (mainWindowHandle == NULL) {
        std::cerr << "Erro ao criar janela principal" << std::endl;
        return false;
    }
    
    currentApplicationState = ApplicationState::DRAWING_POLYGON;
    
    currentWindowDimensions = new WindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    HDC deviceContext = GetDC(mainWindowHandle);
    graphicsRenderer = new GraphicsRenderer(deviceContext, mainWindowHandle);
    
    eventHandler = new EventHandler(&polygonManager, graphicsRenderer, &currentApplicationState, currentWindowDimensions);
    
    return true;
}

/**
 * @brief Exibe as instruções de uso do sistema
 */
void displayUsageInstructions() {
    std::cout << "================================================" << std::endl;
    std::cout << "Sistema de Preenchimento de Polígonos - ET/AET" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Instruções de uso:" << std::endl;
    std::cout << "- Clique esquerdo: Adicionar vértice" << std::endl;
    std::cout << "- Clique direito: Fechar polígono" << std::endl;
    std::cout << "- F: Fechar polígono" << std::endl;
    std::cout << "- C: Limpar polígono" << std::endl;
    std::cout << "- P: Preencher polígono" << std::endl;
    std::cout << "- V: Alternar visibilidade dos vértices" << std::endl;
    std::cout << "- +/-: Alterar espessura da linha" << std::endl;
    std::cout << "- 1-6: Alterar cor do preenchimento" << std::endl;
    std::cout << std::endl;
    std::cout << "Cores disponíveis:" << std::endl;
    std::cout << "1 - Vermelho    2 - Verde      3 - Azul" << std::endl;
    std::cout << "4 - Amarelo     5 - Magenta    6 - Ciano" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Limpa recursos alocados dinamicamente
 */
void cleanupResources() {
    if (eventHandler) {
        delete eventHandler;
        eventHandler = nullptr;
    }
    
    if (graphicsRenderer) {
        delete graphicsRenderer;
        graphicsRenderer = nullptr;
    }
    
    if (currentWindowDimensions) {
        delete currentWindowDimensions;
        currentWindowDimensions = nullptr;
    }
}

/**
 * @brief Função principal do sistema
 * @param argc Número de argumentos da linha de comando
 * @param argv Argumentos da linha de comando
 * @return Código de saída do programa
 */
int main(int argc, char** argv) {
    if (!initializeApplication()) {
        std::cerr << "Falha na inicialização do sistema" << std::endl;
        return 1;
    }
    
    displayUsageInstructions();
    
    ShowWindow(mainWindowHandle, SW_SHOW);
    UpdateWindow(mainWindowHandle);
    
    MSG windowMessage = {};
    while (GetMessage(&windowMessage, NULL, 0, 0)) {
        TranslateMessage(&windowMessage);
        DispatchMessage(&windowMessage);
    }
    
    cleanupResources();
    
    return 0;
}