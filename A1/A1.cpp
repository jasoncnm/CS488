// Termm--Fall 2020

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
        : current_col( 0 ), last_col( -1 ), mouseButtonActive(false)
{

    colourF[0] = 0.5f;
    colourF[1] = 0.5f;
    colourF[2] = 0.5f;
    colourW[0] = 0.3f;
    colourW[1] = 0.3f;
    colourW[2] = 0.7f;
    colourA[0] = 0.7f;
    colourA[1] = 0.3f;
    colourA[2] = 0.3f;

    widgetColour[0] = 0.5f;
    widgetColour[1] = 0.5f;
    widgetColour[2] = 0.5f;

    
    avatarPos[0] = 0.0f;
    avatarPos[1] = 0.0f;
    avatarPos[2] = 0.0f;
    
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
    delete m;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */

void A1::init()
{
    // Initialize random number generator
    int rseed=getpid();
    srandom(rseed);
    // Print random number seed in case we want to rerun with
    // same random numbers

    cout << "Random number seed = " << rseed << endl;
    
    m = new Maze(DIM);
        
    // Set the background colour.
    glClearColor( 0.3, 0.5, 0.7, 1.0 );

    // Build the shader
    m_shader.generateProgramObject();
    m_shader.attachVertexShader(
        getAssetFilePath( "VertexShader.vs" ).c_str() );
    m_shader.attachFragmentShader(
        getAssetFilePath( "FragmentShader.fs" ).c_str() );
    m_shader.link();

    // Set up the uniforms
    P_uni = m_shader.getUniformLocation( "P" );
    V_uni = m_shader.getUniformLocation( "V" );
    M_uni = m_shader.getUniformLocation( "M" );
    col_uni = m_shader.getUniformLocation( "colour" );                                                           

    cubeHeight = 1.0f;
    initFloor();
    initGrid();
    initCubes();
    initAvatar();
     
    // Set up initial view and projection matrices (need to do this here,
    // since it depends on the GLFW window being set up correctly).
    view = glm::lookAt( 
        glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
        glm::vec3( 0.0f, 0.0f, 0.0f ),
        glm::vec3( 0.0f, 1.0f, 0.0f ) );

    proj = glm::perspective( 
        glm::radians( 30.0f ),
        float( m_framebufferWidth ) / float( m_framebufferHeight ),
        1.0f, 1000.0f );
}

void A1::findStartPoint() {
    avatarPos[0] = 0;
    avatarPos[1] = 0;
    avatarPos[2] = 0;
            
    for (int y = 0; y < m->getDim(); y++) {
        if (m->getValue(0, y) == 0) {
            avatarPos[1] = y;
        }
    }
}

void A1::initFloor() {
    
    vec3 floorVertices[6] = {
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, DIM),
        vec3(DIM, 0.0f, 0.0f),
        vec3(DIM, 0.0f, DIM),
        vec3(0.0f, 0.0f, DIM),
        vec3(DIM, 0.0f, 0.0f)         
    };
      
    glGenVertexArrays(1, &floorVao);
    glBindVertexArray(floorVao);

    glGenBuffers(1, &floorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(floorVertices),
                 floorVertices,
                 GL_STATIC_DRAW);


    GLuint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                           nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;       
}

