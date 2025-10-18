#include "user/App.h"

#include "utilities/Maths.h"
#include "utilities/Timer.h"

#include "tools/Resources.h"
#include "tools/Renderer.h"
#include "tools/Physics.h"
#include "tools/Input.h"

#define TEST_BENCH_TIME_SECONDS 10.0f
#define TEST_WINDOW_SIZE NewVector2Int(1080, 720)
#define TEST_OBJECT_COUNT 256
#define TEST_DEBUG_RENDERER RJ_BUILD_DEBUG
#define TEST_VSYNC false
#define TEST_FULL_SCREEN true
#define TEST_GRAVITY_M -GRAVITY_M
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f
#define TEST_BENCHMARK true

#if TEST_BENCHMARK
float benchTimer = 0.0f;
size_t benchFrameCount = 0;

size_t totalFaceCount = 0;
size_t totalVertexCount = 0;
size_t totalBatchCount = 0;
size_t totalObjectCount = 0;
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
// myObjectType objectPlayer = {0};
myObjectType objectPlane = {0};
myObjectType testObjects[TEST_OBJECT_COUNT] = {0};
char titleBuffer[RJ_TEMP_BUFFER_SIZE];

RendererModel *LoadModel(StringView name, StringView matFileName, StringView mdlFileName, StringView texFileName, Vector3 scaleOffset)
{
    ResourceText *matFile = ResourceText_Create(matFileName, scl("models" RJ_PATH_DELIMETER_STR));
    ResourceImage *texture = ResourceImage_Create(texFileName, scl("models" RJ_PATH_DELIMETER_STR));
    ListArray materials = RendererMaterial_CreateTexture(scv(matFile->data), matFile->lineCount, texFileName, texture->data, texture->size, texture->channels);
    // ListArray materials = RendererMaterial_CreateFile(scv(matFile->data), matFile->lineCount);
    ResourceText_Destroy(matFile);
    ResourceImage_Destroy(texture);

    ResourceText *mdlFile = ResourceText_Create(mdlFileName, scl("models" RJ_PATH_DELIMETER_STR));
    RendererModel *model = RendererModel_Create(name, scv(mdlFile->data), mdlFile->lineCount, &materials, NewVector3N(100.0f), NewVector3N(0.0f), scaleOffset);
    ResourceText_Destroy(mdlFile);
    ListArray_Destroy(&materials);

    return model;
}

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);

    ResourceText *rscVertexShader = ResourceText_Create(scl("vertex.glsl"), scl("shaders" RJ_PATH_DELIMETER_STR));
    ResourceText *rscFragmentShader = ResourceText_Create(scl("fragment.glsl"), scl("shaders" RJ_PATH_DELIMETER_STR));
    Renderer_ConfigureShaders(scv(rscVertexShader->data), scv(rscFragmentShader->data));
    ResourceText_Destroy(rscVertexShader);
    ResourceText_Destroy(rscFragmentShader);

#if TEST_DEBUG_RENDERER
    ResourceText *rscDebugVertexShader = ResourceText_Create(scl("debugVertex.glsl"), scl("shaders" RJ_PATH_DELIMETER_STR));
    ResourceText *rscDebugFragmentShader = ResourceText_Create(scl("debugFragment.glsl"), scl("shaders" RJ_PATH_DELIMETER_STR));
    RendererDebug_Initialize(scv(rscDebugVertexShader->data), scv(rscDebugFragmentShader->data), 8192);
    ResourceText_Destroy(rscDebugVertexShader);
    ResourceText_Destroy(rscDebugFragmentShader);
