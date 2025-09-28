/**
 * @file graphics_renderer.h
 * @brief Responsável pela renderização gráfica do sistema
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include "data_structures.h"
#include "polygon_fill_algorithm.h"
#include "polygon_manager.h"
#include <windows.h>
#include <string>

/**
 * @class GraphicsRenderer
 * @brief Classe responsável por toda a renderização gráfica do sistema
 */
class GraphicsRenderer {
private:
    HDC deviceContext;
    HWND windowHandle;
    PolygonFillAlgorithm fillAlgorithm;

    COLORREF convertColorToWindowsFormat(const ColorRGB& color) const {
        return RGB(
            static_cast<int>(color.redComponent * 255),
            static_cast<int>(color.greenComponent * 255),
            static_cast<int>(color.blueComponent * 255)
        );
    }

public:
    /**
     * @brief Construtor da classe GraphicsRenderer
     * @param hdc Contexto de dispositivo
     * @param hwnd Handle da janela
     */
    GraphicsRenderer(HDC hdc, HWND hwnd) 
        : deviceContext(hdc), windowHandle(hwnd) {}

    /**
     * @brief Renderiza o polígono com suas linhas
     * @param polygonVertices Vetor com os vértices do polígono
     * @param configuration Configurações visuais do polígono
     * @param isPolygonClosed Se o polígono está fechado
     */
    void renderPolygon(const std::vector<Point2D>& polygonVertices, 
                      const PolygonConfiguration& configuration,
                      bool isPolygonClosed) const {
        if (polygonVertices.size() < 2) {
            return;
        }
        
        COLORREF lineColor = convertColorToWindowsFormat(configuration.lineColor);
        HPEN polygonPen = CreatePen(PS_SOLID, static_cast<int>(configuration.lineThickness), lineColor);
        HPEN originalPen = (HPEN)SelectObject(deviceContext, polygonPen);
        
        MoveToEx(deviceContext, polygonVertices[0].coordinateX, polygonVertices[0].coordinateY, NULL);
        for (size_t vertexIndex = 1; vertexIndex < polygonVertices.size(); vertexIndex++) {
            LineTo(deviceContext, polygonVertices[vertexIndex].coordinateX, polygonVertices[vertexIndex].coordinateY);
        }
        
        if (isPolygonClosed && polygonVertices.size() > 2) {
            LineTo(deviceContext, polygonVertices[0].coordinateX, polygonVertices[0].coordinateY);
        }
        
        SelectObject(deviceContext, originalPen);
        DeleteObject(polygonPen);
    }

    /**
     * @brief Renderiza os vértices do polígono como círculos
     * @param polygonVertices Vetor com os vértices do polígono
     * @param shouldShowVertices Se deve mostrar os vértices
     */
    void renderPolygonVertices(const std::vector<Point2D>& polygonVertices, 
                              bool shouldShowVertices) const {
        if (!shouldShowVertices) {
            return;
        }
        
        HBRUSH vertexBrush = CreateSolidBrush(RGB(255, 255, 0));
        HBRUSH originalBrush = (HBRUSH)SelectObject(deviceContext, vertexBrush);
        
        const int vertexRadius = 3;
        for (const Point2D& vertex : polygonVertices) {
            Ellipse(deviceContext, 
                   vertex.coordinateX - vertexRadius, 
                   vertex.coordinateY - vertexRadius,
                   vertex.coordinateX + vertexRadius, 
                   vertex.coordinateY + vertexRadius);
        }
        
        SelectObject(deviceContext, originalBrush);
        DeleteObject(vertexBrush);
    }

    /**
     * @brief Preenche o polígono usando o algoritmo ET/AET
     * @param polygonVertices Vetor com os vértices do polígono
     * @param fillColor Cor do preenchimento
     * @param maxHeight Altura máxima da área de desenho
     * @param maxWidth Largura máxima da área de desenho
     */
    void fillPolygon(const std::vector<Point2D>& polygonVertices, 
                    const ColorRGB& fillColor,
                    int maxHeight,
                    int maxWidth) const {
        if (polygonVertices.size() < 3) {
            return;
        }
        
        fillAlgorithm.fillPolygon(polygonVertices, fillColor, deviceContext, maxHeight, maxWidth);
    }

