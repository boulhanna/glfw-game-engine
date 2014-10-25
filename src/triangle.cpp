#include "triangle.h"

Triangle::Triangle(GLfloat points[3], GLenum drawType = GL_STATIC_DRAW) :
    points(points)
{
}

Triangle::~Triangle(){}

// TODO: Move most of this logic to constructor?
void Triangle::draw()
{
    // Make vbo to hold points
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Bind vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Transfer data to vbo
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, drawType);

    // Make vao to transfer data
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Bind vao
    glBindVertexArray(vao);

    // Enable position attribute
    // TODO: Make this work for more than position
    glEnableVertexAttribArray(0);

    // Specify attrib pointer
    // TODO: Figure this out
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Draw Triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);
}