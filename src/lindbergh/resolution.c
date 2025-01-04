#include "config.h"
#include <GL/gl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "patch.h"
#include "resolution.h"

int srtvX = 0;
int srtvW, srtvH;
int phX, phY, phW, phH;

int vf5FSwidth;

void *glVertex3fPatchRetAddrABC[4];
void *glVertex3fNoPatchRetAddrABC[4];

GLenum curTargetABC = 0;
GLuint curTextureIDABC = 0;
const GLfloat *texCoordABC[4];

vertexABC vABC[4];
int texCoordIdxABC = 0;
int vertex3fIdxABC = 0;
int creditCountABC = 0;
int lockedCountABC = 0;
float prevCreditXABC = 0;
void *abcDrawSpriteCAVEAddress;
int textureIDABC;
float newWidthRange_CEI_ABC, newHeightRange_CEI_ABC;
float newWidthABC, newHeightABC;
float newReducedWidthABC, newReducedHeightABC;
float newHalfWidthABC, newHalfHeightABC;
float newWidthRangeABC, newHeightRangeABC;
double new410ABC, new330ABC, new280ABC, new200ABC;
uint32_t *newWidthRangePtr_CEI_ABC, *newHeightRangePtr_CEI_ABC;
uint32_t *newWidthPtrABC, *newHeightPtrABC;
uint32_t *newReducedWidthPtrABC, *newReducedHeightPtrABC;
uint32_t *newHalfWidthPtrABC, *newHalfHeightPtrABC;
uint32_t *new410PtrABC, *new330PtrABC, *new280PtrABC, *new200PtrABC;
float angleABC;

float newVT3, newVT3HW, newVT3HH;
uint32_t *intnewVT3PTr, *intnewVT3HWPTr, *intnewVT3HHPTr;

void *idDisplayTextureCAVEAddress;
void *idDrawBallonCAVEAddress;
void *id4NewCaptionYCAVEAddress;
void *id4DrawTextAddress;
void *iddrawBallonPutAddress;
void *idTestTextAddress;
int idTextShift = 0;
float id4NewCaptionY;
float idShiftX = 0.0;
float isShiftY = 0.0;

uint32_t harleyCreditsOffsets[] = {
    0x000000e8, 0x0000012c, 0x00000170, 0x00000468, 0x000004ac, 0x000004f0, 0x00000578, 0x000005bc, 0x00000a88, 0x00000c48, 0x00000c8c,
    0x00000e98, 0x00000edc, 0x00000f20, 0x00000f64, 0x00000fa8, 0x00000fec, 0x00001030, 0x00001074, 0x000010b8, 0x00001638, 0x0000167c,
    0x000016c0, 0x00001704, 0x00001748, 0x0000178c, 0x000017d0, 0x00001814, 0x00001da8, 0x00001dec, 0x00002008, 0x0000204c, 0x00002090,
    0x000020d4, 0x00002118, 0x0000215c, 0x000021a0, 0x000021e4, 0x00002228, 0x000027a8, 0x000027e0, 0x00002830, 0x00002874, 0x000028b8,
    0x000028fc, 0x00002940, 0x00002e48, 0x00002e8c, 0x000030a8, 0x000030ec, 0x00003130, 0x00003174, 0x000034b8, 0x000034fc, 0x00003540,
    0x00003584, 0x000038b8, 0x000038fc, 0x00003b58, 0x00003b9c, 0x00003f48, 0x000041c8, 0x0000420c, 0x00004250, 0x00004294};

int harleyCreditsOffsetsSize = 65;

int hummerRespatch()
{
    void *addr = __builtin_return_address(0);
    if ((addr == (void *)0x080d8b5d) || (addr == (void *)0x080d8b93) || (addr == (void *)0x080d8bff) || (addr == (void *)0x080d8b19) ||
        (addr == (void *)0x080d8b4f) || (addr == (void *)0x080d8bbb) || (addr == (void *)0x08159eb0) || (addr == (void *)0x08159eec) ||
        (addr == (void *)0x08159f64) || (addr == (void *)0x08163490) || (addr == (void *)0x081634cc) || (addr == (void *)0x08163544))
    {
        return getConfig()->width - 1;
    }
    else if ((addr == (void *)0x080d8b78) || (addr == (void *)0x080d8bae) || (addr == (void *)0x080d8c1a) || (addr == (void *)0x080d8b34) ||
             (addr == (void *)0x080d8b6a) || (addr == (void *)0x080d8bd6) || (addr == (void *)0x08159ece) || (addr == (void *)0x08159f0a) ||
             (addr == (void *)0x08159f82) || (addr == (void *)0x081634ae) || (addr == (void *)0x081634ea) || (addr == (void *)0x08163562))
    {
        return getConfig()->height;
    }
    return 1280;
}

size_t harleyFreadReplace(void *buf, size_t size, size_t count, FILE *stream)
{
    size_t (*_fread)(void *buf, size_t size, size_t count, FILE *stream) = dlsym(RTLD_NEXT, "fread");

    int s = _fread(buf, size, count, stream);
    if (getConfig()->width == 1360 || getConfig()->width == 640)
        return s;

    for (int x = 0; x < harleyCreditsOffsetsSize; x++)
    {
        float oldX, idx;
        idx = 1.2; //((float)getConfig()->width / 1360.0) * 0.81459;
        memcpy(&oldX, buf + harleyCreditsOffsets[x], sizeof(float));
        float newX = oldX - (((float)getConfig()->width - 1360.0) / 2);
        float newY = (((600.0 * (float)getConfig()->height) / 768.0) - 600.0) * -1;

        if (getConfig()->width > 1920)
        {
            newX = oldX - ((((float)getConfig()->width - 1360.0) / 2) / idx);
            newY = (((600.0 * (float)getConfig()->height) / 768.0) - (600.0 * idx)) * -1;
            // Move the words "CREDIT(S)" and  "TO START" and "TO CONTINUE" a little to the right
            if (harleyCreditsOffsets[x] == 0x00000f20 || harleyCreditsOffsets[x] == 0x00001030 || harleyCreditsOffsets[x] == 0x00000f64 ||
                harleyCreditsOffsets[x] == 0x00001074 || harleyCreditsOffsets[x] == 0x0000167c || harleyCreditsOffsets[x] == 0x00001814 ||
                harleyCreditsOffsets[x] == 0x00001638 || harleyCreditsOffsets[x] == 0x000017d0)
            {
                newX -= idx * 10;
            }
            // Do not scale "TO START" and "TO CONTINUE"
            if (harleyCreditsOffsets[x] != 0x0000167c && harleyCreditsOffsets[x] != 0x00001814 && harleyCreditsOffsets[x] != 0x00000f64 &&
                harleyCreditsOffsets[x] != 0x00001074)
            {
                memcpy(buf + harleyCreditsOffsets[x] - 8, &idx, sizeof(float));
                memcpy(buf + harleyCreditsOffsets[x] - 12, &idx, sizeof(float));
            }
        }
        memcpy(buf + harleyCreditsOffsets[x], &newX, sizeof(float));
        memcpy(buf + harleyCreditsOffsets[x] + 4, &newY, sizeof(float));
    }
    return s;
}

/**
 * Function which games use to scale the output, some games behave well when this is changed
 * others less so.
 */
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{

    void (*_glOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) =
        dlsym(RTLD_NEXT, "glOrtho");

    void *returnAddress = __builtin_return_address(0);

    switch (getConfig()->crc32)
    {

    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL:
    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL_REVB:
    {
        if (right == 1024.0 && bottom == 768.0)
        {
            right = 800.0;
            bottom = 600.0;
        }
    }
    break;
    case SEGA_RACE_TV:
    {
        if ((right == 640.0 && bottom == 480.0) && (getConfig()->width != 640 && getConfig()->height != 480))
        {
            right = 600000.0;
            bottom = 600000.0;
        }
    }
    break;
    default:
        break;
    }

    return _glOrtho(left, right, bottom, top, zNear, zFar);
}

