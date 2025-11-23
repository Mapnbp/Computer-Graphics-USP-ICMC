/**
 * @file ui_manager.h
 * @brief Gerenciador central da interface com layout responsivo e controles 3D
 * @author Sistema de UI OpenGL
 * @date 2025
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "ui_button.h"
#include "ui_primitives.h"
#include "ui_theme.h"
#include "data_structures.h"
#include <vector>
#include <memory>
#include <GL/glut.h>

using namespace UIPrimitives;
using namespace UITheme;

/**
 * @brief Estados de controle da aplicação
 */
enum class ShadingMode {
    FLAT = 0,
    GOURAUD = 1,
    PHONG = 2
};

enum class ProjectionMode {
    PERSPECTIVE = 0,
    ORTHOGRAPHIC = 1
};



// ColorTarget já está definido em data_structures.h
// Usar os mesmos valores: OBJECT e LIGHT

// Forward declaration para evitar dependência circular
enum class AppMode;

/**
 * @brief Gerenciador central da UI
 */
class UIManager {
private:
    // Dimensões
    int windowWidth;
    int windowHeight;
    int panelWidth;
    float panelX; // Posição X do painel lateral
    
    // Botões
    std::vector<std::unique_ptr<UIButton>> buttons;
    
    // Grupos de radio buttons
    RadioButtonGroup objectGroup;
    RadioButtonGroup projectionGroup;
    RadioButtonGroup shadingGroup;
    
    // Estados
    AppMode* currentMode;
    ShadingMode currentShading;
    ProjectionMode currentProjection;
    ObjectType currentObject;
    ColorTarget currentColorTarget;
    int selectedColorIndex;
    
    // Callbacks para comunicação com a cena 3D
    std::function<void(ShadingMode)> onShadingChange;
    std::function<void(ProjectionMode)> onProjectionChange;
    std::function<void(ObjectType)> onObjectChange;
    std::function<void(float, float, float)> onObjectColorChange;
    std::function<void(float, float, float)> onLightColorChange;
    
    // Callbacks para modo 2D
    std::function<void()> on2DClosePolygon;
    std::function<void()> on2DFillPolygon;
    std::function<void()> on2DClearPolygon;
    std::function<void()> on2DToggleVertices;
    std::function<void()> on2DSavePolygon;
    std::function<void(bool)> on2DLineWidthChange;
    
    // Botão de toggle para color target
    UIButton* colorTargetToggle;
    
    // Índices para separar botões 2D e 3D
    int button2DStart;
    int button2DEnd;
    int button3DStart;
    int button3DEnd;
    
    // Posição Y da paleta de cores (atualizada dinamicamente)
    float paletteStartY;
    
public:
    UIManager()
        : windowWidth(1000), windowHeight(700), panelWidth(220),
          currentMode(nullptr),
          currentShading(ShadingMode::FLAT),
          currentProjection(ProjectionMode::PERSPECTIVE),
          currentObject(ObjectType::CUBE),
          currentColorTarget(ColorTarget::OBJECT),
          selectedColorIndex(12),
          colorTargetToggle(nullptr),
          button2DStart(0), button2DEnd(0),
          button3DStart(0), button3DEnd(0),
          paletteStartY(0.0f) {
    }
    
    /**
     * @brief Inicializa o UI Manager
     */
    void init(AppMode* mode) {
        currentMode = mode;
        updateLayout(windowWidth, windowHeight);
        createButtons();
    }
    
    /**
     * @brief Define os callbacks para mudanças de estado
     */
    void setCallbacks(
        std::function<void(ShadingMode)> shadingCb,
        std::function<void(ProjectionMode)> projectionCb,
        std::function<void(ObjectType)> objectCb,
        std::function<void(float, float, float)> objectColorCb,
        std::function<void(float, float, float)> lightColorCb) {
        
        onShadingChange = shadingCb;
        onProjectionChange = projectionCb;
        onObjectChange = objectCb;
        onObjectColorChange = objectColorCb;
        onLightColorChange = lightColorCb;
    }
    
    /**
     * @brief Define os callbacks para o modo 2D
     */
    void set2DCallbacks(
        std::function<void()> closeCb,
        std::function<void()> fillCb,
        std::function<void()> clearCb,
        std::function<void()> toggleVerticesCb,
        std::function<void()> saveCb,
        std::function<void(bool)> lineWidthCb) {
        
        on2DClosePolygon = closeCb;
        on2DFillPolygon = fillCb;
        on2DClearPolygon = clearCb;
        on2DToggleVertices = toggleVerticesCb;
        on2DSavePolygon = saveCb;
        on2DLineWidthChange = lineWidthCb;
    }
    
