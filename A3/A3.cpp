// Termm-Fall 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

static MatrixStack matrixStack;

static bool do_picking = false;

/*******************************************************
 * NOTE: this code is taken from trackball.c in Trackball_Example 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}


//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
    : m_luaSceneFile(luaSceneFile),
      m_positionAttribLocation(0),
      m_normalAttribLocation(0),
      m_vao_meshData(0),
      m_vbo_vertexPositions(0),
      m_vbo_vertexNormals(0),
      m_vao_arcCircle(0),
      m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
    // Set the background colour.
    glClearColor(0.369, 0.322, 0.322, 1.0);

    createShaderProgram();

    glGenVertexArrays(1, &m_vao_arcCircle);
    glGenVertexArrays(1, &m_vao_meshData);
    enableVertexShaderInputSlots();

    processLuaSceneFile(m_luaSceneFile);

    // Load and decode all .obj files at once here.  You may add additional .obj files to
    // this list in order to support rendering additional mesh types.  All vertex
    // positions, and normals will be extracted and stored within the MeshConsolidator
    // class.
    unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
            getAssetFilePath("cube.obj"),
            getAssetFilePath("sphere.obj"),
            getAssetFilePath("suzanne.obj")
    });


    // Acquire the BatchInfoMap from the MeshConsolidator.
    meshConsolidator->getBatchInfoMap(m_batchInfoMap);

    // Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
    uploadVertexDataToVbos(*meshConsolidator);

    mapVboDataToVertexShaderInputLocations();

    initPerspectiveMatrix();

    initViewMatrix();

    initLightSources();

    matrixStack.M = mat4(1.);
        
    // Exiting the current scope calls delete automatically on meshConsolidator freeing
    // all vertex data resources.  This is fine since we already copied this data to
    // VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
    // this point.

    ResetAll();
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
    // This version of the code treats the Lua file as an Asset,
    // so that you'd launch the program with just the filename
    // of a puppet in the Assets/ directory.
    // std::string assetFilePath = getAssetFilePath(filename.c_str());
    // m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

    // This version of the code treats the main program argument
    // as a straightforward pathname.
    m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
    if (!m_rootNode) {
        std::cerr << "Could Not Open " << filename << std::endl;
    }
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
    m_shader.generateProgramObject();
    m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
    m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
    m_shader.link();

    m_shader_arcCircle.generateProgramObject();
    m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
    m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
    m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
    //-- Enable input slots for m_vao_meshData:
    {
        glBindVertexArray(m_vao_meshData);

        // Enable the vertex shader attribute location for "position" when rendering.
        m_positionAttribLocation = m_shader.getAttribLocation("position");
        glEnableVertexAttribArray(m_positionAttribLocation);

        // Enable the vertex shader attribute location for "normal" when rendering.
        m_normalAttribLocation = m_shader.getAttribLocation("normal");
        glEnableVertexAttribArray(m_normalAttribLocation);

        CHECK_GL_ERRORS;
    }


    //-- Enable input slots for m_vao_arcCircle:
    {
        glBindVertexArray(m_vao_arcCircle);

        // Enable the vertex shader attribute location for "position" when rendering.
        m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
        glEnableVertexAttribArray(m_arc_positionAttribLocation);

        CHECK_GL_ERRORS;
    }

    // Restore defaults
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
        const MeshConsolidator & meshConsolidator
) {
    // Generate VBO to store all vertex position data
    {
        glGenBuffers(1, &m_vbo_vertexPositions);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

        glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
                meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }

    // Generate VBO to store all vertex normal data
    {
        glGenBuffers(1, &m_vbo_vertexNormals);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

        glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
                meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }

    // Generate VBO to store the trackball circle.
    {
        glGenBuffers( 1, &m_vbo_arcCircle );
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

        float *pts = new float[ 2 * CIRCLE_PTS ];
        for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
            float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
            pts[2*idx] = cos( ang );
            pts[2*idx+1] = sin( ang );
        }

        glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
    // Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao_meshData);

    // Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
    // "position" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
    glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
    // "normal" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
    glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;

    // Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao_arcCircle);

    // Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
    // "position" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
    glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
    float aspect = ((float)m_windowWidth) / m_windowHeight;
    m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
    m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
    // World-space position
    m_light.position = vec3(10.0f, 10.0f, 10.0f);
    m_light.rgbIntensity = vec3(0.5); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
    m_shader.enable();
    {
        //-- Set Perpsective matrix uniform for the scene:
        GLint location = m_shader.getUniformLocation("Perspective");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
        CHECK_GL_ERRORS;

        location = m_shader.getUniformLocation("picking");
        glUniform1i( location, do_picking ? 1 : 0 );

        //-- Set LightSource uniform for the scene:
        if ( !do_picking )
        {
            {
                location = m_shader.getUniformLocation("light.position");
                glUniform3fv(location, 1, value_ptr(m_light.position));
                location = m_shader.getUniformLocation("light.rgbIntensity");
                glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
                CHECK_GL_ERRORS;
            }

            //-- Set background light ambient intensity
            {
                location = m_shader.getUniformLocation("ambientIntensity");
                vec3 ambientIntensity(0.25f);
                glUniform3fv(location, 1, value_ptr(ambientIntensity));
                CHECK_GL_ERRORS;
            }
        }
    }
    m_shader.disable();
}


void A3::ResetPosition() {
    model_translation = vec3(0);
    app_reset_pos = false;
}

void A3::ResetOrientation() {
    trackball_rots = mat4(1.);
    app_reset_ori = false;
}

void A3::ResetJoints() {
    if (joint_offsets) delete[] joint_offsets;
    
    joint_offsets = new vec2[m_rootNode->totalSceneNodes()];
    for (unsigned int i = 0; i < m_rootNode->totalSceneNodes(); i++) {
        joint_offsets[i] = vec2(0);
    }
    app_reset_joints = false;
}

void A3::ResetAll() {
    opt_show_circle = false, opt_show_zbuf = true, opt_show_bf_culling = false, opt_show_ff_culling = false;
    imode = InterationModes::P;
    mouse_button_active = dx = dy = dz = false;
    prev_xpos = cursor_delta_x = 0;
    if ( selected ) delete[] selected;
    do_picking = false;
    selected = new bool[m_rootNode->totalSceneNodes()];
    for (unsigned int i = 0; i < m_rootNode->totalSceneNodes(); i++) {
        selected[i] = false;
    }
    
    ResetPosition();
    ResetOrientation();
    ResetJoints();
    app_reset_all = false;
}

void A3::Quit() {
    glfwSetWindowShouldClose(m_window, GL_TRUE);
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
    // Place per frame, application logic here ...
    uploadCommonSceneUniforms();    
    // trackball_rots[3] = m_rootNode->trans[3];
    if (app_reset_pos) ResetPosition();
    if (app_reset_ori) ResetOrientation();
    if (app_reset_joints) ResetJoints();
    if (app_reset_all) ResetAll();
    if (app_quit) Quit();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
    if( !show_gui ) {
        return;
    }

    static bool firstRun(true);
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(50, 50));
        firstRun = false;
    }
    
    static bool showDebugWindow(true);
    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
            windowFlags);


    // Add more gui elements here here ..
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Application")) {
            ImGui::MenuItem("Reset Position    (I)", NULL, &app_reset_pos);
            ImGui::MenuItem("Reset Orientation (O)", NULL, &app_reset_ori);
            ImGui::MenuItem("Reset Joints      (S)", NULL, &app_reset_joints);            
            ImGui::MenuItem("Reset All         (A)", NULL, &app_reset_all);            
            ImGui::MenuItem("Quit              (Q)", NULL, &app_quit);            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            ImGui::MenuItem("Circle            (C)", NULL, &opt_show_circle);
            ImGui::MenuItem("Z-buffer          (Z)", NULL, &opt_show_zbuf);
            ImGui::MenuItem("Backface culling  (B)", NULL, &opt_show_bf_culling);
            ImGui::MenuItem("Frontface culling (F)", NULL, &opt_show_ff_culling);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::PushID( "radio" );
    ImGui::RadioButton( "Position/Orientation (P)##col", (int *)&imode, (int)InterationModes::P );
    ImGui::RadioButton( "Joints               (J)##col", (int *)&imode, (int)InterationModes::J );
    ImGui::PopID();
 
    ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    ImGui::End();

}

//----------------------------------------------------------------------------------------
void A3::UpdateSelectedNodeOffset(const float dx, const float dy, const SceneNode & pnode,
                                  const bool rightclick, const bool middleclick) {

    if ( selected[pnode.m_nodeId] && pnode.selectable ) {
        unsigned int parent_id = pnode.parent->m_nodeId;
        const JointNode * joint_node = static_cast<const JointNode *>(pnode.parent);
        JointNode::JointRange x_range = joint_node->m_joint_x;

        //NOTE: Horizontal rotation offset
        if (rightclick && pnode.head) {
            float x_rotate_amount = x_range.init + joint_offsets[parent_id].x + dx;
            if (x_rotate_amount <= x_range.max && x_rotate_amount >= x_range.min) {
                joint_offsets[parent_id].x += dx;
            }
        } else if (middleclick && !pnode.head) {
            float x_rotate_amount = x_range.init + joint_offsets[parent_id].x + dy;
            if (x_rotate_amount <= x_range.max && x_rotate_amount >= x_range.min) {
                joint_offsets[parent_id].x += dy;  
            }  
        }

        //NOTE: Vertical rotation offset
        if ( middleclick ) {
            JointNode::JointRange y_range = joint_node->m_joint_y;
            float y_rotate_amount = y_range.init + joint_offsets[parent_id].y + dy;
            if (y_rotate_amount <= y_range.max && y_rotate_amount >= y_range.min) {
                joint_offsets[parent_id].y += dy;
            
            }        
        }
    } // if ( selected ... )
    
    for (const SceneNode * cnode : pnode.children) {
        UpdateSelectedNodeOffset(dx, dy, *cnode, rightclick, middleclick);
    }

}


//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
    unsigned int node_id,
    const ShaderProgram & shader,
    const GeometryNode & node,
    const glm::mat4 & viewMatrix,
    const glm::mat4 & modelMatrix,
    const glm::vec3 kd
                                 )
{

    shader.enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader.getUniformLocation("ModelView");
        mat4 modelView = viewMatrix * modelMatrix;
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

        // NOTE: using code from Picking_Example/PickingExample.cpp
        if ( do_picking ) {
            float r = float(node_id & 0xff) / 255.0f;
            float g = float((node_id >> 8)&0xff) / 255.0f;
            float b = float((node_id >> 16)&0xff) / 255.0f;

            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            glUniform3f(location, r, g, b);
            
            CHECK_GL_ERRORS;
            
        } else {
            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;


            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
        }
    }
    shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

    if (opt_show_zbuf) glEnable( GL_DEPTH_TEST );
    
    if (opt_show_ff_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
    if (opt_show_bf_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    if (opt_show_bf_culling && opt_show_ff_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
    }
    renderSceneGraph(*m_rootNode);


    if (opt_show_bf_culling || opt_show_ff_culling) {
        glDisable(GL_CULL_FACE);
    }
    if (opt_show_zbuf) glDisable( GL_DEPTH_TEST );
    if (opt_show_circle)
        renderArcCircle();
}

// TODO: add undo/redo stack

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

    // Bind the VAO once here, and reuse for all GeometryNode rendering below.
    glBindVertexArray(m_vao_meshData);

    mat4 T = translate(mat4(1.), model_translation);
    
    matrixStack.push(root.trans);
    matrixStack.push(T);
    matrixStack.push(trackball_rots);

    
    for (const SceneNode * cnode : root.children) {
        renderSceneNode(*cnode);
    }

    matrixStack.pop(trackball_rots);
    matrixStack.pop(T);
    matrixStack.pop(root.trans);
    
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
    
}

//----------------------------------------------------------------------------------------
void A3::renderSceneNode(const SceneNode & pnode) {
    mat4 MX, MY;
    matrixStack.push(pnode.trans);
    if (pnode.m_nodeType == NodeType::JointNode) {
        const JointNode * joint_node = static_cast<const JointNode *>(&pnode);
        JointNode::JointRange x_range = joint_node->m_joint_x;
        JointNode::JointRange y_range = joint_node->m_joint_y;
        float x_rotate_amount = x_range.init + joint_offsets[pnode.m_nodeId].x;
        float y_rotate_amount = y_range.init + joint_offsets[pnode.m_nodeId].y;

        /*
        if (x_rotate_amount > x_range.max) x_rotate_amount = x_range.max;
        if (x_rotate_amount < x_range.min) x_rotate_amount = x_range.min;
        if (y_rotate_amount > y_range.max) y_rotate_amount = y_range.max;
        if (y_rotate_amount < y_range.min) y_rotate_amount = y_range.min;
        */
        cout << y_rotate_amount << endl;

        MX = rotate(mat4(1.), degreesToRadians(x_rotate_amount), vec3(0,1,0));
        MY = rotate(mat4(1.), degreesToRadians(y_rotate_amount), vec3(1,0,0));
        matrixStack.push(MX);
        matrixStack.push(MY);
    }
    else if (pnode.m_nodeType == NodeType::GeometryNode)
    { 
        const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&pnode);
        vec3 kd = geometryNode->material.kd;
        if ( selected[pnode.m_nodeId] && pnode.selectable ) {
            kd = glm::vec3( 1, 1, 0 );
        }
        
        updateShaderUniforms(pnode.m_nodeId, m_shader, *geometryNode, m_view, matrixStack.M, kd);

        // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
        BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

        //-- Now render the mesh:
        m_shader.enable();
        glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
        m_shader.disable();
    } // if
    
    for (const SceneNode * cnode : pnode.children) {
        renderSceneNode(*cnode);
    }
    
    if (pnode.m_nodeType == NodeType::JointNode) {
        matrixStack.pop(MY);
        matrixStack.pop(MX);
    }
    matrixStack.pop(pnode.trans);
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
    glBindVertexArray(m_vao_arcCircle);

    m_shader_arcCircle.enable();
        GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
        float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
        glm::mat4 M;
        if( aspect > 1.0 ) {
            M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
        } else {
            M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
        }
        glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
        glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
    m_shader_arcCircle.disable();

    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{
    if (selected) {
        delete[] selected;
    }
    if (joint_offsets) {
        delete[] joint_offsets;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
        int entered
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
    double xPos,
    double yPos
                         ) {
    bool eventHandled(false);
    float scale = 0.02f;
    // Fill in with event handling code...
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        cursor_delta_x = mouse_button_active ? xPos - prev_xpos : 0;
        cursor_delta_y = mouse_button_active ? yPos - prev_ypos : 0;

        switch (imode) {
            case InterationModes::P: {

                if (dx) {
                    model_translation[0] += cursor_delta_x * scale;
                    model_translation[1] -= cursor_delta_y * scale;
                }
                
                if (dy) {
                    float diameter = (m_windowWidth < m_windowHeight) ? m_windowWidth * 0.5f : m_windowHeight * 0.5f;
                    float iCenterX = m_windowWidth  / 2;
                    float iCenterY = m_windowHeight / 2;
                    float fOldModX = prev_xpos - iCenterX;
                    float fOldModY = prev_ypos - iCenterY;
                    float fNewModX = xPos - iCenterX;
                    float fNewModY = yPos - iCenterY;
                    float x, y, z;
                    vCalcRotVec(fNewModX, fNewModY,
                                fOldModX, fOldModY,
                                diameter,
                                &x, &y, &z);

                    float theta = sqrt(x * x + y * y + z * z);
                    if (theta <= -0.00001 || theta > 0.00001) {
                        trackball_rots = rotate(mat4(1.), theta, vec3(-x,y,-z)) * trackball_rots;
                        
                    }
                } // dy
                if (dz) model_translation[2] += cursor_delta_y * scale;
                break;
            }
            case InterationModes::J:
                
                UpdateSelectedNodeOffset(cursor_delta_x, cursor_delta_y, *m_rootNode, dy, dz);                
                break;
        }
    }
    prev_xpos = xPos;
    prev_ypos = yPos;
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
        int button,
        int actions,
        int mods
) {
    bool eventHandled(false);

    // Fill in with event handling code...
    if (actions == GLFW_PRESS && !ImGui::IsMouseHoveringAnyWindow()) {
        mouse_button_active = true;
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                dx = true;
                if (imode == InterationModes::J) { // NOTE: this code is taken from PickingExample.cpp
                    
                    do_picking = true;
                    double xpos, ypos;
                    glfwGetCursorPos( m_window, &xpos, &ypos );
                    
                    uploadCommonSceneUniforms();
                    glClearColor(1.0, 1.0, 1.0, 1.0 );
                    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                    glClearColor(0.369, 0.322, 0.322, 1.0);

                    draw();

                    // I don't know if these are really necessary anymore.
                    // glFlush();
                    // glFinish();

                    CHECK_GL_ERRORS;

                    // Ugly -- FB coordinates might be different than Window coordinates
                    // (e.g., on a retina display).  Must compensate.
                    xpos *= double(m_framebufferWidth) / double(m_windowWidth);
                    // WTF, don't know why I have to measure y relative to the bottom of
                    // the window in this case.
                    ypos = m_windowHeight - ypos;
                    ypos *= double(m_framebufferHeight) / double(m_windowHeight);

                    GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
                    // A bit ugly -- don't want to swap the just-drawn false colours
                    // to the screen, so read from the back buffer.
                    glReadBuffer( GL_BACK );
                    // Actually read the pixel at the mouse location.
                    glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
                    CHECK_GL_ERRORS;

                    // Reassemble the object ID.
                    unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

                    if( what < m_rootNode->totalSceneNodes() ) {
                        selected[what] = !selected[what];
                    }

                    do_picking = false;

                    CHECK_GL_ERRORS;


                }
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                dy = true;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                dz = true;
                break;
        }
    }
    
    if (actions == GLFW_RELEASE) {
        mouse_button_active = false;
        dx = dy = dz = false;
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
    double xOffSet,
    double yOffSet
                           ) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
        int width,
        int height
) {
    bool eventHandled(false);
    initPerspectiveMatrix();
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
        int key,
        int action,
        int mods
) {
    bool eventHandled(true);

    if( action == GLFW_PRESS ) {
        if( key == GLFW_KEY_M ) {
            show_gui = !show_gui;
            eventHandled = true;
        } else if (key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        } else if (key == GLFW_KEY_I ) {
            ResetPosition();
        } else if (key == GLFW_KEY_O ) {
            ResetOrientation();
        } else if (key == GLFW_KEY_S ) {
            ResetJoints();
        } else if (key == GLFW_KEY_A ) {
            ResetAll();
        } else if (key == GLFW_KEY_C ) {
            opt_show_circle = opt_show_circle ? false : true;
        } else if (key == GLFW_KEY_Z ) {
            opt_show_zbuf = opt_show_zbuf ? false : true;
        } else if (key == GLFW_KEY_B ) {
            opt_show_bf_culling = opt_show_bf_culling ? false : true;
        } else if (key == GLFW_KEY_F ) {
            opt_show_ff_culling = opt_show_ff_culling ? false : true;
        } else if (key == GLFW_KEY_P ) {
            imode = InterationModes::P;
        } else if (key == GLFW_KEY_J ) {
            imode = InterationModes::J;
        }
    }
    // Fill in with event handling code...

    return eventHandled;
}