#if 0
void A1::initAvatar() {
    // TODO: Cube for now, need to be a sphere
    vec3 ref = vec3(avatarPos[0], avatarPos[2], avatarPos[1]);
    vec3 avatarVertices[6*6] = {
        ref,
        ref + vec3(1.0f, 0.0f, 0.0f),
        ref + vec3(0.0f, 0.0f, 1.0f),
        ref + vec3(1.0f, 0.0f, 1.0f),
        ref + vec3(1.0f, 0.0f, 0.0f),
        ref + vec3(0.0f, 0.0f, 1.0f),
        // NOTE: Back face
        ref,
        ref + vec3(1.0f, 0.0f, 0.0f),
        ref + vec3(0.0f, 1.0f, 0.0f),
        ref + vec3(1.0f, 1.0f, 0.0f),
        ref + vec3(1.0f, 0.0f, 0.0f),
        ref + vec3(0.0f, 1.0f, 0.0f),
        // NOTE: Left face
        ref,
        ref + vec3(0.0f, 1.0f, 0.0f),
        ref + vec3(0.0f, 0.0f, 1.0f),
        ref + vec3(0.0f, 1.0f, 1.0f),
        ref + vec3(0.0f, 1.0f, 0.0f),
        ref + vec3(0.0f, 0.0f, 1.0f),
        // NOTE: Right face
        ref + vec3(1.0f, 0.0f, 0.0f),
        ref + vec3(1.0f, 1.0f, 0.0f),
        ref + vec3(1.0f, 0.0f, 1.0f),
        ref + vec3(1.0f, 1.0f, 1.0f),
        ref + vec3(1.0f, 1.0f, 0.0f),
        ref + vec3(1.0f, 0.0f, 1.0f),
        // NOTE: Front face
        ref + vec3(0.0f, 0.0f, 1.0f),
        ref + vec3(1.0f, 0.0f, 1.0f),
        ref + vec3(0.0f, 1.0f, 1.0f),
        ref + vec3(1.0f, 1.0f, 1.0f),
        ref + vec3(1.0f, 0.0f, 1.0f),
        ref + vec3(0.0f, 1.0f, 1.0f),
        // NOTE: Top face
        ref + vec3(0.0f, 1.0f, 0.0f),
        ref + vec3(1.0f, 1.0f, 0.0f),
        ref + vec3(0.0f, 1.0f, 1.0f),
        ref + vec3(1.0f, 1.0f, 1.0f),
        ref + vec3(1.0f, 1.0f, 0.0f),
        ref + vec3(0.0f, 1.0f, 1.0f),
        
    };
    glGenVertexArrays(1, &avatarVao);
    glBindVertexArray(avatarVao);

    glGenBuffers(1, &avatarVbo);
    glBindBuffer(GL_ARRAY_BUFFER, avatarVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(avatarVertices),
                 avatarVertices,
                 GL_STATIC_DRAW);


    GLuint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                           nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
    
}
#else

