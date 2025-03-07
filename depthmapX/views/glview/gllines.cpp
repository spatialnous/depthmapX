// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gllines.hpp"

#include <QtMath>

static const char *vertexShaderSourceCore = //
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 colour;\n"
    "out vec3 col;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   col = colour.xyz;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore = //
    "#version 150\n"
    "in vec3 col;\n"
    "out highp vec3 fragColor;\n"
    "void main() {\n"
    "   fragColor = col;\n"
    "}\n";

static const char *vertexShaderSource = //
    "attribute vec4 vertex;\n"
    "attribute vec3 colour;\n"
    "varying vec3 col;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   col = colour.xyz;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource = //
    "varying highp vec3 col;\n"
    "void main() {\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

/**
 * @brief GLLines::GLLines
 * This class is an OpenGL representation of  multiple lines of uniform colour
 */

GLLines::GLLines() : m_count(0), m_program(0) {}

void GLLines::loadLineData(const std::vector<std::pair<SimpleLine, PafColor>> &colouredLines) {
    m_built = false;

    m_count = 0;
    m_data.resize(
        static_cast<qsizetype>(colouredLines.size() * 2 * static_cast<size_t>(DATA_DIMENSIONS)));

    for (auto &colouredLine : colouredLines) {
        const SimpleLine &line = colouredLine.first;
        const PafColor &colour = colouredLine.second;

        QVector3D colourVector(colour.redf(), colour.greenf(), colour.bluef());
        add(QVector3D(static_cast<float>(line.start().x), static_cast<float>(line.start().y), 0.0f),
            colourVector);
        add(QVector3D(static_cast<float>(line.end().x), static_cast<float>(line.end().y), 0.0f),
            colourVector);
    }
}

void GLLines::setupVertexAttribs() {
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                             DATA_DIMENSIONS * static_cast<GLsizei>(sizeof(GLfloat)), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                             DATA_DIMENSIONS * static_cast<GLsizei>(sizeof(GLfloat)),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void GLLines::initializeGL(bool coreProfile) {
    if (m_data.size() == 0)
        return;
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       coreProfile ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(
        QOpenGLShader::Fragment, coreProfile ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("colour", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(constData(), m_count * static_cast<GLsizei>(sizeof(GLfloat)));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();
    m_program->release();
    m_built = true;
}

void GLLines::updateGL(bool coreProfile) {
    if (m_program == 0) {
        // has not been initialised yet, do that instead
        initializeGL(coreProfile);
    } else {
        m_vbo.bind();
        m_vbo.allocate(constData(), m_count * static_cast<GLsizei>(sizeof(GLfloat)));
        m_vbo.release();
        m_built = true;
    }
}

void GLLines::cleanup() {
    if (!m_built)
        return;
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
}

void GLLines::paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                      const QMatrix4x4 &m_mModel) {
    if (!m_built)
        return;
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_mProj);
    m_program->setUniformValue(m_mvMatrixLoc, m_mView * m_mModel);

    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    glFuncs->glDrawArrays(GL_LINES, 0, vertexCount());

    m_program->release();
}

void GLLines::add(const QVector3D &v, const QVector3D &c) {
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = c.x();
    *p++ = c.y();
    *p++ = c.z();
    m_count += DATA_DIMENSIONS;
}