    /**
     * @brief Renderiza informações básicas na área de desenho
     * @param vertexCount Número de vértices do polígono
     * @param currentState Estado atual da aplicação
     * @param windowDims Dimensões atuais da janela
     * @param savedPolygonCount Número de polígonos salvos
     */
    void renderUserInterface(size_t vertexCount, ApplicationState currentState, const WindowDimensions* windowDims, size_t savedPolygonCount = 0) const {
        SetTextColor(deviceContext, RGB(255, 255, 255));
        SetBkMode(deviceContext, TRANSPARENT);
        
        std::string stateString;
        switch (currentState) {
            case ApplicationState::DRAWING_POLYGON:
                stateString = "Desenhando";
                break;
            case ApplicationState::POLYGON_FILLED:
                stateString = "Preenchido";
                break;
            case ApplicationState::POLYGON_READY:
                stateString = "Pronto";
                break;
        }
        
        std::string polygonInfo = "Vertices: " + std::to_string(vertexCount) + " | Estado: " + stateString;
        TextOutA(deviceContext, 10, 10, polygonInfo.c_str(), static_cast<int>(polygonInfo.length()));
        
        if (savedPolygonCount > 0) {
            std::string savedInfo = "Poligonos salvos: " + std::to_string(savedPolygonCount);
            TextOutA(deviceContext, 10, 30, savedInfo.c_str(), static_cast<int>(savedInfo.length()));
        }
    }

    /**
     * @brief Limpa a área de desenho com cor preta
     */
    void clearDrawingArea(const WindowDimensions* windowDims) const {
        RECT drawingArea = {0, 0, windowDims->drawingAreaWidth, windowDims->drawingAreaHeight};
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(deviceContext, &drawingArea, blackBrush);
        DeleteObject(blackBrush);
    }
    
    /**
     * @brief Renderiza o painel lateral direito com botões e paleta de cores
     * @param buttons Vetor de botões da interface
     * @param selectedColorIndex Índice da cor selecionada
     * @param windowDims Dimensões atuais da janela
     */
    void renderRightPanel(const std::vector<Button>& buttons, int selectedColorIndex, const WindowDimensions* windowDims) const {
        HBRUSH panelBrush = CreateSolidBrush(RGB(64, 64, 64));
        RECT rightPanel = {windowDims->drawingAreaWidth, 0, windowDims->width, windowDims->height};
        FillRect(deviceContext, &rightPanel, panelBrush);
        DeleteObject(panelBrush);
        
        HFONT pixelFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
        HFONT oldFont = (HFONT)SelectObject(deviceContext, pixelFont);
        
        for (const Button& button : buttons) {
            renderButton(button);
        }
        
        renderColorPalette(selectedColorIndex, windowDims);
        
        SelectObject(deviceContext, oldFont);
        DeleteObject(pixelFont);
    }
    
    /**
     * @brief Renderiza um botão individual
     * @param button Botão a ser renderizado
     */
    void renderButton(const Button& button) const {
        COLORREF buttonColor;
        if (button.isHovered) {
            buttonColor = RGB(120, 120, 120);
        } else if (button.isPressed) {
            buttonColor = RGB(100, 100, 100);
        } else {
            buttonColor = RGB(80, 80, 80);
        }
        
        HBRUSH buttonBrush = CreateSolidBrush(buttonColor);
        
        RECT buttonRect = {button.x, button.y, button.x + button.width, button.y + button.height};
        FillRect(deviceContext, &buttonRect, buttonBrush);
        
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(150, 150, 150));
        HPEN oldPen = (HPEN)SelectObject(deviceContext, borderPen);
        
        MoveToEx(deviceContext, button.x, button.y, NULL);
        LineTo(deviceContext, button.x + button.width, button.y);
        LineTo(deviceContext, button.x + button.width, button.y + button.height);
        LineTo(deviceContext, button.x, button.y + button.height);
        LineTo(deviceContext, button.x, button.y);
        
        HFONT buttonFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Arial");
        HFONT oldFont = (HFONT)SelectObject(deviceContext, buttonFont);
        
        SetTextColor(deviceContext, RGB(255, 255, 255));
        SetBkMode(deviceContext, TRANSPARENT);
        
