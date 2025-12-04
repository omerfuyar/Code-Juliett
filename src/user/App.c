#include "user/App.h"

#include "utilities/Maths.h"

#include "tools/Resource.h"

#include "systems/Renderer.h"
#include "systems/Physics.h"
#include "systems/Input.h"

#define TEST_WINDOW_SIZE Vector2Int_New(1080, 720)
#define TEST_OBJECT_COUNT 16
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY_M -MATHS_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f

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
} camera = {0};

struct TEST_DATA
{
    RJGlobal_Size count;
    Vector3 positions[TEST_OBJECT_COUNT];
    Vector3 rotations[TEST_OBJECT_COUNT];
    Vector3 scales[TEST_OBJECT_COUNT];
    PhysicsComponent physicsComponents[TEST_OBJECT_COUNT];
    RendererComponent rendererComponents[TEST_OBJECT_COUNT];
    bool actives[TEST_OBJECT_COUNT];
} testEntityDatas = {0};

typedef RJGlobal_Index TestEntity;

TestEntity TestEntity_Create(Vector3 position, Vector3 rotation, Vector3 scale, PhysicsComponent body, RendererComponent renderable)
{
    testEntityDatas.positions[testEntityDatas.count] = position;
    testEntityDatas.rotations[testEntityDatas.count] = rotation;
    testEntityDatas.scales[testEntityDatas.count] = scale;
    testEntityDatas.physicsComponents[testEntityDatas.count] = body;
    testEntityDatas.actives[testEntityDatas.count] = true;
    testEntityDatas.rendererComponents[testEntityDatas.count] = renderable;
    return testEntityDatas.count++;
}

ContextWindow *window = NULL;
char titleBuffer[RJGLOBAL_TEMP_BUFFER_SIZE] = {0};

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);
    Physics_Initialize(TEST_OBJECT_COUNT, (Vector3 *)testEntityDatas.positions, TEST_DRAG, TEST_GRAVITY_M, TEST_ELASTICITY);

    Renderer_ConfigureShaders(scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "vertex.glsl"),
                              scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "fragment.glsl"));

    camera.position = Vector3_New(0.0f, 0.0f, 5.0f);
    camera.rotation = Vector3_New(-45.0f, -90.0f, 0.0f);
    camera.speed = 10.0f;
    camera.rotationSpeed = 75.0f;
    camera.size = 90.0f;
    camera.nearClipPlane = 0.01f;
    camera.farClipPlane = 1000.0f;
    camera.isPerspective = true;

    Renderer_ConfigureCamera(&camera.position, &camera.rotation, &camera.size, &camera.nearClipPlane, &camera.farClipPlane, &camera.isPerspective);

    // ListArray pistolMaterials = RendererMaterial_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Pistol.mat"));
    // RendererModel *pistolMdl = RendererModel_Create(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Pistol.mdl"), &pistolMaterials, Vector3_Zero, Vector3_Zero, Vector3_One);
    // sceneRenderer = RendererScene_CreateEmpty(scl("Main Scene"), TEST_OBJECT_COUNT);
    // RendererBatch *testBatch = RendererScene_CreateBatch(sceneRenderer, pistolMdl, TEST_OBJECT_COUNT);

    // RendererComponent *component = RendererBatch_CreateComponent(testBatch, &testEntityDatas.positions[0], &testEntityDatas.rotations[0], &testEntityDatas.scales[0]);

    RendererBatch testBatch = Renderer_BatchCreate(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Pistol.mdl"), TEST_OBJECT_COUNT);

    TestEntity_Create(Vector3_Zero, Vector3_Zero, Vector3_One, Physics_ComponentCreate(0, Vector3_One, 1.0f, false), Renderer_ComponentCreate(0, testBatch, &testEntityDatas.positions[0], &testEntityDatas.rotations[0], &testEntityDatas.scales[0]));
    TestEntity_Create(Vector3_New(0.0f, -10.0f, 0.0f), Vector3_Zero, Vector3_One, Physics_ComponentCreate(1, Vector3_Scale(Vector3_One, 5.0f), 1.0f, true), Renderer_ComponentCreate(0, testBatch, &testEntityDatas.positions[0], &testEntityDatas.rotations[0], &testEntityDatas.scales[0]));
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
        camera.isPerspective = !camera.isPerspective;
    }

    camera.size -= Input_GetMouseScroll();

    for (RJGlobal_Size i = 0; i < TEST_OBJECT_COUNT; i++)
    {
        testEntityDatas.rotations[i].y += deltaTime;
    }

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Pressed))
    {
        Input_ConfigureMouseMode(InputMouseMode_Captured);

        Vector2Int mousePositionDelta = Input_GetMousePositionDelta();
        Vector3 movementVector = Input_GetMovementVector();

        camera.rotation.y += (float)mousePositionDelta.x * camera.rotationSpeed * deltaTime;
        camera.rotation.x -= (float)mousePositionDelta.y * camera.rotationSpeed * deltaTime;
        camera.rotation.x = Maths_Clamp(camera.rotation.x, -89.0f, 89.0f);

        Vector3 direction = Vector3_Normalized(Vector3_New(
            Maths_Cos(camera.rotation.x) * Maths_Cos(camera.rotation.y),
            Maths_Sin(camera.rotation.x),
            Maths_Cos(camera.rotation.x) * Maths_Sin(camera.rotation.y)));

        Vector3 right = Vector3_Normalized(Vector3_Cross(direction, Vector3_Up));

        Vector3 move = Vector3_Scale(direction, movementVector.y);
        move = Vector3_Add(move, Vector3_Scale(right, movementVector.x));
        move.y += movementVector.z;

        if (Vector3_Magnitude(move) > 0.0f)
        {
            move = Vector3_Normalized(move);
            camera.position = Vector3_Add(camera.position, Vector3_Scale(move, camera.speed * deltaTime * (Input_GetKey(InputKeyCode_LeftShift, InputState_Pressed) ? 2.0f : 1.0f)));
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

    Physics_ResolveCollisions();

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