/**
 * Function which games use to scale the output, some games behave well when this is changed
 * others less so.
 */
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    int (*_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height) = dlsym(RTLD_NEXT, "glViewport");

    void *returnAddress = __builtin_return_address(0);

    switch (getConfig()->crc32)
    {
    case HUMMER:
    case HUMMER_SDLX:
    {
        if (width == 1280 && height == 768)
        {
            width = getConfig()->width;
            height = getConfig()->height;
        }
    }
    break;
    case OUTRUN_2_SP_SDX:
    case OUTRUN_2_SP_SDX_TEST:
    case OUTRUN_2_SP_SDX_REVA:
    case OUTRUN_2_SP_SDX_REVA_TEST:
    case OUTRUN_2_SP_SDX_REVA_TEST2:
    {
        if ((width >= 800) && (width != 1024))
        {
            width = getConfig()->width;
            height = getConfig()->height;
        }
    }
    break;

    case SEGA_RACE_TV:
    {
        if (width == 640 && height == 480 && returnAddress == (void *)0x08123932)
        {
            x = srtvX;
            width = srtvW;
            height = srtvH;
        }
        else if (width > 640)
        {
            width = 0;
            height = 0;
        }
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_REVA:
    case THE_HOUSE_OF_THE_DEAD_4_REVA_TEST:
    case THE_HOUSE_OF_THE_DEAD_4_REVB:
    case THE_HOUSE_OF_THE_DEAD_4_REVB_TEST:
    case THE_HOUSE_OF_THE_DEAD_4_REVC:
    case THE_HOUSE_OF_THE_DEAD_4_REVC_TEST:
    {
        if (width == 640 && height == 480)
        {
            width = getConfig()->width;
            height = getConfig()->height;
        }
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL:
    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL_REVB:
    {
        if (width == 1024 && height == 768)
        {
            width = getConfig()->width;
            height = getConfig()->height;
            x = 250;
            y = 100;
        }
    }
    break;
    case LETS_GO_JUNGLE:
    {
        if (width == 1360 && height == 768)
        {
            width = getConfig()->width;
            height = getConfig()->height;
        }
    }
    break;
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVA:
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVB:
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVB_6000:
    case VIRTUA_FIGHTER_5_R:
    case VIRTUA_FIGHTER_5_R_REVD:
	case VIRTUA_FIGHTER_5_R_REVG:
    {
        if (width == vf5FSwidth)
        {
            width = getConfig()->width;
        }
    }
    break;
    default:
        break;
    }

    _glViewport(x, y, width, height);
}

void glTexImage2D(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format,
                  unsigned int type, const void *pixels)
{
    void (*_glTexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format,
                          unsigned int type, const void *pixels) = dlsym(RTLD_NEXT, "glTexImage2D");

    switch (getConfig()->crc32)
    {
    case OUTRUN_2_SP_SDX:
    case OUTRUN_2_SP_SDX_REVA:
    {
        void *addr = __builtin_return_address(0);
        if ((width >= 800) && (width != 1024) && (addr != (void *)0x80d78d5) && (addr != (void *)0x080d7941))
        {
            width = getConfig()->width;
            height = getConfig()->height;
        }
    }
    break;
    default:
        break;
    }
    _glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void glTexParameteri(unsigned int target, unsigned int pname, int param)
{

    void (*_glTexParameteri)(unsigned int, unsigned int, int) = dlsym(RTLD_NEXT, "glTexParameteri");

    switch (getConfig()->crc32)
    {
    case OUTRUN_2_SP_SDX:
    case OUTRUN_2_SP_SDX_REVA:
    {
        if (param == GL_NEAREST)
        {
            param = GL_LINEAR;
        }
    }
    break;
    default:
        break;
    }
    _glTexParameteri(target, pname, param);
}

int isTestMode()
{
    int fd = open("/proc/self/cmdline", O_RDONLY);
    if (fd == -1)
    {
        return false;
    }

    char buffer[4096];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n <= 0)
    {
        close(fd);
        return false;
    }
    close(fd);

    buffer[n] = '\0';

    char *arg = buffer;
    while (arg < buffer + n)
    {
        if (strcmp(arg, "-t") == 0)
            return true;
        arg += strlen(arg) + 1;
    }
    return false;
}

void glBindTextureABC(GLenum target, GLuint texture)
{
    // const char *targetName = "";
    // switch (target)
    // {
    // case GL_TEXTURE_2D:
    //     targetName = "GL_TEXTURE_2D";
    //     break;
    // case GL_TEXTURE_CUBE_MAP:
    //     targetName = "GL_TEXTURE_CUBE_MAP";
    //     break;
    // case GL_TEXTURE_3D:
    //     targetName = "GL_TEXTURE_3D";
    //     break;
    // case GL_TEXTURE_1D:
    //     targetName = "GL_TEXTURE_1D";
    //     break;
    // default:
    //     targetName = "UNKNOWN";
    //     break;
    // }
    curTargetABC = target;
    curTextureIDABC = texture;

    void (*_glBindTexture)(GLenum, GLuint) = dlsym(RTLD_NEXT, "glBindTexture");
    _glBindTexture(target, texture);
}

void scaleABCVertex()
{
    for (int idx = 0; idx < 4; idx++)
    {
        vABC[idx].x *= (getConfig()->width / 640.0);
        vABC[idx].y *= (getConfig()->height / 480.0);
    }
}

void scaleNumbers()
{
    lockedCountABC++;
    float newFontWidth = 8 * ((getConfig()->height / 480.0) * 0.8);
    float newFontHeight = 16 * ((getConfig()->height / 480.0) * 0.8);
    float newY = 8.0 * (getConfig()->height / 480.0);
    switch (lockedCountABC)
    {
    case 1:
    {
        float shiftLeft = ((newFontWidth * 2.5)) - 20.0;
        vABC[0].x -= shiftLeft;
    }
    break;
    case 2:
    {
        float shiftLeft = ((newFontWidth * 1.5)) - 12.0;
        vABC[0].x -= shiftLeft;
    }
    break;
    case 3:
    {
        vABC[0].x -= (newFontWidth / 2.0) - 4.0;
    }
    break;
    case 4:
    {
        float shitfRight = (newFontWidth / 2.0) - 4.0;
        vABC[0].x += shitfRight;
    }
    break;
    case 5:
    {
        float shitfRight = (newFontWidth * 1.5) - 12.0;
        vABC[0].x += shitfRight;
        lockedCountABC = 0;
    }
    }
    vABC[0].y += newY;
    vABC[1].x = vABC[0].x + newFontWidth;
    vABC[1].y = vABC[0].y;
    vABC[2].x = vABC[1].x;
    vABC[2].y = vABC[0].y + newFontHeight;
    vABC[3].x = vABC[0].x;
    vABC[3].y = vABC[2].y;
}

void scaleByAngle(float percentage, float shift)
{
    percentage = (percentage == 0.0 ? 1 : percentage / 100.0);
    float sizeX, sizeY, newHalfSizeX, newHalfSizeY;
    float idx = (getConfig()->height / 480.0) * percentage;
    switch ((int)angleABC)
    {
    case 0:
    {
        sizeX = vABC[0].x - vABC[3].x;
        sizeY = vABC[1].y - vABC[0].y;
        newHalfSizeX = ((sizeX * idx) - sizeX) / 2.0;
        newHalfSizeY = ((sizeY * idx) - sizeY) / 2.0;
        vABC[0].x += newHalfSizeX;
        vABC[0].y -= newHalfSizeY + shift;
        vABC[1].x += newHalfSizeX;
        vABC[1].y += newHalfSizeY - shift;
        vABC[2].x -= newHalfSizeX;
        vABC[2].y += newHalfSizeY - shift;
        vABC[3].x -= newHalfSizeX;
        vABC[3].y -= newHalfSizeY + shift;
    }
    break;
    case 90:
    {
        sizeX = vABC[1].x - vABC[0].x;
        sizeY = vABC[2].y - vABC[1].y;
        newHalfSizeX = ((sizeX * idx) - sizeX) / 2.0;
        newHalfSizeY = ((sizeY * idx) - sizeY) / 2.0;
        vABC[0].x -= newHalfSizeX + shift;
        vABC[0].y -= newHalfSizeY;
        vABC[1].x += newHalfSizeX - shift;
        vABC[1].y -= newHalfSizeY;
        vABC[2].x += newHalfSizeX - shift;
        vABC[2].y += newHalfSizeY;
        vABC[3].x -= newHalfSizeX + shift;
        vABC[3].y += newHalfSizeY;
    }
    break;
    case -90:
    {
        sizeX = vABC[3].x - vABC[2].x;
        sizeY = vABC[0].y - vABC[3].y;
        newHalfSizeX = ((sizeX * idx) - sizeX) / 2.0;
        newHalfSizeY = ((sizeY * idx) - sizeY) / 2.0;
        vABC[0].x += newHalfSizeX + shift;
        vABC[0].y += newHalfSizeY;
        vABC[1].x -= newHalfSizeX - shift;
        vABC[1].y += newHalfSizeY;
        vABC[2].x -= newHalfSizeX - shift;
        vABC[2].y -= newHalfSizeY;
        vABC[3].x += newHalfSizeX + shift;
        vABC[3].y -= newHalfSizeY;
    }
    break;
    case 180:
    case -180:
    {
        sizeX = vABC[2].x - vABC[1].x;
        sizeY = vABC[3].y - vABC[2].y;
        newHalfSizeX = ((sizeX * idx) - sizeX) / 2.0;
        newHalfSizeY = ((sizeY * idx) - sizeY) / 2.0;
        vABC[0].x -= newHalfSizeX;
        vABC[0].y += newHalfSizeY + shift;
        vABC[1].x -= newHalfSizeX;
        vABC[1].y -= newHalfSizeY - shift;
        vABC[2].x += newHalfSizeX;
        vABC[2].y -= newHalfSizeY - shift;
        vABC[3].x += newHalfSizeX;
        vABC[3].y += newHalfSizeY + shift;
    }
    }
}

void shiftTarget(float percentage)
{
    percentage = percentage / 100;
    float size, newHalfSize;
    float idx = (getConfig()->height / 480.0) * percentage;
    switch ((int)angleABC)
    {
    case 0:
    {
        size = vABC[0].x - vABC[3].x;
        newHalfSize = ((size * idx) - size);
        vABC[0].x += newHalfSize;
        vABC[1].x += newHalfSize;
        vABC[2].x += newHalfSize;
        vABC[3].x += newHalfSize;
    }
    break;
    case 90:
    {
        size = vABC[2].y - vABC[1].y;
        newHalfSize = ((size * idx) - size);
        vABC[0].y -= newHalfSize;
        vABC[1].y -= newHalfSize;
        vABC[2].y -= newHalfSize;
        vABC[3].y -= newHalfSize;
    }
    break;
    case -90:
    {
        size = vABC[0].y - vABC[3].y;
        newHalfSize = ((size * idx) - size);
        vABC[0].y += newHalfSize;
        vABC[1].y += newHalfSize;
        vABC[2].y += newHalfSize;
        vABC[3].y += newHalfSize;
    }
    break;
    case 180:
    case -180:
    {
        size = vABC[2].x - vABC[1].x;
        newHalfSize = ((size * idx) - size);
        vABC[0].x -= newHalfSize;
        vABC[1].x -= newHalfSize;
        vABC[2].x -= newHalfSize;
        vABC[3].x -= newHalfSize;
    }
    break;
    }
}

void glTexCoord2fvABC(const GLfloat *v)
{
    texCoordABC[texCoordIdxABC] = v;
    texCoordIdxABC++;
    if (texCoordIdxABC == 4)
        texCoordIdxABC = 0;
}

void glVertex3fABC1(GLfloat x, GLfloat y, GLfloat z)
{
    vABC[vertex3fIdxABC].x = x;
    vABC[vertex3fIdxABC].y = y;
    vABC[vertex3fIdxABC].z = z;
    if (vertex3fIdxABC < 3)
    {
        vertex3fIdxABC++;
        return;
    }

    if (curTargetABC == GL_TEXTURE_2D && (getConfig()->width != 640 || getConfig()->height != 480))
    {
        float sizeX, sizeY;
        sizeX = vABC[1].x - vABC[0].x;
        sizeY = vABC[2].y - vABC[1].y;

        bool scale = true;

        // Cut scene video
        if (textureIDABC == 0x20350)
        {
            float idx = (getConfig()->height / 480.0);
            if (vABC[2].y < 65.0 && vABC[3].y < 65.0)
            {
                vABC[2].y *= idx * 1.5;
                vABC[3].y *= idx * 1.5;
            }
            else if ((vABC[0].y > (getConfig()->height - 65.0)) && (vABC[1].y > (getConfig()->height - 65.0)))
            {
                vABC[0].y += (getConfig()->height - vABC[0].y) - ((getConfig()->height - vABC[0].y) * (idx * 1.5));
                vABC[1].y += (getConfig()->height - vABC[1].y) - ((getConfig()->height - vABC[1].y) * (idx * 1.5));
            }
            scale = false;
        }
        // 0x20001 Effect in Engage?, 0x20013 Flash effect at the beginning of the game
        else if (textureIDABC == 0x20001 || textureIDABC == 0x20013)
        {
            scale = false;
        }
        // Arrows of the red and yellow circle targets in some enemies.
        else if (textureIDABC == 0x201C0 || textureIDABC == 0x201C1 || textureIDABC == 0x201C2)
        {
            scale = false;
        }
        // 0x20250 Light blue circle in enemies when locked
        else if (textureIDABC == 0x20250)
        {
            scaleByAngle(60, 0);
            shiftTarget(60);
            scale = false;
        }
        // 0x20251 Red circle target in some enemies when locked
        else if (textureIDABC == 0x20251)
        {
            scale = false;
        }
        // 0x200fd Light Blue cross that scales when you shoot.
        else if (textureIDABC == 0x200fd)
        {
            scaleByAngle(60, 0);
            scale = false;
        }
        // 0x200fe Red cross in enemy planes when you shoot 12x12
        else if (textureIDABC == 0x200fe)
        {
            scaleByAngle(60, 0);
            scale = false;
        }
        // 0x20342, 0x20343 Star
        else if (textureIDABC == 0x20349)
        {
            scale = false;
        }
        // X between Missile Icon and Infinite Icon while in Climax mode
        else if (textureIDABC == 0x20302)
        {
            scale = true;
        }
        // Press Start Button / Insert Coins
        else if (textureIDABC == 0x20138 || textureIDABC == 0x2013a)
        {
            float newSizeX = sizeX * (getConfig()->width / 640.0);
            float newSizeY = sizeY * (getConfig()->height / 480.0);
            vABC[0].x = (getConfig()->width - newSizeX) / 2;
            vABC[1].x = vABC[0].x + newSizeX;
            vABC[2].x = vABC[1].x;
            vABC[3].x = vABC[0].x;

            vABC[0].y = getConfig()->height - ((newSizeY * 2) + 30);
            vABC[1].y = vABC[0].y;
            vABC[2].y = vABC[0].y + newSizeY;
            vABC[3].y = vABC[0].y + newSizeY;
            scale = false;
        }
        // FREE PLAY / Credits / Credit count
        else if (textureIDABC >= 0x20057 && textureIDABC <= 0x20062)
        {
            float newSizeX = sizeX * (getConfig()->width / 640.0);
            float newSizeY = sizeY * (getConfig()->height / 480.0);

            vABC[0].x = (getConfig()->width - newSizeX) / 2;
            if (sizeX == 88.0)
            {
                vABC[0].x -= (16 * (getConfig()->width / 640.0)) * 1.5;
            }
            if (sizeX == 16.0)
            {
                float creditsSize = (88 * (getConfig()->width / 640.0)) / 2;
                if (prevCreditXABC == x)
                    creditCountABC = 1;
                if (creditCountABC == 0)
                {
                    vABC[0].x += (creditsSize - newSizeX);
                    creditCountABC += 1;
                }
                else
                {
                    vABC[0].x += (creditsSize - newSizeX) + (newSizeX * 0.86);
                    creditCountABC = 0;
                }
                prevCreditXABC = x;
            }

            vABC[1].x = vABC[0].x + newSizeX;
            vABC[2].x = vABC[1].x;
            vABC[3].x = vABC[0].x;

            vABC[0].y = getConfig()->height - (newSizeY + 20);
            vABC[1].y = vABC[0].y;
            vABC[2].y = vABC[0].y + newSizeY;
            vABC[3].y = vABC[0].y + newSizeY;
            scale = false;
        }
        // Text LOCKED in Climax mode
        else if (textureIDABC == 0x20064)
        {

            float sizeIncX = 26.0 * (getConfig()->height / 480.0);
            float sizeIncY = 8.0 * (getConfig()->height / 480.0);
            vABC[0].x -= sizeIncX / 2.0;
            vABC[1].x += sizeIncX / 2.0;
            vABC[2].x += sizeIncX / 2.0;
            vABC[2].y += sizeIncY;
            vABC[3].x -= sizeIncX / 2.0;
            vABC[3].y += sizeIncY;
            scale = false;
        }
        //  0, 1, 2, 3, 4, 5, 6, 7, 8, 9 Numbers in Climax Effect and "/"
        else if (textureIDABC == 0x20065 || textureIDABC == 0x20066 || textureIDABC == 0x20067 || textureIDABC == 0x20068 ||
                 textureIDABC == 0x2009d || textureIDABC == 0x20069 || textureIDABC == 0x2006a || textureIDABC == 0x2006b ||
                 textureIDABC == 0x2006c || textureIDABC == 0x2006d || textureIDABC == 0x2006e)
        {

            scaleNumbers();
            scale = false;
        }
        // Climax effect, 0x200cf Part of the Climax effect (glow around the aim?)
        else if (textureIDABC == 0x00200 || textureIDABC == 0x200d5 || textureIDABC == 0x200d6 || textureIDABC == 0x200d7 ||
                 textureIDABC == 0x200d8 || textureIDABC == 0x200d9 || textureIDABC == 0x200DA || textureIDABC == 0x200db ||
                 textureIDABC == 0x200dc || textureIDABC == 0x200dd || textureIDABC == 0x200de || textureIDABC == 0x200df ||
                 textureIDABC == 0x200e0 || textureIDABC == 0x2013B || textureIDABC == 0x2013F || textureIDABC == 0x2013E ||
                 textureIDABC == 0x20140 || textureIDABC == 0x20146 || textureIDABC == 0x20147 ||
                 (textureIDABC >= 0x20276 && textureIDABC <= 0x20284) || textureIDABC == 0x200cf)
        {
            scale = false;
        }
        // Stage name Effect
        else if (textureIDABC == 0x200d0)
        {
            for (int x = 0; x < 4; x++)
            {
                vABC[x].x = vABC[x].x * (getConfig()->width / 640.0);
                vABC[x].y = vABC[x].y * (getConfig()->height / 480.0);
            }
            scale = false;
        }
        // Crosshair
        else if (textureIDABC == 0x2014d)
        {
            scaleByAngle(80, 0);
            scale = false;
        }
        // Green arrows in the crosshair
        else if (textureIDABC == 0x2014c)
        {
            float shift = (20.0 * (getConfig()->height / 480.0) - 20.0) / 2.0;
            scaleByAngle(80, shift - 5.0);
            switch ((int)angleABC)
            {
            case 0:
            {
                // Top
                vABC[0].x -= 2;
                vABC[1].x -= 2;
                vABC[2].x -= 2;
                vABC[3].x -= 2;
            }
            break;
            case 90:
            {
                // Left
                vABC[0].x += 3;
                vABC[1].x += 3;
                vABC[2].x += 3;
                vABC[3].x += 3;
            }
            break;
            case -90:
            {
                // Right
                vABC[0].x -= 4;
                vABC[0].y -= 1;
                vABC[1].x -= 4;
                vABC[1].y -= 1;
                vABC[2].x -= 4;
                vABC[2].y -= 1;
                vABC[3].x -= 4;
                vABC[3].y -= 1;
            }
            break;
            case 180:
            case -180:
            {
                // Bottom
                vABC[0].y -= 2;
                vABC[1].y -= 2;
                vABC[2].y -= 2;
                vABC[3].y -= 2;
            }
            }
            scale = false;
        }
        // Enemy pointing Arrows in Climax Effect
        else if (textureIDABC == 0x20141 || textureIDABC == 0x20142 || textureIDABC == 0x20143 || textureIDABC == 0x20144 ||
                 textureIDABC == 0x20145)
        {
            float sizeIncX = 6.0 * (getConfig()->height / 480.0) - 6.0;
            float sizeIncY = 10.0 * (getConfig()->height / 480.0) - 10.0;
            vABC[0].x -= sizeIncX / 2.0;
            vABC[0].y -= sizeIncY / 2.0;
            vABC[1].x += sizeIncX / 2.0;
            vABC[1].y -= sizeIncY / 2.0;
            vABC[2].x += sizeIncX / 2.0;
            vABC[2].y += sizeIncY / 2.0;
            vABC[3].x -= sizeIncX / 2.0;
            vABC[3].y += sizeIncY / 2.0;
            scale = false;
        }
        // 0x201E2 Friend, 0x201e3 Target, 0x201C5 Enemy and 0x201C6 Missile words
        else if (textureIDABC == 0x201E2 || textureIDABC == 0x201e3 || textureIDABC == 0x201C5 || textureIDABC == 0x201C6)
        {
            float newHalfSizeX = ((sizeX * ((getConfig()->height / 480.0) * 0.8)) - sizeX) / 2.0;
            float newSizeY = (sizeY * ((getConfig()->height / 480.0) * 0.8)) - sizeY;
            vABC[0].x -= newHalfSizeX;
            vABC[0].y -= newSizeY * 2;
            vABC[1].x += newHalfSizeX;
            vABC[1].y -= newSizeY * 2;
            vABC[2].x += newHalfSizeX;
            vABC[2].y -= newSizeY;
            vABC[3].x -= newHalfSizeX;
            vABC[3].y -= newSizeY;
            scale = false;
        }
        // 0x201e5 Red, 0x20291 clear, 0x201e4 Blinking Arrows
        // 0x2034E Green, 0x201e6 clear, 0x20312 Blinking Arrows
        // 0x201C7 Yellow, 0x200FF clear, 0x202E7 Blinking Arrows
        else if (textureIDABC == 0x20291 || textureIDABC == 0x2034E || textureIDABC == 0x200FF || textureIDABC == 0x202E7 ||
                 textureIDABC == 0x20312 || textureIDABC == 0x201e4 || textureIDABC == 0x201e5 || textureIDABC == 0x201e6 ||
                 textureIDABC == 0x201C7)
        {
            float newHalfSizeX = ((sizeX * ((getConfig()->height / 480.0) * 0.8)) - sizeX) / 2.0;
            float newSizeY = (sizeY * ((getConfig()->height / 480.0) * 0.8)) - sizeY;
            vABC[0].x -= newHalfSizeX;
            vABC[0].y -= newSizeY;
            vABC[1].x += newHalfSizeX;
            vABC[1].y -= newSizeY;
            vABC[2].x += newHalfSizeX;
            vABC[3].x -= newHalfSizeX;
            scale = false;
        }
        // Fonts and Subtitles in Cut scene Y position
        else if (textureIDABC == 0x30008)
        {
            int width = getConfig()->width;
            int height = getConfig()->height;
            float idx = ((height / 480.0) / 10) + 1;
            float heightShiftUp = height * ((((height / 2.0) - 216) / height) - 0.05);
            if (vABC[0].y == ((height / 2.0) - 216) || vABC[0].y == (((height / 2.0) - 216) + sizeY + 2))
            {
                // Top Subtitles
                for (int x = 0; x < 4; x++)
                {
                    vABC[x].y -= heightShiftUp;
                }
            }
            else if (vABC[0].y == 446.0) // 0/8 text scaled to fit in box
            {
                float originalPosition = vABC[0].x;
                vABC[1].x -= sizeX - 8;
                vABC[2].x -= sizeX - 8;
                vABC[2].y -= sizeY - 16;
                vABC[3].y -= sizeY - 16;
                scaleABCVertex();

                float widthScaleIdx = getConfig()->width / 640.0;
                float fontSizeScaled = 8 * (getConfig()->height / 480.0);
                float fontWidthAfterScaled = 8 * widthScaleIdx;
                if (originalPosition == (374 - fontSizeScaled))
                {
                    // First number
                    vABC[0].x = ((374 * widthScaleIdx) - (fontWidthAfterScaled + 5));
                }
                else if (originalPosition == (374 + fontSizeScaled))
                {
                    // Second number
                    vABC[0].x = ((374 * widthScaleIdx) + (fontWidthAfterScaled + 5));
                }
                else if (originalPosition == (374 + (fontSizeScaled * 2)))
                {
                    // Third Number
                    vABC[0].x = ((374 * widthScaleIdx) + ((fontWidthAfterScaled * 2)));
                }
                vABC[1].x = vABC[0].x + fontWidthAfterScaled;
                vABC[2].x = vABC[1].x;
                vABC[3].x = vABC[0].x;
            }
            else
            {
                // Bottom Subtitles
                for (int x = 0; x < 4; x++)
                {
                    vABC[x].y *= idx;
                }
            }
            scale = false;
        }
        // MainBD text
        else if (textureIDABC == 0x3000b)
        {
            if (!isTestMode())
            {
                for (int x = 0; x < 4; x++)
                {
                    vABC[x].y *= ((getConfig()->height / 480.0) / 10) + 1;
                }
            }
            scale = false;
        }
        if (scale)
        {
            scaleABCVertex();
        }
    }
    for (int x = 0; x < 4; x++)
    {
        glTexCoord2fv(texCoordABC[x]);
        glVertex3f(vABC[x].x, vABC[x].y, vABC[x].z);
    }
    vertex3fIdxABC = 0;
}

void glVertex3fABC2(GLfloat x, GLfloat y, GLfloat z)
{
    void (*_glVertex3f)(GLfloat x, GLfloat y, GLfloat z) = dlsym(RTLD_NEXT, "glVertex3f");

    GLfloat scaledX = x * (getConfig()->width / 640.0);
    GLfloat scaledY = y * (getConfig()->height / 480.0);

    _glVertex3f(scaledX, scaledY, z);
}

void glClearColorPH(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    void (*original_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) =
        (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))dlsym(RTLD_NEXT, "glClearColor");

    if (red == 0.5f)
    {
        red = 0.00f;
        green = 0.00f;
        blue = 0.00f;
        alpha = 0.00f;
    }
    glClearColor(red, green, blue, alpha);
}

