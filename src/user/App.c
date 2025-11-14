#include "user/App.h"

#include "utilities/Maths.h"
#include "utilities/Timer.h"

#include "tools/Resources.h"

#include "systems/Renderer.h"
#include "systems/Physics.h"
#include "systems/Input.h"
// #include "systems/Audio.h"

#define TEST_BENCH_TIME_SECONDS 10.0f
#define TEST_WINDOW_SIZE Vector2Int_New(1080, 720)
#define TEST_OBJECT_COUNT 16
#define TEST_DEBUG_RENDERER RJGLOBAL_BUILD_DEBUG
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY_M -MATHS_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f
#define TEST_OBJECT_SPEED_LIMIT 50
#define TEST_BENCHMARK false
#define TEST_MONITOR false

#if TEST_BENCHMARK
float benchTimer = 0.0f;
size_t benchFrameCount = 0;

size_t totalFaceCount = 0;
size_t totalVertexCount = 0;
size_t totalBatchCount = 0;
size_t totalObjectCount = 0;
#endif

#if TEST_MONITOR
Timer timer = {0};
#endif

typedef struct myObjectType
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    RendererComponent *renderable;
    PhysicsComponent *physics;
} myObjectType;

typedef struct myCameraType
{
    String name;
    Vector3 position;
    Vector3 rotation;
    RendererCameraComponent *camera;
    float rotationSpeed;
    float speed;
} myCameraType;

ContextWindow *window = NULL;
RendererScene *sceneRenderer = NULL;
PhysicsScene *scenePhysics = NULL;
myCameraType camera = {0};
myObjectType testObjects[TEST_OBJECT_COUNT] = {0};
char titleBuffer[RJGLOBAL_TEMP_BUFFER_SIZE];

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);
    // Audio_Initialize(1);

    Renderer_ConfigureShaders(scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "vertex.glsl"),
                              scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "fragment.glsl"));

#if TEST_DEBUG_RENDERER
    RendererDebug_Initialize(scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "debugVertex.glsl"),
                             scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "debugFragment.glsl"), 8192);
#endif

    ListArray materialPool = RendererMaterial_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.mat"));
    ListArray modelPool = RendererModel_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.mdl"), &materialPool);
    ListArray_Destroy(&materialPool);

    sceneRenderer = RendererScene_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.scn"), &modelPool, (void *)testObjects, 0, sizeof(myObjectType), TEST_OBJECT_COUNT);
    ListArray_Destroy(&modelPool);

    // scenePhysics = PhysicsScene_Create(scl("My Physics Scene"), TEST_OBJECT_COUNT + 1, TEST_DRAG, TEST_GRAVITY_M, TEST_ELASTICITY);

    camera.position = Vector3_New(0.0f, 0.0f, 5.0f);
    camera.rotation = Vector3_New(-45.0f, -90.0f, 0.0f);
    camera.speed = 10.0f;
    camera.rotationSpeed = 75.0f;
    camera.camera = RendererCameraComponent_Create(&camera.position, &camera.rotation);
    camera.camera->isPerspective = true;
    camera.camera->size = 90.0f;
    camera.camera->nearClipPlane = 0.1f;
    camera.camera->farClipPlane = 1000.0f;

    RendererScene_SetMainCamera(sceneRenderer, camera.camera);

    // AudioClip *testClip = AudioClip_Create(scl("sounds" RJGLOBAL_PATH_DELIMETER_STR "Test.mp3"));

    // Audio_PlayClip(testClip);

#if TEST_MONITOR
    timer = Timer_Create("main timer");
#endif
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
        camera.camera->isPerspective = !camera.camera->isPerspective;
    }

    camera.camera->size -= Input_GetMouseScroll();

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    for (size_t i = 0; i < TEST_OBJECT_COUNT; i++)
    {
        testObjects[i].rotation.y += deltaTime;
    }

    testObjects[0].rotation.y += deltaTime;
#if TEST_MONITOR
    Timer_Stop(&timer);
    RJGlobal_DebugLog(false, "MONITOR", "rotating : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

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

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    // PhysicsScene_UpdateComponents(scenePhysics, deltaTime);
#if TEST_MONITOR
    Timer_Stop(&timer);
    RJGlobal_DebugLog(false, "MONITOR", "physics update : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

    // access collision data if needed by Physics_IsColliding(...)

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    // PhysicsScene_ResolveCollisions(scenePhysics);
#if TEST_MONITOR
    Timer_Stop(&timer);
    RJGlobal_DebugLog(false, "MONITOR", "physics resolve : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    RendererScene_Update(sceneRenderer);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(sceneRenderer);

#if TEST_DEBUG_RENDERER
    // RendererDebug_StartRendering();
//
// for (size_t i = 0; i < scenePhysics->components.count; i++)
//{
//    PhysicsComponent *physComp = (PhysicsComponent *)ListArray_Get(&scenePhysics->components, i);
//    RendererDebug_DrawBoxLines(*physComp->positionReference, physComp->colliderSize, Color_New((float)(i % 2), (float)((i / 2) % 2), (float)((i / 4) % 2), 1.0f));
//}
//
// RendererDebug_FinishRendering(&camera.camera->projectionMatrix, &camera.camera->viewMatrix);
#endif

    Renderer_FinishRendering();
#if TEST_MONITOR
    Timer_Stop(&timer);
    RJGlobal_DebugLog(false, "MONITOR", "rendering : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));

#if TEST_BENCHMARK
    benchTimer += deltaTime;
    benchFrameCount++;

    if (benchTimer >= TEST_BENCH_TIME_SECONDS)
    {
        char messageBuffer[RJGLOBAL_TEMP_BUFFER_SIZE];
        snprintf(messageBuffer, sizeof(messageBuffer), "bt : %f sec | avgFPS : %f | f : %zu | v : %zu | b : %zu | o : %zu | fs : %s | dr : %s | cc : %zu",
                 benchTimer,
                 (float)benchFrameCount / TEST_BENCH_TIME_SECONDS,
                 totalFaceCount,
                 totalVertexCount,
                 totalBatchCount,
                 totalObjectCount,
                 TEST_FULL_SCREEN ? "true" : "false",
                 TEST_DEBUG_RENDERER ? "true" : "false",
                 scenePhysics->components.count);

        RJGlobal_Terminate(0, messageBuffer);
    }
#endif
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    if (sceneRenderer != NULL)
    {
        RendererScene_Destroy(sceneRenderer);
    }

    if (scenePhysics != NULL)
    {
        PhysicsScene_Destroy(scenePhysics);
    }

#if TEST_DEBUG_RENDERER
    if (sceneRenderer != NULL)
    {
        RendererDebug_Terminate();
    }
#endif

    if (window != NULL)
    {
        Context_Terminate();
    }
}
