#include "user/App.h"

#include "utilities/Maths.h"
#include "utilities/Timer.h"

#include "tools/Resources.h"
#include "tools/Renderer.h"
#include "tools/Physics.h"
#include "tools/Input.h"

#define TEST_BENCH_TIME_SECONDS 10.0f
#define TEST_WINDOW_SIZE NewVector2Int(1080, 720)
#define TEST_BATCH_COUNT 32
#define TEST_BATCH_OBJECT_COUNT 32
#define TEST_OBJECT_COUNT (TEST_BATCH_COUNT * TEST_BATCH_OBJECT_COUNT)
#define TEST_DEBUG_RENDERER RJ_BUILD_DEBUG
#define TEST_VSYNC false
#define TEST_FULL_SCREEN true
#define TEST_GRAVITY_M -RJ_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f
#define TEST_OBJECT_SPEED_LIMIT 50
#define TEST_BENCHMARK true
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
// myObjectType objectPlayer = {0};
myObjectType walls[6] = {0};
myObjectType testObjects[TEST_BATCH_COUNT][TEST_BATCH_OBJECT_COUNT] = {0};
char titleBuffer[RJ_TEMP_BUFFER_SIZE];

RendererModel *LoadModel(StringView name, StringView matFileName, StringView mdlFileName)
{
    ResourceText *matFile = ResourceText_Create(matFileName, scl("models" RJ_PATH_DELIMETER_STR));
    ListArray materials = RendererMaterial_CreateFile(scv(matFile->data), matFile->lineCount);
    // ListArray materials = RendererMaterial_CreateFile(scv(matFile->data), matFile->lineCount);
    ResourceText_Destroy(matFile);

    ResourceText *mdlFile = ResourceText_Create(mdlFileName, scl("models" RJ_PATH_DELIMETER_STR));
    RendererModel *model = RendererModel_Create(name, scv(mdlFile->data), mdlFile->lineCount, &materials, NewVector3N(0.0f), NewVector3N(0.0f), NewVector3N(0.5f));
    ResourceText_Destroy(mdlFile);
    ListArray_Destroy(&materials);

    return model;
}

