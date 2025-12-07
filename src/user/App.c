#include "user/App.h"

#include "utilities/Maths.h"

#include "tools/Resource.h"

#include "systems/Renderer.h"
#include "systems/Physics.h"
#include "systems/Audio.h"
#include "systems/Input.h"

#define TEST_WINDOW_SIZE Vector2Int_New(1080, 720)
#define TEST_OBJECT_COUNT 16
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY -MATHS_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f

struct TEST_DATA
{
    RJGlobal_Size count;

    Vector3 positions[TEST_OBJECT_COUNT];
    Vector3 rotations[TEST_OBJECT_COUNT];
    Vector3 scales[TEST_OBJECT_COUNT];

    PhysicsComponent physicsComponents[TEST_OBJECT_COUNT];
    RendererComponent rendererComponents[TEST_OBJECT_COUNT];
    AudioComponent audioComponents[TEST_OBJECT_COUNT];

    struct TEST_CAMERA
    {
        Vector3 position;
        Vector3 rotation;
        float size;
        float nearClipPlane;
        float farClipPlane;
        bool isPerspective;

        float speed;
        float rotationSpeed;
    } camera;
} TED = {0}; // test entity datas

// float timer = 0.0f;

typedef RJGlobal_Size TestEntity;

TestEntity TestEntity_Create(Vector3 position, Vector3 rotation, Vector3 scale, PhysicsComponent body, RendererComponent renderable)
{
    TED.positions[TED.count] = position;
    TED.rotations[TED.count] = rotation;
    TED.scales[TED.count] = scale;
    TED.physicsComponents[TED.count] = body;
    TED.rendererComponents[TED.count] = renderable;
    return TED.count++;
}

ContextWindow *window = NULL;
char titleBuffer[RJGLOBAL_TEMP_BUFFER_SIZE] = {0};

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    RJGlobal_MemorySet(TED.physicsComponents, sizeof(PhysicsComponent) * TEST_OBJECT_COUNT, RJGLOBAL_INDEX_INVALID);
    RJGlobal_MemorySet(TED.rendererComponents, sizeof(RendererComponent) * TEST_OBJECT_COUNT, RJGLOBAL_INDEX_INVALID);
    RJGlobal_MemorySet(TED.audioComponents, sizeof(AudioComponent) * TEST_OBJECT_COUNT, RJGLOBAL_INDEX_INVALID);

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Physics_Initialize(TEST_OBJECT_COUNT, (Vector3 *)TED.positions, TEST_DRAG, TEST_GRAVITY, TEST_ELASTICITY);
    Audio_Initialize(TEST_OBJECT_COUNT, (Vector3 *)TED.positions);
    Renderer_Initialize(window, 4);

    Renderer_ConfigureShaders(scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "vertex.glsl"),
                              scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "fragment.glsl"));

    TED.camera.position = Vector3_New(0.0f, 0.0f, 5.0f);
    TED.camera.rotation = Vector3_New(-45.0f, -90.0f, 0.0f);
    TED.camera.speed = 10.0f;
    TED.camera.rotationSpeed = 75.0f;
    TED.camera.size = 90.0f;
    TED.camera.nearClipPlane = 0.01f;
    TED.camera.farClipPlane = 1000.0f;
    TED.camera.isPerspective = true;

    Renderer_ConfigureCamera(&TED.camera.position, &TED.camera.rotation, &TED.camera.size, &TED.camera.nearClipPlane, &TED.camera.farClipPlane, &TED.camera.isPerspective);
    Audio_ConfigureListener(&TED.camera.position, &TED.camera.rotation);

    // ListArray pistolMaterials = RendererMaterial_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Pistol.mat"));
    // RendererModel *pistolMdl = RendererModel_Create(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Pistol.mdl"), &pistolMaterials, Vector3_Zero, Vector3_Zero, Vector3_One);
    // sceneRenderer = RendererScene_CreateEmpty(scl("Main Scene"), TEST_OBJECT_COUNT);
    // RendererBatch *testBatch = RendererScene_CreateBatch(sceneRenderer, pistolMdl, TEST_OBJECT_COUNT);

    // RendererComponent *component = RendererBatch_CreateComponent(testBatch, &testEntityDatas.positions[0], &testEntityDatas.rotations[0], &testEntityDatas.scales[0]);

    Vector3 transformOffset[3] = {Vector3_Zero, Vector3_Zero, Vector3_NewN(0.5f)};
    RendererBatch testBatch = Renderer_BatchCreate(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.mdl"), transformOffset, TEST_OBJECT_COUNT, TED.positions, TED.rotations, TED.scales);

    TestEntity_Create(Vector3_New(0.0f, -10.0f, 0.0f),
                      Vector3_Zero,
                      Vector3_One,
                      Physics_ComponentCreate(TED.count, Vector3_One, 1.0f, true),
                      Renderer_ComponentCreate(TED.count, testBatch));

    TestEntity_Create(Vector3_Zero,
                      Vector3_Zero,
                      Vector3_One,
                      Physics_ComponentCreate(TED.count, Vector3_One, 1.0f, false),
                      Renderer_ComponentCreate(TED.count, testBatch));

    TestEntity_Create(Vector3_New(0.0f, 10.0f, 0.0f),
                      Vector3_Zero,
                      Vector3_One,
                      Physics_ComponentCreate(TED.count, Vector3_One, 5.0f, false),
                      Renderer_ComponentCreate(TED.count, testBatch));

    TED.audioComponents[1] = Audio_ComponentCreate(1, scl("audio" RJGLOBAL_PATH_DELIMETER_STR "Test.mp3"));
    Audio_ComponentSetPlaying(TED.audioComponents[1], true);
    Audio_ComponentRewind(TED.audioComponents[1], 0.2f);
}

