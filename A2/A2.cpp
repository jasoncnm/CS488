



// Termm--Fall 2020

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

const float PI = 3.14159265f;
bool firstRun = true;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
    : numVertices(0),
      index(0)
{
    positions.resize(kMaxVertices);
    colours.resize(kMaxVertices);
}


LineIndex l(u32 left, u32 right) {
    LineIndex result;
    result.left = left;
    result.right = right;
    return result;
}

//----------------------------------------------------------------------------------------
// Constructor
A2::A2() : m_currentLineColour(vec3(0.0f))
{
    cube[0] = vec4(-1, -1, -1, 1);
    cube[1] = vec4( 1, -1, -1, 1);
    cube[2] = vec4(-1, -1,  1, 1);
    cube[3] = vec4(-1,  1, -1, 1);
    cube[4] = vec4( 1, -1,  1, 1);
    cube[5] = vec4( 1,  1, -1, 1);
    cube[6] = vec4(-1,  1,  1, 1);
    cube[7] = vec4( 1,  1,  1, 1);

    cubeLines[0] = l(0, 1);
    cubeLines[1] = l(1, 5);
    cubeLines[2] = l(5, 3);
    cubeLines[3] = l(3, 0);
    cubeLines[4] = l(0, 2);
    cubeLines[5] = l(1, 4);
    cubeLines[6] = l(5, 7);
    cubeLines[7] = l(3, 6);
    cubeLines[8] = l(2, 4);
    cubeLines[9] = l(4, 7);
    cubeLines[10] = l(7, 6);
    cubeLines[11] = l(6, 2);
    
    model = mat4(1.);
    modelTransform = mat4(1.);

    view = mat4(1.);
        
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
    // Set the background colour.
    glClearColor(0.3, 0.5, 0.7, 1.0);

    createShaderProgram();

    glGenVertexArrays(1, &m_vao);

    enableVertexAttribIndices();

    generateVertexBuffers();

    mapVboDataToVertexAttributeLocation();

    view = inverse(viewMatrix()) * view;

    gnormScale = Scale(0.25, 0.25, 0.25);

   

    viewport.Corner1 = vec2(-0.9 , -0.9);
    viewport.Corner2 = vec2(0.9, 0.9);
    
}

mat4 A2::viewMatrix() {
    vec3 lookFrom(0,0, 3);
    vec3 lookAt(0,0,1);
    vec3 up(0,1,0);
    vec3 vz = lookAt - lookFrom;
    vz = vz / sqrt(vz.x * vz.x + vz.y * vz.y + vz.z * vz.z);
    vec3 vx = cross(up, vz);
    vx = vx / sqrt(vx.x * vx.x + vx.y * vx.y + vx.z * vx.z);
    vec3 vy = cross(vz, vx);
    mat4 V(vec4(vx, 0), vec4(vy, 0), vec4(vz, 0), vec4(lookFrom, 1));
    return V;
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
    m_shader.generateProgramObject();
    m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
    m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
    m_shader.link();
}

