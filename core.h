#ifndef CORE_H
#define CORE_H

#include "engine_platform.h"
#include "engine.h"

#define CHUNK_SIZE 64
#define CHUNK_WORKGROUP_SIZE 16
#define MAX_LOADED_CHUNKS 32

typedef struct TerrainChunk
{
    Vec3 origin;
    IVec3 chunkCoordinate;
    b32 isAllocate;
    Entity entity;
} TerrainChunk;

typedef struct Game_State
{
    Vec4 sunDir;

    Shader texShaderForw;
    Shader texShader;
    Shader notexShader;
    Shader skyShader;
    Shader normalShader;
    Shader lineShader;
    Shader particleShader;
    Shader terrainGenShader;
    Shader straightShader;

    Entity barrel[3600];
    Entity terrain;
    Entity testBarrel;
    Entity wall;
    Entity wall2;
    Entity voxelTerrain[100];
    Entity dome;

    TerrainChunk loadedChunks[MAX_LOADED_CHUNKS];
    u32 loadedChunkCount;

    u32 voxelTerrainCount;

    Material barrelMat;
    Material grassMat;
} Game_State;

typedef struct TransientStorage
{
    MemoryArena mainArena;
    RenderGroup renderGroup;
    Camera mainCamera;
    TexturePool* texturePool;

    OpenglState glState;
} TransientStorage;

typedef struct Permanent_Storage{
    Camera main_cam;

    TexturePool* texturePool;
    i32 windowWidth;
    i32 windowHeight;
    int numEntities;
    Entity* entities[5000];

    OpenglFrameBuffer shadowmap_fbo;

    Shader postProcShader;
    Shader postProcPointShader;
    Shader shadowPassShader;
    Shader lightCullShader;
    Shader textShader;
    Shader terrainComputeShader;
    Shader terrainComputeShader2;

    TerrainGeneratorState terrainGenState;

    // TODO: make a shader pool
    Shader* shaders[32];
    u32 numShaders;

    u32 mcubesTexture;
    u32 mcubesTexture2;
    u32 numberOfTriangles;
    b32 captured;

    TransientStorage* tstorage;
    DebugState debugState;

    Game_State game;
} Permanent_Storage;

typedef struct PointLight {
    Vec4 color;
    Vec4 position;
    Vec4 paddingAndRadius;
} PointLight;

inline void addEntity(Permanent_Storage *state, Entity *ent);
static inline void addSurfaceShader(Permanent_Storage *state, Shader *shader);

void loadChunkVertices(Permanent_Storage* state, Vec3 origin, ArrayMesh* mesh);
TerrainChunk loadChunk(Permanent_Storage* state, IVec3 chunkId);

#ifdef __cplusplus
extern "C" {
#endif

void init(EngineMemory* gamemem, int width, int height);
void display(EngineMemory *gamemem, Input *input, float dt);
void keyboard( unsigned char key, int mouseX, int mouseY );
void pMouse(int x, int y);
void reshape (EngineMemory *game_memory, int w, int h);
void gameExit(EngineMemory *game_memory);

#ifdef __cplusplus
}
#endif

#endif // CORE_H
