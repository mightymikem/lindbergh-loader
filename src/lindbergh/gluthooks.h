#ifndef GLUTHOOKS_H
#define GLUTHOOKS_H
#include <GL/glx.h>

int myCreateWindow(int *param1);
void hod4glXSwapBuffers(Display *dpy, GLXDrawable drawable);

#endif
// #include <GL/freeglut.h>

// /* The bitmap font structure taken from freeglut*/
// typedef struct tagSFG_Font SFG_Font;
// struct tagSFG_Font
// {
//     char *Name;                 /* The source font name             */
//     int Quantity;               /* Number of chars in font          */
//     int Height;                 /* Height of the characters         */
//     const GLubyte **Characters; /* The characters mapping           */

//     float xorig, yorig; /* Relative origin of the character */
// };

// extern SFG_Font fgFontFixed8x13;
// extern SFG_Font fgFontFixed9x15;
// extern SFG_Font fgFontHelvetica10;
// extern SFG_Font fgFontHelvetica12;
// extern SFG_Font fgFontHelvetica18;
// extern SFG_Font fgFontTimesRoman10;
// extern SFG_Font fgFontTimesRoman24;