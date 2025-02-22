//========================================================================
// GLFW 3.0 OS X - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2009-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _nsgl_platform_h_
#define _nsgl_platform_h_


#define _GLFW_PLATFORM_FBCONFIG
#define _GLFW_PLATFORM_CONTEXT_STATE        _GLFWcontextNSGL nsgl
#define _GLFW_PLATFORM_LIBRARY_OPENGL_STATE _GLFWlibraryNSGL nsgl


//========================================================================
// GLFW platform specific types
//========================================================================

//------------------------------------------------------------------------
// Platform-specific OpenGL context structure
//------------------------------------------------------------------------
typedef struct _GLFWcontextNSGL {
    id           pixelFormat;
    id           context;
} _GLFWcontextNSGL;


//------------------------------------------------------------------------
// Platform-specific library global data for NSGL
//------------------------------------------------------------------------
typedef struct _GLFWlibraryNSGL {
    // dlopen handle for dynamically loading OpenGL extension entry points
    void*           framework;

    // TLS key for per-thread current context/window
    pthread_key_t   current;

} _GLFWlibraryNSGL;


#endif // _nsgl_platform_h_
