///////////////////////////////////////////////////////////////////////////////
// shapemeshes.h
// ============
// basic shape mesh generation
//
//  @updated by: Allan Torres
//  @Version 1.0: 02/10/2026
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShapeMeshes {
public:
    ShapeMeshes();
    ~ShapeMeshes();

    void LoadPlaneMesh();
    void DrawPlaneMesh();

    void LoadHollowCylinderMesh();
    void DrawHollowCylinderMesh();

    void LoadTorusMesh();
    void DrawTorusMesh();

    void LoadFlatSphereMesh();
    void DrawFlatSphereMesh();

    void LoadWedgeMesh();
    void DrawWedgeMesh();

    void LoadBoxMesh();
    void DrawBoxMesh();

    void LoadCylinderMesh();
    void DrawCylinderMesh();

    void LoadHemisphereMesh();
    void DrawHemisphereMesh();

    void LoadPentagonalPrismMesh();
    void DrawPentagonalPrismMesh();

private:
    GLuint m_planeVAO = 0;
    GLuint m_planeVBO = 0;
    int m_planeVertexCount = 0;
    GLuint m_hollowCylinderVAO = 0;
    GLuint m_hollowCylinderVBO = 0;
    GLuint m_torusVAO = 0;
    GLuint m_torusVBO = 0;
    GLuint m_flatSphereVAO = 0;
    GLuint m_flatSphereVBO = 0;
    int m_hollowCylinderVertexCount = 0;
    int m_torusVertexCount = 0;
    int m_flatSphereVertexCount = 0;

    GLuint m_wedgeVAO = 0;
    GLuint m_wedgeVBO = 0;
    int m_wedgeVertexCount = 0;

    GLuint m_boxVAO = 0;
    GLuint m_boxVBO = 0;
    int m_boxVertexCount = 0;

    GLuint m_cylinderVAO = 0;
    GLuint m_cylinderVBO = 0;
    int m_cylinderVertexCount = 0;

    GLuint m_hemisphereVAO = 0;
    GLuint m_hemisphereVBO = 0;
    int m_hemisphereVertexCount = 0;

    GLuint m_pentagonPrismVAO = 0;
    GLuint m_pentagonPrismVBO = 0;
    int m_pentagonPrismVertexCount = 0;
};