    /**
     * @brief Atualiza o layout quando a janela é redimensionada
     */
    void updateLayout(int width, int height) {
        windowWidth = width;
        windowHeight = height;
        panelX = width - panelWidth;
        
        // Reposiciona botões existentes
        repositionButtons();
    }
    
    /**
     * @brief Cria todos os botões da interface baseado no modo atual
     */
    void createButtons() {
        buttons.clear();
        
        float startY = 20;
        float x = panelX + 10;
        float w = panelWidth - 20;
        float h = Typography::buttonHeight;
        float spacing = Typography::buttonSpacing;
        
        // === MODO 2D: CONTROLES DE POLÍGONO ===
        button2DStart = buttons.size();
        
        startY += 10;
        startY += 30; // Espaço para cabeçalho
        
        auto closeBtn = createButton(x, startY, w, h, "Close Polygon (F)", ButtonType::ACTION, -1);
        closeBtn->setCallback([this]() { if (on2DClosePolygon) on2DClosePolygon(); });
        startY += h + spacing;
        
        auto fillBtn = createButton(x, startY, w, h, "Fill Polygon (P)", ButtonType::ACTION, -1);
        fillBtn->setCallback([this]() { if (on2DFillPolygon) on2DFillPolygon(); });
        startY += h + spacing;
        
        auto clearBtn = createButton(x, startY, w, h, "Clear (C)", ButtonType::ACTION, -1);
        clearBtn->setCallback([this]() { if (on2DClearPolygon) on2DClearPolygon(); });
        startY += h + spacing;
        
        auto toggleVertBtn = createButton(x, startY, w, h, "Toggle Vertices (V)", ButtonType::TOGGLE, -1);
        toggleVertBtn->setCallback([this]() { if (on2DToggleVertices) on2DToggleVertices(); });
        toggleVertBtn->setActive(true); // Vértices mostrados por padrão
        startY += h + spacing;
        
        auto saveBtn = createButton(x, startY, w, h, "Save Polygon (S)", ButtonType::ACTION, -1);
        saveBtn->setCallback([this]() { if (on2DSavePolygon) on2DSavePolygon(); });
        startY += h + spacing;

        // === CONTROLE DE ESPESSURA ===
        float halfW = (w - spacing) / 2.0f;
        auto decBtn = createButton(x, startY, halfW, h, "Line -", ButtonType::ACTION, -1);
        decBtn->setCallback([this]() { if (on2DLineWidthChange) on2DLineWidthChange(false); });
        
        auto incBtn = createButton(x + halfW + spacing, startY, halfW, h, "Line +", ButtonType::ACTION, -1);
        incBtn->setCallback([this]() { if (on2DLineWidthChange) on2DLineWidthChange(true); });
        
        startY += h + spacing;
        
        button2DEnd = buttons.size();
        
        // === MODO 3D: CONTROLES 3D ===
        button3DStart = buttons.size();
        
        // Resetar Y para começar do topo novamente
        startY = 20;
        startY += 10;
        startY += 30;
        
        // Botões de seleção de objeto (RADIO)
        auto cubeBtn = createButton(x, startY, w, h, "Cube", ButtonType::RADIO, 0);
        cubeBtn->setIcon(UIButton::IconType::CUBE);
        cubeBtn->setCallback([this]() { setObjectType(ObjectType::CUBE); });
        cubeBtn->setActive(true); // Ativo por padrão
        objectGroup.addButton(cubeBtn);
        startY += h + spacing;
        
        auto sphereBtn = createButton(x, startY, w, h, "Sphere", ButtonType::RADIO, 0);
        sphereBtn->setIcon(UIButton::IconType::SPHERE);
        sphereBtn->setCallback([this]() { setObjectType(ObjectType::SPHERE); });
        objectGroup.addButton(sphereBtn);
        startY += h + spacing;
        
        auto cylinderBtn = createButton(x, startY, w, h, "Cylinder", ButtonType::RADIO, 0);
        cylinderBtn->setIcon(UIButton::IconType::CYLINDER);
        cylinderBtn->setCallback([this]() { setObjectType(ObjectType::CYLINDER); });
        objectGroup.addButton(cylinderBtn);
        startY += h + spacing;
        
        auto pyramidBtn = createButton(x, startY, w, h, "Pyramid", ButtonType::RADIO, 0);
        pyramidBtn->setIcon(UIButton::IconType::PYRAMID);
        pyramidBtn->setCallback([this]() { setObjectType(ObjectType::PYRAMID); });
        objectGroup.addButton(pyramidBtn);
        startY += h + spacing + 15;
        
        // === SEÇÃO: PROJEÇÃO ===
        startY += 30;
        
        auto perspectiveBtn = createButton(x, startY, w, h, "Perspective", ButtonType::RADIO, 1);
        perspectiveBtn->setCallback([this]() { setProjectionMode(ProjectionMode::PERSPECTIVE); });
        perspectiveBtn->setActive(true);
        projectionGroup.addButton(perspectiveBtn);
        startY += h + spacing;
        
        auto orthoBtn = createButton(x, startY, w, h, "Orthographic", ButtonType::RADIO, 1);
        orthoBtn->setCallback([this]() { setProjectionMode(ProjectionMode::ORTHOGRAPHIC); });
        projectionGroup.addButton(orthoBtn);
        startY += h + spacing + 15;
        
        // === SEÇÃO: SHADING ===
        startY += 30;
        
        auto flatBtn = createButton(x, startY, w, h, "Flat", ButtonType::RADIO, 2);
        flatBtn->setCallback([this]() { setShadingMode(ShadingMode::FLAT); });
        flatBtn->setActive(true);
        shadingGroup.addButton(flatBtn);
        startY += h + spacing;
        
        auto gouraudBtn = createButton(x, startY, w, h, "Gouraud", ButtonType::RADIO, 2);
        gouraudBtn->setCallback([this]() { setShadingMode(ShadingMode::GOURAUD); });
        shadingGroup.addButton(gouraudBtn);
        startY += h + spacing;
        
        auto phongBtn = createButton(x, startY, w, h, "Phong", ButtonType::RADIO, 2);
        phongBtn->setCallback([this]() { setShadingMode(ShadingMode::PHONG); });
        shadingGroup.addButton(phongBtn);
        startY += h + spacing + 15;
        
        // === SEÇÃO: COR / LUZ (3D) ===
        startY += 30;
        
        // Toggle para alternar entre pintar objeto ou luz
        colorTargetToggle = createButton(x, startY, w, h * 0.8f, "Object Color", ButtonType::TOGGLE, -1);
        colorTargetToggle->setCallback([this]() { toggleColorTarget(); });
        
        button3DEnd = buttons.size();
        
        // Paleta de cores será renderizada separadamente (16 círculos coloridos)
    }
    
