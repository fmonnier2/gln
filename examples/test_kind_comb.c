/*
 Copyright (c) 2010 Florent Monnier
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#define GL_GLEXT_PROTOTYPES

#if defined(__APPLE__) && !defined (VMDMESA)
  #include <OpenGL/gl.h>
  #include <OpenGL/glext.h>
#else
  #if defined(USE_GL3_CORE_PROFILE)
    #define GL3_PROTOTYPES 1
    #include <GL3/gl3.h>
  #else
    #include <GL/gl.h>
    #include <GL/glext.h>
  #endif
#endif

/* ==== Simple Example ================================ */

#include <math.h>
#include <gln.h>

/* using the SFML for windowing */

#include <SFML/Graphics.h>
#include <SFML/System.h>

/* vertices_kind = GLN_RGB_VERTICES3 */
float rgb_vertices[] = {
    /* RGB colors */   /* XYZ coords */
    0.0, 1.0, 0.0,    -1.0,  1.0, -1.0,
    0.0, 0.0, 0.0,    -1.0, -1.0, -1.0,
    1.0, 1.0, 0.0,    -1.0,  1.0,  1.0,
    1.0, 0.0, 0.0,    -1.0, -1.0,  1.0,
    1.0, 1.0, 1.0,     1.0,  1.0,  1.0,
    1.0, 0.0, 1.0,     1.0, -1.0,  1.0,
    0.0, 1.0, 1.0,     1.0,  1.0, -1.0,
    0.0, 0.0, 1.0,     1.0, -1.0, -1.0,
};
/* vertices_kind = GLN_NORMAL_UV_VERTICES3 */
float normal_uv_vertices[] = {
    /* RGB colors */     /* UV */      /* XYZ coords */
    -1.0,  1.0, -1.0,    0.0, 1.0,    -1.0,  1.0, -1.0,
    -1.0, -1.0, -1.0,    0.0, 0.0,    -1.0, -1.0, -1.0,
    -1.0,  1.0,  1.0,    1.0, 1.0,    -1.0,  1.0,  1.0,
    -1.0, -1.0,  1.0,    1.0, 0.0,    -1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,    1.0, 1.0,     1.0,  1.0,  1.0,
     1.0, -1.0,  1.0,    1.0, 0.0,     1.0, -1.0,  1.0,
     1.0,  1.0, -1.0,    0.0, 1.0,     1.0,  1.0, -1.0,
     1.0, -1.0, -1.0,    0.0, 0.0,     1.0, -1.0, -1.0,
};
unsigned int indices[] = {
    /* a cube, each face is made of 2 triangles */
    0,1,3,  3,2,0,
    4,5,7,  7,6,4,
    3,1,7,  7,5,3,
    0,2,4,  4,6,0,
    6,7,1,  1,0,6,
    2,3,5,  5,4,2,
};

void sfProcessEvents(sfRenderWindow* App)
{
    sfEvent Event;
    while (sfRenderWindow_GetEvent(App, &Event))
    {
      if (Event.Type == sfEvtClosed ||
         ((Event.Type == sfEvtKeyPressed) && (Event.Key.Code == sfKeyEscape)))
          sfRenderWindow_Close(App);

      if (Event.Type == sfEvtResized)
          glViewport(0, 0, Event.Size.Width, Event.Size.Height);
    }
}

int main()
{
    gln_meshSrc src;
    gln_matrices matrices;
    gln_drawMeshParams dm_params;

    int with_tex = 0;

    /*
    src.vertices_kind = GLN_RGB_VERTICES3;
    src.vertices_kind = GLN_UV_RGB_VERTICES3;
    src.vertices_kind = GLN_NORMAL_RGB_VERTICES3;
    */
    src.vertices_kind = GLN_NORMAL_UV_VERTICES3;

    switch (src.vertices_kind)
    {
    case GLN_RGB_VERTICES3:
      src.vertices = rgb_vertices;
      break;
    case GLN_NORMAL_UV_VERTICES3:
      src.vertices = normal_uv_vertices;
      with_tex = 1;
      break;
    }
    src.vertices_size = 8 * glnGetStride(src.vertices_kind);

    src.indices = indices;
    src.indices_size = 36;

    src.color[0] = 0.1;
    src.color[1] = 0.4;
    src.color[2] = 0.9;
    src.color[3] = 1.0;

    int width = 800;
    int height = 600;

    sfRenderWindow* App;
    sfClock *Clock;
    sfWindowSettings Settings = {24, 8, 0};
    sfVideoMode Mode = {width, height, 32};

    App = sfRenderWindow_Create(
                Mode, "gln example", sfResize | sfClose, Settings);
    if (!App) return EXIT_FAILURE;

    gln_mesh *my_mesh;
    my_mesh = glnMakeMesh(&src);

    sfRenderWindow_PreserveOpenGLStates(App, sfTrue);

    GLuint Texture = 0;
    if (with_tex)
    {
        /* load an OpenGL texture */
        sfImage* Image;
        Image = sfImage_CreateFromFile("./texture.png");
        if (!Image) return EXIT_FAILURE;

        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     sfImage_GetWidth(Image),
                     sfImage_GetHeight(Image), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     sfImage_GetPixelsPtr(Image));

        sfImage_Destroy(Image);

        /* bind our texture */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, Texture);
    }

    /* enable Z-buffer read and write */
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    /* background color */
    glClearColor(0.22, 0.22, 0.24, 1.0);

    /* set up a perspective projection matrix */
    glnPerspective(&matrices,
                60.0, (double) width / (double) height, 0.2, 40.0);

    /* create a clock for measuring the time elapsed */
    Clock = sfClock_Create();

    /* start the main loop */
    while (sfRenderWindow_IsOpened(App))
    {
        float time = sfClock_GetTime(Clock);
        sfProcessEvents(App);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Apply some transformations */
        glnLoadIdentity(&matrices);
        glnTranslate(&matrices, 0.0, 0.0, -5.0);
        glnRotated(&matrices, time * 50.0, 1.0, 0.0, 0.0);
        glnRotated(&matrices, time * 30.0, 0.0, 1.0, 0.0);
        glnRotated(&matrices, time * 90.0, 0.0, 0.0, 1.0);
#if 1
        float v = (cosf(time) + 1.0) / 2.0;
        dm_params.color[0] = v;        /* r */
        dm_params.color[1] = 1.0 - v;  /* g */
        dm_params.color[2] = 0.0;      /* b */
#endif
        glnDrawMesh(my_mesh, &matrices, &dm_params);

        sfRenderWindow_Display(App);
    }

    /* cleanup gln-lib resources */
    glnDeleteMesh(my_mesh);

    /* cleanup windowing resources */
    sfClock_Destroy(Clock);
    sfRenderWindow_Destroy(App);

    return EXIT_SUCCESS;
}

