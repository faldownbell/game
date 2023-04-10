#include <gl/gl.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>
#include "mEnu.h"
#include "tExtur.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"

using namespace std;

GLuint ch_tex;
GLuint bg;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);



class Character {
public:
    int frame,idle_frame = 1;
    int x_size,y_size = 160;
    int y_speed = 20;
    int x = 640;
    int y = -50;
    int x_speed = 2;
    void update_frame(){
        if(frame == 5 ){
            frame = 0;
        }
        else{
            frame++;
        }
    }
    void update_idle(){
        if(idle_frame == 5 ){
            idle_frame = 0;
        }
        else{
            idle_frame++;
        }
        frame = 0;
    }
};


void mEnu_init(){
    Menu_AddButton("X", 1450, 50, 50, 50, 2);
    Menu_AddButton("Animation 1", 50, 250, 150, 30, 2);
    Menu_AddButton("Animation 2", 50, 300, 150, 30, 2);
    Menu_AddButton("Animation 3", 50, 350, 150, 30, 2);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1600,
                          900,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd, &rct);
    glOrtho(0,rct.right,0,rct.bottom,1,-1);
    mEnu_init();
    Character* alph = new Character();
    Load_Texture( "456.png", &ch_tex, GL_REPEAT, GL_REPEAT, GL_NEAREST);
    Load_Texture( "back.jpg", &bg, GL_REPEAT, GL_REPEAT, GL_NEAREST);
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            Show_Background(bg);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(0,rct.right,rct.bottom,0,1,-1);
            Menu_ShowMenu();
            glPopMatrix();
            if(GetKeyState(VK_SPACE)<0){
                animation = 1;
                Sprite_animation(alph);
            }
            else{
                if ((GetKeyState(VK_SHIFT)<0 && GetKeyState('D')<0)||(GetKeyState(VK_SHIFT)<0 && GetKeyState('A')<0)){
                    animation = 3;
                    Sprite_animation(alph);
                    alph -> x_speed = 4;
                }
                else{
                    if ((GetKeyState('A')<0 || GetKeyState('D')<0)){
                        animation = 3;
                        Sprite_animation(alph);
                        alph -> x_speed = 2;
                    }
                    else{
                        animation = 2;
                        Sprite_animation(alph);
                    }
                }
            }
            SwapBuffers(hDC);
            theta += 1.0f;
            if ((GetKeyState(VK_SPACE)<0 && GetKeyState(VK_RIGHT)<0)||(GetKeyState(VK_SPACE)<0 && GetKeyState(VK_LEFT)<0)){
                Sleep(50);
                continue;
            }
            Sleep (100);
        }
    }
    printf("0");
    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:
        Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONDOWN:
        MouseDown();
        break;

    case WM_LBUTTONUP:
        Menu_MouseUp();
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


