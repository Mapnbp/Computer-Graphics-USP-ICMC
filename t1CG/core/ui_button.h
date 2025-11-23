/**
 * @file ui_button.h
 * @brief Sistema aprimorado de botões com suporte a estados e callbacks
 * @author Sistema de UI OpenGL
 * @date 2025
 */

#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "ui_primitives.h"
#include "ui_theme.h"
#include <string>
#include <functional>
#include <GL/glut.h>

using namespace UIPrimitives;
using namespace UITheme;

/**
 * @brief Tipos de botão
 */
enum class ButtonType {
    ACTION,  // Dispara uma ação única (ex: Reset)
    TOGGLE,  // Liga/Desliga (mantém estado binário)
    RADIO    // Apenas um ativo por grupo (ex: Cubo/Esfera/Pirâmide)
};

/**
 * @brief Classe de botão moderna com estados visuais
 */
class UIButton {
public:
    // Geometria
    float x, y, width, height;
    
    // Visual
    std::string label;
    ColorRGBA baseColor;
    ColorRGBA hoverColor;
    ColorRGBA activeColor;
    ColorRGBA currentColor; // Cor atual (interpolada)
    
    // Estados
    bool isHovered;
    bool isPressed;
    bool isActive;  // Para botões TOGGLE e RADIO
    bool isEnabled;
    
    // Tipo e grupo
    ButtonType type;
    int radioGroupId; // Para botões RADIO (mesmo grupo = exclusão mútua)
    
    // Callback
    std::function<void()> onClick;
    
    // Ícone (opcional)
    enum class IconType { NONE, CUBE, SPHERE, PYRAMID, CYLINDER, SUN };
    IconType icon;
    
    /**
     * @brief Construtor
     */
    UIButton(float posX, float posY, float w, float h, 
             const std::string& text, ButtonType btnType = ButtonType::ACTION,
             int radioGroup = -1)
        : x(posX), y(posY), width(w), height(h), label(text),
          baseColor(DarkTheme::buttonBase),
          hoverColor(DarkTheme::buttonHover),
          activeColor(DarkTheme::buttonActive),
          currentColor(DarkTheme::buttonBase),
          isHovered(false), isPressed(false), isActive(false), isEnabled(true),
          type(btnType), radioGroupId(radioGroup), icon(IconType::NONE) {
    }
    
    /**
     * @brief Verifica se um ponto está dentro do botão
     */
    bool containsPoint(int mouseX, int mouseY) const {
        return isEnabled && 
               mouseX >= x && mouseX <= x + width &&
               mouseY >= y && mouseY <= y + height;
    }
    
    /**
     * @brief Atualiza o estado de hover
     */
    void updateHover(int mouseX, int mouseY) {
        if (!isEnabled) {
            isHovered = false;
            return;
        }
        isHovered = containsPoint(mouseX, mouseY);
    }
    
    /**
     * @brief Processa clique no botão
     * @return true se o clique foi consumido
     */
    bool handleClick(int mouseX, int mouseY) {
        if (!containsPoint(mouseX, mouseY)) {
            return false;
        }
        
        isPressed = true;
        
        // Comportamento baseado no tipo
        if (type == ButtonType::TOGGLE) {
            isActive = !isActive;
        } else if (type == ButtonType::RADIO) {
            // Radio buttons são ativados externamente pelo grupo
            isActive = true;
        }
        
        // Executa callback
        if (onClick) {
            onClick();
        }
        
        return true; // Clique consumido
    }
    
    /**
     * @brief Libera o botão pressionado
     */
    void releasePress() {
        isPressed = false;
    }
    
    /**
     * @brief Atualiza a cor com interpolação suave
     */
    void update(float deltaTime = 0.016f) {
        ColorRGBA targetColor;
        
        if (!isEnabled) {
            targetColor = DarkTheme::buttonDisabled;
        } else if (isPressed) {
            targetColor = activeColor;
        } else if (isActive) {
            targetColor = activeColor;
        } else if (isHovered) {
            targetColor = hoverColor;
        } else {
            targetColor = baseColor;
        }
        
        // Interpolação suave
        float lerpFactor = Animation::hoverTransitionSpeed;
        currentColor = currentColor.lerp(targetColor, lerpFactor);
    }
    