    /**
     * @brief Renderiza a interface completa usando layout em pilha vertical
     */
    void render() {
        enableAntiAliasing();
        
        // Painel lateral de fundo
        drawRoundedRect(panelX, 0, panelWidth, windowHeight, 0, DarkTheme::panel);
        
        // === CURSOR DE LAYOUT (Stack Layout) ===
        float currentY = 30.0f;  // Margem inicial
        const float sectionSpacing = 25.0f;
        const float labelHeight = 20.0f;
        const float buttonSpacing = Typography::buttonSpacing;
        const float buttonHeight = Typography::buttonHeight;
        
        // === DETECÇÃO DE MODO (HARD SEPARATION) ===
        // currentMode é um ponteiro para AppMode (enum de 2 valores: 0=2D, 1=3D)
        bool is3DMode = false;
        if (currentMode != nullptr) {
            int modeValue = *(int*)currentMode;
            is3DMode = (modeValue == 1);  // MODE_3D_VIEWER = 1
        }
        
        // === RENDERIZAÇÃO EXCLUSIVA POR MODO ===
        if (is3DMode) {
            // ===== MODO 3D EXCLUSIVO =====
            
            // SEÇÃO: OBJECTS
            currentY = renderSectionLabel("OBJECTS", currentY);
            for (int i = button3DStart; i < button3DStart + 4 && i < button3DEnd; i++) {
                buttons[i]->update();
                buttons[i]->render();
                currentY += buttonHeight + buttonSpacing;
            }
            currentY += sectionSpacing;
            
            // SEÇÃO: PROJECTION
            currentY = renderSectionLabel("PROJECTION", currentY);
            for (int i = button3DStart + 4; i < button3DStart + 6 && i < button3DEnd; i++) {
                buttons[i]->update();
                buttons[i]->render();
                currentY += buttonHeight + buttonSpacing;
            }
            currentY += sectionSpacing;
            
            // SEÇÃO: SHADING
            currentY = renderSectionLabel("SHADING", currentY);
            for (int i = button3DStart + 6; i < button3DStart + 9 && i < button3DEnd; i++) {
                buttons[i]->update();
                buttons[i]->render();
                currentY += buttonHeight + buttonSpacing;
            }
            currentY += sectionSpacing;
            
            // SEÇÃO: COLOR TARGET
            currentY = renderSectionLabel("COLOR TARGET", currentY);
            if (colorTargetToggle) {
                colorTargetToggle->update();
                colorTargetToggle->render();
                currentY += buttonHeight * 0.8f + buttonSpacing;
            }
            currentY += sectionSpacing;
            
        } else {
            // ===== MODO 2D EXCLUSIVO =====
            
            // SEÇÃO: POLYGON TOOLS
            currentY = renderSectionLabel("POLYGON TOOLS", currentY);
            
            // Renderiza TODOS os botões 2D (Close, Fill, Clear, Toggle Vertices, Save)
            for (int i = button2DStart; i < button2DEnd && i < buttons.size(); i++) {
                buttons[i]->update();
                buttons[i]->render();
                currentY += buttonHeight + buttonSpacing;
            }
            currentY += sectionSpacing;
        }
        
        // === PALETA DE CORES (Comum a ambos os modos, mas com label diferente) ===
        currentY = renderSectionLabel(is3DMode ? "COLOR PALETTE" : "FILL COLOR", currentY);
        paletteStartY = currentY;  // Armazena posição Y para detecção de cliques
        renderColorPalette(currentY);
        
        disableAntiAliasing();
    }
    