void vf5WidthFix(void *param1)
{
    if (getConfig()->width > 1920)
        setVariable(0x86eb208, 1920);
    (((void (*)(void *))0x080d4c24)(param1));
}

__attribute__((naked)) void idDisplayTextureFixCAVE(void *p1, void *p2, int p3, int p4, int p5)
{
    __asm__ volatile("push %%esi\n\tpush %%ebx\n\tsub $0x14, %%esp\n\tjmp *%0\n\t" ::"m"(idDisplayTextureCAVEAddress));
}

__attribute__((naked)) void idDrawBallonFixCAVE(void *p1, void *p2, float p3, float p4, float p5, float p6, float p7, bool p8)
{
    __asm__ volatile("push %%edi\n\tpush %%esi\n\tpush %%ebx\n\tsub $0x50, %%esp\n\tjmp *%0\n\t" ::"m"(idDrawBallonCAVEAddress));
}

__attribute__((naked)) void yTxtPatch()
{
    asm volatile("subss %0, %%xmm0\njmp *%1\n" ::"m"(id4NewCaptionY), "m"(id4NewCaptionYCAVEAddress));
}

__attribute__((naked)) void abcDrawSpriteFixCAVE(void *p1)
{
    __asm__ volatile("push %%ebp\n\tmov $0x3, %%ecx\n\tjmp *%0\n\t" ::"m"(abcDrawSpriteCAVEAddress));
}

void abcDrawSprite(void *p1)
{
    SprArgs *x = (SprArgs *)p1;
    textureIDABC = x->id;
    Matrix4f mat = x->mat;
    angleABC = atan2(mat.m00, mat.m10) * (180.0 / M_PI);
    abcDrawSpriteFixCAVE(p1);
}

void idDisplayTexture(void *p1, void *p2, int p3, int p4, int p5)
{
    float newX = 0;
    if (p4 > 200)
        newX = ((float)getConfig()->width / 2) - ((1360.0 / 2) - p4);
    else
        newX = p4;

    float newY = (float)getConfig()->height * (p5 / 768.0);

    idDisplayTextureFixCAVE(p1, p2, p3, (int)newX, (int)newY);
}

