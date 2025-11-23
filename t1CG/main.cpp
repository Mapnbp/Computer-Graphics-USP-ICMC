/**
 * @file main.cpp
 * @brief Sistema de Computação Gráfica - Versão Integrada com UI
 */

#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

// Includes dos módulos (usando 'core')
#include "core/data_structures.h"
#include "core/polygon_manager.h"
#include "core/graphics_renderer.h"
#include "core/event_handler.h"
#include "core/scene_manager.h"
#include "core/application_context.h"

// Variáveis Globais de Shader
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;

// --- CALLBACKS GLUT ---

void display() {
    auto* app = ApplicationContext::getInstance();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (app->currentMode == AppMode::MODE_2D_EDITOR) {
        // --- MODO 2D ---
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (app->windowDimensions) {
            glOrtho(0, app->windowDimensions->width, app->windowDimensions->height, 0, -1, 1);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        // Renderiza polígonos
        app->graphicsRenderer.renderSavedPolygons(app->polygonManager.getSavedPolygons(), 
                                                  app->windowDimensions->height, 
                                                  app->windowDimensions->width);
        
        app->graphicsRenderer.renderPolygon(app->polygonManager.getVertices(), 
                                            app->polygonManager.getVisualConfiguration(), 
                                            app->polygonManager.isPolygonCurrentlyClosed());
        
        if (app->polygonManager.canBeFilled() && app->applicationState == ApplicationState::POLYGON_FILLED) {
             app->graphicsRenderer.fillPolygon(app->polygonManager.getVertices(), 
                                               app->polygonManager.getCurrentFillColor(), 
                                               app->windowDimensions->height, 
                                               app->windowDimensions->width);
        }
        
        app->graphicsRenderer.renderPolygonVertices(app->polygonManager.getVertices(), 
                                                    app->polygonManager.getVisualConfiguration().showVertices);
        
        // === RENDERIZA UI NO MODO 2D ===
        app->uiManager.render();

    } else {
        // --- MODO 3D ---
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        app->sceneManager.updateProjectionMatrix(w, h);
        app->sceneManager.render();

        // --- Renderizar UI Overlay no modo 3D ---
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        app->uiManager.render();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    auto* app = ApplicationContext::getInstance();
    
    if (app->windowDimensions) delete app->windowDimensions;
    app->windowDimensions = new WindowDimensions(w, h);
    
    if (app->eventHandler) {
        app->eventHandler->updateWindowDimensions(app->windowDimensions);
    }
    
    app->uiManager.updateLayout(w, h);
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    auto* app = ApplicationContext::getInstance();
    
    if (key == 27) exit(0);
    
    if (key == 'm' || key == 'M') {
        if (app->currentMode == AppMode::MODE_2D_EDITOR) {
            app->create3DObjectsFrom2D();
            app->currentMode = AppMode::MODE_3D_VIEWER;
            std::cout << "--> Modo: Visualizador 3D" << std::endl;
        } else {
            app->currentMode = AppMode::MODE_2D_EDITOR;
            std::cout << "--> Modo: Editor 2D" << std::endl;
        }
        glutPostRedisplay();
        return;
    }

    if (app->currentMode == AppMode::MODE_2D_EDITOR) {
        if (app->eventHandler) app->eventHandler->handleKeyboardInput(key);
    } else {
        // Controles 3D: WASD QE para câmera
        float speed = 0.5f;
        Vector3D camPos = app->sceneManager.getCameraPosition();
        
        switch (key) {
            case 'w': case 'W': camPos.z -= speed; break;  // Frente
            case 's': case 'S': camPos.z += speed; break;  // Trás
            case 'a': case 'A': camPos.x -= speed; break;  // Esquerda
            case 'd': case 'D': camPos.x += speed; break;  // Direita
            case 'q': case 'Q': camPos.y += speed; break;  // Cima
            case 'e': case 'E': camPos.y -= speed; break;  // Baixo
            case '1': app->sceneManager.setLightingModel(LightingModel::FLAT); break;
            case '2': app->sceneManager.setLightingModel(LightingModel::GOURAUD); break;
            case '3': app->sceneManager.setLightingModel(LightingModel::PHONG); break;
            case 'p': case 'P': app->sceneManager.setProjection(ProjectionType::PERSPECTIVE); break;
            case 'o': case 'O': app->sceneManager.setProjection(ProjectionType::ORTHOGRAPHIC); break;
        }
        app->sceneManager.setCameraPosition(camPos.x, camPos.y, camPos.z);
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    auto* app = ApplicationContext::getInstance();
    
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            // Prioridade: UI consome cliques em ambos os modos
            if (app->uiManager.handleClick(x, y)) {
                glutPostRedisplay();
                return;
            }
            // Se UI não consumiu, processa normalmente
            if (app->eventHandler) app->eventHandler->handleMouseClick(x, y, false);
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (app->currentMode == AppMode::MODE_2D_EDITOR) {
                if (app->eventHandler) app->eventHandler->handleMouseClick(x, y, true);
            } else {
                app->isRightMouseButtonPressed = true;
                app->lastMouseX = x;
                app->lastMouseY = y;
            }
        }
    } else if (state == GLUT_UP) {
        if (button == GLUT_RIGHT_BUTTON) {
            app->isRightMouseButtonPressed = false;
        } else if (button == GLUT_LEFT_BUTTON) {
            app->uiManager.releaseAll();
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    auto* app = ApplicationContext::getInstance();
    
    if (app->currentMode == AppMode::MODE_3D_VIEWER) {
        if (app->isRightMouseButtonPressed) {
            int dx = x - app->lastMouseX;
            int dy = y - app->lastMouseY;
            
            Vector3D lightPos = app->sceneManager.getLightPosition();
            lightPos.x += dx * 0.1f;
            lightPos.y -= dy * 0.1f;
            app->sceneManager.setLightPosition(lightPos.x, lightPos.y, lightPos.z);
            
            app->lastMouseX = x;
            app->lastMouseY = y;
        }
    }
    glutPostRedisplay();
}

void passiveMotion(int x, int y) {
    auto* app = ApplicationContext::getInstance();
    
    if (app->currentMode == AppMode::MODE_2D_EDITOR) {
        if (app->eventHandler) app->eventHandler->updateMouseCursor(x, y);
    } else {
        app->uiManager.handleHover(x, y);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Sistema de Computacao Grafica - OpenGL + GLUT");

    ApplicationContext::getInstance()->init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);

    std::cout << "========================================" << std::endl;
    std::cout << "Sistema Iniciado - Modo 2D Editor" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Controles:" << std::endl;
    std::cout << "  M - Alternar 2D/3D" << std::endl;
    std::cout << "  ESC - Sair" << std::endl;
    std::cout << "Modo 2D:" << std::endl;
    std::cout << "  Click - Adicionar vertice" << std::endl;
    std::cout << "  F - Fechar poligono" << std::endl;
    std::cout << "  P - Preencher" << std::endl;
    std::cout << "  S - Salvar poligono" << std::endl;
    std::cout << "Modo 3D:" << std::endl;
    std::cout << "  WASD QE - Mover camera" << std::endl;
    std::cout << "  1/2/3 - Flat/Gouraud/Phong" << std::endl;
    std::cout << "  P/O - Perspectiva/Ortografica" << std::endl;
    std::cout << "  Arrastar botao direito - Mover luz" << std::endl;
    std::cout << "========================================" << std::endl;

    glutMainLoop();
    return 0;
}