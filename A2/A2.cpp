


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

//---------------------------------------------------------------------------------------
void
PrintMat4( glm::mat4 mat )
{
    cout << "------------------------------------------------------------------" << endl;
    for( int col = 0; col < 4; col++ )
    {
        // transpose the matrix here:
        fprintf( stderr, "  %7.2f %7.2f %7.2f %7.2f\n",
                 mat[0][col], mat[1][col], mat[2][col], mat[3][col] );
    }
    cout << "------------------------------------------------------------------" << endl;
    
}

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

    gnormScale = Scale(250, 250, 250);

    // PrintMat4(view);

    viewport.Corner1 = vec2(-0.9 , -0.9);
    viewport.Corner2 = vec2(0.9, 0.9);
    
}

mat4 A2::viewMatrix() {
    vec3 lookFrom(0,0, 350);
    vec3 lookAt(0,0,-1);
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
    M[0][0] = sx * 0.5f;
    M[1][1] = sy * 0.5f;
    M[2][2] = sz * 0.5f;
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

bool A2::clip(vec4 *p1, vec4 *p2) {
   
    vec3 A = vec3(p1->x, p1->y, p1->z);
    vec3 B = vec3(p2->x, p2->y, p2->z);
    {        
        vec3 normal = vec3(0, 0, 1);
        vec3 P = vec3(0, 0, 0);        
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
        vec3 P = vec3(0, 0, 1000);        
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

    {        
        vec3 normal = vec3(1, 0, 0);
        vec3 P = vec3(-0.5f * m_windowWidth, 0, 0);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        
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
        vec3 normal = vec3(-1, 0, 0);
        vec3 P = vec3(0.5f * m_windowWidth, 0, 0);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        
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
        vec3 normal = vec3(0, 1, 0);
        vec3 P = vec3(0, -0.5f * m_windowHeight, 0);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        
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

    {        
        vec3 normal = vec3(0, -1, 0);
        vec3 P = vec3(0, 0.5f * m_windowHeight, 0);        
        float vecA = dot(A-P, normal);
        float vecB = dot(B-P, normal);
        
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
            
    return true;
    
}

vec2 A2::WindowToViewPort(vec2 pw) {
    float Lw = m_windowWidth;
    float Hw = m_windowHeight;
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
#if 0
vec4 A2::homogenize(vec4 P) {

}
#endif
//----------------------------------------------------------------------------------------
void A2::OrthDraw() {
    setLineColour(vec3(1.0f, 0.7f, 0.8f));
    mat4 modelScale = Scale(mScaleX, mScaleY, mScaleZ);
    
    for( u32 i = 0; i < 12; i++) {
        LineIndex index = cubeLines[i];
        vec4 lineLeft = cube[index.left];
        vec4 lineRight = cube[index.right];
        
        vec4 PA = view * modelTransform * modelScale * lineLeft;
        vec4 PB = view * modelTransform * modelScale * lineRight;

        if (clip(&PA, &PB)) {
            vec2 A = WindowToViewPort(vec2(PA[0], PA[1]));
            vec2 B = WindowToViewPort(vec2(PB[0], PB[1]));        
            drawLine(A, B);
        }
    } // for
    
    // NOTE: draw model gnorm
    vec4 Origin =  view * modelTransform * gnormScale * model[3];
    vec4 axisX  =  view * modelTransform * gnormScale * vec4(model[0].x, model[0].y, model[0].z, 1);
    vec4 axisY  =  view * modelTransform * gnormScale * vec4(model[1].x, model[1].y, model[1].z, 1);
    vec4 axisZ  =  view * modelTransform * gnormScale * vec4(model[2].x, model[2].y, model[2].z, 1);
    
    // NOTE: Model X axis
    setLineColour(vec3(1.0f, 0, 0));
    vec2 A = WindowToViewPort(vec2(Origin.x, Origin.y));
    vec2 B = WindowToViewPort(vec2(axisX.x, axisX.y));
    drawLine(A, B);

    // NOTE: Model Y axis
    setLineColour(vec3(0, 1.0f, 0));
    vec2 C = WindowToViewPort(vec2(axisY.x, axisY.y));
    drawLine(A, C);

    // NOTE: Model Z axis
    setLineColour(vec3(0, 0, 1.0f));
    vec2 D = WindowToViewPort(vec2(axisZ.x, axisZ. y));
    drawLine(A, D);

    // NOTE: world gnorm
    Origin = view * gnormScale * vec4(0,0,0,1);
    axisX =  view * gnormScale * vec4(1, 0, 0, 1);
    axisY =  view * gnormScale * vec4(0, 1, 0, 1);
    axisZ =  view * gnormScale * vec4(0, 0, 1, 1);
    
    // NOTE: World X axis
    setLineColour(vec3(.2f, .3f, .7f));
    A = WindowToViewPort(vec2(Origin.x, Origin.y));
    B = WindowToViewPort(vec2(axisX.x, axisX.y));
    drawLine(A, B);

    // NOTE: World Y axis
    setLineColour(vec3(.3f, .2f, .7f));
    C = WindowToViewPort(vec2(axisY.x, axisY.y));
    drawLine(A, C);

    // NOTE: World Z axis
    setLineColour(vec3(.7f, .2f, .3f));
    D = WindowToViewPort(vec2(axisZ.x, axisZ. y));
    drawLine(A, D);

    // NOTE: draw viewport
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
    OrthDraw();
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
    ImGui::RadioButton( "Rotate    View  (O)##Col", (int *)&mode, (int)Modes::RView    );
    ImGui::RadioButton( "Translate View  (E)##Col", (int *)&mode, (int)Modes::TView    );
    ImGui::RadioButton( "Translate Model (T)##Col", (int *)&mode, (int)Modes::TModel   );
    ImGui::RadioButton( "Rotate    Model (R)##Col", (int *)&mode, (int)Modes::RModel   );
    ImGui::RadioButton( "Scale     Model (S)##Col", (int *)&mode, (int)Modes::SModel   );
    ImGui::RadioButton( "Viewport        (V)##Col", (int *)&mode, (int)Modes::Viewport );
    ImGui::PopID();
    
    // Create Button, and check if it was clicked:
    if( ImGui::Button( "Quit Application " ) ) {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }

    if( ImGui::Button( "Reset            " ) ) {
        Reset();
    }

    ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

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
                if (dX) mTranslateX += deltaX;
                if (dY) mTranslateY += deltaX;
                if (dZ) mTranslateZ += deltaX;
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
                if (dX) mScaleX += deltaX;
                if (dY) mScaleY += deltaX;
                if (dZ) mScaleZ += deltaX;
                break;

            case Modes::TView:
                if (dX) mTranslateX += deltaX;
                if (dY) mTranslateY += deltaX;
                if (dZ) mTranslateZ += deltaX;
                if (mouseButtonActive) {
                    view = inverse(Translation(mTranslateX, mTranslateY, mTranslateZ)) * view;
                }
                mTranslateX = mTranslateY = mTranslateZ = 0;
                    
                cout<< "------------------------------------------------" << endl;
                cout << " m_windowWidth " << m_windowWidth * 0.5f << " m_windowHeight " << m_windowHeight * 0.5f << endl;
                 PrintMat4(view);
                 cout<< "------------------------------------------------" << endl;
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

                                
                // cout<< "------------------------------------------------" << endl;
                // PrintMat4(view);
                // cout << "------------------------------------------------" << endl;

                mRotateX = mRotateY = mRotateZ = 0;                
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
        case GLFW_KEY_A:
            Reset();
            break;
    }
    return eventHandled;
}
