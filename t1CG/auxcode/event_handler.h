/**
 * @file event_handler.h
 * @brief Gerenciamento de eventos de mouse, teclado e interface
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "data_structures.h"
#include "polygon_manager.h"
#include "graphics_renderer.h"
#include <windows.h>
#include <iostream>

/**
 * @class EventHandler
 * @brief Classe responsável pelo gerenciamento de eventos de entrada do usuário 
 */
class EventHandler {
private:
    PolygonManager* polygonManager;
    GraphicsRenderer* graphicsRenderer;
    ApplicationState* currentApplicationState;
    std::vector<Button> buttons;
    int selectedColorIndex;
    WindowDimensions* windowDimensions;
    bool needsRedraw;

public:
    /**
     * @brief Construtor da classe EventHandler
     * @param polygonMgr Referência ao gerenciador de polígonos
     * @param graphicsRend Referência ao renderizador gráfico
     * @param appState Referência ao estado atual da aplicação
     * @param windowDims Dimensões atuais da janela
     */
    EventHandler(PolygonManager* polygonMgr, GraphicsRenderer* graphicsRend, ApplicationState* appState, WindowDimensions* windowDims)
        : polygonManager(polygonMgr), graphicsRenderer(graphicsRend), currentApplicationState(appState), 
          selectedColorIndex(12), windowDimensions(windowDims), needsRedraw(false) {
        initializeButtons();
    }
    
