#include <stdio.h>

#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include "tiny_obj_loader.h"

#include "application.h"
#include "program.h"
#include "mesh.h"

namespace GGE
{
    Mesh::Mesh(std::vector<GLfloat> points, std::vector<GLfloat> textureCoords,
            Texture * texture, Program *  program, GLenum drawType, GLenum drawShape) :
            points(points),
            textureCoords(textureCoords),
            texture(texture),
            program(program),
            drawType(drawType),
            drawShape(drawShape)
    {
        // Bind program
        program->bind();

        // Make a VBO to hold points
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), &points[0], drawType);

        // VBO TO hold colors
        GLuint colorVbo = 0;
        glGenBuffers(1, &colorVbo);
        glBindBuffer(GL_ARRAY_BUFFER, colorVbo);

        // Make a VAO for the points VBO
        // TODO: Get indices from shaders
        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Color if no texture coordinates
        // TODO: Remove this and find a better way of testing
        if (textureCoords.empty())
        {
            static const float colors[] = {
                    1.0, 1.0, 1.0,
                    0.0, 1.0, 0.0,
                    0.0, 0.0, 1.0
            };

            // Assign data to vbo
            glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
            // Describe data in vao
            GLuint colorIndex = program->getAttrib("color");
            glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(colorIndex);
        }
        else
        {
            // Assign data to vbo
            glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), &textureCoords[0], drawType);
            // Describe data in vao
            GLuint texCoordIndex = program->getAttrib("texCoord");
            glVertexAttribPointer(texCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(texCoordIndex);
        }

        // Points
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLuint positionIndex = program->getAttrib("position");
        glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(positionIndex);

        // Upload model matrix
        GLint uniModel = program->getUniform("model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // Unbind program
        program->unbind();
    }

    Mesh::Mesh(const char *filename, Program * program) :
    program(program),
    drawType(GL_STATIC_DRAW),
    drawShape(GL_TRIANGLES)
    {
        // Vector for shapes
        std::vector<tinyobj::shape_t> shapes;

        // Vector for material support
        // TODO: Integrate material support with rest of program
        std::vector<tinyobj::material_t> materials;

        printf("Loading %s...\n", filename);
        fflush(stdout);

        std::string error = tinyobj::LoadObj(shapes, materials, filename);

        // Print error message
        if(!error.empty())
        {
            fprintf(stderr, "Error Loading %s: %s\n", filename, error.c_str());
            return;
        }

        printf("Loaded %d shapes and %d materials from %s\n", shapes.size(), materials.size(), filename);
        fflush(stdout);

        // Iterate through shapes
        for(int shape = 0; shape < shapes.size(); shape++)
        {
            // Assign vertex values to mesh
            for (int vertex = 0; vertex < shapes[shape].mesh.positions.size() / 3; vertex++)
            {
                // TODO: Figure out how to do this in a better way
                points.push_back(shapes[shape].mesh.positions[3 * vertex + 0]);
                points.push_back(shapes[shape].mesh.positions[3 * vertex + 1]);
                points.push_back(shapes[shape].mesh.positions[3 * vertex + 2]);
            }
        }

        // Standard initialization
        // TODO: Move this to common function

        // Bind program
        program->bind();

        // Make a VBO to hold points
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), &points[0], drawType);

        // VBO TO hold colors
        GLuint colorVbo = 0;
        glGenBuffers(1, &colorVbo);
        glBindBuffer(GL_ARRAY_BUFFER, colorVbo);

        // Make a VAO for the points VBO
        // TODO: Get indices from shaders
        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Color if no texture coordinates
        // TODO: Remove this and find a better way of testing
        if (textureCoords.empty())
        {
            static const float colors[] = {
                    1.0, 1.0, 1.0,
                    0.0, 1.0, 0.0,
                    0.0, 0.0, 1.0
            };

            // Assign data to vbo
            glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
            // Describe data in vao
            GLuint colorIndex = program->getAttrib("color");
            glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(colorIndex);
        }
        else
        {
            // Assign data to vbo
            glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), &textureCoords[0], drawType);
            // Describe data in vao
            GLuint texCoordIndex = program->getAttrib("texCoord");
            glVertexAttribPointer(texCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(texCoordIndex);
        }

        // Points
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLuint positionIndex = program->getAttrib("position");
        glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(positionIndex);

        // Upload model matrix
        GLint uniModel = program->getUniform("model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // Unbind program
        program->unbind();
    }

    Mesh::~Mesh()
    {
    //TODO: Delete vbos and vaos
    }

    void Mesh::draw()
    {
        // Bind program
        program->bind();
        // Bind vao
        glBindVertexArray(vao);
        // TODO: Bind texture here

        // Draw Mesh
        glDrawArrays(drawShape, 0, points.size());

        // TODO: See if unbinding is costly
        // Unbind vao
        glBindVertexArray(0);
        // Unbind program
        program->unbind();
    }

    glm::vec3 Mesh::move(glm::vec3 translation)
    {
        int dims = modelMatrix.length() - 1;
        // Apply translation
        for (int i = 0; i < dims; i++)
            modelMatrix[dims][i] += translation[i];

        // Upload new model matrix
        // TODO: Figure out about binding/unbinding program here
        program->bind();
        GLint uniModel = program->getUniform("model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        program->unbind();

        return Entity::move(translation);
    }

    glm::vec3 Mesh::setPosition(glm::vec3 newPosition)
    {
        // Set new position
        int dims = modelMatrix.length() - 1;
        for (int i = 0; i < dims; i++)
            modelMatrix[dims][i] = newPosition[i];

        // Upload new model matrix
        program->bind();
        GLint uniModel = program->getUniform("model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        program->unbind();

        return Entity::setPosition(newPosition);
    }

    glm::mat4 Mesh::getModelMatrix()
    {
        return modelMatrix;
    }
}