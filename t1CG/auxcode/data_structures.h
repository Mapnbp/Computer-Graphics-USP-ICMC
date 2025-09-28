/**
 * @file data_structures.h
 * @brief Definições das estruturas de dados utilizadas no sistema de preenchimento de polígonos
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <iostream>
#include <windows.h>

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 700;
const int UI_PANEL_HEIGHT = 0;
const int RIGHT_PANEL_WIDTH = 200;
const int DRAWING_AREA_WIDTH = WINDOW_WIDTH - RIGHT_PANEL_WIDTH;
const int DRAWING_AREA_HEIGHT = WINDOW_HEIGHT;

struct WindowDimensions {
    int width;
    int height;
    int drawingAreaWidth;
    int drawingAreaHeight;
    int rightPanelWidth;
    int uiPanelHeight;
    
    WindowDimensions(int w, int h) : width(w), height(h) {
        rightPanelWidth = 200;
        uiPanelHeight = 0;
        drawingAreaWidth = width - rightPanelWidth;
        drawingAreaHeight = height;
    }
};

struct Color16Bit {
    int r, g, b;
    COLORREF colorRef;
    
    Color16Bit(int red, int green, int blue) : r(red), g(green), b(blue) {
        colorRef = RGB(red, green, blue);
    }
};

const Color16Bit COLOR_PALETTE[16] = {
    Color16Bit(0, 0, 0),
    Color16Bit(128, 128, 128),
    Color16Bit(192, 192, 192),
    Color16Bit(255, 255, 255),
    Color16Bit(128, 0, 0),
    Color16Bit(255, 0, 0),
    Color16Bit(255, 128, 0),
    Color16Bit(255, 255, 0),
    Color16Bit(128, 255, 0),
    Color16Bit(0, 255, 0),
    Color16Bit(0, 255, 128),
    Color16Bit(0, 255, 255),
    Color16Bit(0, 128, 255),
    Color16Bit(0, 0, 255),
    Color16Bit(128, 0, 255),
    Color16Bit(255, 0, 255)
};

/**
 * @enum ApplicationState
 * @brief Estados possíveis da aplicação durante o desenho de polígonos
 */
enum class ApplicationState {
    DRAWING_POLYGON,
    POLYGON_FILLED,
    POLYGON_READY
};

/**
 * @struct Point2D
 * @brief Representa um ponto 2D com coordenadas inteiras
 */
struct Point2D {
    int coordinateX;
    int coordinateY;
    
    Point2D(int x = 0, int y = 0) : coordinateX(x), coordinateY(y) {}
    
    bool operator==(const Point2D& other) const {
        return coordinateX == other.coordinateX && coordinateY == other.coordinateY;
    }
};

/**
 * @struct EdgeData
 * @brief Dados de uma aresta para o algoritmo de preenchimento ET/AET
 */
struct EdgeData {
    int maximumY;
    double currentX;
    double inverseSlope;
    int minimumY;

    EdgeData(int maxY, double currentXPos, double invSlope, int minY) 
        : maximumY(maxY), currentX(currentXPos), inverseSlope(invSlope), minimumY(minY) {}
};

typedef std::vector<std::vector<EdgeData>> EdgeTable;

/**
 * @struct ColorRGB
 * @brief Representa uma cor RGB com componentes de 0.0 a 1.0
 */
struct ColorRGB {
    float redComponent;
    float greenComponent;
    float blueComponent;
    
    ColorRGB() : redComponent(1.0f), greenComponent(1.0f), blueComponent(1.0f) {}
    
    ColorRGB(float r, float g, float b) : redComponent(r), greenComponent(g), blueComponent(b) {}
};

/**
 * @struct Button
 * @brief Estrutura para botões da interface
 */
struct Button {
    int x, y, width, height;
    std::string text;
    std::string shortcut;
    bool isPressed;
    bool isEnabled;
    bool isHovered;
    
    Button(int posX, int posY, int w, int h, const std::string& buttonText, const std::string& keyShortcut = "") 
        : x(posX), y(posY), width(w), height(h), text(buttonText), shortcut(keyShortcut),
          isPressed(false), isEnabled(true), isHovered(false) {}
    
    bool isPointInside(int mouseX, int mouseY) const {
        return mouseX >= x && mouseX <= x + width && 
               mouseY >= y && mouseY <= y + height;
    }
};

/**
 * @struct PolygonConfiguration
 * @brief Configurações visuais do polígono
 */
struct PolygonConfiguration {
    ColorRGB lineColor;
    ColorRGB fillColor;
    float lineThickness;
    bool showVertices;
    int selectedColorIndex;
    
    PolygonConfiguration() 
        : lineColor(0.0f, 0.5f, 1.0f),
          fillColor(0.0f, 0.5f, 1.0f),
          lineThickness(2.0f),
          showVertices(true),
          selectedColorIndex(12) {}
};

#endif // DATA_STRUCTURES_H