/**
 * @file object_3d.h
 * @brief Representação de objetos 3D e estruturas relacionadas
 * @author Sistema de Computação Gráfica
 * @date 2025
 */

#ifndef OBJECT_3D_H
#define OBJECT_3D_H

#include <vector>
#include <cmath>
#include <GL/gl.h>
#include "data_structures.h"

struct Vector3D {
    float x, y, z;

    Vector3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    Vector3D operator+(const Vector3D& v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
    Vector3D operator-(const Vector3D& v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
    Vector3D operator*(float s) const { return Vector3D(x * s, y * s, z * s); }
    
    void normalize() {
        float len = std::sqrt(x*x + y*y + z*z);
        if (len > 0) {
            x /= len; y /= len; z /= len;
        }
    }

    static Vector3D cross(const Vector3D& a, const Vector3D& b) {
        return Vector3D(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
};

struct Vertex3D {
    float x, y, z;
    Vector3D normal; // Normal do vértice (para Gouraud/Phong)

    Vertex3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};

struct Face {
    std::vector<int> vertexIndices; // Índices dos vértices na lista do objeto
    Vector3D normal; // Normal da face (para Flat)

    Face() {}
    Face(const std::vector<int>& indices) : vertexIndices(indices) {}
};

class Object3D {
public:
    std::vector<Vertex3D> vertices;
    std::vector<Face> faces;
    ColorRGB color;
    
    // Transformações
    Vector3D position;
    Vector3D rotation; // Euler angles (x, y, z)
    Vector3D scale;

    Object3D() : position(0,0,0), rotation(0,0,0), scale(1,1,1), color(1.0f, 1.0f, 1.0f) {}

    void addVertex(float x, float y, float z) {
        vertices.emplace_back(x, y, z);
    }

    void addFace(const std::vector<int>& indices) {
        faces.emplace_back(indices);
    }

    void calculateNormals() {
        // 1. Calcular normais das faces
        for (auto& face : faces) {
            if (face.vertexIndices.size() < 3) continue;
            
            Vector3D v0(vertices[face.vertexIndices[0]].x, vertices[face.vertexIndices[0]].y, vertices[face.vertexIndices[0]].z);
            Vector3D v1(vertices[face.vertexIndices[1]].x, vertices[face.vertexIndices[1]].y, vertices[face.vertexIndices[1]].z);
            Vector3D v2(vertices[face.vertexIndices[2]].x, vertices[face.vertexIndices[2]].y, vertices[face.vertexIndices[2]].z);

            Vector3D edge1 = v1 - v0;
            Vector3D edge2 = v2 - v0;
            face.normal = Vector3D::cross(edge1, edge2);
            face.normal.normalize();
        }

        // 2. Calcular normais dos vértices (média das normais das faces adjacentes)
        // Resetar normais dos vértices
        for (auto& v : vertices) {
            v.normal = Vector3D(0, 0, 0);
        }

        // Acumular normais das faces
        for (const auto& face : faces) {
            for (int idx : face.vertexIndices) {
                vertices[idx].normal = vertices[idx].normal + face.normal;
            }
        }

        // Normalizar
        for (auto& v : vertices) {
            v.normal.normalize();
        }
    }

    void draw(bool useFlatShading = false) const {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
        glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
        glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
        glScalef(scale.x, scale.y, scale.z);

        glColor3f(color.redComponent, color.greenComponent, color.blueComponent);

        // Usar GL_POLYGON para suportar faces com > 3 vértices (como quads da extrusão)
        for (const auto& face : faces) {
            glBegin(GL_POLYGON);
            glNormal3f(face.normal.x, face.normal.y, face.normal.z); // Normal da face (Flat)
            
            for (int idx : face.vertexIndices) {
                const auto& v = vertices[idx];
                // Para Gouraud/Phong, enviamos a normal do vértice. Para Flat, NÃO enviamos (usa a da face)
                if (!useFlatShading) {
                    glNormal3f(v.normal.x, v.normal.y, v.normal.z);
                }
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        }

        glPopMatrix();
    }
};

#endif // OBJECT_3D_H
