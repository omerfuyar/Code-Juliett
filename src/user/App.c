#include "user/App.h"

#include "utilities/Maths.h"

#include "tools/Resources.h"
#include "tools/Renderer.h"
#include "tools/Input.h"

#define TEST_BENCH_TIME_SECONDS 10.0f
#define TEST_WINDOW_SIZE NewVector2Int(1080, 720)
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_BENCHMARK false

typedef struct myObjectType
{
    String name;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    RendererComponent *renderable;
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

ContextWindow *mainWindow = NULL;
RendererScene *myRendererScene = NULL;
myCameraType mainCamera = {0};
myObjectType myObj = {0};

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mainWindow = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(mainWindow);
    Renderer_Initialize(mainWindow, 1);

    ResourceText *vertexShaderResource = ResourceText_Create(scl("vertex.glsl"), scl("shaders" PATH_DELIMETER_STR));
    ResourceText *fragmentShaderResource = ResourceText_Create(scl("fragment.glsl"), scl("shaders" PATH_DELIMETER_STR));
    Renderer_ConfigureShaders(scv(vertexShaderResource->data), scv(fragmentShaderResource->data));
    ResourceText_Destroy(vertexShaderResource);
    ResourceText_Destroy(fragmentShaderResource);

    ResourceText *pistolMatFile = ResourceText_Create(scl("Test.mat"), scl("models" PATH_DELIMETER_STR));
    ListArray pistolMaterials = RendererMaterial_CreateFile(scv(pistolMatFile->data), pistolMatFile->lineCount);
    ResourceText_Destroy(pistolMatFile);

    ResourceText *mdlFile = ResourceText_Create(scl("Test.mdl"), scl("models" PATH_DELIMETER_STR));
    RendererModel *mdlPistol = RendererModel_Create(scl("Test"), scv(mdlFile->data), mdlFile->lineCount, &pistolMaterials, NewVector3N(0.0f), NewVector3N(0.0f), NewVector3N(1.0f));
    ResourceText_Destroy(mdlFile);
    ListArray_Destroy(&pistolMaterials);

    myRendererScene = RendererScene_Create(scl("My Scene"), 1);
    RendererBatch *btcPistol = RendererScene_CreateBatch(myRendererScene, scl("Object Batch"), mdlPistol, 1);

    mainCamera.name = scc(scl("Main Camera"));
    mainCamera.position = Vector3_Zero;
    mainCamera.rotation = Vector3_Zero;
    mainCamera.camera = RendererCameraComponent_Create(&mainCamera.position, &mainCamera.rotation);
    mainCamera.speed = 10.0f;
    mainCamera.rotationSpeed = 75.0f;
    RendererCameraComponent_Configure(mainCamera.camera, true, 90.0f, 0.1f, 1000.0f);
    RendererScene_SetMainCamera(myRendererScene, mainCamera.camera);

    myObj.name = scc(scl("Gun"));
    myObj.position = NewVector3N(0.0f);
    myObj.rotation = NewVector3N(0.0f);
    myObj.scale = NewVector3N(1.0f);
    myObj.renderable = RendererBatch_CreateComponent(btcPistol, &myObj.position, &myObj.rotation, &myObj.scale);
}

void App_Loop(float deltaTime)
{
    Input_Update();
    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!mainWindow->fullScreen);
    }

    mainCamera.camera->size -= Input_GetMouseScroll();

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Pressed))
    {
        Input_ConfigureMouseMode(InputMouseMode_Captured);

        Vector2Int mousePositionDelta = Input_GetMousePositionDelta();
        Vector3 movementVector = Input_GetMovementVector();

        mainCamera.rotation.y += (float)mousePositionDelta.x * mainCamera.rotationSpeed * deltaTime;
        mainCamera.rotation.x -= (float)mousePositionDelta.y * mainCamera.rotationSpeed * deltaTime;
        mainCamera.rotation.x = Clamp(mainCamera.rotation.x, -89.0f, 89.0f);

        Vector3 direction = Vector3_Normalized(NewVector3(
            Cos(mainCamera.rotation.x) * Cos(mainCamera.rotation.y),
            Sin(mainCamera.rotation.x),
            Cos(mainCamera.rotation.x) * Sin(mainCamera.rotation.y)));

        Vector3 right = Vector3_Normalized(Vector3_Cross(direction, Vector3_Up));

        Vector3 move = Vector3_Scale(direction, movementVector.y);
        move = Vector3_Add(move, Vector3_Scale(right, movementVector.x));
        move.y += movementVector.z;

        if (Vector3_Magnitude(move) > 0.0f)
        {
            move = Vector3_Normalized(move);
            mainCamera.position = Vector3_Add(mainCamera.position, Vector3_Scale(move, mainCamera.speed * deltaTime));
        }
    }
    // else
    //{
    //     Input_ConfigureMouseMode(InputMouseMode_Normal);
    //
    //    Vector3 movementVector = Input_GetMovementVector();
    //
    //    myObj.position.x += movementVector.x * deltaTime * mainCamera.speed;
    //    myObj.position.z -= movementVector.y * deltaTime * mainCamera.speed;
    //}

    // PhysicsScene_UpdateComponents(myPhysicsScene, DT);
    // PhysicsScene_ResolveCollisions(myPhysicsScene);

    RendererScene_Update(myRendererScene);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(myRendererScene);

    // RendererDebug_StartRendering();

    // RendererDebug_FinishRendering(&mainCamera.camera->projectionMatrix, &mainCamera.camera->viewMatrix);
    Renderer_FinishRendering();

    char titleBuffer[TEMP_BUFFER_SIZE];
    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    Renderer_Terminate();
    // RendererDebug_Terminate();
    Context_Terminate();
}
