/**
 * @file ui_primitives.h
 * @brief Primitivas gráficas vetoriais para UI moderna (Math-based rendering)
 * @author Sistema de UI OpenGL
 * @date 2025
 */

#ifndef UI_PRIMITIVES_H
#define UI_PRIMITIVES_H

#include <GL/gl.h>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace UIPrimitives {

/**
 * @brief Estrutura de cor RGBA para UI
 */
struct ColorRGBA {
    float r, g, b, a;
    
    ColorRGBA(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
    
    // Construtor a partir de hex (ex: 0x1e1e2e)
    ColorRGBA(unsigned int hex, float alpha = 1.0f) {
        r = ((hex >> 16) & 0xFF) / 255.0f;
        g = ((hex >> 8) & 0xFF) / 255.0f;
        b = (hex & 0xFF) / 255.0f;
        a = alpha;
    }
    
    // Interpolação linear entre duas cores
    ColorRGBA lerp(const ColorRGBA& target, float t) const {
        return ColorRGBA(
            r + (target.r - r) * t,
            g + (target.g - g) * t,
            b + (target.b - b) * t,
            a + (target.a - a) * t
        );
    }
};

/**
 * @brief Ativa anti-aliasing para desenho suave
 */
inline void enableAntiAliasing() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

/**
 * @brief Desativa anti-aliasing
 */
inline void disableAntiAliasing() {
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
}

/**
 * @brief Desenha um retângulo com cantos arredondados usando matemática trigonométrica
 * @param x Posição X (canto superior esquerdo)
 * @param y Posição Y (canto superior esquerdo)
 * @param width Largura
 * @param height Altura
 * @param radius Raio dos cantos arredondados
 * @param color Cor de preenchimento
 * @param segments Número de segmentos por canto (padrão: 10)
 */
inline void drawRoundedRect(float x, float y, float width, float height, 
                           float radius, const ColorRGBA& color, int segments = 10) {
    // Limita o raio ao menor lado
    float maxRadius = (width < height ? width : height) * 0.5f;
    if (radius > maxRadius) radius = maxRadius;
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    // Desenha o corpo central (retângulo sem cantos)
    glBegin(GL_QUADS);
        // Parte superior
        glVertex2f(x + radius, y);
        glVertex2f(x + width - radius, y);
        glVertex2f(x + width - radius, y + radius);
        glVertex2f(x + radius, y + radius);
        
        // Parte central
        glVertex2f(x, y + radius);
        glVertex2f(x + width, y + radius);
        glVertex2f(x + width, y + height - radius);
        glVertex2f(x, y + height - radius);
        
        // Parte inferior
        glVertex2f(x + radius, y + height - radius);
        glVertex2f(x + width - radius, y + height - radius);
        glVertex2f(x + width - radius, y + height);
        glVertex2f(x + radius, y + height);
    glEnd();
    
    // Desenha os 4 cantos arredondados usando GL_TRIANGLE_FAN
    float angleStep = (M_PI / 2.0f) / segments;
    
    // Canto superior esquerdo
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + radius, y + radius); // Centro
        for (int i = 0; i <= segments; i++) {
            float angle = M_PI + i * angleStep;
            glVertex2f(x + radius + radius * cos(angle), 
                      y + radius + radius * sin(angle));
        }
    glEnd();
    
    // Canto superior direito
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + width - radius, y + radius);
        for (int i = 0; i <= segments; i++) {
            float angle = 1.5f * M_PI + i * angleStep;
            glVertex2f(x + width - radius + radius * cos(angle), 
                      y + radius + radius * sin(angle));
        }
    glEnd();
    
    // Canto inferior direito
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + width - radius, y + height - radius);
        for (int i = 0; i <= segments; i++) {
            float angle = i * angleStep;
            glVertex2f(x + width - radius + radius * cos(angle), 
                      y + height - radius + radius * sin(angle));
        }
    glEnd();
    
    // Canto inferior esquerdo
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + radius, y + height - radius);
        for (int i = 0; i <= segments; i++) {
            float angle = 0.5f * M_PI + i * angleStep;
            glVertex2f(x + radius + radius * cos(angle), 
                      y + height - radius + radius * sin(angle));
        }
    glEnd();
}

/**
 * @brief Desenha a borda de um retângulo arredondado
 */