void App_Loop(float deltaTime)
{
    Input_Update();

    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!window->fullScreen);
    }

    if (Input_GetKey(InputKeyCode_R, InputState_Down))
    {
        TED.camera.isPerspective = !TED.camera.isPerspective;
    }

    TED.camera.size -= Input_GetMouseScroll();

    // for (RJGlobal_Size i = 0; i < TEST_OBJECT_COUNT; i++)
    // {
    //     testEntityDatas.rotations[i].y += deltaTime;
    // }

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Pressed))
    {
        Input_ConfigureMouseMode(InputMouseMode_Captured);

        Vector2Int mousePositionDelta = Input_GetMousePositionDelta();
        Vector3 movementVector = Input_GetMovementVector();

        TED.camera.rotation.y += (float)mousePositionDelta.x * TED.camera.rotationSpeed * deltaTime;
        TED.camera.rotation.x -= (float)mousePositionDelta.y * TED.camera.rotationSpeed * deltaTime;
        TED.camera.rotation.x = Maths_Clamp(TED.camera.rotation.x, -89.0f, 89.0f);

        Vector3 direction = Vector3_Normalized(Vector3_New(
            Maths_Cos(TED.camera.rotation.x) * Maths_Cos(TED.camera.rotation.y),
            Maths_Sin(TED.camera.rotation.x),
            Maths_Cos(TED.camera.rotation.x) * Maths_Sin(TED.camera.rotation.y)));

        Vector3 right = Vector3_Normalized(Vector3_Cross(direction, Vector3_Up));

        Vector3 move = Vector3_Scale(direction, movementVector.y);
        move = Vector3_Add(move, Vector3_Scale(right, movementVector.x));
        move.y += movementVector.z;

        if (Vector3_Magnitude(move) > 0.0f)
        {
            move = Vector3_Normalized(move);
            TED.camera.position =
                Vector3_Add(TED.camera.position,
                            Vector3_Scale(move,
                                          TED.camera.speed * deltaTime * (Input_GetKey(InputKeyCode_LeftShift, InputState_Pressed) ? 2.0f : 1.0f)));
        }
    }
    else
    {
        Input_ConfigureMouseMode(InputMouseMode_Normal);

        // Vector3 movementVector = Input_GetMovementVector();
        // objectPlayer.position.x += movementVector.x * deltaTime * camera.speed;
        // objectPlayer.position.z -= movementVector.y * deltaTime * camera.speed;
    }

    Physics_UpdateComponents(deltaTime);

    // access collision data if needed by Physics_IsColliding(...)

    // if (Physics_IsColliding(TED.physicsComponents[1], TED.physicsComponents[2], NULL))
    // {
    //     RJGlobal_DebugWarning("test collision");
    // }

    Physics_ResolveCollisions();

    Audio_Update();

    // rendering
    Renderer_Update();

    Renderer_Render();

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    Physics_Terminate();
    Renderer_Terminate();

    if (window != NULL)
    {
        Context_Terminate();
    }
}
