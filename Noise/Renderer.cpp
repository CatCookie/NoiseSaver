#include "Window.cpp"

#include <fstream>
#include "glew.h"


static const char *fragmentCode = {
        "#version 400\n"
                "in vec3 Color;"
                "in vec2 TexCoord;"
                "uniform sampler2D tex;"
                "out vec4 outColor;"
                "void main()"
                "{"
                "outColor =  texture2D(tex, TexCoord); //*vec4(Color, 1.0); \n"
                "}"};

static const char *vertexCode = {
        "#version 400\n"
                "in vec2 texCoord;"
                "in vec2 position;"
                "in vec3 color;"
                "out vec3 Color;"
                "out vec2 TexCoord;"
                "void main()"
                "{"
                "TexCoord = texCoord;"
                "Color = color;"
                "gl_Position = vec4(position, 0.0, 1.0);"
                "}"};


class Renderer {

private:

    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    float ratio;
    float aspect;
    int width, height;

    //char* fragmentCode;
    //char* vertexCode;
    GLuint program_object;
    GLuint texture;
    GLuint vbo;
    GLuint ebo;
    GLuint vertex_shader;
    GLuint fragment_shader;

public:

    Renderer::Renderer(HWND hWnd, int w, int h) {
        hwnd = hWnd;
        hdc = GetDC(hwnd);

        width = w;
        height = h;
    }

    Renderer::Renderer() { }

    Renderer::~Renderer(void) {

    }

    void Renderer::InitGL() {
        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_SUPPORT_OPENGL |
                      PFD_DOUBLEBUFFER |
                      PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 1;
        pfd.cDepthBits = 1;

        int chosenPixelFormat = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, chosenPixelFormat, &pfd);

        hglrc = wglCreateContext(hdc);

        ratio = 2.0f / height;
        aspect = (float) width / height;
        glClearColor(0.0, 0.0, 0.0, 0.0);  //Set the cleared screen colour to black
        glViewport(0, 0, width,
                   height);   //This sets up the viewport so that the coordinates (0, 0) are at the top left of the window

        wglMakeCurrent(hdc, hglrc);

        GLenum res = glewInit();
        if (res != GLEW_OK) {
            MessageBox(0, (char *) glewGetErrorString(res), "error", 0);
            exit(1231);
        }

        //loadShader();

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex_shader, 1, &vertexCode, NULL);
        glShaderSource(fragment_shader, 1, &fragmentCode, NULL);
        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);
        program_object = glCreateProgram();
        glAttachShader(program_object, fragment_shader);
        glAttachShader(program_object, vertex_shader);
        glLinkProgram(program_object);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float vertices[] = {
                -1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0,
                1.0, -1.0, 0.0, 1.0, 0.0, 1.0, 1.0,
                -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);

        GLuint elements[] = {
                0, 1, 2,
                2, 3, 0
        };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        GLint posAttrib = glGetAttribLocation(program_object, "position");
        GLint colAttrib = glGetAttribLocation(program_object, "color");
        GLint texAttrib = glGetAttribLocation(program_object, "texCoord");

        glEnableVertexAttribArray(posAttrib);
        glEnableVertexAttribArray(colAttrib);
        glEnableVertexAttribArray(texAttrib);

        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (2 * sizeof(float)));
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (5 * sizeof(float)));

        glGenTextures(1, &texture);
    }

    void Renderer::CloseGL() {
        wglMakeCurrent(NULL, NULL);

        glDeleteTextures(1, &texture);
        glDeleteProgram(program_object);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);

        wglDeleteContext(hglrc);
        ReleaseDC(hwnd, hdc);
    }

    void Renderer::draw(void *noise, int texWidth, int texHeight) {
        wglMakeCurrent(hdc, hglrc);

        glUseProgram(program_object);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, noise);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texWidth, texHeight, 0, GL_LUMINANCE, GL_FLOAT, noise);

        glDrawArrays(GL_QUADS, 0, 4);

        glUseProgram(0);
        SwapBuffers(hdc);
    }

    void Renderer::loadShader() {
        /*
        using namespace std;

        int array_size;
        int position;

        ifstream fin("fragment.glsl", ios::ate);
        array_size = static_cast<int>( fin.tellg() ); // define the size of character array

        fin.seekg(ios::beg);

        fragmentCode = new char[array_size]; // allocating an array of 1kb
        position = 0; //this will be used incremently to fill characters in the array


        while(!fin.eof() && position < array_size)
        {
            fin.get(fragmentCode[position]); //reading one character from file to array
            position++;
        }
        fragmentCode[position-1] = '\0'; //placing character array terminating character
        fin.close();

        ifstream fin2("vertex.glsl", ios::ate);
        array_size = static_cast<int>( fin2.tellg() ); // define the size of character array

        fin2.seekg(ios::beg);
        vertexCode = new char[array_size]; // allocating an array of 1kb
        position = 0; //this will be used incremently to fill characters in the array


        while(!fin2.eof() && position < array_size)
        {
            fin2.get(vertexCode[position]); //reading one character from file to array
            position++;
        }
        vertexCode[position-1] = '\0'; //placing character array terminating character
        fin2.close();
        */
        return;
    }


};
