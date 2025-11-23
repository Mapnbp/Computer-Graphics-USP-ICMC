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
        
        // Calcula o centroide para centralizar o objeto
        float minX = (float)vertices2D[0].coordinateX, maxX = (float)vertices2D[0].coordinateX;
        float minY = (float)vertices2D[0].coordinateY, maxY = (float)vertices2D[0].coordinateY;
        
        for (const auto& p : vertices2D) {
            if (p.coordinateX < minX) minX = (float)p.coordinateX;
            if (p.coordinateX > maxX) maxX = (float)p.coordinateX;
            if (p.coordinateY < minY) minY = (float)p.coordinateY;
            if (p.coordinateY > maxY) maxY = (float)p.coordinateY;
        }
        
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;
        
        // Fator de escala para caber na visualizacao (coords tela 0-800 -> coords 3D aprox -4 a 4)
        float scale = 0.01f; 
        
        int n = vertices2D.size();
        
        // Vertices da face frontal (z = +depth/2)
        for (const auto& p : vertices2D) {
            // Inverte Y porque Y da tela e para baixo, Y 3D e para cima
            obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, (depth * scale) / 2.0f);
        }
        
        // Vertices da face traseira (z = -depth/2)
        for (const auto& p : vertices2D) {
            obj->addVertex((p.coordinateX - centerX) * scale, -(p.coordinateY - centerY) * scale, -(depth * scale) / 2.0f);
        }
        
        // Front face
        std::vector<int> frontFaceIndices;
        for (int i = 0; i < n; i++) frontFaceIndices.push_back(i);
        obj->addFace(frontFaceIndices);
        
        // Face traseira (ordem reversa para apontar para fora)
        std::vector<int> backFaceIndices;
        for (int i = 0; i < n; i++) backFaceIndices.push_back(n + (n - 1 - i));
        obj->addFace(backFaceIndices);
        
        // Side faces
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            // Quad: i, next, next+n, i+n
            std::vector<int> sideFace;
            sideFace.push_back(i);
            sideFace.push_back(next);
            sideFace.push_back(next + n);
            sideFace.push_back(i + n);
            obj->addFace(sideFace);
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

        // Configurar Modelo de Iluminação
        if (currentLightingModel == LightingModel::PHONG) {
            glUseProgram(phongProgram);
        } else {
            glUseProgram(0); // Desativar shader
            if (currentLightingModel == LightingModel::FLAT) {
                glShadeModel(GL_FLAT);
            } else {
                glShadeModel(GL_SMOOTH); // Gouraud
            }
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