inline void drawRoundedRectOutline(float x, float y, float width, float height, 
                                   float radius, const ColorRGBA& color, 
                                   float lineWidth = 2.0f, int segments = 10) {
    float maxRadius = (width < height ? width : height) * 0.5f;
    if (radius > maxRadius) radius = maxRadius;
    
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(lineWidth);
    
    float angleStep = (M_PI / 2.0f) / segments;
    
    glBegin(GL_LINE_STRIP);
        // Canto superior esquerdo
        for (int i = segments; i >= 0; i--) {
            float angle = M_PI + i * angleStep;
            glVertex2f(x + radius + radius * cos(angle), 
                      y + radius + radius * sin(angle));
        }
        
        // Linha superior
        glVertex2f(x + width - radius, y);
        
        // Canto superior direito
        for (int i = 0; i <= segments; i++) {
            float angle = 1.5f * M_PI + i * angleStep;
            glVertex2f(x + width - radius + radius * cos(angle), 
                      y + radius + radius * sin(angle));
        }
        
        // Linha direita
        glVertex2f(x + width, y + height - radius);
        
        // Canto inferior direito
        for (int i = 0; i <= segments; i++) {
            float angle = i * angleStep;
            glVertex2f(x + width - radius + radius * cos(angle), 
                      y + height - radius + radius * sin(angle));
        }
        
        // Linha inferior
        glVertex2f(x + radius, y + height);
        
        // Canto inferior esquerdo
        for (int i = 0; i <= segments; i++) {
            float angle = 0.5f * M_PI + i * angleStep;
            glVertex2f(x + radius + radius * cos(angle), 
                      y + height - radius + radius * sin(angle));
        }
        
        // Linha esquerda (volta ao início)
        glVertex2f(x, y + radius);
        for (int i = segments; i >= 0; i--) {
            float angle = M_PI + i * angleStep;
            glVertex2f(x + radius + radius * cos(angle), 
                      y + radius + radius * sin(angle));
        }
    glEnd();
    
    glLineWidth(1.0f);
}

/**
 * @brief Desenha um círculo perfeito
 */
inline void drawCircle(float centerX, float centerY, float radius, 
                       const ColorRGBA& color, int segments = 32) {
    glColor4f(color.r, color.g, color.b, color.a);
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, centerY);
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * M_PI * i / segments;
            glVertex2f(centerX + radius * cos(angle), 
                      centerY + radius * sin(angle));
        }
    glEnd();
}

/**
 * @brief Desenha a borda de um círculo
 */
inline void drawCircleOutline(float centerX, float centerY, float radius, 
                              const ColorRGBA& color, float lineWidth = 2.0f, 
                              int segments = 32) {
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(lineWidth);
    
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float angle = 2.0f * M_PI * i / segments;
            glVertex2f(centerX + radius * cos(angle), 
                      centerY + radius * sin(angle));
        }
    glEnd();
    
    glLineWidth(1.0f);
}

/**
 * @brief Desenha um interruptor deslizante (toggle switch)
 * @param isOn Estado do interruptor
 */
inline void drawToggleSwitch(float x, float y, float width, float height, 
                             bool isOn, const ColorRGBA& offColor, 
                             const ColorRGBA& onColor) {
    // Fundo do switch
    ColorRGBA bgColor = isOn ? onColor : offColor;
    drawRoundedRect(x, y, width, height, height * 0.5f, bgColor);
    
    // Círculo deslizante (knob)
    float knobRadius = height * 0.4f;
    float knobX = isOn ? (x + width - height * 0.5f) : (x + height * 0.5f);
    float knobY = y + height * 0.5f;
    
    ColorRGBA knobColor(1.0f, 1.0f, 1.0f, 1.0f);
    drawCircle(knobX, knobY, knobRadius, knobColor);
}

/**
 * @brief Desenha um gradiente vertical
 */
inline void drawVerticalGradient(float x, float y, float width, float height,
                                 const ColorRGBA& topColor, const ColorRGBA& bottomColor) {
    glBegin(GL_QUADS);
        glColor4f(topColor.r, topColor.g, topColor.b, topColor.a);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        
        glColor4f(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.a);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}

/**
 * @brief Desenha uma sombra suave (simulada com retângulos semi-transparentes)
 */
inline void drawShadow(float x, float y, float width, float height, 
                       float radius, float shadowOffset = 4.0f) {
    ColorRGBA shadowColor(0.0f, 0.0f, 0.0f, 0.3f);
    drawRoundedRect(x + shadowOffset, y + shadowOffset, width, height, 
                   radius, shadowColor);
}

/**
 * @brief Desenha um ícone de cubo (wireframe simplificado)
 */
inline void drawCubeIcon(float centerX, float centerY, float size, const ColorRGBA& color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(2.0f);
    
    float hs = size * 0.5f; // Half size
    float offset = size * 0.2f;
    
    // Face frontal
    glBegin(GL_LINE_LOOP);
        glVertex2f(centerX - hs, centerY - hs);
        glVertex2f(centerX + hs, centerY - hs);
        glVertex2f(centerX + hs, centerY + hs);
        glVertex2f(centerX - hs, centerY + hs);
    glEnd();
    
    // Face traseira (offset)
    glBegin(GL_LINE_LOOP);
        glVertex2f(centerX - hs + offset, centerY - hs - offset);
        glVertex2f(centerX + hs + offset, centerY - hs - offset);
        glVertex2f(centerX + hs + offset, centerY + hs - offset);
        glVertex2f(centerX - hs + offset, centerY + hs - offset);
    glEnd();
    
    // Linhas conectando frente e trás
    glBegin(GL_LINES);
        glVertex2f(centerX - hs, centerY - hs);
        glVertex2f(centerX - hs + offset, centerY - hs - offset);
        
        glVertex2f(centerX + hs, centerY - hs);
        glVertex2f(centerX + hs + offset, centerY - hs - offset);
        
        glVertex2f(centerX + hs, centerY + hs);
        glVertex2f(centerX + hs + offset, centerY + hs - offset);
        
        glVertex2f(centerX - hs, centerY + hs);
        glVertex2f(centerX - hs + offset, centerY + hs - offset);
    glEnd();
    
    glLineWidth(1.0f);
}

