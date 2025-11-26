/**
 * @file scene_manager.h
 * @brief Gerenciamento da cena 3D, câmera e iluminação
 * @author Sistema de Computação Gráfica
 * @date 2025
 */

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include "object_3d.h"
#include "shader_utils.h"
#include "polygon_fill_algorithm.h"

enum class LightingModel {
    FLAT,
    GOURAUD,
    PHONG
};

enum class ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class SceneManager {
private:
    std::vector<Object3D*> objects;
    LightingModel currentLightingModel;
    ProjectionType currentProjection;
    ObjectType currentObjectType;
    
    // Câmera
    Vector3D cameraPosition;
    Vector3D cameraTarget;
    Vector3D cameraUp;

    // Luz
    Vector3D lightPosition;
    ColorRGB lightColor;
    
    // Objeto 3D
    ColorRGB objectColor;

    // Shader Phong
    GLuint phongProgram;
    bool shadersLoaded;

public:
    SceneManager() 
        : currentLightingModel(LightingModel::FLAT),
          currentProjection(ProjectionType::PERSPECTIVE),
          currentObjectType(ObjectType::CUBE),
          cameraPosition(0, 0, 5),
          cameraTarget(0, 0, 0),
          cameraUp(0, 1, 0),
          lightPosition(5, 5, 5),
          lightColor(1, 1, 1),
          objectColor(0.8f, 0.8f, 0.8f),
          phongProgram(0),
          shadersLoaded(false) {}

    ~SceneManager() {
        for (auto obj : objects) {
            delete obj;
        }
        objects.clear();
    }

    void init() {
        // Configurações iniciais OpenGL
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE); // Importante para normais corretas após escala