        RECT textRect = {button.x + 8, button.y + 8, button.x + button.width - 8, button.y + button.height - 20};
        DrawTextA(deviceContext, button.text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        if (!button.shortcut.empty()) {
            SetTextColor(deviceContext, RGB(200, 200, 200));
            RECT shortcutRect = {button.x + 8, button.y + button.height - 18, button.x + button.width - 8, button.y + button.height - 8};
            std::string shortcutText = "[" + button.shortcut + "]";
            DrawTextA(deviceContext, shortcutText.c_str(), -1, &shortcutRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        
        SelectObject(deviceContext, oldFont);
        SelectObject(deviceContext, oldPen);
        DeleteObject(buttonFont);
        DeleteObject(borderPen);
        DeleteObject(buttonBrush);
    }
    
    /**
     * @brief Renderiza a paleta de cores
     * @param selectedColorIndex Índice da cor selecionada
     * @param windowDims Dimensões atuais da janela
     */
    void renderColorPalette(int selectedColorIndex, const WindowDimensions* windowDims) const {
        int paletteX = windowDims->drawingAreaWidth + 10;
        int paletteY = 460;
        int colorSize = 35;
        int colorSpacing = 5;
        
        SetTextColor(deviceContext, RGB(255, 255, 255));
        SetBkMode(deviceContext, TRANSPARENT);
        TextOutA(deviceContext, paletteX, paletteY - 20, "Paleta de Cores", 15);
        
        for (int i = 0; i < 16; i++) {
            int colorX = paletteX + (i % 4) * (colorSize + colorSpacing);
            int colorY = paletteY + (i / 4) * (colorSize + colorSpacing);
            
            COLORREF cellColor = i == selectedColorIndex ? RGB(200, 200, 200) : RGB(100, 100, 100);
            HBRUSH cellBrush = CreateSolidBrush(cellColor);
            RECT cellRect = {colorX - 2, colorY - 2, colorX + colorSize + 2, colorY + colorSize + 2};
            FillRect(deviceContext, &cellRect, cellBrush);
            DeleteObject(cellBrush);
            
            HBRUSH colorBrush = CreateSolidBrush(RGB(COLOR_PALETTE[i].r, COLOR_PALETTE[i].g, COLOR_PALETTE[i].b));
            RECT colorRect = {colorX, colorY, colorX + colorSize, colorY + colorSize};
            FillRect(deviceContext, &colorRect, colorBrush);
            DeleteObject(colorBrush);
            
            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            HPEN oldPen = (HPEN)SelectObject(deviceContext, borderPen);
            
            MoveToEx(deviceContext, colorX, colorY, NULL);
            LineTo(deviceContext, colorX + colorSize, colorY);
            LineTo(deviceContext, colorX + colorSize, colorY + colorSize);
            LineTo(deviceContext, colorX, colorY + colorSize);
            LineTo(deviceContext, colorX, colorY);
            
            SelectObject(deviceContext, oldPen);
            DeleteObject(borderPen);
        }
    }

    /**
     * @brief Renderiza todos os polígonos salvos
     * @param savedPolygons Vetor com polígonos salvos
     * @param maxHeight Altura máxima da área de desenho
     * @param maxWidth Largura máxima da área de desenho
     */
    void renderSavedPolygons(const std::vector<PolygonManager::SavedPolygon>& savedPolygons, 
                           int maxHeight, 
                           int maxWidth) const {
        for (const auto& savedPolygon : savedPolygons) {
            renderPolygon(savedPolygon.vertices, savedPolygon.configuration, true);
            
            if (savedPolygon.isFilled) {
                fillPolygon(savedPolygon.vertices, savedPolygon.configuration.fillColor, maxHeight, maxWidth);
            }
            
            renderPolygonVertices(savedPolygon.vertices, savedPolygon.configuration.showVertices);
        }
    }

    /**
     * @brief Atualiza o contexto de dispositivo
     * @param newDeviceContext Novo contexto de dispositivo
     */
    void updateDeviceContext(HDC newDeviceContext) {
        deviceContext = newDeviceContext;
    }
};

#endif // GRAPHICS_RENDERER_H