/**
 * @file ui_theme.h
 * @brief Sistema de temas e paletas de cores para UI moderna
 * @author Sistema de UI OpenGL
 * @date 2025
 */

#ifndef UI_THEME_H
#define UI_THEME_H

#include "ui_primitives.h"
#include <vector>

namespace UITheme {

using namespace UIPrimitives;

/**
 * @brief Paleta Dark/Cyberpunk
 */
struct DarkTheme {
    // Cores de fundo
    static const ColorRGBA background;      // #1e1e2e - Azul escuro profundo
    static const ColorRGBA panel;           // #26283b - Painel lateral
    static const ColorRGBA panelDark;       // #16213e - Variação mais escura
    
    // Cores de texto
    static const ColorRGBA textPrimary;     // #cdd6f4 - Texto principal
    static const ColorRGBA textSecondary;   // #9399b2 - Texto secundário
    static const ColorRGBA textDisabled;    // #555566 - Texto desabilitado
    
    // Cores de botões
    static const ColorRGBA buttonBase;      // #26283b - Estado normal
    static const ColorRGBA buttonHover;     // #3b82f6 - Hover (azul neon)
    static const ColorRGBA buttonActive;    // #2563eb - Pressionado/Selecionado
    static const ColorRGBA buttonDisabled;  // #33333f - Desabilitado
    
    // Cores de destaque
    static const ColorRGBA accentBlue;      // #3b82f6 - Azul neon
    static const ColorRGBA accentTeal;      // #0f4c75 - Teal
    static const ColorRGBA accentPurple;    // #8b5cf6 - Roxo
    static const ColorRGBA accentGreen;     // #10b981 - Verde
    static const ColorRGBA accentRed;       // #ef4444 - Vermelho
    static const ColorRGBA accentYellow;    // #f59e0b - Amarelo
    
    // Cores de borda
    static const ColorRGBA borderNormal;    // #414558 - Borda normal
    static const ColorRGBA borderHighlight; // #6c7086 - Borda destacada
    
    // Transparências úteis
    static const ColorRGBA overlayDark;     // Preto semi-transparente
    static const ColorRGBA overlayLight;    // Branco semi-transparente
};

// Definições das cores estáticas (com sufixo 'u' para unsigned)
const ColorRGBA DarkTheme::background      = ColorRGBA(0x1e1e2eu, 1.0f);
const ColorRGBA DarkTheme::panel           = ColorRGBA(0x26283bu, 0.95f);
const ColorRGBA DarkTheme::panelDark       = ColorRGBA(0x16213eu, 0.95f);

const ColorRGBA DarkTheme::textPrimary     = ColorRGBA(0xcdd6f4u, 1.0f);
const ColorRGBA DarkTheme::textSecondary   = ColorRGBA(0x9399b2u, 1.0f);
const ColorRGBA DarkTheme::textDisabled    = ColorRGBA(0x555566u, 1.0f);

const ColorRGBA DarkTheme::buttonBase      = ColorRGBA(0x26283bu, 0.9f);
const ColorRGBA DarkTheme::buttonHover     = ColorRGBA(0x3b82f6u, 0.8f);
const ColorRGBA DarkTheme::buttonActive    = ColorRGBA(0x2563ebu, 1.0f);
const ColorRGBA DarkTheme::buttonDisabled  = ColorRGBA(0x33333fu, 0.6f);

const ColorRGBA DarkTheme::accentBlue      = ColorRGBA(0x3b82f6u, 1.0f);
const ColorRGBA DarkTheme::accentTeal      = ColorRGBA(0x0f4c75u, 1.0f);
const ColorRGBA DarkTheme::accentPurple    = ColorRGBA(0x8b5cf6u, 1.0f);
const ColorRGBA DarkTheme::accentGreen     = ColorRGBA(0x10b981u, 1.0f);
const ColorRGBA DarkTheme::accentRed       = ColorRGBA(0xef4444u, 1.0f);
const ColorRGBA DarkTheme::accentYellow    = ColorRGBA(0xf59e0bu, 1.0f);

const ColorRGBA DarkTheme::borderNormal    = ColorRGBA(0x414558u, 0.5f);
const ColorRGBA DarkTheme::borderHighlight = ColorRGBA(0x6c7086u, 0.8f);

const ColorRGBA DarkTheme::overlayDark     = ColorRGBA(0.0f, 0.0f, 0.0f, 0.4f);
const ColorRGBA DarkTheme::overlayLight    = ColorRGBA(1.0f, 1.0f, 1.0f, 0.1f);

/**
 * @brief Paleta de 16 cores vibrantes para objetos e iluminação
 */
struct ColorPalette {
    static std::vector<ColorRGBA> getColors() {
        return {
            ColorRGBA(0x000000u, 1.0f),  // 0: Preto
            ColorRGBA(0x808080u, 1.0f),  // 1: Cinza
            ColorRGBA(0xc0c0c0u, 1.0f),  // 2: Prata
            ColorRGBA(0xffffffu, 1.0f),  // 3: Branco
            ColorRGBA(0x800000u, 1.0f),  // 4: Marrom
            ColorRGBA(0xff0000u, 1.0f),  // 5: Vermelho
            ColorRGBA(0xff8000u, 1.0f),  // 6: Laranja
            ColorRGBA(0xffff00u, 1.0f),  // 7: Amarelo
            ColorRGBA(0x80ff00u, 1.0f),  // 8: Lima
            ColorRGBA(0x00ff00u, 1.0f),  // 9: Verde
            ColorRGBA(0x00ff80u, 1.0f),  // 10: Verde-água
            ColorRGBA(0x00ffffu, 1.0f),  // 11: Ciano
            ColorRGBA(0x0080ffu, 1.0f),  // 12: Azul claro
            ColorRGBA(0x0000ffu, 1.0f),  // 13: Azul
            ColorRGBA(0x8000ffu, 1.0f),  // 14: Roxo
            ColorRGBA(0xff00ffu, 1.0f)   // 15: Magenta
        };
    }
    
    // Conversão para valores RGB normalizados (0-1)
    static ColorRGBA getColorNormalized(int index) {
        auto colors = getColors();
        if (index >= 0 && index < colors.size()) {
            return colors[index];
        }
        return ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f); // Branco padrão
    }
};

/**
 * @brief Configurações de tipografia
 */
struct Typography {
    static const int buttonTextSize = 12;
    static const int headerTextSize = 16;
    static const int smallTextSize = 10;
    
    static const int buttonPadding = 10;
    static const int buttonHeight = 36;
    static const int buttonSpacing = 8;
    
    static constexpr float borderRadius = 8.0f;
    static constexpr float borderRadiusSmall = 4.0f;
};

/**
 * @brief Configurações de animação
 */
struct Animation {
    static constexpr float hoverTransitionSpeed = 0.15f; // Velocidade de transição do hover
    static constexpr float clickFeedbackDuration = 0.1f; // Duração do feedback visual
};

} // namespace UITheme

#endif // UI_THEME_H
