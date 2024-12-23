#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    const char *search;
    const char *replacement;
} SearchReplace;

typedef struct
{
    uint32_t startOffset; // Offset inside the ELF
    uint32_t totalSize;   // Available size for the shader program
    uint32_t sizeOfCode;  // Size used by the original shader program
} ElfShaderOffsets;

typedef struct
{
    char *fileName;
    size_t shaderBufferSize;
    char *shaderBuffer;
} ShaderFilesToMod;

typedef struct
{
    const char *fileName;
    size_t patchBufferSize;
    const unsigned char *patchBuffer;
    size_t shaderBufferSize;
    const char *shaderBuffer;
} ShaderFilesToPatch;

SearchReplace attribsSet[] = {{"POSITION;", "ATTR0;"},  {"COLOR0;", "ATTR3;"},      {"COLOR;", "ATTR3;"},
                              {"BINORMAL;", "ATTR15;"}, {"NORMAL;", "ATTR2;"},      {"TEXCOORD0;", "ATTR8;"},
                              {"TEXCOORD1;", "ATTR9;"}, {"TEXCOORD2;", "ATTR10;"},  {"TEXCOORD3;", "ATTR11;"},
                              {"TANGENT;", "ATTR14;"},  {"BLENDWEIGHT;", "ATTR1;"}, {"BLENDINDICES;", "ATTR7;"}};

int attribsSetCount = sizeof(attribsSet) / sizeof(SearchReplace);

char *nnstdshader_vert;
char *nnstdshader_frag;