void calculateShift(int W, int H, float *ShiftX, float *ShiftY, bool right)
{
    int baseWidth1 = 1360, baseHeight1 = 768;
    int baseWidth2 = 2560, baseHeight2 = 1440;

    float knownShiftX1 = 0, knownShiftY1 = 0;
    float knownShiftX2 = 250, knownShiftY2 = 100;
    if (right)
        knownShiftX2 = 900;

    *ShiftX = ((float)(W - baseWidth1) / (baseWidth2 - baseWidth1)) * (knownShiftX2 - knownShiftX1);
    *ShiftY = ((float)(H - baseHeight1) / (baseHeight2 - baseHeight1)) * (knownShiftY2 - knownShiftY1);
}

void idDrawBallon(void *p1, void *p2, float p3, float p4, float p5, float p6, float p7, bool p8)
{
    calculateShift(getConfig()->width, getConfig()->height, &idShiftX, &isShiftY, p8);
    idDrawBallonFixCAVE(p1, p2, p3 + idShiftX, p4 + isShiftY, p5, p6, p7, p8);
}

void idBalloonPut(void *p1, float p2, float p3, ushort p4)
{
    (((void (*)(void *p1, float p2, float p3, ushort p4))iddrawBallonPutAddress)(p1, p2 + idShiftX, p3 + isShiftY, p4));
}

void id4DrawText(void *p1, void *p2, float p3, float p4)
{
    (((void (*)(void *p1, void *p2, float p3, float p4))id4DrawTextAddress)(p1, p2, idShiftX + p3, isShiftY + p4));
}

void idTestText(long p1, long p2, unsigned int p3, char *p4)
{
    (((void (*)(long p1, long p2, unsigned int p3, char *p4))idTestTextAddress)(p1 - idTextShift, p2, p3, p4));
}

void patchABCMarkers(uint32_t addrCtrlEnemyInfo, uint32_t addrCtrlPlayerRivalFont, uint32_t addrCtrlEnemyPursuit2d)
{
    // ctrl_enemy_information
    newWidthRange_CEI_ABC = getConfig()->width + 100.0;
    newHeightRange_CEI_ABC = getConfig()->height + 68.0;
    uint32_t *widthRangePtr_CEI = (uint32_t *)&newWidthRange_CEI_ABC;   // 750
    uint32_t *heightRangePtr_CEI = (uint32_t *)&newHeightRange_CEI_ABC; // 548
    setVariable(addrCtrlEnemyInfo + 0x127, (size_t)widthRangePtr_CEI);  // 750
    setVariable(addrCtrlEnemyInfo + 0x13d, (size_t)heightRangePtr_CEI); // 548
    setVariable(addrCtrlEnemyInfo + 0x42e, (size_t)widthRangePtr_CEI);  // 750
    setVariable(addrCtrlEnemyInfo + 0x44c, (size_t)heightRangePtr_CEI); // 548

    // Ctrl_Enemy_Pursuit_2d and Ctrl_Player, Rival_Font
    newWidthABC = (float)getConfig()->width;               // 640
    newHeightABC = (float)getConfig()->height;             // 480
    newReducedWidthABC = (float)getConfig()->width - 40;   // 600
    newReducedHeightABC = (float)getConfig()->height - 40; // 440
    newHalfWidthABC = (float)getConfig()->width / 2.0;     // 320
    newHalfHeightABC = (float)getConfig()->height / 2.0;   // 240
    newWidthRangeABC = getConfig()->width + 90.0;          // 730
    newHeightRangeABC = getConfig()->height + 90.0;        // 570
    new410ABC = (getConfig()->width / 2.0) + 90.0;         // 410
    new330ABC = (getConfig()->height / 2.0) + 90.0;        // 330
    new280ABC = (getConfig()->width / 2.0) - 40.0;         // 280
    new200ABC = (getConfig()->height / 2.0) - 40.0;        // 200

    newWidthPtrABC = (uint32_t *)&newWidthABC;
    newHeightPtrABC = (uint32_t *)&newHeightABC;
    setVariable(addrCtrlPlayerRivalFont + 0x15b, (size_t)newWidthPtrABC);  // 640
    setVariable(addrCtrlPlayerRivalFont + 0x163, (size_t)newHeightPtrABC); // 480
    setVariable(addrCtrlEnemyPursuit2d + 0x80, (size_t)newWidthPtrABC);    // 640
    setVariable(addrCtrlEnemyPursuit2d + 0x88, (size_t)newHeightPtrABC);   // 480

    newReducedWidthPtrABC = (uint32_t *)&newReducedWidthABC;
    newReducedHeightPtrABC = (uint32_t *)&newReducedHeightABC;
    setVariable(addrCtrlPlayerRivalFont + 0x272, (size_t)newReducedWidthPtrABC);     // 600
    setVariable(addrCtrlPlayerRivalFont + 0x29f, (size_t)newReducedHeightPtrABC);    // 440
    setVariable(addrCtrlPlayerRivalFont + 0x8ff, *(uint32_t *)&newReducedHeightABC); // 440
    setVariable(addrCtrlPlayerRivalFont + 0x96b, *(uint32_t *)&newReducedHeightABC); // 440

    setVariable(addrCtrlEnemyPursuit2d + 0x138, (size_t)newReducedWidthPtrABC);     // 600
    setVariable(addrCtrlEnemyPursuit2d + 0x15f, (size_t)newReducedHeightPtrABC);    // 440
    setVariable(addrCtrlEnemyPursuit2d + 0x7b3, *(uint32_t *)&newReducedHeightABC); // 440
    setVariable(addrCtrlEnemyPursuit2d + 0x812, *(uint32_t *)&newReducedHeightABC); // 440

    newHalfWidthPtrABC = (uint32_t *)&newHalfWidthABC;
    newHalfHeightPtrABC = (uint32_t *)&newHalfHeightABC;
    setVariable(addrCtrlPlayerRivalFont + 0x346, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlPlayerRivalFont + 0x34e, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlPlayerRivalFont + 0x4cb, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlPlayerRivalFont + 0x4d3, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlPlayerRivalFont + 0x519, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlPlayerRivalFont + 0x511, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlPlayerRivalFont + 0x6dc, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlPlayerRivalFont + 0x6e4, (size_t)newHalfHeightPtrABC); // 240
    // ctrl_player_info_rival_arrow
    setVariable(addrCtrlPlayerRivalFont + 0xbc2, *(uint32_t *)&newHalfWidthABC); // 320

    setVariable(addrCtrlEnemyPursuit2d + 0x1f7, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlEnemyPursuit2d + 0x1ff, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlEnemyPursuit2d + 0x35d, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlEnemyPursuit2d + 0x365, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlEnemyPursuit2d + 0x391, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlEnemyPursuit2d + 0x399, (size_t)newHalfHeightPtrABC); // 240
    setVariable(addrCtrlEnemyPursuit2d + 0x520, (size_t)newHalfWidthPtrABC);  // 320
    setVariable(addrCtrlEnemyPursuit2d + 0x528, (size_t)newHalfHeightPtrABC); // 240

    setVariable(addrCtrlPlayerRivalFont + 0x695, *(uint32_t *)&newWidthRangeABC);  // 730
    setVariable(addrCtrlPlayerRivalFont + 0x8b6, *(uint32_t *)&newHeightRangeABC); // 570
    setVariable(addrCtrlEnemyPursuit2d + 0x4ea, *(uint32_t *)&newWidthRangeABC);   // 730
    setVariable(addrCtrlEnemyPursuit2d + 0x77b, *(uint32_t *)&newHeightRangeABC);  // 570

    new410PtrABC = (uint32_t *)&new410ABC;
    new330PtrABC = (uint32_t *)&new330ABC;
    new280PtrABC = (uint32_t *)&new280ABC;
    new200PtrABC = (uint32_t *)&new200ABC;
    setVariable(addrCtrlPlayerRivalFont + 0x40e, (size_t)new410PtrABC); // 410
    setVariable(addrCtrlPlayerRivalFont + 0x44a, (size_t)new330PtrABC); // 330
    setVariable(addrCtrlPlayerRivalFont + 0x5de, (size_t)new280PtrABC); // 280
    setVariable(addrCtrlPlayerRivalFont + 0x61a, (size_t)new200PtrABC); // 200
    setVariable(addrCtrlEnemyPursuit2d + 0x2b1, (size_t)new410PtrABC);  // 410
    setVariable(addrCtrlEnemyPursuit2d + 0x2e6, (size_t)new330PtrABC);  // 330
    setVariable(addrCtrlEnemyPursuit2d + 0x442, (size_t)new280PtrABC);  // 280
    setVariable(addrCtrlEnemyPursuit2d + 0x477, (size_t)new200PtrABC);  // 200
}

void hookABCGLFunctions(uint32_t glBindTextureAddr, uint32_t glVertex3fAddr1, uint32_t glVertex3fAddr2)
{
    replaceCallAtAddress(glBindTextureAddr, glBindTextureABC);

    replaceCallAtAddress(glVertex3fAddr1, glTexCoord2fvABC);
    replaceCallAtAddress(glVertex3fAddr1 + 0x1f, glVertex3fABC1);
    replaceCallAtAddress(glVertex3fAddr1 + 0x27, glTexCoord2fvABC);
    replaceCallAtAddress(glVertex3fAddr1 + 0x46, glVertex3fABC1);
    replaceCallAtAddress(glVertex3fAddr1 + 0x4e, glTexCoord2fvABC);
    replaceCallAtAddress(glVertex3fAddr1 + 0x67, glVertex3fABC1);
    replaceCallAtAddress(glVertex3fAddr1 + 0x6f, glTexCoord2fvABC);
    replaceCallAtAddress(glVertex3fAddr1 + 0x88, glVertex3fABC1);

    replaceCallAtAddress(glVertex3fAddr2, glVertex3fABC2);
    replaceCallAtAddress(glVertex3fAddr2 + 0x29, glVertex3fABC2);
    replaceCallAtAddress(glVertex3fAddr2 + 0x52, glVertex3fABC2);
    replaceCallAtAddress(glVertex3fAddr2 + 0x7b, glVertex3fABC2);
}

