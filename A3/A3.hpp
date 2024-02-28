// Termm-Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>

struct LightSource {
    glm::vec3 position;
    glm::vec3 rgbIntensity;
};

struct MatrixStack {
    glm::mat4 M;
    void push(glm::mat4 trans) {
        M = M * trans;
    }
    void pop(glm::mat4 trans) {
        M = M * glm::inverse(trans);
    }
};

enum InterationModes { P, J, None };

class A3 : public CS488Window {
public:
    A3(const std::string & luaSceneFile);
    virtual ~A3();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    //-- Virtual callback methods
    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

    //-- One time initialization methods:
    void processLuaSceneFile(const std::string & filename);
    void createShaderProgram();
    void enableVertexShaderInputSlots();
    void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
    void mapVboDataToVertexShaderInputLocations();
    void initViewMatrix();
    void initLightSources();

    void initPerspectiveMatrix();
    void uploadCommonSceneUniforms();
    void renderSceneGraph(const SceneNode &node);
    void renderSceneNode(const SceneNode &node);
    void renderArcCircle();

    void UpdateSelectedNodeOffset(const float dx, const float dy, const SceneNode & pnode,
                                      const bool rightclick, const bool middleclick);
    void UpdateSelectedNodeOffsetVertical(const float dy, const SceneNode & pnode);
    void UpdateSelectedNodeOffsetHorizontal(const float dx, const SceneNode & pnode);
    void ResetPosition();
    void ResetOrientation();
    void ResetJoints();
    void ResetAll();
    void Quit();
    
    glm::mat4 m_perpsective;
    glm::mat4 m_view;
    
    LightSource m_light;

    //-- GL resources for mesh geometry data:
    GLuint m_vao_meshData;
    GLuint m_vbo_vertexPositions;
    GLuint m_vbo_vertexNormals;
    GLint m_positionAttribLocation;
    GLint m_normalAttribLocation;
    ShaderProgram m_shader;

    //-- GL resources for trackball circle geometry:
    GLuint m_vbo_arcCircle;
    GLuint m_vao_arcCircle;
    GLint m_arc_positionAttribLocation;
    ShaderProgram m_shader_arcCircle;

    // BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
    // object. Each BatchInfo object contains an index offset and the number of indices
    // required to render the mesh with identifier MeshId.
    BatchInfoMap m_batchInfoMap;

    std::string m_luaSceneFile;

    std::shared_ptr<SceneNode> m_rootNode;

    bool opt_show_circle, opt_show_zbuf, opt_show_bf_culling, opt_show_ff_culling;
    bool app_reset_pos, app_reset_ori, app_reset_joints, app_reset_all, app_quit;

    glm::mat4 w_trans;
    glm::vec3 model_translation;
    glm::vec4 init_pos;

    glm::mat4 trackball_rots;
    float model_rotx, model_roty, model_rotz;
    glm::mat3 init_rot;

    InterationModes imode;

    float cursor_delta_x, cursor_delta_y, prev_xpos, prev_ypos;
    bool dx, dy, dz, mouse_button_active;
    bool *selected;
    glm::vec2 *joint_offsets;
};
