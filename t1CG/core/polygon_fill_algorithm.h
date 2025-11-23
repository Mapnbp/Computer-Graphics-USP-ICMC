/**
 * @file polygon_fill_algorithm.h
 * @brief Implementação do algoritmo de preenchimento de polígonos usando ET/AET
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#ifndef POLYGON_FILL_ALGORITHM_H
#define POLYGON_FILL_ALGORITHM_H

#include "data_structures.h"
#include <algorithm>
#include <GL/gl.h> // Adicionado para chamadas OpenGL

/**
 * @class PolygonFillAlgorithm
 * @brief Classe responsável pelo algoritmo de preenchimento de polígonos usando ET/AET
 */
class PolygonFillAlgorithm {
private:
    /**
     * @brief Calcula o inverso da inclinação entre dois pontos
     * @param point1 Primeiro ponto da aresta
     * @param point2 Segundo ponto da aresta
     * @return Inverso da inclinação (1/m) ou 0 se a aresta for horizontal
     */
    double calculateInverseSlope(const Point2D& point1, const Point2D& point2) const {
        double deltaX = static_cast<double>(point2.coordinateX - point1.coordinateX);
        double deltaY = static_cast<double>(point2.coordinateY - point1.coordinateY);
        
        if (deltaY == 0) {
            return 0.0;
        }
        
        return deltaX / deltaY;
    }

public:
    /**
     * @brief Constrói a Edge Table (ET) a partir dos vértices do polígono
     * @param polygonVertices Vetor com os vértices do polígono
     * @param maxHeight Altura máxima da área de desenho
     * @return Edge Table organizada por coordenada Y
     */
    EdgeTable buildEdgeTable(const std::vector<Point2D>& polygonVertices, int maxHeight) const {
        EdgeTable edgeTable(maxHeight);
        
        if (polygonVertices.size() < 2) {
            return edgeTable;
        }

        for (size_t vertexIndex = 0; vertexIndex < polygonVertices.size(); ++vertexIndex) {
            Point2D currentVertex = polygonVertices[vertexIndex];
            Point2D nextVertex = polygonVertices[(vertexIndex + 1) % polygonVertices.size()];

            if (currentVertex.coordinateY == nextVertex.coordinateY) {
                int minY = currentVertex.coordinateY;
                int maxY = nextVertex.coordinateY;
                double initX = static_cast<double>(currentVertex.coordinateX);
                
                if (minY >= 0 && minY < maxHeight) {
                    edgeTable[minY].push_back(
                        EdgeData(maxY, initX, 0.0, minY)
                    );
                }
                continue;
            }

            Point2D minYPoint = currentVertex;
            Point2D maxYPoint = nextVertex;

            if (currentVertex.coordinateY > nextVertex.coordinateY) {
                std::swap(minYPoint, maxYPoint);
            }

            double inverseSlope = calculateInverseSlope(minYPoint, maxYPoint);
            double initialX = static_cast<double>(minYPoint.coordinateX);
            int maximumY = maxYPoint.coordinateY;
            int minimumY = minYPoint.coordinateY;
            
            if (minimumY >= 0 && minimumY < maxHeight) {
                Point2D prevVertex, nextAdjacentVertex;
                
                if (minYPoint.coordinateX == currentVertex.coordinateX && 
                    minYPoint.coordinateY == currentVertex.coordinateY) {
                    prevVertex = polygonVertices[(vertexIndex - 1 + polygonVertices.size()) % polygonVertices.size()];
                    nextAdjacentVertex = nextVertex;
                } else {
                    prevVertex = currentVertex;
                    nextAdjacentVertex = polygonVertices[(vertexIndex + 2) % polygonVertices.size()];
                }
                
                bool prevAbove = prevVertex.coordinateY < minimumY;
                bool nextAbove = nextAdjacentVertex.coordinateY < minimumY;
                
                if (prevAbove == nextAbove) {
                    if (prevAbove) {
                        // Pico: ambos os vértices estão acima
                    } else {
                        // Vale: ambos os vértices estão abaixo
                        minimumY = minimumY + 1;
                        initialX = initialX + inverseSlope;
                    }
                }
            }

            if (minimumY >= 0 && minimumY < maxHeight) {
                edgeTable[minimumY].push_back(
                    EdgeData(maximumY, initialX, inverseSlope, minimumY)
                );
            }
        }

        return edgeTable;
    }