RendererModel *LoadModelWithTexture(StringView name, StringView matFileName, StringView mdlFileName, StringView texFileName)
{
    ResourceText *matFile = ResourceText_Create(matFileName, scl("models" RJ_PATH_DELIMETER_STR));
    ResourceImage *texture = ResourceImage_Create(texFileName, scl("models" RJ_PATH_DELIMETER_STR));
    ListArray materials = RendererMaterial_CreateTexture(scv(matFile->data), matFile->lineCount, texFileName, texture->data, texture->size, texture->channels);
    // ListArray materials = RendererMaterial_CreateFile(scv(matFile->data), matFile->lineCount);
    ResourceText_Destroy(matFile);
    ResourceImage_Destroy(texture);

    ResourceText *mdlFile = ResourceText_Create(mdlFileName, scl("models" RJ_PATH_DELIMETER_STR));
    RendererModel *model = RendererModel_Create(name, scv(mdlFile->data), mdlFile->lineCount, &materials, NewVector3N(0.0f), NewVector3N(0.0f), NewVector3N(1.0f));
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

    // RendererModel *modelPlane = LoadModelWithTexture(scl("Plane"), scl("Plane.mat"), scl("Plane.mdl"), scl("Plane.png"));
    RendererModel *modelPlane = LoadModel(scl("Plane"), scl("Plane.mat"), scl("Plane.mdl"));
    RendererModel *modelPistol = LoadModel(scl("Pistol"), scl("Pistol.mat"), scl("Pistol.mdl"));
    RendererModel *modelHouse = LoadModel(scl("House"), scl("House.mat"), scl("House.mdl"));

    scenePhysics = PhysicsScene_Create(scl("My Physics Scene"), TEST_OBJECT_COUNT + 1, TEST_DRAG, TEST_GRAVITY_M, TEST_ELASTICITY);

    sceneRenderer = RendererScene_Create(scl("My Renderer Scene"), TEST_BATCH_COUNT + 1);

    camera.position = NewVector3(0.0f, 20.0f, 20.0f);
    camera.rotation = NewVector3(-45.0f, -90.0f, 0.0f);
    camera.speed = 10.0f;
    camera.rotationSpeed = 75.0f;
    camera.camera = RendererCameraComponent_Create(&camera.position, &camera.rotation);
    camera.camera->isPerspective = true;
    camera.camera->size = 120.0f;
    camera.camera->nearClipPlane = 0.1f;
    camera.camera->farClipPlane = 1000.0f;

    RendererScene_SetMainCamera(sceneRenderer, camera.camera);

    // RendererBatch *batchPlayer = RendererScene_CreateBatch(sceneRenderer, scl("Player Batch"), modelPlayer, 1);
    RendererBatch *batchPlane = RendererScene_CreateBatch(sceneRenderer, scl("Plane Batch"), modelPlane, 1);

    walls[0].position = NewVector3(0.0f, 10.0f, 0.0f);
    walls[0].rotation = Vector3_Zero;
    walls[0].scale = NewVector3(20.0f, 0.5f, 20.0f);
    walls[0].renderable = RendererBatch_CreateComponent(batchPlane, &walls[0].position, &walls[0].rotation, &walls[0].scale);
    walls[0].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[0].position, walls[0].scale, 0.0f, true);

    walls[1].position = NewVector3(-10.0f, 0.0f, 0.0f);
    walls[1].rotation = Vector3_Zero;
    walls[1].scale = NewVector3(0.5f, 20.0f, 20.0f);
    walls[1].renderable = RendererBatch_CreateComponent(batchPlane, &walls[1].position, &walls[1].rotation, &walls[1].scale);
    walls[1].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[1].position, walls[1].scale, 0.0f, true);

    walls[2].position = NewVector3(10.0f, 0.0f, 0.0f);
    walls[2].rotation = Vector3_Zero;
    walls[2].scale = NewVector3(0.5f, 20.0f, 20.0f);
    walls[2].renderable = RendererBatch_CreateComponent(batchPlane, &walls[2].position, &walls[2].rotation, &walls[2].scale);
    walls[2].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[2].position, walls[2].scale, 0.0f, true);

    walls[3].position = NewVector3(0.0f, 0.0f, -10.0f);
    walls[3].rotation = Vector3_Zero;
    walls[3].scale = NewVector3(20.0f, 20.0f, 0.5f);
    walls[3].renderable = RendererBatch_CreateComponent(batchPlane, &walls[3].position, &walls[3].rotation, &walls[3].scale);
    walls[3].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[3].position, walls[3].scale, 0.0f, true);

    walls[4].position = NewVector3(0.0f, 0.0f, 10.0f);
    walls[4].rotation = Vector3_Zero;
    walls[4].scale = NewVector3(20.0f, 20.0f, 0.5f);
    walls[4].renderable = RendererBatch_CreateComponent(batchPlane, &walls[4].position, &walls[4].rotation, &walls[4].scale);
    walls[4].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[4].position, walls[4].scale, 0.0f, true);

    walls[5].position = NewVector3(0.0f, -10.0f, 0.0f);
    walls[5].rotation = Vector3_Zero;
    walls[5].scale = NewVector3(20.0f, 0.5f, 20.0f);
    walls[5].renderable = RendererBatch_CreateComponent(batchPlane, &walls[5].position, &walls[5].rotation, &walls[5].scale);
    walls[5].physics = PhysicsScene_CreateComponent(scenePhysics, &walls[5].position, walls[5].scale, 0.0f, true);

#if TEST_BENCHMARK
    totalBatchCount++;
    totalObjectCount++;
    totalFaceCount += modelPlane->meshes.count;
    totalVertexCount += modelPlane->vertices.count;
