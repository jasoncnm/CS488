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

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
    : current_col( 0 )
{
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

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
    

    // DELETE FROM HERE...
    m = new Maze(DIM);
    m->digMaze();
    m->printMaze();
    
    // ...TO HERE
    
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
    initGrid();
    initCubes();

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
    
    cout << "Cube Count " << cubeCount << endl;
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
    size_t sz = 3 * 2 * 2 * (DIM+3); // 228

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

    cubeHeight = clamp(cubeHeight, 1.0f, 5.0f);
    
}

// TODO: Implement this function
void A1::resetGridStates() {

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

        ImGui::PushID( 0 );
        ImGui::ColorEdit3( "##Colour", colour );
        ImGui::SameLine();
        if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
            // Select this colour.
        }
        ImGui::PopID();

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
    // Create a global transformation for the model (centre it).
    mat4 W;
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

        // Draw the cubes
        glBindVertexArray( cubesVao );
        glUniform3f( col_uni, 0.3f, 0.3f, 0.7f );
        glDrawArrays( GL_TRIANGLES, 0, cubeCount * 6 * 6); 
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
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // The user clicked in the window.  If it's the left
        // mouse button, initiate a rotation.
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

        if (key == GLFW_KEY_R) {
            resetGridStates();
        }

        if (key == GLFW_KEY_SPACE) {
            cubeHeight += 0.5f;
            initCubes();
        }

        if (key == GLFW_KEY_BACKSPACE) {
            cubeHeight -= 0.5f;
            initCubes();
        }
    }

    return eventHandled;
}