        // Carregar Shaders se possível
        if (ShaderUtils::loadExtensions()) {
            loadPhongShader();
            shadersLoaded = true;
        } else {
            std::cerr << "Aviso: Nao foi possivel carregar extensoes OpenGL para Shaders." << std::endl;
        }
    }

    void addObject(Object3D* obj) {
        objects.push_back(obj);
    }

    void clearObjects() {
        for (auto obj : objects) delete obj;
        objects.clear();
    }



    void createExtrudedObject(const std::vector<Point2D>& vertices2D, float depth) {
        if (vertices2D.size() < 3) return;

        Object3D* obj = new Object3D();
        
        // Fator de escala para caber na visualizacao (coords tela 0-800 -> coords 3D aprox -4 a 4)
        float scale = 0.01f; 
        
        // --- 0. Calcular Centroide para Centralizar na Origem ---
        float centerX = 0.0f;
        float centerY = 0.0f;
        int n = vertices2D.size();
        
        for (const auto& p : vertices2D) {
            centerX += p.coordinateX;
            centerY += p.coordinateY;
        }
        if (n > 0) {
            centerX /= n;
            centerY /= n;
        }

        // --- 1. Gerar Paredes Laterais (Side Walls) ---
        // Usamos os vértices originais para garantir o contorno correto
        int baseIndex = 0;
        
        // Adicionar vértices para as paredes (frente e trás)
        for (const auto& p : vertices2D) {
            // Frente (Z = +depth/2) - Centralizado
            obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, (depth * scale) / 2.0f);
        }
        for (const auto& p : vertices2D) {
            // Trás (Z = -depth/2) - Centralizado
            obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, -(depth * scale) / 2.0f);
        }
        
        // Criar faces laterais
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            // Quad: i (frente), next (frente), next+n (trás), i+n (trás)
            // Ordem para normal apontar para fora
            std::vector<int> sideFace;
            sideFace.push_back(i);
            sideFace.push_back(next);
            sideFace.push_back(next + n);
            sideFace.push_back(i + n);
            obj->addFace(sideFace);
        }
        
        // --- 2. Gerar Tampas (Caps) usando Triangulação (Scanline) ---
        // Isso resolve problemas de polígonos côncavos e auto-interseção
        PolygonFillAlgorithm algo;
        // Usamos uma altura suficiente para cobrir a tela
        std::vector<std::vector<Point2D>> triangles = algo.generateTriangulation(vertices2D, 2000); 
        
        // Tampa Frontal
        for (const auto& tri : triangles) {
            std::vector<int> faceIndices;
            for (const auto& p : tri) {
                // Centralizado
                obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, (depth * scale) / 2.0f);
                faceIndices.push_back(obj->vertices.size() - 1);
            }
            // A ordem da triangulação do scanline é geralmente consistente, mas precisamos verificar o winding.
            // Scanline varre Y, depois X. 
            // Vamos assumir CCW. Se ficar invertido, trocamos.
            obj->addFace(faceIndices);
        }
        
        // Tampa Traseira
        for (const auto& tri : triangles) {
            std::vector<int> faceIndices;
            // Para a tampa traseira, precisamos inverter a ordem dos vértices para a normal apontar para trás
            for (int i = 2; i >= 0; i--) {
                const auto& p = tri[i];
                // Centralizado
                obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, -(depth * scale) / 2.0f);
                faceIndices.push_back(obj->vertices.size() - 1);
            }
            obj->addFace(faceIndices);
        }
        
        obj->calculateNormals();
        obj->color = ColorRGB(0.7f, 0.7f, 0.7f); // Cor cinza padrao
        
        addObject(obj);
    }

    void setLightingModel(LightingModel model) {
        currentLightingModel = model;
        if (currentLightingModel == LightingModel::PHONG && !shadersLoaded) {
            std::cerr << "Aviso: Phong nao suportado (shaders nao carregados). Usando Gouraud." << std::endl;
            currentLightingModel = LightingModel::GOURAUD;
        }
    }

    void setProjection(ProjectionType proj) {
        currentProjection = proj;
        // updateProjectionMatrix will be called in render loop with correct dimensions
    }

    void setObjectType(ObjectType type) {
        currentObjectType = type;
    }

    void updateProjectionMatrix(int w, int h) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        float aspect = (float)w / (h > 0 ? h : 1);

        if (currentProjection == ProjectionType::PERSPECTIVE) {
            gluPerspective(45.0f, aspect, 0.1f, 100.0f);
        } else {
            float size = 5.0f;
            if (w >= h)
                glOrtho(-size * aspect, size * aspect, -size, size, -100.0f, 100.0f);
            else
                glOrtho(-size, size, -size / aspect, size / aspect, -100.0f, 100.0f);
        }
        glMatrixMode(GL_MODELVIEW);
    }

    void render() {
        // === GARANTIR ESTADO OPENGL CORRETO ===
        // Re-habilitar lighting que pode ter sido desabilitado durante renderização da UI
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        
        // Configurar Câmera
        glLoadIdentity();
        gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
                  cameraTarget.x, cameraTarget.y, cameraTarget.z,
                  cameraUp.x, cameraUp.y, cameraUp.z);

        // Configurar Luz com lightColor dinâmica
        GLfloat lightPos[] = { lightPosition.x, lightPosition.y, lightPosition.z, 1.0f };
        GLfloat lightDiffuse[] = { lightColor.redComponent, lightColor.greenComponent, lightColor.blueComponent, 1.0f };
        GLfloat lightAmbient[] = { lightColor.redComponent * 0.2f, lightColor.greenComponent * 0.2f, lightColor.blueComponent * 0.2f, 1.0f };
        GLfloat lightSpecular[] = { lightColor.redComponent, lightColor.greenComponent, lightColor.blueComponent, 1.0f };
        
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

        // Configurar Material com objectColor dinâmica
        GLfloat matAmbient[] = { objectColor.redComponent * 0.3f, objectColor.greenComponent * 0.3f, objectColor.blueComponent * 0.3f, 1.0f };
        GLfloat matDiffuse[] = { objectColor.redComponent, objectColor.greenComponent, objectColor.blueComponent, 1.0f };
        GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat matShininess[] = { 50.0f };
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

        // Configurar Modelo de Iluminação (Persistência de Estado)
        // Garanta que o glShadeModel seja aplicado a cada frame
        if (currentLightingModel == LightingModel::FLAT) {
            glShadeModel(GL_FLAT);
        } else {
            glShadeModel(GL_SMOOTH); // Gouraud e Phong usam Smooth
        }

        if (currentLightingModel == LightingModel::PHONG) {
            glUseProgram(phongProgram);
        } else {
            glUseProgram(0); // Desativar shader
        }

        // Desenhar Objetos
        bool useFlat = (currentLightingModel == LightingModel::FLAT);
        
        // Se tiver objetos na lista (do extrusor 2D), desenha eles
        // Caso contrário, desenha a primitiva selecionada
        if (!objects.empty()) {
             for (auto obj : objects) {
                obj->draw(useFlat);
            }
        } else {
            // Desenha primitiva baseada no tipo selecionado
            switch (currentObjectType) {
                case ObjectType::CUBE:
                    glutSolidCube(1.5f);
                    break;
                case ObjectType::SPHERE:
                    drawSphere(1.0f, 20, 20);
                    break;
                case ObjectType::CYLINDER:
                    drawCylinder(0.8f, 0.8f, 2.0f, 20, 5);
                    break;
                case ObjectType::PYRAMID:
                    drawPyramid(1.5f);
                    break;
            }
        }
        
        glUseProgram(0);
    }

    // Funções de desenho de primitivas
    void drawSphere(float radius, int slices, int stacks) {
        glutSolidSphere(radius, slices, stacks);
    }

    void drawCylinder(float baseRadius, float topRadius, float height, int slices, int stacks) {
        GLUquadric* quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluQuadricNormals(quad, GLU_SMOOTH);
        
        glPushMatrix();
        glTranslatef(0.0f, -height/2.0f, 0.0f); // Centraliza
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotaciona para ficar em pé
        gluCylinder(quad, baseRadius, topRadius, height, slices, stacks);
        
        // Tampas do cilindro
        gluDisk(quad, 0.0f, baseRadius, slices, 1);
        glTranslatef(0.0f, 0.0f, height);
        gluDisk(quad, 0.0f, topRadius, slices, 1);
        
        glPopMatrix();
        gluDeleteQuadric(quad);
    }

    void drawPyramid(float size) {
        float halfSize = size / 2.0f;
        
        glBegin(GL_TRIANGLES);
            // Frente
            glNormal3f(0.0f, 0.5f, 1.0f);
            glVertex3f(0.0f, halfSize, 0.0f);
            glVertex3f(-halfSize, -halfSize, halfSize);
            glVertex3f(halfSize, -halfSize, halfSize);
            
            // Direita
            glNormal3f(1.0f, 0.5f, 0.0f);
            glVertex3f(0.0f, halfSize, 0.0f);
            glVertex3f(halfSize, -halfSize, halfSize);
            glVertex3f(halfSize, -halfSize, -halfSize);
            
            // Trás
            glNormal3f(0.0f, 0.5f, -1.0f);
            glVertex3f(0.0f, halfSize, 0.0f);
            glVertex3f(halfSize, -halfSize, -halfSize);
            glVertex3f(-halfSize, -halfSize, -halfSize);
            
            // Esquerda
            glNormal3f(-1.0f, 0.5f, 0.0f);
            glVertex3f(0.0f, halfSize, 0.0f);
            glVertex3f(-halfSize, -halfSize, -halfSize);
            glVertex3f(-halfSize, -halfSize, halfSize);
        glEnd();
        
        glBegin(GL_QUADS);
            // Base
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-halfSize, -halfSize, halfSize);
            glVertex3f(halfSize, -halfSize, halfSize);
            glVertex3f(halfSize, -halfSize, -halfSize);
            glVertex3f(-halfSize, -halfSize, -halfSize);
        glEnd();
    }

    // Getters e Setters para Câmera e Luz...
    void setCameraPosition(float x, float y, float z) { cameraPosition = Vector3D(x, y, z); }
    Vector3D getCameraPosition() const { return cameraPosition; }
    
    void setLightPosition(float x, float y, float z) { lightPosition = Vector3D(x, y, z); }
    Vector3D getLightPosition() const { return lightPosition; }
    
    // Métodos para customização de cores 3D
    void setObjectColor(float r, float g, float b) {
        objectColor = ColorRGB(r, g, b);
        // Aplicar a todos os objetos via material properties
        GLfloat mat_diffuse[] = {r, g, b, 1.0f};
        GLfloat mat_ambient[] = {r * 0.2f, g * 0.2f, b * 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    }
    
    void setLightColor(float r, float g, float b) {
        lightColor = ColorRGB(r, g, b);
        GLfloat diffuse[] = {r, g, b, 1.0f};
        GLfloat ambient[] = {r * 0.2f, g * 0.2f, b * 0.2f, 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    }

private:
    void loadPhongShader() {
        std::string vertexShader = 
            "varying vec3 N;\n"
            "varying vec3 v;\n"
            "void main(void) {\n"
            "   v = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
            "   N = normalize(gl_NormalMatrix * gl_Normal);\n"
            "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}";

        std::string fragmentShader = 
            "varying vec3 N;\n"
            "varying vec3 v;\n"
            "void main(void) {\n"
            "   vec3 L = normalize(gl_LightSource[0].position.xyz - v);\n"
            "   vec3 E = normalize(-v);\n" // Vetor do olho
            "   vec3 R = normalize(-reflect(L,N));\n"
            "   vec4 Iamb = gl_FrontLightProduct[0].ambient;\n"
            "   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);\n"
            "   Idiff = clamp(Idiff, 0.0, 1.0);\n"
            "   vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0), 0.3 * gl_FrontMaterial.shininess);\n"
            "   Ispec = clamp(Ispec, 0.0, 1.0);\n"
            "   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;\n"
            "}";
            
        phongProgram = ShaderUtils::createShaderProgram(vertexShader, fragmentShader);
    }
};

#endif // SCENE_MANAGER_H