    /**
     * @brief Processa clique do mouse
     */
    bool handleClick(int mouseX, int mouseY) {
        bool is3DMode = false;
        if (currentMode != nullptr) {
            int modeValue = *(int*)currentMode;
            is3DMode = (modeValue == 1);
        }

        int start = is3DMode ? button3DStart : button2DStart;
        int end = is3DMode ? button3DEnd : button2DEnd;

        // Verifica clique nos botões
        for (int i = start; i < end; i++) {
            auto& btn = buttons[i];
            if (btn->handleClick(mouseX, mouseY)) {
                // Para botões RADIO, atualiza o grupo
                if (btn->type == ButtonType::RADIO) {
                    if (btn->radioGroupId == 0) {
                        objectGroup.setActive(btn.get());
                    } else if (btn->radioGroupId == 1) {
                        projectionGroup.setActive(btn.get());
                    } else if (btn->radioGroupId == 2) {
                        shadingGroup.setActive(btn.get());
                    }
                }
                glutPostRedisplay();
                return true;
            }
        }
        
        // Verifica clique na paleta de cores
        if (handleColorPaletteClick(mouseX, mouseY)) {
            glutPostRedisplay();
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief Atualiza hover de todos os botões
     */
    void handleHover(int mouseX, int mouseY) {
        bool is3DMode = false;
        if (currentMode != nullptr) {
            int modeValue = *(int*)currentMode;
            is3DMode = (modeValue == 1);
        }

        int start = is3DMode ? button3DStart : button2DStart;
        int end = is3DMode ? button3DEnd : button2DEnd;

        for (int i = start; i < end; i++) {
            buttons[i]->updateHover(mouseX, mouseY);
        }
    }
    
    /**
     * @brief Libera todos os botões pressionados
     */
    void releaseAll() {
        for (auto& btn : buttons) {
            btn->releasePress();
        }
    }
    
    // Getters de estado
    ShadingMode getShadingMode() const { return currentShading; }
    ProjectionMode getProjectionMode() const { return currentProjection; }
    ObjectType getObjectType() const { return currentObject; }
    ColorTarget getColorTarget() const { return currentColorTarget; }
    int getSelectedColorIndex() const { return selectedColorIndex; }

private:
    /**
     * @brief Cria um botão e o adiciona à lista
     */
    UIButton* createButton(float x, float y, float w, float h, 
                          const std::string& label, ButtonType type, int radioGroup) {
        auto btn = std::make_unique<UIButton>(x, y, w, h, label, type, radioGroup);
        UIButton* ptr = btn.get();
        buttons.push_back(std::move(btn));
        return ptr;
    }
    
    /**
     * @brief Reposiciona botões após redimensionamento
     */
    void repositionButtons() {
        // Recria os botões com novas posições
        if (!buttons.empty()) {
            createButtons();
        }
    }
    
    /**
     * @brief Renderiza um label de seção e retorna a próxima posição Y
     */
    float renderSectionLabel(const std::string& text, float currentY) {
        glColor4f(DarkTheme::textSecondary.r, DarkTheme::textSecondary.g, 
                 DarkTheme::textSecondary.b, DarkTheme::textSecondary.a);
        
        void* font = GLUT_BITMAP_HELVETICA_12;
        glRasterPos2f(panelX + 15, currentY);
        for (char c : text) {
            glutBitmapCharacter(font, c);
        }
        
        // Retorna Y + altura do label + espaçamento
        return currentY + 20.0f;
    }
    
    /**
     * @brief Renderiza a paleta de cores (16 círculos) usando cursor Y
     */
    void renderColorPalette(float startY) {
        float startX = panelX + 15;
        float circleRadius = 12;
        float spacing = 30;
        
        auto colors = ColorPalette::getColors();
        
        for (int i = 0; i < 16; i++) {
            int row = i / 4;
            int col = i % 4;
            
            float cx = startX + col * spacing + circleRadius;
            float cy = startY + row * spacing + circleRadius;
            
            // Desenha o círculo de cor
            drawCircle(cx, cy, circleRadius, colors[i]);
            
            // Destaca a cor selecionada
            if (i == selectedColorIndex) {
                drawCircleOutline(cx, cy, circleRadius + 3, DarkTheme::accentBlue, 3.0f);
            }
        }
    }
    
    /**
     * @brief Processa clique na paleta de cores
     */
    bool handleColorPaletteClick(int mouseX, int mouseY) {
        float startX = panelX + 15;
        float startY = paletteStartY;  // Usa posição Y dinâmica
        float circleRadius = 12;
        float spacing = 30;
        
        for (int i = 0; i < 16; i++) {
            int row = i / 4;
            int col = i % 4;
            
            float cx = startX + col * spacing + circleRadius;
            float cy = startY + row * spacing + circleRadius;
            
            // Verifica se clicou neste círculo
            float dx = mouseX - cx;
            float dy = mouseY - cy;
            float distSq = dx * dx + dy * dy;
            
            if (distSq <= circleRadius * circleRadius) {
                selectedColorIndex = i;
                applyColorChange(i);
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief Aplica mudança de cor baseada no target atual
     */
    void applyColorChange(int colorIndex) {
        ColorRGBA color = ColorPalette::getColorNormalized(colorIndex);
        
        if (currentColorTarget == ColorTarget::OBJECT) {
            if (onObjectColorChange) {
                onObjectColorChange(color.r, color.g, color.b);
            }
        } else {
            if (onLightColorChange) {
                onLightColorChange(color.r, color.g, color.b);
            }
        }
    }
    
    /**
     * @brief Alterna entre pintar objeto e pintar luz
     */
    void toggleColorTarget() {
        if (currentColorTarget == ColorTarget::OBJECT) {
            currentColorTarget = ColorTarget::LIGHT;
            if (colorTargetToggle) {
                colorTargetToggle->label = "Light Color";
            }
        } else {
            currentColorTarget = ColorTarget::OBJECT;
            if (colorTargetToggle) {
                colorTargetToggle->label = "Object Color";
            }
        }
    }
    
    /**
     * @brief Define o modo de shading
     */
    void setShadingMode(ShadingMode mode) {
        currentShading = mode;
        if (onShadingChange) {
            onShadingChange(mode);
        }
    }
    
    /**
     * @brief Define o modo de projeção
     */
    void setProjectionMode(ProjectionMode mode) {
        currentProjection = mode;
        if (onProjectionChange) {
            onProjectionChange(mode);
        }
    }
    
    /**
     * @brief Define o tipo de objeto
     */
    void setObjectType(ObjectType type) {
        currentObject = type;
        if (onObjectChange) {
            onObjectChange(type);
        }
    }
};

#endif // UI_MANAGER_H