int initResolutionPatches()
{
    switch (getConfig()->crc32)
    {
    case R_TUNED:
    {
        patchMemory(0x08051c2b, "01");         // Enable Anti Alias
        patchMemory(0x08052d58, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x083c7db8, getConfig()->width);
        setVariable(0x083c7dbc, getConfig()->height);
        patchMemory(0x083c7dc0, "00");
    }
    break;
    case AFTER_BURNER_CLIMAX:
    {
        setVariable(0x082c0308, getConfig()->width);
        setVariable(0x082c030c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806cd72, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076a28 + 6;
        detourFunction(0x08076a28, abcDrawSprite);

        patchABCMarkers(0x080b066a, 0x0809d3dc, 0x080b1668);
        hookABCGLFunctions(0x08076db8, 0x08076dd2, 0x08076b78);
    }
    break;
    case AFTER_BURNER_CLIMAX_REVA:
    {
        setVariable(0x082c0a68, getConfig()->width);
        setVariable(0x082c0a6c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806cd6a, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076a20 + 6;
        detourFunction(0x08076a20, abcDrawSprite);

        patchABCMarkers(0x080b06be, 0x0809d430, 0x080b16bc);
        hookABCGLFunctions(0x08076db0, 0x08076dca, 0x08076b70);
    }
    break;
    case AFTER_BURNER_CLIMAX_REVB:
    {
        setVariable(0x082C0AE8, getConfig()->width);
        setVariable(0x082C0AEC, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806cd72, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076a28 + 6;
        detourFunction(0x08076a28, abcDrawSprite);

        patchABCMarkers(0x080b06c6, 0x0809d438, 0x080b16c4);
        hookABCGLFunctions(0x08076db8, 0x08076dd2, 0x08076b78);
    }
    break;
    case AFTER_BURNER_CLIMAX_SDX:
    {
        setVariable(0x082c2228, getConfig()->width);
        setVariable(0x082c222c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806ce2e, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076ae4 + 6;
        detourFunction(0x08076ae4, abcDrawSprite);

        patchABCMarkers(0x080b0a22, 0x0809d6cc, 0x080b1a20);
        hookABCGLFunctions(0x08076e74, 0x08076e8e, 0x08076c34);
    }
    break;
    case AFTER_BURNER_CLIMAX_SDX_REVA:
    {
        setVariable(0x082c2228, getConfig()->width);
        setVariable(0x082c222c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806ce36, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076aec + 6;
        detourFunction(0x08076aec, abcDrawSprite);

        patchABCMarkers(0x080b0a2a, 0x0809d6d4, 0x080b1a28);
        hookABCGLFunctions(0x08076e7c, 0x08076e96, 0x08076c3c);
    }
    break;
    case AFTER_BURNER_CLIMAX_SE:
    {
        setVariable(0x082c0b28, getConfig()->width);
        setVariable(0x082c0b2c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806cd6a, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076a20 + 6;
        detourFunction(0x08076a20, abcDrawSprite);

        patchABCMarkers(0x080b06be, 0x0809d430, 0x080b16bc);
        hookABCGLFunctions(0x08076db0, 0x08076dca, 0x08076b70);
    }
    break;
    case AFTER_BURNER_CLIMAX_SE_REVA:
    {
        setVariable(0x082c0b28, getConfig()->width);
        setVariable(0x082c0b2c, getConfig()->height);
        float newFontScale = getConfig()->height / 480.0;
        unsigned int *newFontScalePtr = (unsigned int *)&newFontScale;
        setVariable(0x0806cd72, *newFontScalePtr);
        abcDrawSpriteCAVEAddress = (void *)0x08076a28 + 6;
        detourFunction(0x08076a28, abcDrawSprite);

        patchABCMarkers(0x080b06c6, 0x0809d438, 0x080b16c4);
        hookABCGLFunctions(0x08076db8, 0x08076dd2, 0x08076b78);
    }
    break;
    case SEGA_RACE_TV:
    {
        if (getConfig()->keepAspectRatio)
        {
            srtvX = (getConfig()->width - (getConfig()->height / 3) * 4) / 2;
            srtvW = (getConfig()->height / 3) * 4;
        }
        else
        {
            srtvW = getConfig()->width;
        }
        srtvH = getConfig()->height;
    }
    break;
    case OUTRUN_2_SP_SDX:
    {
        // If resolution is not the native of the game this patch kind of fix the Sun when the LensGlare effect is
        // shown.
        if ((getConfig()->width > 800) && (getConfig()->height > 480))
        {
            patchMemory(0x080e8e72, "9090909090"); // removes a call to a light function
            patchMemory(0x080e8e83, "9090909090"); // removes a call to a light function
            if (!getConfig()->outrunLensGlareEnabled)
            {
                detourFunction(0x080e8b34, stubReturn); // Completely disables the lens glare effect
            }
        }
        setVariable(0x080b913a, 0x44200000);
        setVariable(0x081dae28, 0x44200000);
        setVariable(0x081dae30, 0x44200000);
    }
    break;
    case OUTRUN_2_SP_SDX_TEST:
    {
        setVariable(0x0804a490, getConfig()->width);
        setVariable(0x0804a4ad, getConfig()->height);
    }
    break;
    case OUTRUN_2_SP_SDX_REVA:
    {
        // If resolution is not the native of the game this patch kind of fix the Sun when the LensGlare effect is
        // shown.
        if ((getConfig()->width > 800) && (getConfig()->height > 480))
        {
            patchMemory(0x080e8e06, "9090909090"); // removes a call to a light function
            patchMemory(0x080e8e17, "9090909090"); // removes a call to a light function
            if (!getConfig()->outrunLensGlareEnabled)
            {
                detourFunction(0x080e8ac8, stubReturn); // Completely disables the lens glare effect
            }
        }
        setVariable(0x080b913a, 0x44200000);
        setVariable(0x081dada8, 0x44200000);
        setVariable(0x081dadb0, 0x44200000);

        // Cleans patched elf floating around
        patchMemory(0x080b912e, "f043");
        patchMemory(0x080f39de, "c0ef43");
        patchMemory(0x080f3aa6, "2048");
        patchMemory(0x080f3c94, "04b91c");
        patchMemory(0x080f3cba, "20561b");
        patchMemory(0x080f40cf, "c047");
        patchMemory(0x080f40f8, "80ef43");
        patchMemory(0x080f4d80, "04b91c");
        patchMemory(0x080f4e52, "20561b");
        patchMemory(0x080fed40, "80ef43");
        patchMemory(0x080fed52, "c047");
        setVariable(0x081dadb4, 0x43f00000);
        setVariable(0x081dadbc, 0x43f00000);
        patchMemory(0x081e7b3e, "7e");
        patchMemory(0x081e7b46, "89");
    }
    break;
    case OUTRUN_2_SP_SDX_REVA_TEST:
    case OUTRUN_2_SP_SDX_REVA_TEST2:
    {
        setVariable(0x0804a490, getConfig()->width);
        setVariable(0x0804a4ad, getConfig()->height);
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_REVA:
    {
        if (getConfig()->width == 1280 && getConfig()->height == 768)
        {
            setVariable(0x084c9dbc, 1280);
            return 0;
        }
        patchMemory(0x0804d142, "9090909090"); // setresolutiontype
        // patchMemory(0x084c9dbc, "0005");
        // patchMemory(0x08448458, "0005");
        // patchMemory(0x0844845c, "0003");
        setVariable(0x084c9dbc, getConfig()->width);
        setVariable(0x08448458, getConfig()->width);
        setVariable(0x0844845c, getConfig()->height);
        patchMemory(0x08448460, "8002");
        patchMemory(0x08448464, "E001");
        patchMemory(0x0817ff6d, "e80ed5ecff"); // call crtgetresolution
        patchMemory(0x08448468, "8002");       // set crtgetresolution to 640x480
        patchMemory(0x0844846c, "E001");
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_REVB:
    {
        if (getConfig()->width == 1280 && getConfig()->height == 768)
        {
            setVariable(0x084c3a9c, 1280);
            return 0;
        }
        patchMemory(0x0804d174, "9090909090"); // setresolutiontype
        setVariable(0x084c3a9c, getConfig()->width);
        setVariable(0x08443118, getConfig()->width);
        setVariable(0x0844311c, getConfig()->height);
        patchMemory(0x08443120, "8002");
        patchMemory(0x08443124, "E001");
        patchMemory(0x0818080d, "e89eccecff"); // call crtgetresolution
        patchMemory(0x08443128, "8002");       // set crtgetresolution to 640x480
        patchMemory(0x0844312c, "E001");
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_REVC:
    {
        if (getConfig()->width == 1280 && getConfig()->height == 768)
        {
            setVariable(0x084c3a9c, 1280);
            return 0;
        }
        patchMemory(0x0804d174, "9090909090"); // setresolutiontype
        setVariable(0x084c3a9c, getConfig()->width);
        setVariable(0x08443118, getConfig()->width);
        setVariable(0x0844311c, getConfig()->height);
        patchMemory(0x08443120, "8002"); // gallwinres (2D)
        patchMemory(0x08443124, "E001");
        patchMemory(0x0818080d, "e89eccecff"); // sideselect fix
        patchMemory(0x08443128, "8002");
        patchMemory(0x0844312c, "E001");
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL:
    {
        patchMemory(0x0804d2f4, "9090909090"); // setresolutiontype
        setVariable(0x084563c4, getConfig()->width);
        setVariable(0x08424448, getConfig()->width);
        setVariable(0x0842444c, getConfig()->height);
        patchMemory(0x0815bfa9, "e8de7f0100"); // sideselect fix
        patchMemory(0x08424450, "0004");
        patchMemory(0x08424454, "0003");
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_4_SPECIAL_REVB:
    {
        patchMemory(0x0804d7e2, "9090909090"); // setresolutiontype
        setVariable(0x084c35c4, getConfig()->width);
        setVariable(0x08491648, getConfig()->width);
        setVariable(0x0849164c, getConfig()->height);
        patchMemory(0x081b268f, "e8264d0200"); // sideselect fix
        patchMemory(0x08491650, "0004");
        patchMemory(0x08491654, "0003");
    }
    break;
    case THE_HOUSE_OF_THE_DEAD_EX:
    {
        patchMemory(0x087aa080, "5005"); // Boost Resolution
    }
    break;
    case VIRTUA_FIGHTER_5:
    {
        patchMemory(0x08054057, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x0847cf58, getConfig()->width);
        setVariable(0x0847cf5c, getConfig()->height);
        replaceCallAtAddress(0x080d40c4, vf5WidthFix);
    }
    break;
    case VIRTUA_FIGHTER_5_REVA:
    {
        patchMemory(0x8053167, "01");          // Enable Anti Alias
        patchMemory(0x080541af, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x08487df8, getConfig()->width);
        setVariable(0x08487dfc, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_REVB:
    {
        patchMemory(0x08053673, "01");         // Enable Anti Alias
        patchMemory(0x080546cf, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x08536bb8, getConfig()->width);
        setVariable(0x08536bbc, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_REVE: // Also the public REV C version
    {
        patchMemory(0x080546c7, "01");         // Enable Anti Alias
        patchMemory(0x080557a3, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x085efb18, getConfig()->width);
        setVariable(0x085efb1c, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_EXPORT:
    {
        patchMemory(0x08052b97, "01");         // Enable Anti Alias
        patchMemory(0x08053c67, "b80a000000"); // Skips resolution set by the Dip Switches.
        setVariable(0x085259f8, getConfig()->width);
        setVariable(0x085259fc, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVA:
    {
        patchMemory(0x08054b5e, "01");         // Enable Anti Alias
        patchMemory(0x08055980, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x088b2bd8, vf5FSwidth);
        setVariable(0x088b2bdc, getConfig()->height);
        setVariable(0x088b2bec, getConfig()->width);
        setVariable(0x088b2bf0, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVB:
    {
        patchMemory(0x080548c4, "01");         // Enable Anti Alias
        patchMemory(0x080556e6, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x088d2b78, vf5FSwidth);
        setVariable(0x088d2b7c, getConfig()->height);
        setVariable(0x088d2b8c, getConfig()->width);
        setVariable(0x088d2b90, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_FINAL_SHOWDOWN_REVB_6000:
    {
        patchMemory(0x08054bfe, "01");         // Enable Anti Alias
        patchMemory(0x08055a20, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x088d53d8, vf5FSwidth);
        setVariable(0x088d53dc, getConfig()->height);
        setVariable(0x088d53ec, getConfig()->width);
        setVariable(0x088d53f0, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_R:
    {
        patchMemory(0x0805421a, "01");         // Enable Anti Alias
        patchMemory(0x080554b0, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x08767d58, vf5FSwidth);
        setVariable(0x08767d5c, getConfig()->height);
        setVariable(0x08767d6c, getConfig()->width);
        setVariable(0x08767d70, getConfig()->height);
    }
    break;
    case VIRTUA_FIGHTER_5_R_REVD:
    {
        patchMemory(0x080543aa, "01");         // Enable Anti Alias
        patchMemory(0x080555f6, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x08822b18, vf5FSwidth);
        setVariable(0x08822b1c, getConfig()->height);
        setVariable(0x08822b2c, getConfig()->width);
        setVariable(0x08822b30, getConfig()->height);
    }
    break;
	case VIRTUA_FIGHTER_5_R_REVG:
    {
        patchMemory(0x0805436a, "01");         // Enable Anti Alias
        patchMemory(0x0805577c, "b80a000000"); // Skips resolution set by the Dip Switches.
        vf5FSwidth = (getConfig()->height * 5) / 3;
        setVariable(0x0887d4d8, vf5FSwidth);
        setVariable(0x0887d4dc, getConfig()->height);
        setVariable(0x0887d4ec, getConfig()->width);
        setVariable(0x0887d4f0, getConfig()->height);
    }
    break;	
    case LETS_GO_JUNGLE_REVA:
    {
        setVariable(0x082e1323, getConfig()->width);  // Set ResX
        setVariable(0x082e1330, getConfig()->height); // Set ResY
        // FSAA
        patchMemory(0x082e12ff, "01");
    }
    break;
    case LETS_GO_JUNGLE:
    {
        setVariable(0x082E006b, getConfig()->width);  // Set ResX
        setVariable(0x082E0078, getConfig()->height); // Set ResY
        // FSAA
        patchMemory(0x082e0047, "01");
    }
    break;
    case LETS_GO_JUNGLE_SPECIAL:
    {
        // setVariable(0x08303C4B, 0x00000780); // Set ResX
        // setVariable(0x08303C58, 0x00000438); // Set ResY
    }
    break;
    case INITIALD_4_REVA:
    {
        setVariable(0x0835640d, 0x0000f0e9);          // Force set resolution
        setVariable(0x08356503, getConfig()->width);  // Set ResX
        setVariable(0x08356508, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08247e77, getConfig()->width);  // Set ResX
        setVariable(0x08247e6f, getConfig()->height); // Set ResY
        setVariable(0x08247f37, getConfig()->width);  // Set ResX
        setVariable(0x08247f2f, getConfig()->height); // Set ResY
        setVariable(0x08247ff7, getConfig()->width);  // Set ResX
        setVariable(0x08247fef, getConfig()->height); // Set ResY
        setVariable(0x08248056, getConfig()->width);  // Set ResX
        setVariable(0x0824804e, getConfig()->height); // Set ResY
        setVariable(0x082487e7, getConfig()->width);  // Set ResX
        setVariable(0x082487df, getConfig()->height); // Set ResY
        setVariable(0x08248872, getConfig()->width);  // Set ResX
        setVariable(0x0824886a, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x083188a0 + 5;
        detourFunction(0x083188a0, idDisplayTexture);
        // FSAA
        patchMemory(0x08538c72, "9090");
        patchMemory(0x085582c9, "01"); // FSAA Enabled
        setVariable(0x089c7af0, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081dacde + 6;
        id4DrawTextAddress = (void *)0x081dbe42;
        detourFunction(0x081dacde, idDrawBallon);
        replaceCallAtAddress(0x081dc23a, id4DrawText);
    }
    break;
    case INITIALD_4_REVB:
    {
        setVariable(0x0835664d, 0x0000f0e9);          // Force set resolution
        setVariable(0x08356743, getConfig()->width);  // Set ResX
        setVariable(0x08356748, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08248037, getConfig()->width);  // Set ResX
        setVariable(0x0824802f, getConfig()->height); // Set ResY
        setVariable(0x082480f7, getConfig()->width);  // Set ResX
        setVariable(0x082480ef, getConfig()->height); // Set ResY
        setVariable(0x082481b7, getConfig()->width);  // Set ResX
        setVariable(0x082481af, getConfig()->height); // Set ResY
        setVariable(0x08248216, getConfig()->width);  // Set ResX
        setVariable(0x0824820e, getConfig()->height); // Set ResY
        setVariable(0x082489a7, getConfig()->width);  // Set ResX
        setVariable(0x0824899f, getConfig()->height); // Set ResY
        setVariable(0x08248a32, getConfig()->width);  // Set ResX
        setVariable(0x08248a2a, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x08318ab0 + 5;
        detourFunction(0x08318ab0, idDisplayTexture);
        // FSAA
        patchMemory(0x08538eb2, "9090");
        patchMemory(0x08558509, "01"); // FSAA Enabled
        setVariable(0x089c7dd0, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081dadde + 6;
        id4DrawTextAddress = (void *)0x081dbf42;
        detourFunction(0x081dadde, idDrawBallon);
        replaceCallAtAddress(0x081dc33a, id4DrawText);
    }
    break;
    case INITIALD_4_REVC:
    {
        setVariable(0x0835eebd, 0x0000f0e9);          // Force set resolution
        setVariable(0x0835efb3, getConfig()->width);  // Set ResX
        setVariable(0x0835efb8, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08248ad7, getConfig()->width);  // Set ResX
        setVariable(0x08248acf, getConfig()->height); // Set ResY
        setVariable(0x08248b97, getConfig()->width);  // Set ResX
        setVariable(0x08248b8f, getConfig()->height); // Set ResY
        setVariable(0x08248c57, getConfig()->width);  // Set ResX
        setVariable(0x08248c4f, getConfig()->height); // Set ResY
        setVariable(0x08248cb6, getConfig()->width);  // Set ResX
        setVariable(0x08248cae, getConfig()->height); // Set ResY
        setVariable(0x08249447, getConfig()->width);  // Set ResX
        setVariable(0x0824943f, getConfig()->height); // Set ResY
        setVariable(0x082494d2, getConfig()->width);  // Set ResX
        setVariable(0x082494ca, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x0831ea60 + 5;
        detourFunction(0x0831ea60, idDisplayTexture);
        // FSAA
        patchMemory(0x08541722, "9090");
        patchMemory(0x08560d79, "01"); // FSAA Enabled
        setVariable(0x089d2f70, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081db63e + 6;
        id4DrawTextAddress = (void *)0x081dc7a4;
        detourFunction(0x081db63e, idDrawBallon);
        replaceCallAtAddress(0x081dcb9a, id4DrawText);
    }
    break;
    case INITIALD_4_REVD:
    {
        setVariable(0x0835c55d, 0x0000f0e9);          // Force set resolution
        setVariable(0x0835c653, getConfig()->width);  // Set ResX
        setVariable(0x0835c658, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08248f47, getConfig()->width);  // Set ResX
        setVariable(0x08248f3f, getConfig()->height); // Set ResY
        setVariable(0x08249007, getConfig()->width);  // Set ResX
        setVariable(0x08248fff, getConfig()->height); // Set ResY
        setVariable(0x082490c7, getConfig()->width);  // Set ResX
        setVariable(0x082490bf, getConfig()->height); // Set ResY
        setVariable(0x08249126, getConfig()->width);  // Set ResX
        setVariable(0x0824911e, getConfig()->height); // Set ResY
        setVariable(0x082498b7, getConfig()->width);  // Set ResX
        setVariable(0x082498af, getConfig()->height); // Set ResY
        setVariable(0x08249942, getConfig()->width);  // Set ResX
        setVariable(0x0824993a, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x0831bf90 + 5;
        detourFunction(0x0831bf90, idDisplayTexture);
        // FSAA
        patchMemory(0x0853edc2, "9090");
        patchMemory(0x0855e419, "01"); // FSAA Enabled
        setVariable(0x089cff30, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081dbaae + 6;
        id4DrawTextAddress = (void *)0x081dcc14;
        detourFunction(0x081dbaae, idDrawBallon);
        replaceCallAtAddress(0x081dd00a, id4DrawText);
    }
    break;
    case INITIALD_4_REVG:
    {
        setVariable(0x08393fbd, 0x0000f0e9);          // Force set resolution
        setVariable(0x083940b3, getConfig()->width);  // Set ResX
        setVariable(0x083940b8, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08262eda, getConfig()->width);  // Set ResX
        setVariable(0x08262ed2, getConfig()->height); // Set ResY
        setVariable(0x08262f9a, getConfig()->width);  // Set ResX
        setVariable(0x08262f92, getConfig()->height); // Set ResY
        setVariable(0x0826305a, getConfig()->width);  // Set ResX
        setVariable(0x08263052, getConfig()->height); // Set ResY
        setVariable(0x082630b9, getConfig()->width);  // Set ResX
        setVariable(0x082630b1, getConfig()->height); // Set ResY
        setVariable(0x0826395f, getConfig()->width);  // Set ResX
        setVariable(0x08263957, getConfig()->height); // Set ResY
        setVariable(0x082639ea, getConfig()->width);  // Set ResX
        setVariable(0x082639e2, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x083502b0 + 5;
        detourFunction(0x083502b0, idDisplayTexture);
        // FSAA
        patchMemory(0x08578e92, "9090");
        patchMemory(0x08598719, "01"); // FSAA Enabled
        setVariable(0x08a34670, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081f2b7e + 6;
        id4DrawTextAddress = (void *)0x081f3ce4;
        detourFunction(0x081f2b7e, idDrawBallon);
        replaceCallAtAddress(0x081f40da, id4DrawText);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x089e15d8, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x085945c2;
            replaceCallAtAddress(0x083748ce, idTestText);
        }
    }
    break;
    case INITIALD_4_EXP_REVB:
    {
        setVariable(0x0837979d, 0x0000f0e9);          // Force set resolution
        setVariable(0x08379893, getConfig()->width);  // Set ResX
        setVariable(0x08379898, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x082560a2, getConfig()->width);  // Set ResX
        setVariable(0x0825609a, getConfig()->height); // Set ResY
        setVariable(0x08256162, getConfig()->width);  // Set ResX
        setVariable(0x0825615a, getConfig()->height); // Set ResY
        setVariable(0x08256222, getConfig()->width);  // Set ResX
        setVariable(0x0825621a, getConfig()->height); // Set ResY
        setVariable(0x08256281, getConfig()->width);  // Set ResX
        setVariable(0x08256279, getConfig()->height); // Set ResY
        setVariable(0x08256b27, getConfig()->width);  // Set ResX
        setVariable(0x08256b1f, getConfig()->height); // Set ResY
        setVariable(0x08256bb2, getConfig()->width);  // Set ResX
        setVariable(0x08256baa, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x08337360 + 5;
        detourFunction(0x08337360, idDisplayTexture);
        // FSAA
        patchMemory(0x0855e762, "9090");
        patchMemory(0x0857dfe9, "01"); // FSAA Enabled
        setVariable(0x089ea1d0, 1);    // FSAA Quality
        // Fix Subtitles Position
        id4NewCaptionY = ((getConfig()->height - 768.0) / 2) + 64.0;
        detourFunction(0x081f0f91, yTxtPatch);
        id4NewCaptionYCAVEAddress = (void *)(0x081f0f91 + 8);
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081ef2ee + 6;
        id4DrawTextAddress = (void *)0x081f0454;
        detourFunction(0x081ef2ee, idDrawBallon);
        replaceCallAtAddress(0x081f084a, id4DrawText);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x0899bb78, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x08579e92;
            replaceCallAtAddress(0x0835a1fe, idTestText);
        }
    }
    break;
    case INITIALD_4_EXP_REVC:
    {
        setVariable(0x0837961d, 0x0000f0e9);          // Force set resolution
        setVariable(0x08379713, getConfig()->width);  // Set ResX
        setVariable(0x08379718, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x08256192, getConfig()->width);  // Set ResX
        setVariable(0x0825618a, getConfig()->height); // Set ResY
        setVariable(0x08256252, getConfig()->width);  // Set ResX
        setVariable(0x0825624a, getConfig()->height); // Set ResY
        setVariable(0x08256312, getConfig()->width);  // Set ResX
        setVariable(0x0825630a, getConfig()->height); // Set ResY
        setVariable(0x08256371, getConfig()->width);  // Set ResX
        setVariable(0x08256369, getConfig()->height); // Set ResY
        setVariable(0x08256c17, getConfig()->width);  // Set ResX
        setVariable(0x08256c0f, getConfig()->height); // Set ResY
        setVariable(0x08256ca2, getConfig()->width);  // Set ResX
        setVariable(0x08256c9a, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x08337200 + 5;
        detourFunction(0x08337200, idDisplayTexture);
        // FSAA
        patchMemory(0x0855e4e2, "9090");
        patchMemory(0x0857dd69, "01"); // FSAA Enabled
        setVariable(0x089ea1d0, 1);    // FSAA Quality
        // Fix Subtitles
        float newCaptionY = ((getConfig()->height - 768.0) / 2) + 64.0;
        setVariable(0x089a7d58, *(unsigned int *)&newCaptionY);
        // Fix Subtitles Position
        id4NewCaptionY = ((getConfig()->height - 768.0) / 2) + 64.0;
        detourFunction(0x081f1051, yTxtPatch);
        id4NewCaptionYCAVEAddress = (void *)(0x081f1051 + 8);
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081ef39e + 6;
        id4DrawTextAddress = (void *)0x081f0504;
        detourFunction(0x081ef39e, idDrawBallon);
        replaceCallAtAddress(0x081f08fc, id4DrawText);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x0899bb78, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x08579c12;
            replaceCallAtAddress(0x0835a09e, idTestText);
        }
    }
    break;
    case INITIALD_4_EXP_REVD:
    {
        setVariable(0x0837b12d, 0x0000f0e9);          // Force set resolution
        setVariable(0x0837b223, getConfig()->width);  // Set ResX
        setVariable(0x0837b228, getConfig()->height); // Set ResY
        // Renderbuffer Resolution
        setVariable(0x0825728a, getConfig()->width);  // Set ResX
        setVariable(0x08257282, getConfig()->height); // Set ResY
        setVariable(0x0825734a, getConfig()->width);  // Set ResX
        setVariable(0x08257342, getConfig()->height); // Set ResY
        setVariable(0x0825740a, getConfig()->width);  // Set ResX
        setVariable(0x08257402, getConfig()->height); // Set ResY
        setVariable(0x08257469, getConfig()->width);  // Set ResX
        setVariable(0x08257461, getConfig()->height); // Set ResY
        setVariable(0x08257d0f, getConfig()->width);  // Set ResX
        setVariable(0x08257d07, getConfig()->height); // Set ResY
        setVariable(0x08257d9a, getConfig()->width);  // Set ResX
        setVariable(0x08257d92, getConfig()->height); // Set ResY

        idDisplayTextureCAVEAddress = (void *)0x08338d50 + 5;
        detourFunction(0x08338d50, idDisplayTexture);
        // FSAA
        patchMemory(0x0855fff2, "9090");
        patchMemory(0x0857f879, "01"); // FSAA Enabled
        setVariable(0x089ed930, 1);    // FSAA Quality
        // Fix Subtitles Position
        id4NewCaptionY = ((getConfig()->height - 768.0) / 2) + 64.0;
        detourFunction(0x081f2061, yTxtPatch);
        id4NewCaptionYCAVEAddress = (void *)(0x081f2061 + 8);
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x081f03be + 6;
        id4DrawTextAddress = (void *)0x081f1524;
        detourFunction(0x081f03be, idDrawBallon);
        replaceCallAtAddress(0x081f191a, id4DrawText);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x0899f2d8, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x0857b722;
            replaceCallAtAddress(0x0835bbee, idTestText);
        }
    }
    break;
    case INITIALD_5_EXP_30:
    {
        patchMemory(0x0855a48d, "E9f000");            // Accept different Resolutions
        setVariable(0x0855a583, getConfig()->width);  // Set ResX
        setVariable(0x0855a588, getConfig()->height); // Set ResY

        patchMemory(0x08363c24, "bb01000000eb6b");        // Prevents renrer.ini from loading
        setVariable(0x08363b00, getConfig()->width);      // Framebuffer Main Width
        setVariable(0x08363b07, getConfig()->height);     // Framebuffer Main Height
        setVariable(0x08363ba1, 256);                     // Framebuffer Road Specular width
        setVariable(0x08363ba8, 256);                     // Framebuffer Road Specular height
        setVariable(0x08363bc7, getConfig()->width);      // Framebuffer Glare Width
        setVariable(0x08363bce, getConfig()->height);     // Framebuffer Glare Height
        setVariable(0x08363bf4, getConfig()->width >> 2); // Framebuffer Reduced width
        patchMemory(0x08363c19, "00000001");              // Enable Cube Secular
        // Fix Press start and Insert coins text
        idDisplayTextureCAVEAddress = (void *)0x084fdfa0 + 5;
        detourFunction(0x084fdfa0, idDisplayTexture);
        // setViewport for track selection screen
        setVariable(0x0821e75e, (int)(getConfig()->height * (112.0 / 768)));
        setVariable(0x0821e766, (int)(getConfig()->width * (724.0 / 1315)));
        setVariable(0x0821e76e, (int)(getConfig()->height * (592.0 / 768)));
        setVariable(0x0821e776, (int)(getConfig()->width * (962.0 / 1315)));
        setVariable(0x0821e77e, getConfig()->height);
        // FSAA
        patchMemory(0x087775f6, "9090");
        patchMemory(0x087a5499, "01"); // FSAA Enabled
        setVariable(0x08cf9ce8, 1);    // FSAA Quality
        // Balloon fix
        idDrawBallonCAVEAddress = (void *)0x0825c046 + 6;
        iddrawBallonPutAddress = (void *)0x086ef13c;
        detourFunction(0x0825c046, idDrawBallon);
        replaceCallAtAddress(0x0825d4e8, idBalloonPut);
        replaceCallAtAddress(0x0825d5a9, idBalloonPut);
        // START and VIEW CHANGE Text fix
        float explanationScaleX = getConfig()->width - (1360.0 - 815.0);
        float explanationScaleY = (getConfig()->height / 768.0) * 680.0;
        setVariable(0x082737b5, *(unsigned int *)&explanationScaleY);
        setVariable(0x0827382d, *(unsigned int *)&explanationScaleX);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x08ca3d00, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x0879276c;
            replaceCallAtAddress(0x0852528a, idTestText);
        }
    }
    break;
    case INITIALD_5_EXP_40:
    {
        patchMemory(0x0855a6dd, "E9f000");            // Accept different Resolutions
        setVariable(0x0855a7d3, getConfig()->width);  // Set ResX
        setVariable(0x0855a7d8, getConfig()->height); // Set ResY

        patchMemory(0x08363d94, "bb01000000eb6b");        // Prevents renrer.ini from loading
        setVariable(0x08363c70, getConfig()->width);      // Framebuffer Main Width
        setVariable(0x08363c77, getConfig()->height);     // Framebuffer Main Height
        setVariable(0x08363d11, 256);                     // Framebuffer Road Specular width
        setVariable(0x08363d18, 256);                     // Framebuffer Road Specular height
        setVariable(0x08363d37, getConfig()->width);      // Framebuffer Glare Width
        setVariable(0x08363d3e, getConfig()->height);     // Framebuffer Glare Height
        setVariable(0x08363d64, getConfig()->width >> 2); // Framebuffer Reduced width
        patchMemory(0x08363d89, "00000001");              // Enable Cube Secular
        // Fix Press start and Insert coins text
        idDisplayTextureCAVEAddress = (void *)0x084fe1d0 + 5;
        detourFunction(0x084fe1d0, idDisplayTexture);
        // setViewport for track selection screen
        setVariable(0x0821e8ee, (int)(getConfig()->height * (112.0 / 768)));
        setVariable(0x0821e8f6, (int)(getConfig()->width * (724.0 / 1315)));
        setVariable(0x0821e8fe, (int)(getConfig()->height * (592.0 / 768)));
        setVariable(0x0821e906, (int)(getConfig()->width * (962.0 / 1315)));
        setVariable(0x0821e90e, getConfig()->height);
        // FSAA
        patchMemory(0x08777846, "9090");
        patchMemory(0x087a56e9, "01"); // FSAA Enabled
        setVariable(0x08cf9ce8, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x0825c1b6 + 6;
        iddrawBallonPutAddress = (void *)0x086ef38c;
        detourFunction(0x0825c1b6, idDrawBallon);
        replaceCallAtAddress(0x0825d656, idBalloonPut);
        replaceCallAtAddress(0x0825d717, idBalloonPut);
        // START and VIEW CHANGE Text fix
        float explanationScaleX = getConfig()->width - (1360.0 - 815.0);
        float explanationScaleY = (getConfig()->height / 768.0) * 680.0;
        setVariable(0x08273915, *(unsigned int *)&explanationScaleY);
        setVariable(0x0827398d, *(unsigned int *)&explanationScaleX);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x08ca3d00, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x087929bc;
            replaceCallAtAddress(0x085254ea, idTestText);
        }
    }
    break;
    case INITIALD_5_JAP_REVA: // ID5 - DVP-0070A
    {
        patchMemory(0x0853ccdd, "E9f000");            // Accept different Resolutions
        setVariable(0x0853cdd3, getConfig()->width);  // Set ResX
        setVariable(0x0853cdd8, getConfig()->height); // Set ResY

        patchMemory(0x083557f4, "bb01000000eb6b");        // Prevents renrer.ini from loading
        setVariable(0x083556d0, getConfig()->width);      // Framebuffer Main Width
        setVariable(0x083556d7, getConfig()->height);     // Framebuffer Main Height
        setVariable(0x08355771, 256);                     // Framebuffer Road Specular width
        setVariable(0x08355778, 256);                     // Framebuffer Road Specular height
        setVariable(0x08355797, getConfig()->width);      // Framebuffer Glare Width
        setVariable(0x0835579e, getConfig()->height);     // Framebuffer Glare Height
        setVariable(0x083557c4, getConfig()->width >> 2); // Framebuffer Reduced width
        patchMemory(0x083557e9, "00000001");              // Enable Cube Secular
        // Fix Press start and Insert coins text
        idDisplayTextureCAVEAddress = (void *)0x084e1770 + 5;
        detourFunction(0x084e1770, idDisplayTexture);
        // setViewport for track selection screen
        setVariable(0x08214b9e, (int)(getConfig()->height * (112.0 / 768)));
        setVariable(0x08214ba6, (int)(getConfig()->width * (724.0 / 1315)));
        setVariable(0x08214bae, (int)(getConfig()->height * (592.0 / 768)));
        setVariable(0x08214bb6, (int)(getConfig()->width * (962.0 / 1315)));
        setVariable(0x08214bbe, getConfig()->height);
        // FSAA
        patchMemory(0x08759eb6, "9090");
        patchMemory(0x08787d59, "01"); // FSAA Enabled
        setVariable(0x08cac028, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x082526a6 + 6;
        iddrawBallonPutAddress = (void *)0x086d198c;
        detourFunction(0x082526a6, idDrawBallon);
        replaceCallAtAddress(0x08253b48, idBalloonPut);
        replaceCallAtAddress(0x08253c09, idBalloonPut);
        // START and VIEW CHANGE Text fix
        float explanationScaleX = getConfig()->width - (1360.0 - 815.0);
        float explanationScaleY = (getConfig()->height / 768.0) * 680.0;
        setVariable(0x08269c25, *(unsigned int *)&explanationScaleY);
        setVariable(0x08269c9d, *(unsigned int *)&explanationScaleX);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x08c55b80, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x0877502c;
            replaceCallAtAddress(0x0850783e, idTestText);
        }
    }
    break;
    case INITIALD_5_JAP_REVF: // ID5 - DVP-0070F
    {
        patchMemory(0x08556bbd, "E9f000");            // Accept different Resolutions
        setVariable(0x08556cb3, getConfig()->width);  // Set ResX
        setVariable(0x08556cb8, getConfig()->height); // Set ResY

        patchMemory(0x08364484, "bb01000000eb6b");        // Prevents renrer.ini from loading
        setVariable(0x08364360, getConfig()->width);      // Framebuffer Main Width
        setVariable(0x08364367, getConfig()->height);     // Framebuffer Main Height
        setVariable(0x08364401, 256);                     // Framebuffer Road Specular width
        setVariable(0x08364408, 256);                     // Framebuffer Road Specular height
        setVariable(0x08364427, getConfig()->width);      // Framebuffer Glare Width
        setVariable(0x0836442e, getConfig()->height);     // Framebuffer Glare Height
        setVariable(0x08364454, getConfig()->width >> 2); // Framebuffer Reduced width
        patchMemory(0x08364479, "00000001");              // Enable Cube Secular
        // Fix Press start and Insert coins text
        idDisplayTextureCAVEAddress = (void *)0x084fa3c0 + 5;
        detourFunction(0x084fa3c0, idDisplayTexture);
        // setViewport for track selection screen
        setVariable(0x0821ec6e, (int)(getConfig()->height * (112.0 / 768)));
        setVariable(0x0821ec76, (int)(getConfig()->width * (724.0 / 1315)));
        setVariable(0x0821ec7e, (int)(getConfig()->height * (592.0 / 768)));
        setVariable(0x0821ec86, (int)(getConfig()->width * (962.0 / 1315)));
        setVariable(0x0821ec8e, getConfig()->height);
        // FSAA
        patchMemory(0x08773d26, "9090");
        patchMemory(0x087a1bc9, "01"); // FSAA Enabled
        setVariable(0x08ce8028, 1);    // FSAA Quality
        // Ballon fix
        idDrawBallonCAVEAddress = (void *)0x0825c556 + 6;
        iddrawBallonPutAddress = (void *)0x086eb86c;
        detourFunction(0x0825c556, idDrawBallon);
        replaceCallAtAddress(0x0825d9f8, idBalloonPut);
        replaceCallAtAddress(0x0825dab9, idBalloonPut);
        // START and VIEW CHANGE Text fix
        float explanationScaleX = getConfig()->width - (1360.0 - 815.0);
        float explanationScaleY = (getConfig()->height / 768.0) * 680.0;
        setVariable(0x08273ce5, *(unsigned int *)&explanationScaleY);
        setVariable(0x08273d5d, *(unsigned int *)&explanationScaleX);
        // Scale Testmode text
        if (isTestMode() && getConfig()->width >= 1920)
        {
            patchMemory(0x08c92040, "02");
            idTextShift = (getConfig()->width == 1920 ? 27 : ((1920 / getConfig()->width) * 27) + 2);
            idTestTextAddress = (void *)0x0878ee9c;
            replaceCallAtAddress(0x08521aaa, idTestText);
        }
    }
    break;
    case SEGABOOT_2_4_SYM:
    {
        patchMemory(0x08050ad3, "9090");
        patchMemory(0x08050ac4, "07");
        patchMemory(0x08050c00, "07");
        patchMemory(0x08050c12, "07");
        patchMemory(0x08050c25, "07");
        patchMemory(0x08050c37, "07");
        setVariable(0x08064611, getConfig()->width);
        setVariable(0x080645c6, getConfig()->height);
    }
    break;
    case VIRTUA_TENNIS_3:
    {
        if (getConfig()->width <= 1360 && getConfig()->height <= 768)
            break;
        patchMemory(0x081759b3, "9090");
        setVariable(0x081759bb, getConfig()->width);
        setVariable(0x081759c5, getConfig()->height);

        // setPerspective
        newVT3 = getConfig()->width / 1360.0;
        intnewVT3PTr = (uint32_t *)&newVT3;

        setVariable(0x082682b6, (size_t)intnewVT3PTr);

        // vecToScr_check
        newVT3HW = (getConfig()->width / 1360.0) * 320.0;
        newVT3HH = (getConfig()->height / 768.0) * 240.0;
        intnewVT3HWPTr = (uint32_t *)&newVT3HW;
        intnewVT3HHPTr = (uint32_t *)&newVT3HH;
        uint32_t addressvt31 = 0x0817a55e;
        setVariable(addressvt31, (size_t)intnewVT3HWPTr);
        setVariable(addressvt31 + 0x18, (size_t)intnewVT3HHPTr);
    }
    break;
    case VIRTUA_TENNIS_3_REVA:
    {
        if (getConfig()->width <= 1360 && getConfig()->height <= 768)
            break;
        patchMemory(0x081759e3, "9090");
        setVariable(0x081759eb, getConfig()->width);
        setVariable(0x081759f5, getConfig()->height);

        // setPerspective
        newVT3 = getConfig()->width / 1360.0;
        intnewVT3PTr = (uint32_t *)&newVT3;

        setVariable(0x082682e6, (size_t)intnewVT3PTr);

        // vecToScr_check
        newVT3HW = (getConfig()->width / 1360.0) * 320.0;
        newVT3HH = (getConfig()->height / 768.0) * 240.0;
        intnewVT3HWPTr = (uint32_t *)&newVT3HW;
        intnewVT3HHPTr = (uint32_t *)&newVT3HH;
        uint32_t addressvt31 = 0x0817a58e;
        setVariable(addressvt31, (size_t)intnewVT3HWPTr);
        setVariable(addressvt31 + 0x18, (size_t)intnewVT3HHPTr);
    }
    break;
    case VIRTUA_TENNIS_3_REVB:
    {
        if (getConfig()->width <= 1360 && getConfig()->height <= 768)
            break;
        patchMemory(0x08175ad7, "9090");
        setVariable(0x08175adf, getConfig()->width);
        setVariable(0x08175ae9, getConfig()->height);

        // setPerspective
        newVT3 = getConfig()->width / 1360.0;
        intnewVT3PTr = (uint32_t *)&newVT3;

        setVariable(0x082684d2, (size_t)intnewVT3PTr);

        // vecToScr_check
        newVT3HW = (getConfig()->width / 1360.0) * 320.0;
        newVT3HH = (getConfig()->height / 768.0) * 240.0;
        intnewVT3HWPTr = (uint32_t *)&newVT3HW;
        intnewVT3HHPTr = (uint32_t *)&newVT3HH;
        uint32_t addressvt31 = 0x0817a77a;
        setVariable(addressvt31, (size_t)intnewVT3HWPTr);
        setVariable(addressvt31 + 0x18, (size_t)intnewVT3HHPTr);
    }
    break;
    case VIRTUA_TENNIS_3_REVC:
    {
        if (getConfig()->width <= 1360 && getConfig()->height <= 768)
            break;
        patchMemory(0x08175b03, "9090");
        setVariable(0x08175b0b, getConfig()->width);
        setVariable(0x08175b15, getConfig()->height);

        // setPerspective
        newVT3 = getConfig()->width / 1360.0;
        intnewVT3PTr = (uint32_t *)&newVT3;

        setVariable(0x082684fe, (size_t)intnewVT3PTr);

        // vecToScr_check
        newVT3HW = (getConfig()->width / 1360.0) * 320.0;
        newVT3HH = (getConfig()->height / 768.0) * 240.0;
        intnewVT3HWPTr = (uint32_t *)&newVT3HW;
        intnewVT3HHPTr = (uint32_t *)&newVT3HH;
        uint32_t addressvt31 = 0x0817a7a6;
        setVariable(addressvt31, (size_t)intnewVT3HWPTr);
        setVariable(addressvt31 + 0x18, (size_t)intnewVT3HHPTr);
    }
    break;
    case VIRTUA_TENNIS_3_REVC_TEST:
    {
        setVariable(0x0806fb3b, getConfig()->width);
        setVariable(0x0806fb04, getConfig()->height);
    }
    break;
    case RAMBO:
    {
        patchMemory(0x080c70d0, "9090909090"); // setresolutiontype
        setVariable(0x08416df0, getConfig()->width); // render res
        setVariable(0x08416d60, getConfig()->width); // main res
        setVariable(0x08416d64, getConfig()->height);
        patchMemory(0x08416d68, "5005"); // Allwinres
        patchMemory(0x08416d6c, "0003");

        patchMemory(0x080ef960, "66ff"); // 2d
        patchMemory(0x0806b510, "b6");   // proper marker fix
        //  patchMemory(0x0806b4f6, "c3");    //get rid of shot marker
    }
    break;
    case TOO_SPICY:
    {
        patchMemory(0x08202b22, "0005"); // render res 1280
    }
    break;
    case PRIMEVAL_HUNT:
    {
        phX = 0;
        phY = 0;
        phW = getConfig()->width;
        phH = getConfig()->height;
        int phX2, phY2, phW2, phH2;
        switch (getConfig()->phMode)
        {
        case 0:
        {
            return 0;
        }
        case 1: // No touch screen
        {
            if (getConfig()->keepAspectRatio)
            {
                phX = (getConfig()->width - (getConfig()->height / 3) * 4) / 2;
                phW = (getConfig()->height / 3) * 4;
            }
            phW2 = 0;
            phH2 = 0;
            phX2 = phW;
            phY2 = 0;
        }
        break;
        case 2: // Side by side
        {
            if (getConfig()->keepAspectRatio)
            {
                phH = ((getConfig()->width / 2) / 4) * 3;
                phY = (getConfig()->height - phH) / 2;
            }
            phX = 0;
            phW = phW / 2;
            phX2 = phW;
            phY2 = phY;
            phW2 = phW;
            phH2 = phH;
        }
        break;
        case 3: // 3ds Mode 1 (Small screen to the right)
        {
            // We always force 4:3 aspect ratio for this mode.
            phX = 0;
            phW = (getConfig()->height / 3) * 4;
            phH2 = ((getConfig()->width - phW) / 4) * 3;

            phX2 = phW;
            phY2 = (phH - phH2) / 2;
            phW2 = getConfig()->width - phW;
        }
        break;
        case 4:
        {
            // We always force 4:3 aspect ratio for this mode.
            phW = (getConfig()->height / 3) * 4;
            phX2 = (getConfig()->width / 2) - ((getConfig()->width - phW) / 2);
            phW2 = getConfig()->width - phW;
            phY2 = 0; // phH - ((phW2 / 4) * 3);
            phH2 = (phW2 / 4) * 3;

            phW = phW - phW2;
            phH = phH - phH2;
            phX = (getConfig()->width - (phH / 3) * 4) / 2;
            phY = phH2;
        }
        break;
        }
        setVariable(0x0805b104, phX); // X 1st screen
        setVariable(0x0805b0fd, phY); // Y 1st screen
        setVariable(0x0805b0f5, phW); // Width 1st screen
        setVariable(0x0805b0ed, phH); // Height 1st screen

        setVariable(0x0805afa2, phX2); // X 2nd screen
        setVariable(0x0805af9b, phY2); // Y 2st screen
        setVariable(0x0805af93, phW2); // Width 2nd screen
        setVariable(0x0805af8b, phH2); // Height 2nd screen
        // patchMemory(0x08052cc4, "0000"); // swap screens
        detourFunction(0x0804c628, glClearColorPH);
        if (getConfig()->phMode == 4) // In mode 4, the screens are inverted.
            phY = 0;
    }
    break;
    case GHOST_SQUAD_EVOLUTION:
    {
        // test
        // patchMemory(0x081c1d71, "38303078363030");
        setVariable(0x082c87f8, 0x00000550);
        setVariable(0x082c87fc, 0x00000300);
    }
    break;
    case HARLEY_DAVIDSON:
    {
        int w = getConfig()->width;
        int h = getConfig()->height;
        setVariable(0x088a57e0, w); // main res
        setVariable(0x088a57e4, h);
        setVariable(0x088a57e8, w);
        setVariable(0x088a57ec, h);
        setVariable(0x088a57f0, w);
        setVariable(0x088a57f4, h);
        setVariable(0x088a55e0, w); // render res
        patchMemory(0x08056c31, "9090909090");
        // setVariable(0x08056c2d, w); // render res
        patchMemory(0x082a8500, "9090909090"); // restype
        patchMemory(0x088a55e4, "14");
        patchMemory(0x082a2412, "00008040"); // Fixes white logo background.
        if (getConfig()->width > 1360)
        {
            unsigned int idx = ((21 * h) / 768) + 1;
            setVariable(0x082a250e, idx);
        }
    }
    break;
    case HUMMER:
    {
        replaceCallAtAddress(0x080d8b58, hummerRespatch);
        replaceCallAtAddress(0x080d8b73, hummerRespatch);
        replaceCallAtAddress(0x080d8b8e, hummerRespatch);
        replaceCallAtAddress(0x080d8ba9, hummerRespatch);
        replaceCallAtAddress(0x080d8bfa, hummerRespatch);
        replaceCallAtAddress(0x080d8c15, hummerRespatch);
    }
    break;
    case HUMMER_SDLX:
    {
        replaceCallAtAddress(0x080d8b14, hummerRespatch);
        replaceCallAtAddress(0x080d8b2f, hummerRespatch);
        replaceCallAtAddress(0x080d8b4a, hummerRespatch);
        replaceCallAtAddress(0x080d8b65, hummerRespatch);
        replaceCallAtAddress(0x080d8bb6, hummerRespatch);
        replaceCallAtAddress(0x080d8bd1, hummerRespatch);
    }
    break;
    case HUMMER_EXTREME:
    {
        replaceCallAtAddress(0x08159eab, hummerRespatch);
        replaceCallAtAddress(0x08159ec9, hummerRespatch);
        replaceCallAtAddress(0x08159ee7, hummerRespatch);
        replaceCallAtAddress(0x08159f05, hummerRespatch);
        replaceCallAtAddress(0x08159f5f, hummerRespatch);
        replaceCallAtAddress(0x08159f7d, hummerRespatch);
    }
    break;
    case HUMMER_EXTREME_MDX:
    {
        replaceCallAtAddress(0x0816348b, hummerRespatch);
        replaceCallAtAddress(0x081634a9, hummerRespatch);
        replaceCallAtAddress(0x081634c7, hummerRespatch);
        replaceCallAtAddress(0x081634e5, hummerRespatch);
        replaceCallAtAddress(0x0816353f, hummerRespatch);
        replaceCallAtAddress(0x0816355d, hummerRespatch);
    }
    break;
    case MJ4_REVG:
    {
        // Not Supported yet.
        //  patchMemory(0x080524a1, "01");         // Enable Anti Alias
        //  patchMemory(0x08053668, "b80a000000"); // Skips resolution set by the Dip Switches.
        //  vf5FSwidth = (getConfig()->height * 4) / 3;
        //  setVariable(0x08901598, vf5FSwidth);
        //  setVariable(0x0890159c, getConfig()->height);
        //  setVariable(0x089015a4, vf5FSwidth);

        // setVariable(0x089015a8, getConfig()->height);

        // setVariable(0x08901544, vf5FSwidth);
        // setVariable(0x08901548, getConfig()->height);

        // setVariable(0x0890158c, vf5FSwidth);
        // setVariable(0x08901590, getConfig()->height);
    }
    break;
    default:
        break;
    }
    return 0;
}
