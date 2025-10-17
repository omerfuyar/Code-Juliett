#include "user/App.h"

#include "utilities/Maths.h"
#include "utilities/Timer.h"

#include "tools/Resources.h"
#include "tools/Renderer.h"
#include "tools/Physics.h"
#include "tools/Input.h"

#define TEST_BENCH_TIME_SECONDS 10.0f
#define TEST_WINDOW_SIZE NewVector2Int(1080, 720)
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY_M -GRAVITY_M * 10.0f
#define TEST_DRAG 0.05f
#define TEST_ELASTICITY 1.0f
#define TEST_BENCHMARK false

#if TEST_BENCHMARK
float benchTimer = 0.0f;
size_t benchFrameCount = 0;
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
myObjectType objectPlayer = {0};
myObjectType objectPlane = {0};
char titleBuffer[TEMP_BUFFER_SIZE];

RendererModel *LoadModel(StringView name, StringView matFileName, StringView mdlFileName, StringView texFileName)
{
    ResourceText *matFile = ResourceText_Create(matFileName, scl("models" PATH_DELIMETER_STR));
    ResourceImage *texture = ResourceImage_Create(texFileName, scl("models" PATH_DELIMETER_STR));
    ListArray materials = RendererMaterial_CreateTexture(scv(matFile->data), matFile->lineCount, name, texture->data, texture->size, texture->channels);
    // ListArray materials = RendererMaterial_CreateFile(scv(matFile->data), matFile->lineCount);
    ResourceText_Destroy(matFile);
    ResourceImage_Destroy(texture);

    ResourceText *mdlFile = ResourceText_Create(mdlFileName, scl("models" PATH_DELIMETER_STR));
    RendererModel *model = RendererModel_Create(name, scv(mdlFile->data), mdlFile->lineCount, &materials, NewVector3N(100.0f), NewVector3N(0.0f), NewVector3N(1.0f));
    ResourceText_Destroy(mdlFile);
    ListArray_Destroy(&materials);

    return model;
}

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);

    ResourceText *rscDebugVertexShader = ResourceText_Create(scl("debugVertex.glsl"), scl("shaders" PATH_DELIMETER_STR));
    ResourceText *rscDebugFragmentShader = ResourceText_Create(scl("debugFragment.glsl"), scl("shaders" PATH_DELIMETER_STR));
    RendererDebug_Initialize(scv(rscDebugVertexShader->data), scv(rscDebugFragmentShader->data), 1024);
    ResourceText_Destroy(rscDebugVertexShader);
    ResourceText_Destroy(rscDebugFragmentShader);

    ResourceText *rscVertexShader = ResourceText_Create(scl("vertex.glsl"), scl("shaders" PATH_DELIMETER_STR));
    ResourceText *rscFragmentShader = ResourceText_Create(scl("fragment.glsl"), scl("shaders" PATH_DELIMETER_STR));
    Renderer_ConfigureShaders(scv(rscVertexShader->data), scv(rscFragmentShader->data));
    ResourceText_Destroy(rscVertexShader);
    ResourceText_Destroy(rscFragmentShader);

    RendererModel *modelPlayer = LoadModel(scl("Player"), scl("Player.mat"), scl("Player.mdl"), scl("Player.png"));
    RendererModel *modelPlane = LoadModel(scl("Plane"), scl("Plane.mat"), scl("Plane.mdl"), scl("Plane.jpg"));

    sceneRenderer = RendererScene_Create(scl("My Renderer Scene"), 4);
    RendererBatch *batchPlayer = RendererScene_CreateBatch(sceneRenderer, scl("Player Batch"), modelPlayer, 1);
    RendererBatch *batchPlane = RendererScene_CreateBatch(sceneRenderer, scl("Plane Batch"), modelPlane, 1);

    camera.position = NewVector3(-3.0f, 0.0f, 0.0f);
    camera.rotation = Vector3_Zero;
    camera.speed = 10.0f;
    camera.rotationSpeed = 75.0f;
    camera.camera = RendererCameraComponent_Create(&camera.position, &camera.rotation);
    camera.camera->isPerspective = true;
    camera.camera->size = 90.0f;
    camera.camera->nearClipPlane = 0.1f;
    camera.camera->farClipPlane = 1000.0f;

    RendererScene_SetMainCamera(sceneRenderer, camera.camera);

    scenePhysics = PhysicsScene_Create(scl("My Physics Scene"), 2, TEST_DRAG, TEST_GRAVITY_M, TEST_ELASTICITY);

    objectPlayer.position = Vector3_Zero;
    objectPlayer.rotation = Vector3_Zero;
    objectPlayer.scale = Vector3_One;
    objectPlayer.renderable = RendererBatch_CreateComponent(batchPlayer, &objectPlayer.position, &objectPlayer.rotation, &objectPlayer.scale);
    objectPlayer.physics = PhysicsScene_CreateComponent(scenePhysics, &objectPlayer.position, objectPlayer.scale, 1.0f, false);

    objectPlane.position = NewVector3(0.0f, -3.0f, 0.0f);
    objectPlane.rotation = Vector3_Zero;
    objectPlane.scale = NewVector3(20.0f, 1.0f, 20.0f);
    objectPlane.renderable = RendererBatch_CreateComponent(batchPlane, &objectPlane.position, &objectPlane.rotation, &objectPlane.scale);
    objectPlane.physics = PhysicsScene_CreateComponent(scenePhysics, &objectPlane.position, objectPlane.scale, 0.0f, true);
}

