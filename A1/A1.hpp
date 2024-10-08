// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
    A1();
    virtual ~A1();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

private:
    void initGrid();
    void initCubes();
    void initAvatar();
    void initFloor();
    void resetGridStates();
    void findStartPoint();
    bool noWall(int x, int y);

    // Fields related to the shader and uniforms.
    ShaderProgram m_shader;
    GLint P_uni; // Uniform location for Projection matrix.
    GLint V_uni; // Uniform location for View matrix.
    GLint M_uni; // Uniform location for Model matrix.
    GLint col_uni;   // Uniform location for cube colour.

    // Fields related to grid geometry.
    GLuint m_grid_vao; // Vertex Array Object
    GLuint m_grid_vbo; // Vertex Buffer Object
    GLuint cubesVao;
    GLuint cubesVbo;
    GLuint avatarVao;
    GLuint avatarVbo;
    GLuint floorVao;
    GLuint floorVbo;

    // Matrices controlling the camera and projection.
    glm::mat4 proj;
    glm::mat4 view;

    Maze *m;
    float deltaX = 0.0f;
    float rotation = 0.0f;
    float persistence = 0.0f;
    float scaleSize = 1.0f;
    float widgetColour[3];
    float colourF[3];
    float colourW[3];
    float colourA[3];
    float xPrev;
    int last_col;
    int current_col;
    int cubeCount;        
    int sectorCount = 20;
    int stackCount = 20;
    float cubeHeight;
    int  avatarPos[3];
    bool mouseButtonActive;
};