/**
 * @brief Desenha um ícone de esfera
 */
inline void drawSphereIcon(float centerX, float centerY, float size, const ColorRGBA& color) {
    float radius = size * 0.5f;
    
    // Círculo principal
    drawCircleOutline(centerX, centerY, radius, color, 2.0f);
    
    // Linhas de latitude/longitude
    glColor4f(color.r, color.g, color.b, color.a * 0.5f);
    glLineWidth(1.0f);
    
    // Linha horizontal (equador)
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) {
            float t = i / 20.0f;
            float x = centerX - radius + 2 * radius * t;
            float y = centerY;
            glVertex2f(x, y);
        }
    glEnd();
    
    // Linha vertical (meridiano)
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) {
            float angle = M_PI * i / 20.0f;
            float x = centerX + radius * 0.3f * sin(angle);
            float y = centerY - radius * cos(angle);
            glVertex2f(x, y);
        }
    glEnd();
}

/**
 * @brief Desenha um ícone de pirâmide
 */
inline void drawPyramidIcon(float centerX, float centerY, float size, const ColorRGBA& color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(2.0f);
    
    float hs = size * 0.5f;
    
    // Base
    glBegin(GL_LINE_LOOP);
        glVertex2f(centerX - hs, centerY + hs);
        glVertex2f(centerX + hs, centerY + hs);
        glVertex2f(centerX + hs * 0.7f, centerY + hs * 0.5f);
        glVertex2f(centerX - hs * 0.7f, centerY + hs * 0.5f);
    glEnd();
    
    // Linhas da base ao topo
    float topX = centerX;
    float topY = centerY - hs;
    
    glBegin(GL_LINES);
        glVertex2f(centerX - hs, centerY + hs);
        glVertex2f(topX, topY);
        
        glVertex2f(centerX + hs, centerY + hs);
        glVertex2f(topX, topY);
        
        glVertex2f(centerX - hs * 0.7f, centerY + hs * 0.5f);
        glVertex2f(topX, topY);
    glEnd();
    
    glLineWidth(1.0f);
}

/**
 * @brief Desenha um ícone de cilindro
 */
inline void drawCylinderIcon(float centerX, float centerY, float size, const ColorRGBA& color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(2.0f);
    
    float radius = size * 0.35f;
    float height = size * 0.8f;
    float topY = centerY - height * 0.5f;
    float bottomY = centerY + height * 0.5f;
    
    // Elipse superior
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) {
            float angle = M_PI * i / 20.0f;
            float x = centerX + radius * cos(angle);
            float y = topY + radius * 0.3f * sin(angle);
            glVertex2f(x, y);
        }
    glEnd();
    
    // Elipse inferior
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) {
            float angle = -M_PI + M_PI * i / 20.0f;
            float x = centerX + radius * cos(angle);
            float y = bottomY + radius * 0.3f * sin(angle);
            glVertex2f(x, y);
        }
    glEnd();
    
    // Linhas laterais
    glBegin(GL_LINES);
        glVertex2f(centerX - radius, topY);
        glVertex2f(centerX - radius, bottomY);
        
        glVertex2f(centerX + radius, topY);
        glVertex2f(centerX + radius, bottomY);
    glEnd();
    
    glLineWidth(1.0f);
}

/**
 * @brief Desenha um ícone de sol (para iluminação)
 */
inline void drawSunIcon(float centerX, float centerY, float size, const ColorRGBA& color) {
    float radius = size * 0.3f;
    
    // Círculo central
    drawCircle(centerX, centerY, radius, color);
    
    // Raios
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(2.0f);
    
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * M_PI * i / 8.0f;
        float innerX = centerX + radius * 1.3f * cos(angle);
        float innerY = centerY + radius * 1.3f * sin(angle);
        float outerX = centerX + radius * 1.8f * cos(angle);
        float outerY = centerY + radius * 1.8f * sin(angle);
        
        glBegin(GL_LINES);
            glVertex2f(innerX, innerY);
            glVertex2f(outerX, outerY);
        glEnd();
    }
    
    glLineWidth(1.0f);
}

} // namespace UIPrimitives

#endif // UI_PRIMITIVES_H
