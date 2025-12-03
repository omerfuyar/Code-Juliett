#include "user/App.h"

#include "utilities/Maths.h"
#include "utilities/Timer.h"

#include "tools/Resource.h"

#include "systems/Renderer.h"
#include "systems/Physics.h"
#include "systems/Input.h"
#include "systems/Audio.h"

#define TEST_WINDOW_SIZE Vector2Int_New(1080, 720)
#define TEST_OBJECT_COUNT 16
#define TEST_VSYNC false
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY_M -MATHS_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f
#define TEST_OBJECT_SPEED_LIMIT 50

typedef struct myObjectType
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    RendererComponent *renderable;
    PhysicsComponent *physics;
    AudioComponent *audio;
} myObjectType;

typedef struct myCameraType
{
    String name;
    Vector3 position;
    Vector3 rotation;
    RendererCameraComponent *camera;
    AudioListenerComponent *listener;
    float rotationSpeed;
    float speed;
} myCameraType;

ContextWindow *window = NULL;
RendererScene *sceneRenderer = NULL;
PhysicsScene *scenePhysics = NULL;
AudioScene *sceneAudio = NULL;
myCameraType camera = {0};
myObjectType testObjects[TEST_OBJECT_COUNT] = {0};
char titleBuffer[RJGLOBAL_TEMP_BUFFER_SIZE] = {0};
float timer = 0.0f;

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);
    Audio_Initialize(1);

    Renderer_ConfigureShaders(scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "vertex.glsl"),
                              scl("shaders" RJGLOBAL_PATH_DELIMETER_STR "fragment.glsl"));

    ListArray materialPool = RendererMaterial_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.mat"));
    ListArray modelPool = RendererModel_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.mdl"), &materialPool);
    ListArray_Destroy(&materialPool);

    sceneRenderer = RendererScene_CreateFromFile(scl("models" RJGLOBAL_PATH_DELIMETER_STR "Test.scn"), &modelPool, (void *)testObjects, 0, sizeof(myObjectType), TEST_OBJECT_COUNT);
    ListArray_Destroy(&modelPool);

    sceneAudio = AudioScene_Create(scl("TestAudioScene"), 5);
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
    // camera.listener = AudioScene_CreateListenerComponent(sceneAudio, &camera.position, &camera.rotation);

    RendererScene_SetMainCamera(sceneRenderer, camera.camera);

    testObjects[0].audio = AudioScene_CreateComponent(sceneAudio, scl("sounds" RJGLOBAL_PATH_DELIMETER_STR "Test.mp3"), &testObjects[0].position);
    AudioComponent_Play(testObjects[0].audio);
    AudioComponent_Rewind(testObjects[0].audio, 0.5f);
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

    testObjects[0].rotation.y += deltaTime;

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

    timer += deltaTime;
    if (timer > 3.0f)
    {
        AudioComponent_Pause(testObjects[0].audio);
    }

    // PhysicsScene_UpdateComponents(scenePhysics, deltaTime);

    // access collision data if needed by Physics_IsColliding(...)

    // PhysicsScene_ResolveCollisions(scenePhysics);

    RendererScene_Update(sceneRenderer);

    // rendering
    Renderer_StartRendering();
    Renderer_RenderScene(sceneRenderer);

    Renderer_FinishRendering();

    snprintf(titleBuffer, sizeof(titleBuffer), "%s | FPS: %f | Frame Time: %f ms", "Juliette", 1.0f / deltaTime, deltaTime * 1000);
    Context_ConfigureTitle(scl(titleBuffer));
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

    if (sceneAudio != NULL)
    {
        AudioScene_Destroy(sceneAudio);
    }

    if (window != NULL)
    {
        Context_Terminate();
    }
}