//---------------------------------------------------------------------------------------- Spring 2020
void A2::enableVertexAttribIndices()
{
    glBindVertexArray(m_vao);

    // Enable the attribute index location for "position" when rendering.
    GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray(positionAttribLocation);

    // Enable the attribute index location for "colour" when rendering.
    GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
    glEnableVertexAttribArray(colourAttribLocation);

    // Restore defaults
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
    // Generate a vertex buffer to store line vertex positions
    {
        glGenBuffers(1, &m_vbo_positions);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

        // Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
                GL_DYNAMIC_DRAW);


        // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }

    // Generate a vertex buffer to store line colors
    {
        glGenBuffers(1, &m_vbo_colours);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

        // Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
                GL_DYNAMIC_DRAW);


        // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
    // Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao);

    // Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
    // "position" vertex attribute index for any bound shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
    GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
    glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
    // "colour" vertex attribute index for any bound shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
    GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
    glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

void A2::Reset() {
    mode = Modes::RModel;
    mScaleX = mScaleY = mScaleZ = 0.25f;
    modelTransform = mat4(1.);
    dX = dY = dZ = false;
    mouseButtonActive = false;
    viewportFirstClick = false;
    view = inverse(viewMatrix()) * mat4(1.);
    viewport.Corner1 = vec2(-0.9, -0.9);
    viewport.Corner2 = vec2(0.9, 0.9);
    fov = 15;
    near = 1;
    far = 5;
    mTranslateX = mTranslateY = mTranslateZ = 0;
    mRotateX = mRotateY = mRotateZ = 0;                
             
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
    m_vertexData.numVertices = 0;
    m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
        const glm::vec3 & colour
) {
    m_currentLineColour = colour;
}

mat4 A2::Translation (
    float dx, float dy, float dz
) {
    mat4 M(1.);
    M[3].x = dx;
    M[3].y = dy;
    M[3].z = dz;   
    return M;
}

//---------------------------------------------------------------------------------------
mat4 A2::Scale (
    float sx, float sy, float sz
                ) {
    mat4 M(1.);
    M[0][0] = sx;
    M[1][1] = sy;
    M[2][2] = sz;
    return M;
}

//---------------------------------------------------------------------------------------
mat4 A2::RotationOnAxis( float theta, Axis axi ) {
    mat4 M(1.);
    theta *= 0.01f;
    switch (axi) {
        case Axis::X:
            M[1][1] =  cos(theta);
            M[1][2] = -sin(theta);
            M[2][1] =  sin(theta);
            M[2][2] =  cos(theta);
            break;
        case Axis::Y:
            M[0][0] =  cos(theta);
            M[2][0] =  sin(theta);
            M[0][2] = -sin(theta);
            M[2][2] =  cos(theta);
            break;
        case Axis::Z:
            M[0][0] =  cos(theta);
            M[0][1] =  sin(theta);
            M[1][0] = -sin(theta);
            M[1][1] =  cos(theta);            
            break;
        default:
            break;
    }
    return M;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
        const glm::vec2 & V0,   // Line Start (NDC coordinate)
        const glm::vec2 & V1    // Line End (NDC coordinate)
) {

    m_vertexData.positions[m_vertexData.index] = V0;
    m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
    ++m_vertexData.index;
    m_vertexData.positions[m_vertexData.index] = V1;
    m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
    ++m_vertexData.index;

    m_vertexData.numVertices += 2;
}

bool A2::outerPlanesClip(vec4 *p1, vec4 *p2) {
  
    {        
        float vecA = p1->x + p1->w;
        float vecB = p2->x + p2->w;
        
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L3;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }
    } L3:;

    {        
        float vecA = p1->w - p1->x;
        float vecB = p2->w - p2->x;
        
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L4;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }
    } L4:;

    {        
        float vecA = p1->w + p1->y;
        float vecB = p2->w + p2->y;
        
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L6;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }
    } L6:;
            
    {        
        float vecA = p1->w - p1->y;
        float vecB = p2->w - p2->y;
        
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L5;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }
    } L5:;

    return true;
    
}


bool A2::nearFarPlaneClip(vec4 *p1, vec4 *p2) {

    vec3 A(p1->x, p1->y, p1->z);
    vec3 B(p2->x, p2->y, p2->z);
    {        
        vec3 normal = vec3(0, 0, 1);
        vec3 P = vec3(0, 0, near);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L1;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }

    } L1:;

    {        
        vec3 normal = vec3(0, 0, -1);
        vec3 P = vec3(0, 0, far);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        
        if (vecA < 0 && vecB < 0) {
            return false;
        } else if (vecA >= 0 && vecB >= 0) {
            goto L2;
        }
        float t = vecA / (vecA - vecB);
        if (vecA < 0) {
            *p1 = *p1 + t * (*p2 - *p1);
        } else  {
            *p2 = *p1 + t * (*p2 - *p1);
        }
    } L2:;
    return true;    
}

vec2 A2::WindowToViewPort(vec2 pw) {
    float Lw = 2;
    float Hw = 2;
    float xc1 = viewport.Corner1.x;
    float xc2 = viewport.Corner2.x;
    float yc1 = viewport.Corner1.y;
    float yc2 = viewport.Corner2.y;
    float Lv = abs(xc1 - xc2);
    float Hv = abs(yc1 - yc2); 
    float xwl = -0.5f * Lw;
    float ywb = -0.5f * Hw;
    float xvl = (xc1 < xc2) ? xc1 : xc2;
    float yvb = (yc1 < yc2) ? yc1 : yc2;
    
    vec2 result = vec2((Lv / Lw) * (pw[0] - xwl) + xvl, (Hv / Hw) * (pw[1] - ywb) + yvb);

    return result;
}