void App_Loop(float deltaTime)
{
    Input_Update();

    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!window->fullScreen);
    }

    camera.camera->size -= Input_GetMouseScroll();

    objectPlayer.rotation.y += deltaTime;

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Pressed))
    {
        Input_ConfigureMouseMode(InputMouseMode_Captured);

        Vector2Int mousePositionDelta = Input_GetMousePositionDelta();
        Vector3 movementVector = Input_GetMovementVector();

        camera.rotation.y += (float)mousePositionDelta.x * camera.rotationSpeed * deltaTime;
        camera.rotation.x -= (float)mousePositionDelta.y * camera.rotationSpeed * deltaTime;
        camera.rotation.x = Clamp(camera.rotation.x, -89.0f, 89.0f);

        Vector3 direction = Vector3_Normalized(NewVector3(
            Cos(camera.rotation.x) * Cos(camera.rotation.y),
            Sin(camera.rotation.x),
            Cos(camera.rotation.x) * Sin(camera.rotation.y)));

        Vector3 right = Vector3_Normalized(Vector3_Cross(direction, Vector3_Up));

        Vector3 move = Vector3_Scale(direction, movementVector.y);
        move = Vector3_Add(move, Vector3_Scale(right, movementVector.x));
        move.y += movementVector.z;

        if (Vector3_Magnitude(move) > 0.0f)
        {
            move = Vector3_Normalized(move);
            camera.position = Vector3_Add(camera.position, Vector3_Scale(move, camera.speed * deltaTime));
        }
    }
    else
    {
        Input_ConfigureMouseMode(InputMouseMode_Normal);

        // Vector3 movementVector = Input_GetMovementVector();
        // myObj.position.x += movementVector.x * deltaTime * mainCamera.speed;
        // myObj.position.z -= movementVector.y * deltaTime * mainCamera.speed;
    }

    PhysicsScene_UpdateComponents(scenePhysics, deltaTime);

    // access collision data if needed by Physics_IsColliding(...)

    PhysicsScene_ResolveCollisions(scenePhysics);

    RendererScene_Update(sceneRenderer);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(sceneRenderer);

    RendererDebug_StartRendering();

    RendererDebug_DrawBoxLines(objectPlayer.position, objectPlayer.physics->colliderSize, Color_Cyan);
    RendererDebug_DrawBoxLines(objectPlane.position, objectPlane.physics->colliderSize, Color_Red);

    RendererDebug_FinishRendering(&camera.camera->projectionMatrix, &camera.camera->viewMatrix);

    Renderer_FinishRendering();

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));

#if (TEST_BENCHMARK)
    benchTimer += deltaTime;
    benchFrameCount++;

    if (benchTimer >= TEST_BENCH_TIME_SECONDS)
    {
        char messageBuffer[TEMP_BUFFER_SIZE];
        snprintf(messageBuffer, sizeof(messageBuffer), "BenchMark Time : %f seconds | Average FPS : %f | Full Screen : %s",
                 benchTimer,
                 (float)benchFrameCount / TEST_BENCH_TIME_SECONDS,
                 TEST_FULL_SCREEN ? "true" : "false");

        Global_Terminate(0, messageBuffer);
    }
#endif
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    Renderer_Terminate();
    // RendererDebug_Terminate();
    Context_Terminate();
}
