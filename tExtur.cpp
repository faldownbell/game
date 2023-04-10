#include <gl/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"
#include "Character.hpp"


using namespace std;

typedef struct
{
    float left, right, top, bottom;
} spriteCoord;
spriteCoord vrtcoord;

int idle_where = 1;
int is_jump = 0;

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter)
{
    int twidth, thight, tcnt;
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,4);
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,twidth,thight,0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

}

void Show_Background(GLuint texture)
{
    static float svertix[]= {0,0,  0,900,  1600,900,  1600,0};
    static float TexCord[]= {0,1, 0,0, 1,0, 1,1 };

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,svertix);
    glTexCoordPointer(2,GL_FLOAT,0,TexCord);

    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Sprite_animation(Character *ch)
{
    static float svertix[]= {ch ->x,ch ->y, ch ->x + 2 * ch -> x_size,ch ->y, ch ->x + 2 * ch -> x_size,ch ->y + 2* ch -> y_size, ch ->x,ch ->y + 2* ch -> y_size};
    static float TexCord[]= {0,0, 0.12,0, 0.12,1, 0,1};
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ch_tex);
    glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,svertix);
    glTexCoordPointer(2,GL_FLOAT,0,TexCord);

    static float spriteXsize = 960;
    static float spriteYsize = 480;
    vrtcoord.left = (ch ->x_size * ch -> frame) / spriteXsize;
    vrtcoord.right = vrtcoord.left + (ch -> x_size / spriteXsize);
    vrtcoord.top = (ch -> y_size * animation) / spriteYsize;
    vrtcoord.bottom = vrtcoord.top + (ch -> y_size / spriteYsize);
    TexCord[1] = TexCord[3] = vrtcoord.bottom;
    TexCord[5] = TexCord[7] = vrtcoord.top;

    if (idle_where == 0){
        TexCord[2] = TexCord[4] = vrtcoord.left ;
        TexCord[0] = TexCord[6] = vrtcoord.right ;
    }

    if (idle_where == 1){
        TexCord[0] = TexCord[6] = vrtcoord.left ;
        TexCord[2] = TexCord[4] = vrtcoord.right ;
    }

    if (GetKeyState('A')<0)
    {
        TexCord[2] = TexCord[4] = vrtcoord.left ;
        TexCord[0] = TexCord[6] = vrtcoord.right ;
        idle_where = 0;
        ch -> x -= ch -> x_speed;
    }

    if (GetKeyState('D')<0)
    {
        TexCord[0] = TexCord[6] = vrtcoord.left ;
        TexCord[2] = TexCord[4] = vrtcoord.right ;
        idle_where = 1;
        ch -> x += ch -> x_speed;

    }
    if(is_jump == 0){
        ch -> y += ch -> y_speed;
    }
    if(ch -> y > 50){
        is_jump = 1;
    }
    if(ch -> y >= 10){
        ch -> y -= 10;
    }
    else{
        is_jump = 0;
        ch -> y = 0;
    }
    if(ch -> x > 90){
        ch -> x = 90;
    }
    else{
        if(ch -> x < -93){
            ch -> x = -93;
        }
    }
    glTranslatef(ch -> x*8,ch -> y,0);

    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}
