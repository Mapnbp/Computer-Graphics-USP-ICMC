/**
```cpp
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
#include <string>
#include <GL/glut.h>
#include <GL/gl.h>

class GraphicsRenderer {
private:
    PolygonFillAlgorithm fillAlgorithm;

public:
    GraphicsRenderer() {}

    void renderPolygon(const std::vector<Point2D>& polygonVertices, 
                      const PolygonConfiguration& configuration,
                      bool isPolygonClosed) const {
        if (polygonVertices.size() < 2) {
            return;
        }
        
        glColor3f(configuration.lineColor.redComponent, configuration.lineColor.greenComponent, configuration.lineColor.blueComponent);
        glLineWidth(configuration.lineThickness);
        
        glBegin(isPolygonClosed ? GL_LINE_LOOP : GL_LINE_STRIP);
        for (const auto& vertex : polygonVertices) {
            glVertex2i(vertex.coordinateX, vertex.coordinateY);
        }
        glEnd();
        
        glLineWidth(1.0f);
    }

    void renderPolygonVertices(const std::vector<Point2D>& polygonVertices, 
                              bool shouldShowVertices) const {
        if (!shouldShowVertices) {
            return;
        }
        
        glColor3f(1.0f, 1.0f, 0.0f);
        glPointSize(6.0f);
        
        glBegin(GL_POINTS);
        for (const Point2D& vertex : polygonVertices) {
            glVertex2i(vertex.coordinateX, vertex.coordinateY);
        }
        glEnd();
        
        glPointSize(1.0f);
    }

    void fillPolygon(const std::vector<Point2D>& polygonVertices, 
                    const ColorRGB& fillColor,
                    int maxHeight,
                    int maxWidth) const {
        if (polygonVertices.size() < 3) {
            return;
        }
        
        fillAlgorithm.fillPolygon(polygonVertices, fillColor, maxHeight, maxWidth);
    }

    // renderText removed

    // renderUserInterface removed (Migrated to Qt)

    void clearDrawingArea(const WindowDimensions* windowDims) const {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRecti(0, 0, windowDims->drawingAreaWidth, windowDims->drawingAreaHeight);
    }

    // UI Rendering methods removed (Migrated to Qt)

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
};

#endif // GRAPHICS_RENDERER_H