#endif

    for (size_t i = 0; i < TEST_BATCH_COUNT; i++)
    {
        RendererModel *modelToUse = (i % 2 == 0) ? modelPistol : modelHouse;
        RendererBatch *batchTest = RendererScene_CreateBatch(sceneRenderer, scl("Test Batch"), modelToUse, TEST_BATCH_OBJECT_COUNT);

#if TEST_BENCHMARK
        totalBatchCount++;
        totalObjectCount += TEST_BATCH_OBJECT_COUNT;
        totalVertexCount += modelToUse->vertices.count * TEST_BATCH_OBJECT_COUNT;

        for (size_t f = 0; f < modelToUse->meshes.count; f++)
        {
            RendererMesh *mesh = *(RendererMesh **)ListArray_Get(&modelToUse->meshes, f);
            totalFaceCount += mesh->indices.count / 3 * TEST_BATCH_OBJECT_COUNT;
        }
#endif

        for (size_t j = 0; j < TEST_BATCH_OBJECT_COUNT; j++)
        {
            testObjects[i][j].position = NewVector3((float)RandomRange(-10, 10), (float)RandomRange(-10, 10), (float)RandomRange(-10, 10));
            testObjects[i][j].rotation = Vector3_Zero;
            testObjects[i][j].scale = Vector3_One;
            testObjects[i][j].renderable = RendererBatch_CreateComponent(batchTest, &testObjects[i][j].position, &testObjects[i][j].rotation, &testObjects[i][j].scale);
            testObjects[i][j].physics = PhysicsScene_CreateComponent(scenePhysics, &testObjects[i][j].position, testObjects[i][j].scale, 1.0f, false);
            testObjects[i][j].physics->velocity = NewVector3((float)RandomRange(-TEST_OBJECT_SPEED_LIMIT, TEST_OBJECT_SPEED_LIMIT), (float)RandomRange(-TEST_OBJECT_SPEED_LIMIT, TEST_OBJECT_SPEED_LIMIT), (float)RandomRange(-TEST_OBJECT_SPEED_LIMIT, TEST_OBJECT_SPEED_LIMIT));
        }
    }

#if TEST_MONITOR
    timer = Timer_Create("main timer");
#endif

    // objectPlayer.position = Vector3_Zero;
    // objectPlayer.rotation = Vector3_Zero;
    // objectPlayer.scale = Vector3_One;
    // objectPlayer.renderable = RendererBatch_CreateComponent(batchPlayer, &objectPlayer.position, &objectPlayer.rotation, &objectPlayer.scale);
    // objectPlayer.physics = PhysicsScene_CreateComponent(scenePhysics, &objectPlayer.position, objectPlayer.scale, 1.0f, false);
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
    for (size_t i = 0; i < TEST_BATCH_COUNT; i++)
    {
        for (size_t j = 0; j < TEST_BATCH_OBJECT_COUNT; j++)
        {
            testObjects[i][j].rotation.y += deltaTime;
        }
    }
#if TEST_MONITOR
    Timer_Stop(&timer);
    DebugLog(false, "MONITOR", "rotating : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

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

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    PhysicsScene_UpdateComponents(scenePhysics, deltaTime);
#if TEST_MONITOR
    Timer_Stop(&timer);
    DebugLog(false, "MONITOR", "physics update : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

    // access collision data if needed by Physics_IsColliding(...)

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    PhysicsScene_ResolveCollisions(scenePhysics);
#if TEST_MONITOR
    Timer_Stop(&timer);
    DebugLog(false, "MONITOR", "physics resolve : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

#if TEST_MONITOR
    Timer_Start(&timer);
#endif
    RendererScene_Update(sceneRenderer);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(sceneRenderer);

#if TEST_DEBUG_RENDERER
    RendererDebug_StartRendering();

    for (size_t i = 0; i < scenePhysics->components.count; i++)
    {
        PhysicsComponent *physComp = (PhysicsComponent *)ListArray_Get(&scenePhysics->components, i);
        RendererDebug_DrawBoxLines(*physComp->positionReference, physComp->colliderSize, NewColor((float)(i % 2), (float)((i / 2) % 2), (float)((i / 4) % 2), 1.0f));
    }

    RendererDebug_FinishRendering(&camera.camera->projectionMatrix, &camera.camera->viewMatrix);
#endif

    Renderer_FinishRendering();
#if TEST_MONITOR
    Timer_Stop(&timer);
    DebugLog(false, "MONITOR", "rendering : %f ms", Timer_GetElapsedMilliseconds(&timer));
#endif

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));

#if TEST_BENCHMARK
    benchTimer += deltaTime;
    benchFrameCount++;

    if (benchTimer >= TEST_BENCH_TIME_SECONDS)
    {
        char messageBuffer[RJ_TEMP_BUFFER_SIZE];
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
