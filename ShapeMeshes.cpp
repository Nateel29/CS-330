///////////////////////////////////////////////////////////////////////////////
// shapemeshes.cpp
// ============
// basic shape mesh generation
//
//  @updated by: Allan Torres
//  @Version 1.0: 02/10/2026
/////////////////////////////////////////////////////////////////////////////

#include "ShapeMeshes.h"
#include <vector>
#include <cmath>

namespace
{
    constexpr float kPi = 3.14159265358979323846f;
}

ShapeMeshes::ShapeMeshes() {}
ShapeMeshes::~ShapeMeshes() {}

void ShapeMeshes::LoadPlaneMesh() {
    // Simple quad for table top with normals and UVs
    float vertices[] = {
        // positions          // normals         // UVs
        -0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };
    m_planeVertexCount = 6;
    glGenVertexArrays(1, &m_planeVAO);
    glGenBuffers(1, &m_planeVBO);
    glBindVertexArray(m_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}
void ShapeMeshes::DrawPlaneMesh() {
    glBindVertexArray(m_planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_planeVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadHollowCylinderMesh() {
    const int segments = 64;
    const float height = 1.0f;
    const float halfHeight = height * 0.5f;
    const float outerRadius = 1.0f;
    const float innerRadius = 0.8f;

    std::vector<float> vertices;
    vertices.reserve(segments * 6 * 2 * 8);

    for (int i = 0; i < segments; ++i) {
        float u0 = static_cast<float>(i) / static_cast<float>(segments);
        float u1 = static_cast<float>(i + 1) / static_cast<float>(segments);
        float angle0 = u0 * 2.0f * kPi;
        float angle1 = u1 * 2.0f * kPi;

        float c0 = cosf(angle0);
        float s0 = sinf(angle0);
        float c1 = cosf(angle1);
        float s1 = sinf(angle1);

        // Outer surface
        float p0x = outerRadius * c0;
        float p0z = outerRadius * s0;
        float p1x = outerRadius * c1;
        float p1z = outerRadius * s1;

        glm::vec3 n0(c0, 0.0f, s0);
        glm::vec3 n1(c1, 0.0f, s1);

        // Triangle 1
        vertices.insert(vertices.end(), { p0x, -halfHeight, p0z, n0.x, n0.y, n0.z, u0, 0.0f });
        vertices.insert(vertices.end(), { p1x, -halfHeight, p1z, n1.x, n1.y, n1.z, u1, 0.0f });
        vertices.insert(vertices.end(), { p1x,  halfHeight, p1z, n1.x, n1.y, n1.z, u1, 1.0f });
        // Triangle 2
        vertices.insert(vertices.end(), { p0x, -halfHeight, p0z, n0.x, n0.y, n0.z, u0, 0.0f });
        vertices.insert(vertices.end(), { p1x,  halfHeight, p1z, n1.x, n1.y, n1.z, u1, 1.0f });
        vertices.insert(vertices.end(), { p0x,  halfHeight, p0z, n0.x, n0.y, n0.z, u0, 1.0f });

        // Inner surface (normals inward)
        float ip0x = innerRadius * c0;
        float ip0z = innerRadius * s0;
        float ip1x = innerRadius * c1;
        float ip1z = innerRadius * s1;

        glm::vec3 in0(-c0, 0.0f, -s0);
        glm::vec3 in1(-c1, 0.0f, -s1);

        vertices.insert(vertices.end(), { ip0x, -halfHeight, ip0z, in0.x, in0.y, in0.z, u0, 0.0f });
        vertices.insert(vertices.end(), { ip1x,  halfHeight, ip1z, in1.x, in1.y, in1.z, u1, 1.0f });
        vertices.insert(vertices.end(), { ip1x, -halfHeight, ip1z, in1.x, in1.y, in1.z, u1, 0.0f });
        vertices.insert(vertices.end(), { ip0x, -halfHeight, ip0z, in0.x, in0.y, in0.z, u0, 0.0f });
        vertices.insert(vertices.end(), { ip0x,  halfHeight, ip0z, in0.x, in0.y, in0.z, u0, 1.0f });
        vertices.insert(vertices.end(), { ip1x,  halfHeight, ip1z, in1.x, in1.y, in1.z, u1, 1.0f });
    }

    m_hollowCylinderVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_hollowCylinderVAO);
    glGenBuffers(1, &m_hollowCylinderVBO);
    glBindVertexArray(m_hollowCylinderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_hollowCylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawHollowCylinderMesh() {
    glBindVertexArray(m_hollowCylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_hollowCylinderVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadTorusMesh() {
    const int majorSegments = 64;
    const int minorSegments = 24;
    const float majorRadius = 0.9f;
    const float minorRadius = 0.15f;

    std::vector<float> vertices;
    vertices.reserve(majorSegments * minorSegments * 6 * 8);

    const int majorStart = 0;
    const int majorEnd = majorSegments / 2; // render half a torus for a handle opening

    for (int i = majorStart; i < majorEnd; ++i) {
        float u0 = static_cast<float>(i) / static_cast<float>(majorSegments);
        float u1 = static_cast<float>(i + 1) / static_cast<float>(majorSegments);
        float angle0 = u0 * 2.0f * kPi;
        float angle1 = u1 * 2.0f * kPi;

        for (int j = 0; j < minorSegments; ++j) {
            float v0 = static_cast<float>(j) / static_cast<float>(minorSegments);
            float v1 = static_cast<float>(j + 1) / static_cast<float>(minorSegments);
            float angleV0 = v0 * 2.0f * kPi;
            float angleV1 = v1 * 2.0f * kPi;

            auto makeVertex = [&](float u, float v, float angleU, float angleV) {
                float cosU = cosf(angleU);
                float sinU = sinf(angleU);
                float cosV = cosf(angleV);
                float sinV = sinf(angleV);

                float x = (majorRadius + minorRadius * cosV) * cosU;
                float y = minorRadius * sinV;
                float z = (majorRadius + minorRadius * cosV) * sinU;

                glm::vec3 normal(cosU * cosV, sinV, sinU * cosV);
                normal = glm::normalize(normal);

                vertices.insert(vertices.end(), { x, y, z, normal.x, normal.y, normal.z, u, v });
            };

            // Triangle 1
            makeVertex(u0, v0, angle0, angleV0);
            makeVertex(u1, v0, angle1, angleV0);
            makeVertex(u1, v1, angle1, angleV1);
            // Triangle 2
            makeVertex(u0, v0, angle0, angleV0);
            makeVertex(u1, v1, angle1, angleV1);
            makeVertex(u0, v1, angle0, angleV1);
        }
    }

    m_torusVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_torusVAO);
    glGenBuffers(1, &m_torusVBO);
    glBindVertexArray(m_torusVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_torusVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawTorusMesh() {
    glBindVertexArray(m_torusVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_torusVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadFlatSphereMesh() {
    const int latitudeSegments = 24;
    const int longitudeSegments = 48;
    const float radius = 0.5f;

    std::vector<float> vertices;
    vertices.reserve(latitudeSegments * longitudeSegments * 6 * 8);

    for (int i = 0; i < latitudeSegments; ++i) {
        float v0 = static_cast<float>(i) / static_cast<float>(latitudeSegments);
        float v1 = static_cast<float>(i + 1) / static_cast<float>(latitudeSegments);
        float theta0 = v0 * kPi;
        float theta1 = v1 * kPi;

        for (int j = 0; j < longitudeSegments; ++j) {
            float u0 = static_cast<float>(j) / static_cast<float>(longitudeSegments);
            float u1 = static_cast<float>(j + 1) / static_cast<float>(longitudeSegments);
            float phi0 = u0 * 2.0f * kPi;
            float phi1 = u1 * 2.0f * kPi;

            auto makeVertex = [&](float u, float v, float theta, float phi) {
                float sinTheta = sinf(theta);
                float cosTheta = cosf(theta);
                float sinPhi = sinf(phi);
                float cosPhi = cosf(phi);

                float x = radius * sinTheta * cosPhi;
                float y = radius * cosTheta;
                float z = radius * sinTheta * sinPhi;

                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
                vertices.insert(vertices.end(), { x, y, z, normal.x, normal.y, normal.z, u, v });
            };

            // Triangle 1
            makeVertex(u0, v0, theta0, phi0);
            makeVertex(u1, v0, theta0, phi1);
            makeVertex(u1, v1, theta1, phi1);
            // Triangle 2
            makeVertex(u0, v0, theta0, phi0);
            makeVertex(u1, v1, theta1, phi1);
            makeVertex(u0, v1, theta1, phi0);
        }
    }

    m_flatSphereVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_flatSphereVAO);
    glGenBuffers(1, &m_flatSphereVBO);
    glBindVertexArray(m_flatSphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_flatSphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawFlatSphereMesh() {
    glBindVertexArray(m_flatSphereVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_flatSphereVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadWedgeMesh() {
    const float slopeNormalY = 0.70710678f;
    const float slopeNormalZ = -0.70710678f;

    float vertices[] = {
        // Bottom face (y = 0)
        -0.5f, 0.0f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

        // Top sloped face
        -0.5f, 0.0f, -0.5f,   0.0f, slopeNormalY, slopeNormalZ,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, slopeNormalY, slopeNormalZ,  1.0f, 0.0f,
         0.5f, 1.0f,  0.5f,   0.0f, slopeNormalY, slopeNormalZ,  1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, slopeNormalY, slopeNormalZ,  0.0f, 0.0f,
         0.5f, 1.0f,  0.5f,   0.0f, slopeNormalY, slopeNormalZ,  1.0f, 1.0f,
        -0.5f, 1.0f,  0.5f,   0.0f, slopeNormalY, slopeNormalZ,  0.0f, 1.0f,

        // Back face (z = 0.5)
        -0.5f, 0.0f,  0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f, 1.0f,  0.5f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, 1.0f,  0.5f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, 1.0f,  0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

        // Left face (x = -0.5)
        -0.5f, 0.0f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, 1.0f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

        // Right face (x = 0.5)
         0.5f, 0.0f, -0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 1.0f,  0.5f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, 0.0f,  0.5f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f
    };

    m_wedgeVertexCount = static_cast<int>(sizeof(vertices) / (8 * sizeof(float)));

    glGenVertexArrays(1, &m_wedgeVAO);
    glGenBuffers(1, &m_wedgeVBO);
    glBindVertexArray(m_wedgeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_wedgeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawWedgeMesh() {
    glBindVertexArray(m_wedgeVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_wedgeVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadBoxMesh() {
    // Unit box centered at origin with per-face normals and UVs.
    float vertices[] = {
        // Front face (+Z)
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

        // Back face (-Z)
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,

        // Left face (-X)
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

        // Right face (+X)
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

        // Top face (+Y)
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

        // Bottom face (-Y)
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f
    };

    m_boxVertexCount = static_cast<int>(sizeof(vertices) / (8 * sizeof(float)));

    glGenVertexArrays(1, &m_boxVAO);
    glGenBuffers(1, &m_boxVBO);
    glBindVertexArray(m_boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawBoxMesh() {
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_boxVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadCylinderMesh() {
    const int segments = 64;
    const float radius = 0.5f;
    const float halfLength = 0.5f;

    std::vector<float> vertices;
    vertices.reserve(segments * 6 * 8);

    for (int i = 0; i < segments; ++i) {
        float u0 = static_cast<float>(i) / static_cast<float>(segments);
        float u1 = static_cast<float>(i + 1) / static_cast<float>(segments);
        float angle0 = u0 * 2.0f * kPi;
        float angle1 = u1 * 2.0f * kPi;

        float y0 = radius * cosf(angle0);
        float z0 = radius * sinf(angle0);
        float y1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);

        glm::vec3 n0(0.0f, cosf(angle0), sinf(angle0));
        glm::vec3 n1(0.0f, cosf(angle1), sinf(angle1));

        // Triangle 1
        vertices.insert(vertices.end(), { -halfLength, y0, z0, n0.x, n0.y, n0.z, u0, 0.0f });
        vertices.insert(vertices.end(), {  halfLength, y0, z0, n0.x, n0.y, n0.z, u0, 1.0f });
        vertices.insert(vertices.end(), {  halfLength, y1, z1, n1.x, n1.y, n1.z, u1, 1.0f });
        // Triangle 2
        vertices.insert(vertices.end(), { -halfLength, y0, z0, n0.x, n0.y, n0.z, u0, 0.0f });
        vertices.insert(vertices.end(), {  halfLength, y1, z1, n1.x, n1.y, n1.z, u1, 1.0f });
        vertices.insert(vertices.end(), { -halfLength, y1, z1, n1.x, n1.y, n1.z, u1, 0.0f });
    }

    m_cylinderVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_cylinderVAO);
    glGenBuffers(1, &m_cylinderVBO);
    glBindVertexArray(m_cylinderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawCylinderMesh() {
    glBindVertexArray(m_cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_cylinderVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadHemisphereMesh() {
    const int latitudeSegments = 16;
    const int longitudeSegments = 48;
    const float radius = 0.5f;

    std::vector<float> vertices;
    vertices.reserve(latitudeSegments * longitudeSegments * 6 * 8);

    for (int i = 0; i < latitudeSegments; ++i) {
        float v0 = static_cast<float>(i) / static_cast<float>(latitudeSegments);
        float v1 = static_cast<float>(i + 1) / static_cast<float>(latitudeSegments);
        float phi0 = v0 * (kPi * 0.5f);
        float phi1 = v1 * (kPi * 0.5f);

        for (int j = 0; j < longitudeSegments; ++j) {
            float u0 = static_cast<float>(j) / static_cast<float>(longitudeSegments);
            float u1 = static_cast<float>(j + 1) / static_cast<float>(longitudeSegments);
            float theta0 = u0 * 2.0f * kPi;
            float theta1 = u1 * 2.0f * kPi;

            auto makeVertex = [&](float u, float v, float phi, float theta) {
                float cosPhi = cosf(phi);
                float sinPhi = sinf(phi);
                float cosTheta = cosf(theta);
                float sinTheta = sinf(theta);

                float x = radius * cosPhi;
                float y = radius * sinPhi * cosTheta;
                float z = radius * sinPhi * sinTheta;

                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
                vertices.insert(vertices.end(), { x, y, z, normal.x, normal.y, normal.z, u, v });
            };

            // Triangle 1
            makeVertex(u0, v0, phi0, theta0);
            makeVertex(u1, v0, phi0, theta1);
            makeVertex(u1, v1, phi1, theta1);
            // Triangle 2
            makeVertex(u0, v0, phi0, theta0);
            makeVertex(u1, v1, phi1, theta1);
            makeVertex(u0, v1, phi1, theta0);
        }
    }

    m_hemisphereVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_hemisphereVAO);
    glGenBuffers(1, &m_hemisphereVBO);
    glBindVertexArray(m_hemisphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_hemisphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawHemisphereMesh() {
    glBindVertexArray(m_hemisphereVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_hemisphereVertexCount);
    glBindVertexArray(0);
}

void ShapeMeshes::LoadPentagonalPrismMesh() {
    const int sides = 5;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;

    std::vector<glm::vec3> ring;
    ring.reserve(sides);

    for (int i = 0; i < sides; ++i)
    {
        float angle = glm::radians(90.0f - (static_cast<float>(i) * 72.0f));
        ring.emplace_back(radius * cosf(angle), 0.0f, radius * sinf(angle));
    }

    std::vector<float> vertices;
    vertices.reserve(sides * 12 * 8);

    // Side faces
    for (int i = 0; i < sides; ++i)
    {
        int next = (i + 1) % sides;
        glm::vec3 p0 = ring[i];
        glm::vec3 p1 = ring[next];
        glm::vec3 normal = glm::normalize(glm::vec3(p0.z - p1.z, 0.0f, p1.x - p0.x));

        glm::vec3 v00(p0.x, -halfHeight, p0.z);
        glm::vec3 v01(p1.x, -halfHeight, p1.z);
        glm::vec3 v10(p0.x,  halfHeight, p0.z);
        glm::vec3 v11(p1.x,  halfHeight, p1.z);

        float u0 = static_cast<float>(i) / static_cast<float>(sides);
        float u1 = static_cast<float>(i + 1) / static_cast<float>(sides);

        // Triangle 1
        vertices.insert(vertices.end(), { v00.x, v00.y, v00.z, normal.x, normal.y, normal.z, u0, 0.0f });
        vertices.insert(vertices.end(), { v11.x, v11.y, v11.z, normal.x, normal.y, normal.z, u1, 1.0f });
        vertices.insert(vertices.end(), { v10.x, v10.y, v10.z, normal.x, normal.y, normal.z, u0, 1.0f });
        // Triangle 2
        vertices.insert(vertices.end(), { v00.x, v00.y, v00.z, normal.x, normal.y, normal.z, u0, 0.0f });
        vertices.insert(vertices.end(), { v01.x, v01.y, v01.z, normal.x, normal.y, normal.z, u1, 0.0f });
        vertices.insert(vertices.end(), { v11.x, v11.y, v11.z, normal.x, normal.y, normal.z, u1, 1.0f });
    }

    // Top cap
    glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
    glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < sides; ++i)
    {
        int next = (i + 1) % sides;
        glm::vec3 p0 = ring[i];
        glm::vec3 p1 = ring[next];

        float u0 = (p0.x / radius + 1.0f) * 0.5f;
        float v0 = (p0.z / radius + 1.0f) * 0.5f;
        float u1 = (p1.x / radius + 1.0f) * 0.5f;
        float v1 = (p1.z / radius + 1.0f) * 0.5f;

        vertices.insert(vertices.end(), { topCenter.x, topCenter.y, topCenter.z, topNormal.x, topNormal.y, topNormal.z, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { p0.x, halfHeight, p0.z, topNormal.x, topNormal.y, topNormal.z, u0, v0 });
        vertices.insert(vertices.end(), { p1.x, halfHeight, p1.z, topNormal.x, topNormal.y, topNormal.z, u1, v1 });
    }

    // Bottom cap
    glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
    glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
    for (int i = 0; i < sides; ++i)
    {
        int next = (i + 1) % sides;
        glm::vec3 p0 = ring[i];
        glm::vec3 p1 = ring[next];

        float u0 = (p0.x / radius + 1.0f) * 0.5f;
        float v0 = (p0.z / radius + 1.0f) * 0.5f;
        float u1 = (p1.x / radius + 1.0f) * 0.5f;
        float v1 = (p1.z / radius + 1.0f) * 0.5f;

        vertices.insert(vertices.end(), { bottomCenter.x, bottomCenter.y, bottomCenter.z, bottomNormal.x, bottomNormal.y, bottomNormal.z, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { p1.x, -halfHeight, p1.z, bottomNormal.x, bottomNormal.y, bottomNormal.z, u1, v1 });
        vertices.insert(vertices.end(), { p0.x, -halfHeight, p0.z, bottomNormal.x, bottomNormal.y, bottomNormal.z, u0, v0 });
    }

    m_pentagonPrismVertexCount = static_cast<int>(vertices.size() / 8);

    glGenVertexArrays(1, &m_pentagonPrismVAO);
    glGenBuffers(1, &m_pentagonPrismVBO);
    glBindVertexArray(m_pentagonPrismVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pentagonPrismVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ShapeMeshes::DrawPentagonalPrismMesh() {
    glBindVertexArray(m_pentagonPrismVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_pentagonPrismVertexCount);
    glBindVertexArray(0);
}
