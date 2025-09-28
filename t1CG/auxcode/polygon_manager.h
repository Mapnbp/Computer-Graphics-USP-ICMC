/**
 * @file polygon_manager.h
 * @brief Gerenciamento de polígonos e suas operações
 * @author Sistema de Preenchimento ET/AET
 * @date 2025
 */

#ifndef POLYGON_MANAGER_H
#define POLYGON_MANAGER_H

#include "data_structures.h"
#include <vector>

/**
 * @class PolygonManager
 * @brief Classe responsável pelo gerenciamento de polígonos e suas operações
 */
class PolygonManager {
private:
    std::vector<Point2D> polygonVertices;
    bool isPolygonClosed;
    PolygonConfiguration visualConfiguration;

public:
    struct SavedPolygon {
        std::vector<Point2D> vertices;
        PolygonConfiguration configuration;
        bool isFilled;
        
        SavedPolygon(const std::vector<Point2D>& verts, const PolygonConfiguration& config, bool filled)
            : vertices(verts), configuration(config), isFilled(filled) {}
    };
    
private:
    std::vector<SavedPolygon> savedPolygons;

public:
    /**
     * @brief Construtor da classe PolygonManager
     */
    PolygonManager() : isPolygonClosed(false) {}

    /**
     * @brief Adiciona um novo vértice ao polígono
     * @param newVertex Ponto a ser adicionado como vértice
     */
    void addVertex(const Point2D& newVertex) {
        polygonVertices.push_back(newVertex);
        isPolygonClosed = false;
    }

    /**
     * @brief Remove o último vértice adicionado ao polígono
     */
    void removeLastVertex() {
        if (!polygonVertices.empty()) {
            polygonVertices.pop_back();
            isPolygonClosed = false;
        }
    }

    /**
     * @brief Fecha o polígono conectando o último vértice ao primeiro
     */
    void closePolygon() {
        if (polygonVertices.size() >= 3) {
            isPolygonClosed = true;
        }
    }

    /**
     * @brief Limpa todos os vértices do polígono
     */
    void clearPolygon() {
        polygonVertices.clear();
        isPolygonClosed = false;
    }

    /**
     * @brief Verifica se o polígono está fechado
     * @return true se o polígono está fechado, false caso contrário
     */
    bool isPolygonCurrentlyClosed() const {
        return isPolygonClosed;
    }

    /**
     * @brief Retorna o número de vértices do polígono
     * @return Número de vértices
     */
    size_t getVertexCount() const {
        return polygonVertices.size();
    }

    /**
     * @brief Retorna uma referência constante aos vértices do polígono
     * @return Referência constante ao vetor de vértices
     */
    const std::vector<Point2D>& getVertices() const {
        return polygonVertices;
    }

    /**
     * @brief Verifica se o polígono pode ser preenchido
     * @return true se o polígono pode ser preenchido, false caso contrário
     */
    bool canBeFilled() const {
        return isPolygonClosed && polygonVertices.size() >= 3;
    }

    /**
     * @brief Define a cor das linhas do polígono
     * @param redComponent Componente vermelho (0.0 - 1.0)
     * @param greenComponent Componente verde (0.0 - 1.0)
     * @param blueComponent Componente azul (0.0 - 1.0)
     */
    void setLineColor(float redComponent, float greenComponent, float blueComponent) {
        visualConfiguration.lineColor = ColorRGB(redComponent, greenComponent, blueComponent);
    }

    /**
     * @brief Define a cor do preenchimento do polígono
     * @param redComponent Componente vermelho (0.0 - 1.0)
     * @param greenComponent Componente verde (0.0 - 1.0)
     * @param blueComponent Componente azul (0.0 - 1.0)
     */
    void setFillColor(float redComponent, float greenComponent, float blueComponent) {
        visualConfiguration.fillColor = ColorRGB(redComponent, greenComponent, blueComponent);
    }

    /**
     * @brief Define a espessura das linhas do polígono
     * @param thickness Nova espessura das linhas
     */
    void setLineThickness(float thickness) {
        visualConfiguration.lineThickness = thickness;
    }

    /**
     * @brief Define se os vértices devem ser mostrados
     * @param shouldShow true para mostrar vértices, false para ocultar
     */
    void setShowVertices(bool shouldShow) {
        visualConfiguration.showVertices = shouldShow;
    }

    /**
     * @brief Retorna as configurações visuais do polígono
     * @return Referência constante às configurações visuais
     */
    const PolygonConfiguration& getVisualConfiguration() const {
        return visualConfiguration;
    }

    /**
     * @brief Retorna a cor de preenchimento atual
     * @return Cor de preenchimento atual
     */
    ColorRGB getCurrentFillColor() const {
        return visualConfiguration.fillColor;
    }

    /**
     * @brief Aplica uma cor predefinida ao preenchimento
     * @param colorIndex Índice da cor (1-6)
     */
    void applyPresetFillColor(int colorIndex) {
        switch (colorIndex) {
            case 1:
                setFillColor(1.0f, 0.0f, 0.0f);
                break;
            case 2:
                setFillColor(0.0f, 1.0f, 0.0f);
                break;
            case 3:
                setFillColor(0.0f, 0.0f, 1.0f);
                break;
            case 4:
                setFillColor(1.0f, 1.0f, 0.0f);
                break;
            case 5:
                setFillColor(1.0f, 0.0f, 1.0f);
                break;
            case 6:
                setFillColor(0.0f, 1.0f, 1.0f);
                break;
            default:
                break;
        }
    }

    /**
     * @brief Ajusta a espessura da linha
     * @param increase true para aumentar, false para diminuir
     */
    void adjustLineThickness(bool increase) {
        if (increase) {
            visualConfiguration.lineThickness = std::min(10.0f, visualConfiguration.lineThickness + 1.0f);
        } else {
            visualConfiguration.lineThickness = std::max(1.0f, visualConfiguration.lineThickness - 1.0f);
        }
    }

    /**
     * @brief Alterna a visibilidade dos vértices
     */
    void toggleVertexVisibility() {
        visualConfiguration.showVertices = !visualConfiguration.showVertices;
    }

    /**
     * @brief Salva o polígono atual como um polígono permanente
     * @param isFilled Indica se o polígono foi preenchido
     */
    void saveCurrentPolygon(bool isFilled = false) {
        if (polygonVertices.size() >= 3 && isPolygonClosed) {
            savedPolygons.push_back(SavedPolygon(polygonVertices, visualConfiguration, isFilled));
            std::cout << "Poligono salvo com " << polygonVertices.size() << " vertices" << std::endl;
        }
    }

    /**
     * @brief Retorna uma referência constante aos polígonos salvos
     * @return Referência constante ao vetor de polígonos salvos
     */
    const std::vector<SavedPolygon>& getSavedPolygons() const {
        return savedPolygons;
    }

    /**
     * @brief Limpa todos os polígonos salvos
     */
    void clearSavedPolygons() {
        savedPolygons.clear();
        std::cout << "Todos os poligonos salvos foram removidos" << std::endl;
    }

    /**
     * @brief Retorna o número de polígonos salvos
     * @return Número de polígonos salvos
     */
    size_t getSavedPolygonCount() const {
        return savedPolygons.size();
    }
};

#endif // POLYGON_MANAGER_H