    /**
     * @brief Renderiza o botão
     */
    void render() const {
        // Sombra (opcional)
        if (isEnabled && (isHovered || isActive)) {
            drawShadow(x, y, width, height, Typography::borderRadius, 3.0f);
        }
        
        // Corpo do botão
        drawRoundedRect(x, y, width, height, Typography::borderRadius, currentColor);
        
        // Borda (se ativo)
        if (isActive && isEnabled) {
            drawRoundedRectOutline(x, y, width, height, Typography::borderRadius,
                                  DarkTheme::accentBlue, 2.0f);
        }
        
        // Ícone (se houver)
        if (icon != IconType::NONE) {
            float iconSize = height * 0.5f;
            float iconX = x + Typography::buttonPadding + iconSize * 0.5f;
            float iconY = y + height * 0.5f;
            ColorRGBA iconColor = isEnabled ? DarkTheme::textPrimary : DarkTheme::textDisabled;
            
            switch (icon) {
                case IconType::CUBE:
                    drawCubeIcon(iconX, iconY, iconSize, iconColor);
                    break;
                case IconType::SPHERE:
                    drawSphereIcon(iconX, iconY, iconSize, iconColor);
                    break;
                case IconType::PYRAMID:
                    drawPyramidIcon(iconX, iconY, iconSize, iconColor);
                    break;
                case IconType::CYLINDER:
                    drawCylinderIcon(iconX, iconY, iconSize, iconColor);
                    break;
                case IconType::SUN:
                    drawSunIcon(iconX, iconY, iconSize, iconColor);
                    break;
                default:
                    break;
            }
        }
        
        // Texto (renderização simplificada com GLUT)
        if (!label.empty()) {
            renderText();
        }
    }
    
    /**
     * @brief Define o ícone do botão
     */
    void setIcon(IconType iconType) {
        icon = iconType;
    }
    
    /**
     * @brief Define o callback de clique
     */
    void setCallback(std::function<void()> callback) {
        onClick = callback;
    }
    
    /**
     * @brief Ativa/desativa o botão
     */
    void setEnabled(bool enabled) {
        isEnabled = enabled;
        if (!enabled) {
            isHovered = false;
            isPressed = false;
        }
    }
    
    /**
     * @brief Define o estado ativo (para TOGGLE e RADIO)
     */
    void setActive(bool active) {
        isActive = active;
    }

private:
    /**
     * @brief Renderiza texto usando bitmap do GLUT
     */
    void renderText() const {
        ColorRGBA textColor = isEnabled ? DarkTheme::textPrimary : DarkTheme::textDisabled;
        glColor4f(textColor.r, textColor.g, textColor.b, textColor.a);
        
        // Fonte e cálculo de largura do texto
        void* font = GLUT_BITMAP_HELVETICA_12;
        int textWidth = 0;
        for (char c : label) {
            textWidth += glutBitmapWidth(font, c);
        }
        
        // Calcula posição do texto
        float textX, textY;
        
        // Y sempre centralizado verticalmente
        textY = y + height * 0.5f + 4.5f; // Offset para centralização vertical
        
        // X depende se tem ícone
        if (icon != IconType::NONE) {
            // Com ícone: texto à direita do ícone
            textX = x + Typography::buttonPadding + height * 0.6f + 4;
        } else {
            // Sem ícone: centralizado horizontalmente
            textX = x + (width - textWidth) * 0.5f;
        }
        
        glRasterPos2f(textX, textY);
        for (char c : label) {
            glutBitmapCharacter(font, c);
        }
    }
};

/**
 * @brief Gerenciador de grupos de botões RADIO
 */
class RadioButtonGroup {
private:
    std::vector<UIButton*> buttons;
    int activeIndex;

public:
    RadioButtonGroup() : activeIndex(-1) {}
    
    void addButton(UIButton* button) {
        if (button->type == ButtonType::RADIO) {
            buttons.push_back(button);
        }
    }
    
    void setActive(UIButton* clickedButton) {
        for (size_t i = 0; i < buttons.size(); i++) {
            if (buttons[i] == clickedButton) {
                buttons[i]->setActive(true);
                activeIndex = i;
            } else {
                buttons[i]->setActive(false);
            }
        }
    }
    
    int getActiveIndex() const {
        return activeIndex;
    }
    
    UIButton* getActiveButton() const {
        if (activeIndex >= 0 && activeIndex < buttons.size()) {
            return buttons[activeIndex];
        }
        return nullptr;
    }
};

#endif // UI_BUTTON_H
