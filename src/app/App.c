#include "RJGlobal.h"

#include "systems/Renderer.h"
#include "systems/Input.h"

#include "tools/Context.h"
#include "tools/Resource.h"

#include "utilities/Maths.h"

#define check(result) RJ_DebugAssert(!(result), "Error received : " #result)

#define MAP_ENTITIES 63
#define OTHER_ENTITIES 1 // player

#define MAX_ENTITIES (MAP_ENTITIES + OTHER_ENTITIES)

#define PLAYER_SENSITIVITY 10.0f
#define PLAYER_SPEED 2.0f

struct Test
{
    RendererBatch batch;
    Entity entities[MAP_ENTITIES];
    // Entity entities[MAP_ENTITIES];
    //  RendererBatch meshBatch;
    //  Entity colliderEntities[MAX_COLLIDERS - 1 /*player collider*/];
    //  RJ_Size colliderCount;
    //  Entity audioEntities[MAX_AUDIOS];
    //  RJ_Size audioCount;
} test = {0};

struct Player
{
    Entity entity;
    RendererCamera camera;
    float camSens;
    float speed;
    // AudioListener listener;
} player = {0};

bool focus = false;

#pragma region helpers

void fetchPlayerTransform()
{
    Vector3 eyePos = Vector3G_Sum(Entity_GetPosition(player.entity), Vector3_New(0.0f, 0.5f, 0.0f));

    player.camera.position = eyePos;
    player.camera.rotation = Entity_GetRotation(player.entity);

    Renderer_SetCameraData(&player.camera);
}

void movement(float deltaTime)
{
    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Down))
    {
        focus = true;
        Input_ConfigureCursorMode(InputCursorMode_Captured);
    }
    else if (Input_GetKey(InputKeyCode_Escape, InputState_Down))
    {
        focus = false;
        Input_ConfigureCursorMode(InputCursorMode_Normal);
    }

    if (!focus)
    {
        return;
    }

    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!Context_GetInternalData()->fullScreen);
    }

    Vector2Int mousePositionDelta = Input_GetMousePositionDelta();
    Vector3 movementVector = Input_GetMovementVector();

    Vector3 playerRotation = Entity_GetRotation(player.entity);

    playerRotation.y += (float)mousePositionDelta.x * player.camSens * deltaTime;
    playerRotation.x -= (float)mousePositionDelta.y * player.camSens * deltaTime;
    playerRotation.x = Maths_Clamp(playerRotation.x, -89.0f, 89.0f);

    Entity_SetRotation(player.entity, playerRotation);

    Vector3 direction = Vector3_Normalized(Vector3_New(
        Maths_Cos(playerRotation.x) * Maths_Cos(playerRotation.y),
        Maths_Sin(playerRotation.x),
        Maths_Cos(playerRotation.x) * Maths_Sin(playerRotation.y)));

    Vector3 right = Vector3_Normalized(Vector3_Cross(direction, Vector3_Up));
    Vector3 up = Vector3_Normalized(Vector3_Cross(direction, right));

    Vector3 move = Vector3G_Scale(direction, movementVector.y);
    move = Vector3G_Sum(move, Vector3G_Scale(right, movementVector.x));
    move = Vector3G_Sum(move, Vector3G_Scale(up, -movementVector.z));

    if (Vector3_Magnitude(move) > 0.01f)
    {
        move = Vector3_Normalized(move);
        Entity_AddPosition(player.entity,
                           Vector3G_Scale(move, player.speed * deltaTime * (Input_GetKey(InputKeyCode_LeftShift, InputState_Pressed) ? 2.0f : 1.0f)));
    }

    fetchPlayerTransform();
}

#pragma endregion helpers

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    Resource_Initialize();
    check(Context_Initialize());
    check(Entity_Initialize(MAX_ENTITIES));

    Input_Initialize();
    check(Renderer_Initialize(1));
    check(Renderer_ConfigureShaders(scl("shaders/vertex.glsl"), scl("shaders/fragment.glsl")));

    test.entities[0] = Entity_Create(Vector3_Zero, Vector3_Zero, Vector3_One);
    check(Renderer_BatchCreate(&test.batch, scl("models/BoxTextured.glb"), MAP_ENTITIES));
    check(Renderer_ComponentCreate(test.batch, test.entities[0]));

    // float sqrt = Maths_Root(MAP_ENTITIES, 2);
    // for (RJ_Size i = 0; i < MAP_ENTITIES; i++)
    //{
    //     test.entities[i] = Entity_Create(Vector3_New((-(RJ_Size)sqrt / 2 + (i % (RJ_Size)sqrt)), i / (RJ_Size)sqrt, 0), Vector3_Zero, Vector3_One);
    //     check(Renderer_ComponentCreate(test.batch, test.entities[i]));
    // }

    player.entity = Entity_Create(Vector3_Zero, Vector3_Zero, Vector3_One);
    player.camSens = PLAYER_SENSITIVITY;
    player.speed = PLAYER_SPEED;
    player.camera = RendererCamera_Default;

    fetchPlayerTransform();
}

void App_Loop(float deltaTime)
{
    Input_Update();

    if (!Context_Update())
    {
        RJ_Terminate(RJ_OK, "Main window close input received");
    }

    movement(deltaTime);

    Renderer_Update();
    Renderer_Render();
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    Renderer_BatchDestroy(test.batch);

    if (Renderer_IsInitialized())
    {
        Renderer_Terminate();
    }

    if (Input_IsInitialized())
    {
        Input_Terminate();
    }

    if (Context_IsInitialized())
    {
        Context_Terminate();
    }

    if (Resource_IsInitialized())
    {
        Resource_Terminate();
    }

    if (Entity_IsInitialized())
    {
        Entity_Terminate();
    }
}