#endif

    // RendererModel *modelPlayer = LoadModel(scl("Player"), scl("Player.mat"), scl("Player.mdl"), scl("Player.png"), NewVector3N(0.5f));
    RendererModel *modelPlane = LoadModel(scl("Plane"), scl("Plane.mat"), scl("Plane.mdl"), scl("Plane.png"), NewVector3N(0.5f));
    RendererModel *modelTest = LoadModel(scl("Test"), scl("Pistol.mat"), scl("Pistol.mdl"), scl("Player.png"), NewVector3N(1.0f));

    sceneRenderer = RendererScene_Create(scl("My Renderer Scene"), TEST_OBJECT_COUNT);
    // RendererBatch *batchPlayer = RendererScene_CreateBatch(sceneRenderer, scl("Player Batch"), modelPlayer, 1);
    RendererBatch *batchPlane = RendererScene_CreateBatch(sceneRenderer, scl("Plane Batch"), modelPlane, 1);
    RendererBatch *batchTest = RendererScene_CreateBatch(sceneRenderer, scl("Test Batch"), modelTest, TEST_OBJECT_COUNT);

    camera.position = NewVector3(0.0f, 8.0f, 30.0f);
    camera.rotation = NewVector3(0.0f, -90.0f, 0.0f);
    camera.speed = 10.0f;
    camera.rotationSpeed = 75.0f;
    camera.camera = RendererCameraComponent_Create(&camera.position, &camera.rotation);
    camera.camera->isPerspective = true;
    camera.camera->size = 90.0f;
    camera.camera->nearClipPlane = 0.1f;
    camera.camera->farClipPlane = 1000.0f;

    RendererScene_SetMainCamera(sceneRenderer, camera.camera);

    scenePhysics = PhysicsScene_Create(scl("My Physics Scene"), TEST_OBJECT_COUNT + 1, TEST_DRAG, TEST_GRAVITY_M, TEST_ELASTICITY);

    // objectPlayer.position = Vector3_Zero;
    // objectPlayer.rotation = Vector3_Zero;
    // objectPlayer.scale = Vector3_One;
    // objectPlayer.renderable = RendererBatch_CreateComponent(batchPlayer, &objectPlayer.position, &objectPlayer.rotation, &objectPlayer.scale);
    // objectPlayer.physics = PhysicsScene_CreateComponent(scenePhysics, &objectPlayer.position, objectPlayer.scale, 1.0f, false);

    objectPlane.position = NewVector3(0.0f, -3.0f, 0.0f);
    objectPlane.rotation = Vector3_Zero;
    objectPlane.scale = NewVector3(20.0f, 1.0f, 20.0f);
    objectPlane.renderable = RendererBatch_CreateComponent(batchPlane, &objectPlane.position, &objectPlane.rotation, &objectPlane.scale);
    objectPlane.physics = PhysicsScene_CreateComponent(scenePhysics, &objectPlane.position, objectPlane.scale, 0.0f, true);

    for (size_t i = 0; i < TEST_OBJECT_COUNT; i++)
    {
        myObjectType *testObject = testObjects + i;
        testObject->position = NewVector3(RandomRange(-10, 10), RandomRange(0, 20), RandomRange(-10, 10));
        testObject->rotation = Vector3_Zero;
        testObject->scale = Vector3_One;
        testObject->renderable = RendererBatch_CreateComponent(batchTest, &testObject->position, &testObject->rotation, &testObject->scale);
        testObject->physics = PhysicsScene_CreateComponent(scenePhysics, &testObject->position, testObject->scale, 1.0f, false);
    }

    for (size_t i = 0; i < sceneRenderer->batches.count; i++)
    {
        RendererBatch *batch = (RendererBatch *)ListArray_Get(&sceneRenderer->batches, i);
        totalVertexCount += batch->model->vertices.count * batch->components.count;
        totalBatchCount++;
        totalObjectCount += batch->components.count;
        for (size_t j = 0; j < batch->model->meshes.count; j++)
        {
            RendererMesh *mesh = *(RendererMesh **)ListArray_Get(&batch->model->meshes, j);
            totalFaceCount += mesh->indices.count / 3 * batch->components.count;
        }
    }
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

    for (size_t i = 0; i < TEST_OBJECT_COUNT; i++)
    {
        testObjects[i].rotation.y += deltaTime;
    }

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
        // objectPlayer.position.x += movementVector.x * deltaTime * camera.speed;
        // objectPlayer.position.z -= movementVector.y * deltaTime * camera.speed;
    }

    PhysicsScene_UpdateComponents(scenePhysics, deltaTime);

    // access collision data if needed by Physics_IsColliding(...)

    PhysicsScene_ResolveCollisions(scenePhysics);

    RendererScene_Update(sceneRenderer);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(sceneRenderer);

#if TEST_DEBUG_RENDERER
    RendererDebug_StartRendering();

    for (size_t i = 0; i < scenePhysics->components.count; i++)
    {
        PhysicsComponent *physComp = (PhysicsComponent *)ListArray_Get(&scenePhysics->components, i);
        RendererDebug_DrawBoxLines(*physComp->positionReference, physComp->colliderSize, NewColor(i % 2, (i / 2) % 2, (i / 4) % 2, 1.0f));
    }

    RendererDebug_FinishRendering(&camera.camera->projectionMatrix, &camera.camera->viewMatrix);
#endif

    Renderer_FinishRendering();

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));

#if (TEST_BENCHMARK)
    benchTimer += deltaTime;
    benchFrameCount++;

    if (benchTimer >= TEST_BENCH_TIME_SECONDS)
    {
        char messageBuffer[RJ_TEMP_BUFFER_SIZE];
        snprintf(messageBuffer, sizeof(messageBuffer), "bt : %f sec | avgFPS : %f | f : %zu | v : %zu | b : %zu | o : %zu | fs : %s | dr : %s",
                 benchTimer,
                 (float)benchFrameCount / TEST_BENCH_TIME_SECONDS,
                 totalFaceCount,
                 totalVertexCount,
                 totalBatchCount,
                 totalObjectCount,
                 TEST_FULL_SCREEN ? "true" : "false",
                 TEST_DEBUG_RENDERER ? "true" : "false");

        Global_Terminate(0, messageBuffer);
    }
#endif
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    Renderer_Terminate();
#if TEST_DEBUG_RENDERER
    RendererDebug_Terminate();
#endif
    Context_Terminate();
}