    /**
     * @brief Inicializa os botões da interface
     */
    void initializeButtons() {
        if (!windowDimensions) return;
        
        buttons.clear();
        
        int buttonY = 20;
        int buttonWidth = 180;
        int buttonHeight = 50;
        int buttonSpacing = 60;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Fechar Poligono", "F"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Limpar Poligono", "C"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Preencher", "P"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Mostrar Vertices", "V"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Linha +", "+"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Linha -", "-"));
        buttonY += buttonSpacing;
        
        buttons.push_back(Button(windowDimensions->drawingAreaWidth + 10, buttonY, buttonWidth, buttonHeight, "Salvar Poligono", "S"));
    }
    
    /**
     * @brief Atualiza as dimensões da janela e reposiciona os elementos
     * @param newDimensions Novas dimensões da janela
     */
    void updateWindowDimensions(WindowDimensions* newDimensions) {
        windowDimensions = newDimensions;
        initializeButtons();
    }
    
    /**
     * @brief Atualiza o estado de hover dos botões
     * @param mouseX Coordenada X do mouse
     * @param mouseY Coordenada Y do mouse
     */
    void updateButtonHover(int mouseX, int mouseY) {
        for (Button& button : buttons) {
            button.isHovered = button.isPointInside(mouseX, mouseY);
        }
    }
    
    /**
     * @brief Processa movimento do mouse para efeito hover
     * @param mouseX Coordenada X do mouse
     * @param mouseY Coordenada Y do mouse
     */
    void handleMouseMove(int mouseX, int mouseY) {
        bool hoverChanged = false;
        
        for (Button& button : buttons) {
            bool wasHovered = button.isHovered;
            button.isHovered = button.isPointInside(mouseX, mouseY);
            if (wasHovered != button.isHovered) {
                hoverChanged = true;
            }
        }
        
        if (hoverChanged) {
            needsRedraw = true;
            InvalidateRect(GetActiveWindow(), NULL, FALSE);
        }
    }
    
    /**
     * @brief Atualiza o cursor do mouse baseado na posição
     * @param mouseX Coordenada X do mouse
     * @param mouseY Coordenada Y do mouse
     */
    void updateMouseCursor(int mouseX, int mouseY) {
        if (!windowDimensions) return;
        
        if (mouseX < windowDimensions->drawingAreaWidth && mouseY < windowDimensions->drawingAreaHeight) {
            SetCursor(LoadCursor(NULL, IDC_CROSS));
        }
        else {
            bool isOverButton = false;
            for (const Button& button : buttons) {
                if (button.isPointInside(mouseX, mouseY)) {
                    isOverButton = true;
                    break;
                }
            }
            
            if (isOverButton) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
            } else {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
        }
    }

    /**
     * @brief Processa eventos de clique do mouse
     * @param mouseX Coordenada X do clique
     * @param mouseY Coordenada Y do clique
     * @param isRightButton true se foi clique direito, false se foi esquerdo
     */
    void handleMouseClick(int mouseX, int mouseY, bool isRightButton) {
        if (!windowDimensions) return;
        
        if (mouseX < windowDimensions->drawingAreaWidth && mouseY < windowDimensions->drawingAreaHeight) {
            if (!isRightButton) {
                Point2D newVertex(mouseX, mouseY);
                polygonManager->addVertex(newVertex);
                *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                
                std::cout << "Vertice adicionado: (" << mouseX << ", " << mouseY << ")" << std::endl;
            } else {
                if (polygonManager->getVertexCount() >= 3) {
                    polygonManager->closePolygon();
                    *currentApplicationState = ApplicationState::POLYGON_READY;
                    
                    std::cout << "Poligono fechado com " << polygonManager->getVertexCount() 
                              << " vertices" << std::endl;
                }
            }
        }
        else if (mouseX >= windowDimensions->drawingAreaWidth + 10 && mouseX <= windowDimensions->drawingAreaWidth + 170 && 
                 mouseY >= 460 && mouseY <= 620) {
            int colorX = (mouseX - (windowDimensions->drawingAreaWidth + 10)) / 40;
            int colorY = (mouseY - 460) / 40;
            int colorIndex = colorY * 4 + colorX;
            
            if (colorIndex >= 0 && colorIndex < 16) {
                selectedColorIndex = colorIndex;
                polygonManager->setFillColor(
                    COLOR_PALETTE[colorIndex].r / 255.0f,
                    COLOR_PALETTE[colorIndex].g / 255.0f,
                    COLOR_PALETTE[colorIndex].b / 255.0f
                );
                polygonManager->setLineColor(
                    COLOR_PALETTE[colorIndex].r / 255.0f,
                    COLOR_PALETTE[colorIndex].g / 255.0f,
                    COLOR_PALETTE[colorIndex].b / 255.0f
                );
                std::cout << "Cor selecionada: " << colorIndex << std::endl;
            }
        }
        else {
            for (size_t i = 0; i < buttons.size(); i++) {
                if (buttons[i].isPointInside(mouseX, mouseY)) {
                    handleButtonClick(i);
                    break;
                }
            }
        }
        
        needsRedraw = true;
        InvalidateRect(GetActiveWindow(), NULL, FALSE);
    }
    
    /**
     * @brief Processa cliques em botões
     * @param buttonIndex Índice do botão clicado
     */
    void handleButtonClick(int buttonIndex) {
        switch (buttonIndex) {
            case 0:
                if (polygonManager->getVertexCount() >= 3) {
                    polygonManager->closePolygon();
                    *currentApplicationState = ApplicationState::POLYGON_READY;
                    std::cout << "Poligono fechado" << std::endl;
                }
                break;
                
            case 1:
                polygonManager->clearPolygon();
                *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                std::cout << "Poligono limpo" << std::endl;
                break;
                
            case 2:
                if (polygonManager->canBeFilled()) {
                    graphicsRenderer->fillPolygon(
                        polygonManager->getVertices(), 
                        polygonManager->getCurrentFillColor(),
                        windowDimensions->drawingAreaHeight,
                        windowDimensions->drawingAreaWidth
                    );
                    *currentApplicationState = ApplicationState::POLYGON_FILLED;
                    std::cout << "Poligono preenchido" << std::endl;
                }
                break;
                
            case 3:
                polygonManager->toggleVertexVisibility();
                std::cout << "Vertices " 
                          << (polygonManager->getVisualConfiguration().showVertices ? "mostrados" : "ocultos") 
                          << std::endl;
                break;
                
            case 4:
                polygonManager->adjustLineThickness(true);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness 
                          << std::endl;
                break;
                
            case 5:
                polygonManager->adjustLineThickness(false);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness 
                          << std::endl;
                break;
                
            case 6:
                if (polygonManager->canBeFilled()) {
                    bool isFilled = (*currentApplicationState == ApplicationState::POLYGON_FILLED);
                    polygonManager->saveCurrentPolygon(isFilled);
                    polygonManager->clearPolygon();
                    *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                    std::cout << "Poligono salvo! Pode desenhar um novo." << std::endl;
                } else {
                    std::cout << "Poligono deve estar fechado para ser salvo!" << std::endl;
                }
                break;
        }
    }

    /**
     * @brief Processa eventos de teclado
     * @param keyCode Código da tecla pressionada
     */
    void handleKeyboardInput(char keyCode) {
        switch (keyCode) {
            case 'f':
            case 'F':
                if (polygonManager->getVertexCount() >= 3) {
                    polygonManager->closePolygon();
                    *currentApplicationState = ApplicationState::POLYGON_READY;
                    std::cout << "Poligono fechado" << std::endl;
                }
                break;
                
            case 'c':
            case 'C':
                polygonManager->clearPolygon();
                *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                std::cout << "Poligono limpo" << std::endl;
                break;
                
            case 'p':
            case 'P':
                if (polygonManager->canBeFilled()) {
                    graphicsRenderer->fillPolygon(
                        polygonManager->getVertices(), 
                        polygonManager->getCurrentFillColor(),
                        windowDimensions->drawingAreaHeight,
                        windowDimensions->drawingAreaWidth
                    );
                    *currentApplicationState = ApplicationState::POLYGON_FILLED;
                    std::cout << "Poligono preenchido" << std::endl;
                }
                break;
                
            case 'v':
            case 'V':
                polygonManager->toggleVertexVisibility();
                std::cout << "Vertices " 
                          << (polygonManager->getVisualConfiguration().showVertices ? "mostrados" : "ocultos") 
                          << std::endl;
                break;
                
            case '+':
            case '=':
                polygonManager->adjustLineThickness(true);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness 
                          << std::endl;
                break;
                
            case '-':
                polygonManager->adjustLineThickness(false);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness 
                          << std::endl;
                break;
                
            case '1':
                polygonManager->applyPresetFillColor(1);
                std::cout << "Cor alterada para: Vermelho" << std::endl;
                break;
            case '2':
                polygonManager->applyPresetFillColor(2);
                std::cout << "Cor alterada para: Verde" << std::endl;
                break;
            case '3':
                polygonManager->applyPresetFillColor(3);
                std::cout << "Cor alterada para: Azul" << std::endl;
                break;
            case '4':
                polygonManager->applyPresetFillColor(4);
                std::cout << "Cor alterada para: Amarelo" << std::endl;
                break;
            case '5':
                polygonManager->applyPresetFillColor(5);
                std::cout << "Cor alterada para: Magenta" << std::endl;
                break;
            case '6':
                polygonManager->applyPresetFillColor(6);
                std::cout << "Cor alterada para: Ciano" << std::endl;
                break;
                
            case 's':
            case 'S':
                if (polygonManager->canBeFilled()) {
                    bool isFilled = (*currentApplicationState == ApplicationState::POLYGON_FILLED);
                    polygonManager->saveCurrentPolygon(isFilled);
                    polygonManager->clearPolygon();
                    *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                    std::cout << "Poligono salvo! Pode desenhar um novo." << std::endl;
                } else {
                    std::cout << "Poligono deve estar fechado para ser salvo!" << std::endl;
                }
                break;
        }
        
        needsRedraw = true;
        InvalidateRect(GetActiveWindow(), NULL, FALSE);
    }

    /**
     * @brief Processa o evento de pintura da janela
     * @param windowHandle Handle da janela
     * @param deviceContext Contexto de dispositivo
     */
    void handlePaintEvent(HWND windowHandle, HDC deviceContext) {
        if (!windowDimensions) return;
        
        graphicsRenderer->updateDeviceContext(deviceContext);
        
        graphicsRenderer->clearDrawingArea(windowDimensions);
        
        graphicsRenderer->renderSavedPolygons(polygonManager->getSavedPolygons(), 
                                            windowDimensions->drawingAreaHeight,
                                            windowDimensions->drawingAreaWidth);
        
        graphicsRenderer->renderPolygon(
            polygonManager->getVertices(),
            polygonManager->getVisualConfiguration(),
            polygonManager->isPolygonCurrentlyClosed()
        );
        
        if (*currentApplicationState == ApplicationState::POLYGON_FILLED) {
            graphicsRenderer->fillPolygon(
                polygonManager->getVertices(),
                polygonManager->getCurrentFillColor(),
                windowDimensions->drawingAreaHeight,
                windowDimensions->drawingAreaWidth
            );
        }
        
        graphicsRenderer->renderPolygonVertices(
            polygonManager->getVertices(),
            polygonManager->getVisualConfiguration().showVertices
        );
        
        graphicsRenderer->renderUserInterface(
            polygonManager->getVertexCount(),
            *currentApplicationState,
            windowDimensions,
            polygonManager->getSavedPolygonCount()
        );
        
        graphicsRenderer->renderRightPanel(buttons, selectedColorIndex, windowDimensions);
        
        needsRedraw = false;
    }
};

#endif // EVENT_HANDLER_H