float cot(float theta) {
    return 1 / tan(theta);
}

//----------------------------------------------------------------------------------------
void A2::DoProjection(vec4 *P) {
    if (fov <= 2.5f ) fov = 2.5f;
    if (fov >= 80) fov = 80;
    float x_scale = cot(radians(fov));
    float y_scale = cot(radians(fov));
    P->x *= x_scale;
    P->y *= y_scale;
    P->w = P->z / near;
}

//-------------------------------------------------------------------------------------------
void A2::homogenize(vec4 *P) {
    P->x = (P->x / (P->w));
    P->y = (P->y / (P->w));
}

//----------------------------------------------------------------------------------------
void A2::ProjDraw() {
    setLineColour(vec3(0, 1, 0.686));
    mat4 modelScale = Scale(mScaleX, mScaleY, mScaleZ);
    
    for( u32 i = 0; i < 12; i++) {
        LineIndex index = cubeLines[i];
        vec4 lineLeft = cube[index.left];
        vec4 lineRight = cube[index.right];
        
        vec4 PA = view * modelTransform * modelScale * lineLeft;
        vec4 PB = view * modelTransform * modelScale * lineRight;

        if (nearFarPlaneClip(&PA, &PB)) {
            DoProjection(&PA);
            DoProjection(&PB);
            if (outerPlanesClip(&PA, &PB)) {
                homogenize(&PA);
                homogenize(&PB);
                vec2 A = WindowToViewPort(vec2(PA.x, PA.y));
                vec2 B = WindowToViewPort(vec2(PB.x, PB.y));        
                drawLine(A, B);
            }
        }
    } // for

    
    // NOTE: draw model gnorm
    {
        vec4 Origin =  view * modelTransform * gnormScale * model[3];
        vec4 axisX  =  view * modelTransform * gnormScale * vec4(model[0].x, model[0].y, model[0].z, 1);
        
        // NOTE: Model X axis
        if (nearFarPlaneClip(&Origin, &axisX)) {
            DoProjection(&Origin);
            DoProjection(&axisX);
            if (outerPlanesClip(&Origin, &axisX)) {
                homogenize(&Origin);
                homogenize(&axisX);                                
                setLineColour(vec3(1.0f, 0, 0));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 B = WindowToViewPort(vec2(axisX.x, axisX.y));
                drawLine(A, B);
            }
        }
    }

    {
        vec4 Origin =  view * modelTransform * gnormScale * model[3];
        vec4 axisY  =  view * modelTransform * gnormScale * vec4(model[1].x, model[1].y, model[1].z, 1);

        // NOTE: Model Y axis
        if (nearFarPlaneClip(&Origin, &axisY)) {
            DoProjection(&Origin);
            DoProjection(&axisY);
            if (outerPlanesClip(&Origin, &axisY)) {    
                homogenize(&Origin);
                homogenize(&axisY);                                
                setLineColour(vec3(0, 1.0f, 0));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 C = WindowToViewPort(vec2(axisY.x, axisY.y));
                drawLine(A, C);
            }
        }
    }
    
    {
        vec4 Origin =  view * modelTransform * gnormScale * model[3];
        vec4 axisZ  =  view * modelTransform * gnormScale * vec4(model[2].x, model[2].y, model[2].z, 1);
        // NOTE: Model Z axis
        if (nearFarPlaneClip(&Origin, &axisZ)) {
            DoProjection(&Origin);
            DoProjection(&axisZ);
            if (outerPlanesClip(&Origin, &axisZ)) {    
                homogenize(&Origin);
                homogenize(&axisZ);                                
                setLineColour(vec3(0, 0, 1.0f));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 D = WindowToViewPort(vec2(axisZ.x, axisZ. y));
                drawLine(A, D);
            }
        }
    }
    
    // NOTE: world gnorm

    {
        vec4 Origin = view * gnormScale * vec4(0,0,0,1);
        vec4 axisX =  view * gnormScale * vec4(1, 0, 0, 1);
    
        // NOTE: World X axis
        if (nearFarPlaneClip(&Origin, &axisX)) {
            DoProjection(&Origin);
            DoProjection(&axisX);
            if (outerPlanesClip(&Origin, &axisX)) {
                homogenize(&Origin);
                homogenize(&axisX);                                                           
                setLineColour(vec3(0.831f, 0.82f, 0.161f));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 B = WindowToViewPort(vec2(axisX.x, axisX.y));
                drawLine(A, B);
            }
        }
    }

    {
        vec4 Origin = view * gnormScale * vec4(0,0,0,1);
        vec4 axisY =  view * gnormScale * vec4(0, 1, 0, 1);
    
        // NOTE: World Y axis
        if (nearFarPlaneClip(&Origin, &axisY)) {
            DoProjection(&Origin);
            DoProjection(&axisY);
            if (outerPlanesClip(&Origin, &axisY)) {    
                homogenize(&Origin);
                homogenize(&axisY);                                
                setLineColour(vec3(0.69f, 0.106f, 0.851f));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 C = WindowToViewPort(vec2(axisY.x, axisY.y));
                drawLine(A, C);
            }
        }
    }

    {
        vec4 Origin = view * gnormScale * vec4(0, 0, 0, 1);
        vec4 axisZ =  view * gnormScale * vec4(0, 0, 1, 1);

        // NOTE: World Z axis
        if (nearFarPlaneClip(&Origin, &axisZ)) {
            DoProjection(&Origin);
            DoProjection(&axisZ);
            if (outerPlanesClip(&Origin, &axisZ)) {    
                homogenize(&Origin);
                homogenize(&axisZ);                                
                setLineColour(vec3(0.067f, 0.831f, 0.808f));
                vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
                vec2 D = WindowToViewPort(vec2(axisZ.x, axisZ. y));
                drawLine(A, D);
            }
        }
    }
    // NOTE: draw viewport
    setLineColour(vec3(0,0,0));
    vec2 E = viewport.Corner1;
    vec2 F = viewport.Corner2;
    vec2 G = vec2(F.x, E.y);
    vec2 H = vec2(E.x, F.y);
    
    drawLine(E, G);
    drawLine(E, H);
    drawLine(F, G);
    drawLine(F, H);
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
    // Place per frame, application logic here ...
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(50,50));
        // cout<< "windowWidth " << m_windowWidth << " windowHeight " << m_windowHeight << endl;
        firstRun = false;
    }
    
    // Call at the beginning of frame, before drawing lines:
    initLineData();
    ProjDraw();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
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


    // Add more gui elements here here ...
    ImGui::PushID( "radio" );
    ImGui::RadioButton( "Rotate    View  (O)##Col", (int *)&mode, (int)Modes::RView       );
    ImGui::RadioButton( "Translate View  (E)##Col", (int *)&mode, (int)Modes::TView       );
    ImGui::RadioButton( "Perspective     (P)##Col", (int *)&mode, (int)Modes::Perspective );
    ImGui::RadioButton( "Rotate    Model (R)##Col", (int *)&mode, (int)Modes::RModel      );
    ImGui::RadioButton( "Translate Model (T)##Col", (int *)&mode, (int)Modes::TModel      );
    ImGui::RadioButton( "Scale     Model (S)##Col", (int *)&mode, (int)Modes::SModel      );
    ImGui::RadioButton( "Viewport        (V)##Col", (int *)&mode, (int)Modes::Viewport    );
    ImGui::PopID();
    
    // Create Button, and check if it was clicked:
    if( ImGui::Button( "Quit Application " ) ) {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }

    if( ImGui::Button( "Reset            " ) ) {
        Reset();
    }

    ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
    ImGui::Text( "Near: %.1f, Far: %.1f", near, far);
    ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

    //-- Copy vertex position data into VBO, m_vbo_positions:
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
                m_vertexData.positions.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }

    //-- Copy vertex colour data into VBO, m_vbo_colours:
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
                m_vertexData.colours.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
    uploadVertexDataToVbos();

    glBindVertexArray(m_vao);

    m_shader.enable();
        glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
    m_shader.disable();

    // Restore defaults
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
        double xPos,
        double yPos
) {
    bool eventHandled(false);

    // Fill in with event handling code...
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        deltaX = mouseButtonActive ? (xPos - xPrev) : 0;
        // cout << deltaX << endl;
        // cout << " mTranslateX " << mTranslateX;

        switch (mode) {
            case Modes::TModel:
                if (dX) mTranslateX += deltaX * 0.005f;
                if (dY) mTranslateY += deltaX * 0.005f;
                if (dZ) mTranslateZ += deltaX * 0.005f;
                if (mouseButtonActive) {
                    modelTransform *= Translation(mTranslateX, mTranslateY, mTranslateZ);
                }
                mTranslateX = mTranslateY = mTranslateZ = 0;
                break;
                
            case Modes::RModel:
                if (dX) mRotateX += deltaX;
                if (dY) mRotateY += deltaX;
                if (dZ) mRotateZ += deltaX;
                if (dX || dY || dZ) {
                    modelTransform *= RotationOnAxis( mRotateX, Axis::X);
                    modelTransform *= RotationOnAxis( mRotateY, Axis::Y);
                    modelTransform *= RotationOnAxis( mRotateZ, Axis::Z);   
                }
                
                mRotateX = mRotateY = mRotateZ = 0;                
                break;
            case Modes::SModel:
                // if (deltaX < 2) deltaX = 2;
                if (dX) mScaleX += deltaX * 0.005f;
                if (dY) mScaleY += deltaX * 0.005f;
                if (dZ) mScaleZ += deltaX * 0.005f;
                break;

            case Modes::TView:
                if (dX) mTranslateX += deltaX * 0.005f;
                if (dY) mTranslateY += deltaX * 0.005f;
                if (dZ) mTranslateZ += deltaX * 0.005f;
                if (mouseButtonActive) {
                    view = inverse(Translation(mTranslateX, mTranslateY, mTranslateZ)) * view;
                }
                mTranslateX = mTranslateY = mTranslateZ = 0;
                break;
                
            case Modes::RView:
                if (dX) mRotateX += deltaX;
                if (dY) mRotateY += deltaX;
                if (dZ) mRotateZ += deltaX;
                if (dX || dY || dZ) {
                    view = inverse(RotationOnAxis( mRotateX, Axis::X )) * view;
                    view = inverse(RotationOnAxis( mRotateY, Axis::Y )) * view;
                    view = inverse(RotationOnAxis( mRotateZ, Axis::Z )) * view;
                }
                mRotateX = mRotateY = mRotateZ = 0;                
                break;

            case Modes::Perspective:
                if (dX) fov += 0.1f * deltaX;
                if (dY) far += 0.005f * deltaX;
                if (dZ) near += 0.005f * deltaX;
                clamp(fov, 5.0f, 160.0f);
                // cout << " FOV: " << fov << endl;
                break;
            case Modes::Viewport:
                float xPosScale = xPos * (2.f / m_windowWidth) - 1;
                float yPosScale = -(yPos * (2.f / m_windowWidth) - 1);
                if (viewportFirstClick) {
                    viewportFirstClick = false;
                    viewport.Corner1 = vec2(xPosScale, yPosScale);
                } else if (dX) {
                    viewport.Corner2 = vec2(xPosScale, yPosScale);
                }                
                break;

        }        
    }

    xPrev = xPos;
    return eventHandled;
} 

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
        int button,
        int actions,
        int mods
) {
    bool eventHandled(false);

    // Fill in with event handling code...
    if (actions == GLFW_PRESS) {
        if (!ImGui::IsMouseHoveringAnyWindow()) {
            mouseButtonActive = true;
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    viewportFirstClick = true;
                    dX = true;
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    dY = true;
                    break;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    dZ = true;
                    break;
            }
            
        }
    }

    if (actions == GLFW_RELEASE) {
        mouseButtonActive = false;
        viewportFirstClick = false;
        dX = dY = dZ = false;
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
        int width,
        int height
) {
    bool eventHandled(false);

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
        int key,
        int action,
        int mods
) {
    bool eventHandled(false);

    // Fill in with event handling code...
    switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            break;
        case GLFW_KEY_R:
            mode = Modes::RModel;
            break;
        case GLFW_KEY_T:
            mode = Modes::TModel;
            break;
        case GLFW_KEY_S:
            mode = Modes::SModel;
            break;
        case GLFW_KEY_O:
            mode = Modes::RView;
            break;
        case GLFW_KEY_E:
            mode = Modes::TView;
            break;
        case GLFW_KEY_P:
            mode = Modes::Perspective;
            break;
        case GLFW_KEY_V:
            mode = Modes::Viewport;
        case GLFW_KEY_A:
            Reset();
            break;
    }
    return eventHandled;
}