    /**
     * @brief Executa o algoritmo de preenchimento ET/AET
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
        
        EdgeTable edgeTable = buildEdgeTable(polygonVertices, maxHeight);
        
        int currentScanLine = 0;
        while (currentScanLine < edgeTable.size() && edgeTable[currentScanLine].empty()) {
            currentScanLine++;
        }
        
        if (currentScanLine >= edgeTable.size()) {
            return;
        }
        
        std::vector<EdgeData> activeEdgeTable;
        
        glColor3f(fillColor.redComponent, fillColor.greenComponent, fillColor.blueComponent);
        
        while (currentScanLine < edgeTable.size() || !activeEdgeTable.empty()) {
            
            if (currentScanLine < edgeTable.size() && !edgeTable[currentScanLine].empty()) {
                for (const EdgeData& edge : edgeTable[currentScanLine]) {
                    activeEdgeTable.push_back(edge);
                }
            }
            
            std::sort(activeEdgeTable.begin(), activeEdgeTable.end(), 
                [](const EdgeData& edge1, const EdgeData& edge2) {
                    return edge1.currentX < edge2.currentX;
                });
            
            if (activeEdgeTable.size() >= 2) {
                glBegin(GL_LINES); // Usar linhas para preencher o span horizontal
                for (size_t edgeIndex = 0; edgeIndex < activeEdgeTable.size() - 1; edgeIndex += 2) {
                    int x1 = static_cast<int>(activeEdgeTable[edgeIndex].currentX + 0.5);
                    int x2 = static_cast<int>(activeEdgeTable[edgeIndex + 1].currentX + 0.5);
                    
                    if (x1 > x2) {
                        std::swap(x1, x2);
                    }
                    
                    // Clamping
                    if (x1 < 0) x1 = 0;
                    if (x2 >= maxWidth) x2 = maxWidth - 1;
                    
                    if (x1 <= x2 && currentScanLine >= 0 && currentScanLine < maxHeight) {
                         glVertex2i(x1, currentScanLine);
                         glVertex2i(x2 + 1, currentScanLine); 
                    }
                }
                glEnd();
                
                if (activeEdgeTable.size() % 2 == 1) {
                    int x = static_cast<int>(activeEdgeTable[activeEdgeTable.size() - 1].currentX + 0.5);
                    if (x >= 0 && x < maxWidth && currentScanLine >= 0 && currentScanLine < maxHeight) {
                        glBegin(GL_POINTS);
                        glVertex2i(x, currentScanLine);
                        glEnd();
                    }
                }
            }
            
            currentScanLine++;
            
            for (EdgeData& edge : activeEdgeTable) {
                edge.currentX += edge.inverseSlope;
            }
            
            activeEdgeTable.erase(
                std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(), 
                    [currentScanLine](const EdgeData& edge) { 
                        return edge.maximumY <= currentScanLine; 
                    }),
                activeEdgeTable.end()
            );
            
            if (activeEdgeTable.empty() && currentScanLine >= edgeTable.size()) {
                break;
            }
        }
    }
    /**
     * @brief Gera uma triangulação do polígono usando o algoritmo ET/AET (Scanline)
     * @param polygonVertices Vetor com os vértices do polígono
     * @param maxHeight Altura máxima da área de desenho
     * @return Vetor de triângulos, onde cada triângulo é um vetor de 3 Point2D
     */
    std::vector<std::vector<Point2D>> generateTriangulation(const std::vector<Point2D>& polygonVertices, int maxHeight) const {
        std::vector<std::vector<Point2D>> triangles;
        
        if (polygonVertices.size() < 3) {
            return triangles;
        }
        
        EdgeTable edgeTable = buildEdgeTable(polygonVertices, maxHeight);
        
        int currentScanLine = 0;
        while (currentScanLine < edgeTable.size() && edgeTable[currentScanLine].empty()) {
            currentScanLine++;
        }
        
        if (currentScanLine >= edgeTable.size()) {
            return triangles;
        }
        
        std::vector<EdgeData> activeEdgeTable;
        
        while (currentScanLine < edgeTable.size() || !activeEdgeTable.empty()) {
            
            // 1. Mover arestas da ET para AET
            if (currentScanLine < edgeTable.size() && !edgeTable[currentScanLine].empty()) {
                for (const EdgeData& edge : edgeTable[currentScanLine]) {
                    activeEdgeTable.push_back(edge);
                }
            }
            
            // 2. Ordenar AET por X
            std::sort(activeEdgeTable.begin(), activeEdgeTable.end(), 
                [](const EdgeData& edge1, const EdgeData& edge2) {
                    return edge1.currentX < edge2.currentX;
                });
            
            // 3. Gerar triângulos (trapezoides degenerados) para os spans
            // A ideia é conectar o span atual com o span da próxima linha (ou anterior)
            // Mas para simplificar e garantir robustez, vamos gerar "faixas" de 1 pixel de altura
            // Isso gera muitos triângulos, mas resolve o problema de concavidade e auto-interseção
            // Otimização: Poderíamos tentar agrupar linhas, mas a lógica de mudança de topologia é complexa.
            
            if (activeEdgeTable.size() >= 2) {
                for (size_t i = 0; i < activeEdgeTable.size() - 1; i += 2) {
                    double x1_start = activeEdgeTable[i].currentX;
                    double x2_start = activeEdgeTable[i+1].currentX;
                    
                    // Previsão do próximo X (para formar o quad/triângulos)
                    double x1_end = x1_start + activeEdgeTable[i].inverseSlope;
                    double x2_end = x2_start + activeEdgeTable[i+1].inverseSlope;
                    
                    // Criar 2 triângulos para representar esse trapézio de altura 1
                    // T1: (x1_start, y), (x2_start, y), (x1_end, y+1)
                    // T2: (x2_start, y), (x2_end, y+1), (x1_end, y+1)
                    
                    Point2D p1((int)x1_start, currentScanLine);
                    Point2D p2((int)x2_start, currentScanLine);
                    Point2D p3((int)x1_end, currentScanLine + 1);
                    Point2D p4((int)x2_end, currentScanLine + 1);
                    
                    // Adiciona T1
                    triangles.push_back({p1, p2, p3});
                    
                    // Adiciona T2
                    triangles.push_back({p2, p4, p3});
                }
            }
            
            // 4. Incrementar Y e X
            currentScanLine++;
            
            for (EdgeData& edge : activeEdgeTable) {
                edge.currentX += edge.inverseSlope;
            }
            
            // 5. Remover arestas que terminaram
            activeEdgeTable.erase(
                std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(), 
                    [currentScanLine](const EdgeData& edge) { 
                        return edge.maximumY <= currentScanLine; 
                    }),
                activeEdgeTable.end()
            );
            
            if (activeEdgeTable.empty() && currentScanLine >= edgeTable.size()) {
                break;
            }
        }
        
        return triangles;
    }
};

#endif // POLYGON_FILL_ALGORITHM_H