#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include "data_structures.h"
#include "polygon_manager.h"
#include "graphics_renderer.h"
#include "scene_manager.h"
#include "event_handler.h"
#include "ui_manager.h"
#include <GL/glut.h>

enum class AppMode {
    MODE_2D_EDITOR,
    MODE_3D_VIEWER
};

class ApplicationContext {
public:
    PolygonManager polygonManager;
    GraphicsRenderer graphicsRenderer;
    SceneManager sceneManager;
    EventHandler* eventHandler;
    UIManager uiManager;
    
    WindowDimensions* windowDimensions;
    ApplicationState applicationState;
    AppMode currentMode;
    
    int lastMouseX;
    int lastMouseY;
    bool isRightMouseButtonPressed;
    
    ApplicationContext() 
        : eventHandler(nullptr), windowDimensions(nullptr),
          applicationState(ApplicationState::DRAWING_POLYGON),
          currentMode(AppMode::MODE_2D_EDITOR),
          lastMouseX(0), lastMouseY(0), isRightMouseButtonPressed(false) {
    }
    
    ~ApplicationContext() {
        if (eventHandler) delete eventHandler;
        if (windowDimensions) delete windowDimensions;
    }
    
    void init() {
        glClearColor(0.12f, 0.12f, 0.18f, 1.0f); // Dark background (#1e1e2e)
        sceneManager.init();
        
        windowDimensions = new WindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);
        
        // Inicializa UI Manager
        uiManager.init(&currentMode);
        
        // Define callbacks para a UI controlar a cena 3D
        uiManager.setCallbacks(
            // Shading callback
            [this](ShadingMode mode) {
                switch (mode) {
                    case ShadingMode::FLAT:
                        sceneManager.setLightingModel(LightingModel::FLAT);
                        break;
                    case ShadingMode::GOURAUD:
                        sceneManager.setLightingModel(LightingModel::GOURAUD);
                        break;
                    case ShadingMode::PHONG:
                        sceneManager.setLightingModel(LightingModel::PHONG);
                        break;
                }
            },
            // Projection callback
            [this](ProjectionMode mode) {
                switch (mode) {
                    case ProjectionMode::PERSPECTIVE:
                        sceneManager.setProjection(ProjectionType::PERSPECTIVE);
                        break;
                    case ProjectionMode::ORTHOGRAPHIC:
                        sceneManager.setProjection(ProjectionType::ORTHOGRAPHIC);
                        break;
                }
            },
            // Object type callback (placeholder - pode adicionar lógica depois)
            [this](ObjectType type) {
                // TODO: Implementar criação de diferentes objetos
            },
            // Object color callback
            [this](float r, float g, float b) {
                sceneManager.setObjectColor(r, g, b);
                polygonManager.setFillColor(r, g, b);
            },
            // Light color callback
            [this](float r, float g, float b) {
                sceneManager.setLightColor(r, g, b);
            }
        );
        
        // Define callbacks para o modo 2D
        uiManager.set2DCallbacks(
            // Close polygon (F)
            [this]() {
                if (polygonManager.getVertexCount() >= 3) {
                    polygonManager.closePolygon();
                    applicationState = ApplicationState::POLYGON_READY;
                }
            },
            // Fill polygon (P)
            [this]() {
                if (polygonManager.canBeFilled()) {
                    applicationState = ApplicationState::POLYGON_FILLED;
                }
            },
            // Clear polygon (C)
            [this]() {
                polygonManager.clearPolygon();
                applicationState = ApplicationState::DRAWING_POLYGON;
            },
            // Toggle vertices (V)
            [this]() {
                polygonManager.toggleVertexVisibility();
            },
            // Save polygon (S)
            [this]() {
                if (polygonManager.canBeFilled()) {
                    bool isFilled = (applicationState == ApplicationState::POLYGON_FILLED);
                    polygonManager.saveCurrentPolygon(isFilled);
                    polygonManager.clearPolygon();
                    applicationState = ApplicationState::DRAWING_POLYGON;
                } else {
                }
            },
            // Line width change
            [this](bool increase) {
                polygonManager.adjustLineThickness(increase);
            }
        );
        
        eventHandler = new EventHandler(&polygonManager, &graphicsRenderer, &applicationState, 
                                          windowDimensions, &currentMode);
    }
    
    void create3DObjectsFrom2D() {
        sceneManager.clearObjects();
        bool hasObjects = false;

        const auto& savedPolys = polygonManager.getSavedPolygons();
        for (const auto& poly : savedPolys) {
            if (poly.vertices.size() >= 3) {
                sceneManager.createExtrudedObject(poly.vertices, 50.0f);
                hasObjects = true;
            }
        }
        
        if (polygonManager.isPolygonCurrentlyClosed() && polygonManager.getVertexCount() >= 3) {
            sceneManager.createExtrudedObject(polygonManager.getVertices(), 50.0f);
            hasObjects = true;
        }
        
        if (!hasObjects) {
            std::vector<Point2D> square;
            square.push_back(Point2D(350, 250));
            square.push_back(Point2D(450, 250));
            square.push_back(Point2D(450, 350));
            square.push_back(Point2D(350, 350));
            sceneManager.createExtrudedObject(square, 100.0f);
        }
    }
    
    static ApplicationContext* getInstance() {
        static ApplicationContext instance;
        return &instance;
    }
};

#endif // APPLICATION_CONTEXT_H
