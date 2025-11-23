/**
 * @file event_handler.h
 * @brief Gerenciamento de eventos com suporte completo a cores 3D
 * @date 2025
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "data_structures.h"
#include "polygon_manager.h"
#include "graphics_renderer.h"
#include <GL/glut.h>
#include <iostream>

enum class AppMode;

class EventHandler {
private:
    PolygonManager* polygonManager;
    GraphicsRenderer* graphicsRenderer;
    ApplicationState* currentApplicationState;
    AppMode* currentMode;
    int selectedColorIndex;
    WindowDimensions* windowDimensions;
    bool needsRedraw;
    
    // Callbacks 3D
    void (*onLightingChange)(int);
    void (*onProjectionChange)(int);
    void (*onObjectColorChange)(float, float, float);
    void (*onLightColorChange)(float, float, float);
    
    // Alvo de cor para modo 3D
    ColorTarget currentColorTarget;

public:
    EventHandler(PolygonManager* polygonMgr, GraphicsRenderer* graphicsRend, ApplicationState* appState, 
                 WindowDimensions* windowDims, AppMode* mode = nullptr,
                 void (*lightingCb)(int) = nullptr, void (*projectionCb)(int) = nullptr,
                 void (*objectColorCb)(float, float, float) = nullptr, void (*lightColorCb)(float, float, float) = nullptr)
        : polygonManager(polygonMgr), graphicsRenderer(graphicsRend), currentApplicationState(appState), 
          currentMode(mode), selectedColorIndex(12), windowDimensions(windowDims), needsRedraw(false),
          onLightingChange(lightingCb), onProjectionChange(projectionCb),
          onObjectColorChange(objectColorCb), onLightColorChange(lightColorCb),
          currentColorTarget(ColorTarget::OBJECT) {
    }
    
    void updateWindowDimensions(WindowDimensions* newDimensions) {
        windowDimensions = newDimensions;
    }
    
    void updateMouseCursor(int mouseX, int mouseY) {
        if (!windowDimensions) return;
        
        if (mouseX < windowDimensions->drawingAreaWidth && mouseY < windowDimensions->drawingAreaHeight) {
            glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        } else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
    }

    void handleMouseClick(int mouseX, int mouseY, bool isRightButton) {
        if (!windowDimensions) return;
        
        // Área de desenho
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
                    std::cout << "Poligono fechado" << std::endl;
                }
            }
        }
        
        glutPostRedisplay();
    }

    void handleKeyboardInput(char keyCode) {
        switch (keyCode) {
            case 'f': case 'F':
                if (polygonManager->getVertexCount() >= 3) {
                    polygonManager->closePolygon();
                    *currentApplicationState = ApplicationState::POLYGON_READY;
                    std::cout << "Poligono fechado" << std::endl;
                }
                break;
            case 'c': case 'C':
                polygonManager->clearPolygon();
                *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                std::cout << "Poligono limpo" << std::endl;
                break;
            case 'p': case 'P':
                if (polygonManager->canBeFilled()) {
                    *currentApplicationState = ApplicationState::POLYGON_FILLED;
                    std::cout << "Poligono marcado para preenchimento" << std::endl;
                }
                break;
            case 'v': case 'V':
                polygonManager->toggleVertexVisibility();
                std::cout << "Vertices " << (polygonManager->getVisualConfiguration().showVertices ? "mostrados" : "ocultos") << std::endl;
                break;
            case '+': case '=':
                polygonManager->adjustLineThickness(true);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness << std::endl;
                break;
            case '-':
                polygonManager->adjustLineThickness(false);
                std::cout << "Espessura: " << polygonManager->getVisualConfiguration().lineThickness << std::endl;
                break;
            case '1': case '2': case '3': case '4': case '5': case '6':
                polygonManager->applyPresetFillColor(keyCode - '0');
                std::cout << "Cor alterada" << std::endl;
                break;
            case 's': case 'S':
                if (polygonManager->canBeFilled()) {
                    bool isFilled = (*currentApplicationState == ApplicationState::POLYGON_FILLED);
                    polygonManager->saveCurrentPolygon(isFilled);
                    polygonManager->clearPolygon();
                    *currentApplicationState = ApplicationState::DRAWING_POLYGON;
                    std::cout << "Poligono salvo!" << std::endl;
                } else {
                    std::cout << "Poligono deve estar fechado para ser salvo!" << std::endl;
                }
                break;
        }
        
        glutPostRedisplay();
    }

    void renderInterface() {
        // Interface removida - agora usamos apenas controles do teclado
    }
};

#endif // EVENT_HANDLER_H