void A1::initAvatar() {

    int size = 6 * sectorCount * (stackCount - 2) + 3 * sectorCount * 2;
    vec3 avatarVertices[size];
    vec3 offset = vec3(0.5f, 0.5f, 0.5f) + vec3(avatarPos[0], avatarPos[2], avatarPos[1]);

    float thPerSector = 2 * PI / sectorCount;
    float phiPerStack = PI / stackCount;

    size_t ct = 0;
    for (int i = 0; i < stackCount; i++) {
        float phi = PI / 2 - i * phiPerStack;
        float nphi = PI / 2 - (i + 1) * phiPerStack;
        for (int j = 0; j < sectorCount; j++) {
            float theta = j * thPerSector;
            float ntheta = (j + 1) * thPerSector;

            if (i > 0 && i < (stackCount - 1)) {
                avatarVertices[ct]   = vec3(0.5f*cos(phi) * cos(theta),
                                            0.5f*sin(phi),
                                            0.5f*cos(phi) * sin(theta)) + offset;
                avatarVertices[ct+1] = vec3(0.5f*cos(phi) * cos(ntheta),
                                            0.5f*sin(phi),
                                            0.5f*cos(phi) * sin(ntheta)) + offset;
                avatarVertices[ct+2] = vec3(0.5f*cos(nphi) * cos(theta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(theta)) + offset;
                avatarVertices[ct+3] = vec3(0.5f*cos(nphi) * cos(ntheta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(ntheta)) + offset;
                avatarVertices[ct+4] = vec3(0.5f*cos(phi) * cos(ntheta),
                                            0.5f*sin(phi),
                                            0.5f*cos(phi) * sin(ntheta)) + offset;
                avatarVertices[ct+5] = vec3(0.5f*cos(nphi) * cos(theta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(theta)) + offset;
                ct += 6;
            } else if (i == 0) {
                avatarVertices[ct]   = vec3(0.5f*cos(phi) * cos(theta),
                                            0.5f*sin(phi),
                                            0.5f*cos(phi) * sin(theta)) + offset;
                avatarVertices[ct+1] = vec3(0.5f*cos(nphi) * cos(theta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(theta)) + offset;
                avatarVertices[ct+2] = vec3(0.5f*cos(nphi) * cos(ntheta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(ntheta)) + offset;
                ct += 3;
            } else {
                avatarVertices[ct]   = vec3(0.5f*cos(phi) * cos(theta),
                                            0.5f*sin(phi),
                                            0.5f*cos(phi) * sin(theta)) + offset;
                avatarVertices[ct+1] = vec3(0.5f*cos(phi) * cos(ntheta),
                                            0.5f*sin(phi),
                                            0.5f*cos(nphi) * sin(ntheta)) + offset;
                avatarVertices[ct+2] = vec3(0.5f*cos(nphi) * cos(theta),
                                            0.5f*sin(nphi),
                                            0.5f*cos(nphi) * sin(theta)) + offset;
                ct += 3;
            }
        }
    }

        
    glGenVertexArrays(1, &avatarVao);
    glBindVertexArray(avatarVao);

    glGenBuffers(1, &avatarVbo);
    glBindBuffer(GL_ARRAY_BUFFER, avatarVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(avatarVertices),
                 avatarVertices,
                 GL_STATIC_DRAW);


    GLuint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                           nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;

}

#endif 
void A1::initCubes() {
    cubeCount = 0;
    size_t mazeDim = m->getDim();
    vec3 cubesVertices[mazeDim * mazeDim * 6 * 6];
    size_t ct = 0;
    for (int x = 0; x < mazeDim; x++) {
        for (int y = 0; y < mazeDim; y++) {
            if (m->getValue(x, y) == 1) {
                cubeCount += 1;
                vec3 ref = vec3((float)x, 0.0f, (float)y);
                // NOTE: Botton face
                cubesVertices[ct] = ref;
                cubesVertices[ct+1] = ref + vec3(1.0f, 0.0f, 0.0f);
                cubesVertices[ct+2] = ref + vec3(0.0f, 0.0f, 1.0f);
                cubesVertices[ct+3] = ref + vec3(1.0f, 0.0f, 1.0f);
                cubesVertices[ct+4] = ref + vec3(1.0f, 0.0f, 0.0f);
                cubesVertices[ct+5] = ref + vec3(0.0f, 0.0f, 1.0f);
                ct += 6;
                // NOTE: Back face
                cubesVertices[ct] = ref;
                cubesVertices[ct+1] = ref + vec3(1.0f, 0.0f, 0.0f);
                cubesVertices[ct+2] = ref + vec3(0.0f, cubeHeight, 0.0f);
                cubesVertices[ct+3] = ref + vec3(1.0f, cubeHeight, 0.0f);
                cubesVertices[ct+4] = ref + vec3(1.0f, 0.0f, 0.0f);
                cubesVertices[ct+5] = ref + vec3(0.0f, cubeHeight, 0.0f);
                ct += 6;
                // NOTE: Left face
                cubesVertices[ct] = ref;
                cubesVertices[ct+1] = ref + vec3(0.0f, cubeHeight, 0.0f);
                cubesVertices[ct+2] = ref + vec3(0.0f, 0.0f, 1.0f);
                cubesVertices[ct+3] = ref + vec3(0.0f, cubeHeight, 1.0f);
                cubesVertices[ct+4] = ref + vec3(0.0f, cubeHeight, 0.0f);
                cubesVertices[ct+5] = ref + vec3(0.0f, 0.0f, 1.0f);
                ct += 6;
                // NOTE: Right face
                cubesVertices[ct] = ref + vec3(1.0f, 0.0f, 0.0f);
                cubesVertices[ct+1] = ref + vec3(1.0f, cubeHeight, 0.0f);
                cubesVertices[ct+2] = ref + vec3(1.0f, 0.0f, 1.0f);
                cubesVertices[ct+3] = ref + vec3(1.0f, cubeHeight, 1.0f);
                cubesVertices[ct+4] = ref + vec3(1.0f, cubeHeight, 0.0f);
                cubesVertices[ct+5] = ref + vec3(1.0f, 0.0f, 1.0f);
                ct += 6;
                // NOTE: Front face
                cubesVertices[ct] = ref + vec3(0.0f, 0.0f, 1.0f);
                cubesVertices[ct+1] = ref + vec3(1.0f, 0.0f, 1.0f);
                cubesVertices[ct+2] = ref + vec3(0.0f, cubeHeight, 1.0f);
                cubesVertices[ct+3] = ref + vec3(1.0f, cubeHeight, 1.0f);
                cubesVertices[ct+4] = ref + vec3(1.0f, 0.0f, 1.0f);
                cubesVertices[ct+5] = ref + vec3(0.0f, cubeHeight, 1.0f);
                ct += 6;
                // NOTE: Top face
                cubesVertices[ct] = ref + vec3(0.0f, cubeHeight, 0.0f);
                cubesVertices[ct+1] = ref + vec3(1.0f, cubeHeight, 0.0f);
                cubesVertices[ct+2] = ref + vec3(0.0f, cubeHeight, 1.0f);
                cubesVertices[ct+3] = ref + vec3(1.0f, cubeHeight, 1.0f);
                cubesVertices[ct+4] = ref + vec3(1.0f, cubeHeight, 0.0f);
                cubesVertices[ct+5] = ref + vec3(0.0f, cubeHeight, 1.0f);
                ct += 6;
            }
        }
    }
    
    glGenVertexArrays(1, &cubesVao);
    glBindVertexArray(cubesVao);

    glGenBuffers(1, &cubesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubesVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(cubesVertices),
                 cubesVertices,
                 GL_STATIC_DRAW);


    GLuint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                           nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
    
}

void A1::initGrid()
{
    size_t sz = 3 * 2 * 2 * (DIM+3);

    float *verts = new float[ sz ];
    size_t ct = 0;
    for( int idx = 0; idx < DIM + 3 ; ++idx ) {
        // NOTE: End points of horiziontal grid lines
        verts[ ct ] = -1;
        verts[ ct+1 ] = 0;
        verts[ ct+2 ] = idx-1;
        verts[ ct+3 ] = DIM+1;
        verts[ ct+4 ] = 0;
        verts[ ct+5 ] = idx-1;
        ct += 6;
        // NOTE: End points of vertical grid lines
        verts[ ct ] = idx-1;
        verts[ ct+1 ] = 0;
        verts[ ct+2 ] = -1;
        verts[ ct+3 ] = idx-1;
        verts[ ct+4 ] = 0;
        verts[ ct+5 ] = DIM+1;
        ct += 6;
    }

    // Create the vertex array to record buffer assignments.
    glGenVertexArrays( 1, &m_grid_vao );
    glBindVertexArray( m_grid_vao );

    // Create the cube vertex buffer
    glGenBuffers( 1, &m_grid_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
    glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
                  verts, GL_STATIC_DRAW );

    // Specify the means of extracting the position values properly.
    GLint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

    // Reset state to prevent rogue code from messing with *my* 
    // stuff!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // OpenGL has the buffer now, there's no need for us to keep a copy.
    delete [] verts;

    CHECK_GL_ERRORS;
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
    // Place per frame, application logic here ...

    cubeHeight = clamp(cubeHeight, 1.0f, 3.0f);
    scaleSize = clamp (scaleSize, 0.5f, 3.0f);
    
}

// TODO: Implement this function
void A1::resetGridStates() {
    colourF[0] = 0.5f;
    colourF[1] = 0.5f;
    colourF[2] = 0.5f;
    colourW[0] = 0.3f;
    colourW[1] = 0.3f;
    colourW[2] = 0.7f;
    colourA[0] = 0.7f;
    colourA[1] = 0.3f;
    colourA[2] = 0.3f;
    current_col = 0;
    last_col = -1;        
    avatarPos[0] = 0.0f;
    avatarPos[1] = 0.0f;
    avatarPos[2] = 0.0f;
    m->reset();    
    mouseButtonActive = false;
    initGrid();
    initFloor();
    initCubes();
    initAvatar();
    rotation = 0.0f;
    scaleSize = 1.0f;
        
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
    // We already know there's only going to be one window, so for 
    // simplicity we'll store button states in static local variables.
    // If there was ever a possibility of having multiple instances of
    // A1 running simultaneously, this would break; you'd want to make
    // this into instance fields of A1.
    static bool showTestWindow(false);
    static bool showDebugWindow(true);

    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
    if( ImGui::Button( "Quit Application" ) ) {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }

    if( ImGui::Button( "Dig" ) ) {
        m->digMaze();
        findStartPoint();        
        initCubes();
        initAvatar();
    }

    if ( ImGui::Button( "Reset" ) ) {
        resetGridStates();
    }

    // Eventually you'll create multiple colour widgets with
    // radio buttons.  If you use PushID/PopID to give them all
    // unique IDs, then ImGui will be able to keep them separate.
    // This is unnecessary with a single colour selector and
    // radio button, but I'm leaving it in as an example.

    // Prefixing a widget name with "##" keeps it from being
    // displayed.

    ImGui::PushID( "radio" );
    ImGui::RadioButton( "Floor##Col", &current_col, 0 );
    ImGui::SameLine();
    ImGui::RadioButton( "Wall##Col", &current_col, 1 );
    ImGui::SameLine();
    ImGui::RadioButton( "Avatar##Col", &current_col, 2);
    ImGui::PopID();
    
    if (current_col != last_col) {
        if (current_col == 0) {
            widgetColour[0] = colourF[0];
            widgetColour[1] = colourF[1];
            widgetColour[2] = colourF[2];
        }

        if (current_col == 1) {
            widgetColour[0] = colourW[0];
            widgetColour[1] = colourW[1];
            widgetColour[2] = colourW[2];
        }

        if (current_col == 2) {
            widgetColour[0] = colourA[0];
            widgetColour[1] = colourA[1];
            widgetColour[2] = colourA[2];
        }
    }    
    
    ImGui::PushID( 0 );
    ImGui::ColorEdit3( "##Colour", widgetColour );
    ImGui::PopID();
    
    if (current_col == 0) {
        colourF[0] = widgetColour[0];
        colourF[1] = widgetColour[1];
        colourF[2] = widgetColour[2];
    }

    if (current_col == 1) {
        colourW[0] = widgetColour[0];
        colourW[1] = widgetColour[1];
        colourW[2] = widgetColour[2];
    }

    if (current_col == 2) {
        colourA[0] = widgetColour[0];
        colourA[1] = widgetColour[1];
        colourA[2] = widgetColour[2];
    }
    
    last_col = current_col;

/*
// For convenience, you can uncomment this to show ImGui's massive
// demonstration window right in your application.  Very handy for
// browsing around to get the widget you want.  Then look in 
// shared/imgui/imgui_demo.cpp to see how it's done.
if( ImGui::Button( "Test Window" ) ) {
showTestWindow = !showTestWindow;
}
*/

    ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    ImGui::End();

    if( showTestWindow ) {
        ImGui::ShowTestWindow( &showTestWindow );
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
    vec3 axis(0.0f, 1.0f, 0.0f);
    // Create a global transformation for the model (centre it).
    mat4 W;
    W = glm::rotate(W, rotation, axis);
    W = glm::scale(W, vec3(scaleSize));
    W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
    
    
    m_shader.enable();
    glEnable( GL_DEPTH_TEST );

    glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
    glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
    glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );
        
    // Just draw the grid for now.
    glBindVertexArray( m_grid_vao );
    glUniform3f( col_uni, 1, 1, 1 );
    glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

    // Draw the floor
    glBindVertexArray( floorVao );
    glUniform3f( col_uni, colourF[0], colourF[1], colourF[2] );
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    // Draw the cubes
    glBindVertexArray( cubesVao );
    glUniform3f( col_uni, colourW[0], colourW[1], colourW[2] );
    glDrawArrays( GL_TRIANGLES, 0, cubeCount * 6 * 6);

    // Draw Avatar
    glBindVertexArray( avatarVao );
    glUniform3f( col_uni, colourA[0], colourA[1], colourA[2] );
    int numVerts = 6 * sectorCount * (stackCount - 2) + 3 * sectorCount * 2;
    glDrawArrays( GL_TRIANGLES, 0, numVerts);
            
    // Highlight the active square.
    m_shader.disable();

    // Restore defaults
    glBindVertexArray( 0 );

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // Put some code here to handle rotations.  Probably need to
        // check whether we're *dragging*, not just moving the mouse.
        // Probably need some instance variables to track the current
        // rotation amount, and maybe the previous X position (so 
        // that you can rotate relative to the *change* in X.
        deltaX = (xPos - xPrev);
        if (mouseButtonActive) {
            rotation += deltaX / 2000;
        }
    }

    xPrev = xPos;

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
    bool eventHandled(false);

    if (actions == GLFW_PRESS) {
        if (!ImGui::IsMouseHoveringAnyWindow()) {
            // The user clicked in the window.  If it's the left
            // mouse button, initiate a rotation.
            mouseButtonActive = true;
            
        }
    }

    if (actions == GLFW_RELEASE) {
        mouseButtonActive = false;
    }
    
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
    bool eventHandled(false);

    // Zoom in or out.
    if (yOffSet == -1) {
        scaleSize -= 0.1f;
    } else if (yOffSet == 1) {
        scaleSize += 0.1f;
    }
    
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
bool A1::noWall(int x, int y) {
    bool result = x == -1 || y == -1 || x == DIM || y == DIM || !m->getValue(x, y);
    return result;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
    bool eventHandled(false);

    // Fill in with event handling code...
    if( action == GLFW_PRESS ) {
        // Respond to some key events.
        if (key == GLFW_KEY_Q) {
            // cout << "Q key pressed" << endl;
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }

        if (key == GLFW_KEY_D) {
            m->digMaze();
            findStartPoint();        
            initCubes();
            initAvatar();
        }

        if (key == GLFW_KEY_R) {
            resetGridStates();
        }

        if (key == GLFW_KEY_SPACE) {
            cubeHeight += 0.2f;
            initCubes();
        }

        if (key == GLFW_KEY_BACKSPACE) {
            cubeHeight -= 0.2f;
            initCubes();
        }

        if (key == GLFW_KEY_LEFT) {
            if (avatarPos[0] > -1
                && noWall(avatarPos[0] - 1, avatarPos[1])) {
                avatarPos[0] -= 1;
                initAvatar();
            } else if (mods == GLFW_MOD_SHIFT) {
                avatarPos[0] -= 1;
                m->setValue( avatarPos[0], avatarPos[1], 0 );
                initCubes();
                initAvatar();
            }
        }
        
        if (key == GLFW_KEY_RIGHT) {
            if ((avatarPos[0] < DIM || avatarPos[0] == -1)
                && noWall(avatarPos[0] + 1, avatarPos[1])){
                avatarPos[0] += 1;
                initAvatar();
            } else if (mods == GLFW_MOD_SHIFT) {
                avatarPos[0] += 1;
                m->setValue( avatarPos[0], avatarPos[1], 0 );
                initCubes();
                initAvatar();
            }
        }
        
        if (key == GLFW_KEY_UP) {
            if (avatarPos[1] > -1
                && noWall(avatarPos[0], avatarPos[1] - 1))
            {
                avatarPos[1] -= 1;
                initAvatar();
            } else if (mods == GLFW_MOD_SHIFT) {
                avatarPos[1] -= 1;
                m->setValue( avatarPos[0], avatarPos[1], 0 );
                initCubes();
                initAvatar();
            }
        }
        
        if (key == GLFW_KEY_DOWN) {
            
            if ((avatarPos[1] < DIM || avatarPos[1] == -1)
                && noWall(avatarPos[0], avatarPos[1] + 1))
            {
                avatarPos[1] += 1;
                initAvatar();
            } else if (mods == GLFW_MOD_SHIFT) {
                avatarPos[1] += 1;
                m->setValue( avatarPos[0], avatarPos[1], 0 );
                initCubes();
                initAvatar();
            }
        }        
    }

    return eventHandled;
}
