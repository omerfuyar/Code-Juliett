#include "app/App.h"

#include "utilities/Maths.h"

#include "systems/Renderer.h"
#include "systems/Input.h"

#define TEST_WINDOW_SIZE Vector2Int_New(1080, 720)
#define TEST_GRID_X 4
#define TEST_GRID_Y 4
#define TEST_OBJECT_COUNT TEST_GRID_X *TEST_GRID_Y + 1
#define TEST_VSYNC true
#define TEST_FULL_SCREEN false
#define TEST_GRAVITY -MATHS_GRAVITY
#define TEST_DRAG 0.0f
#define TEST_ELASTICITY 1.0f
#define TEST_TIME 10.0f

struct TEST_DATA
{
    RJ_Size count;

    Vector3 positions[TEST_OBJECT_COUNT];
    Vector3 rotations[TEST_OBJECT_COUNT];
    Vector3 scales[TEST_OBJECT_COUNT];

    RendererComponent rendererComponents[TEST_OBJECT_COUNT];
    RendererBatch rendererBatches[TEST_OBJECT_COUNT];

    struct TEST_CAMERA
    {
        Vector3 position;
        Vector3 rotation;
        float size;
        float nearClipPlane;
        float farClipPlane;
        bool isPerspective;

        float speed;
    } camera;
} TED = {0}; // test entity datas

#define tedPosition(entity) TED.positions[entity]
#define tedRotation(entity) TED.rotations[entity]
#define tedScale(entity) TED.scales[entity]

#define tedRendererComponent(entity) TED.rendererComponents[entity]
#define tedRendererBatch(entity) TED.rendererBatches[entity]

float testTimer = 0.0f;
RJ_Size testFrameCount = 0;

typedef RJ_Size TestEntity;

static TestEntity TestEntity_Create(Vector3 position, Vector3 rotation, Vector3 scale, RendererBatch batch)
{
    tedPosition(TED.count) = position;
    tedRotation(TED.count) = rotation;
    tedScale(TED.count) = scale;
    tedRendererComponent(TED.count) = Renderer_ComponentCreate(TED.count, batch);
    tedRendererBatch(TED.count) = batch;

    // RJ_DebugWarning("entity : %u, batch %u, renderable : %u", TED.count, batch, tedRendererComponent(TED.count));

    return TED.count++;
}

ContextWindow *window = NULL;
TestEntity mark;

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    memset(TED.rendererComponents, (int)RJ_INDEX_INVALID, sizeof(RendererComponent) * TEST_OBJECT_COUNT);
    memset(TED.rendererBatches, (int)RJ_INDEX_INVALID, sizeof(RendererBatch) * TEST_OBJECT_COUNT);

    srand((unsigned int)time(NULL));

    window = Context_Initialize();

    Context_Configure(scl("Juliette"), TEST_WINDOW_SIZE, TEST_VSYNC, TEST_FULL_SCREEN, NULL);

    Input_Initialize(window);
    Renderer_Initialize(window, 4);

    Renderer_ConfigureShaders(scl("shaders/vertex.glsl"),
                              scl("shaders/fragment.glsl"));

    TED.camera.position = Vector3_New(0.0f, 0.0f, -10.0f);
    TED.camera.rotation = Vector3_New(0.0f, 90.0f, 0.0f);
    TED.camera.size = 10.0f;
    TED.camera.nearClipPlane = 0.01f;
    TED.camera.farClipPlane = 1000.0f;
    TED.camera.isPerspective = false;
    TED.camera.speed = 0.0025f;

    Renderer_ConfigureCamera(&TED.camera.position,
                             &TED.camera.rotation,
                             &TED.camera.size,
                             &TED.camera.nearClipPlane,
                             &TED.camera.farClipPlane,
                             &TED.camera.isPerspective);

    RendererBatch markBatch = Renderer_BatchCreate(scl("models/Mark.mdl"), NULL, 1, TED.positions, TED.rotations, TED.scales);
    RendererBatch testBatch = Renderer_BatchCreate(scl("models/Test.mdl"), NULL, TEST_OBJECT_COUNT - 1, TED.positions, TED.rotations, TED.scales);

    mark = TestEntity_Create(Vector3_New(0.0f, 0.0f, -1.0f),
                             Vector3_Zero,
                             Vector3_One,
                             markBatch);

    for (RJ_Size y = 0; y < TEST_GRID_Y; y++)
    {
        for (RJ_Size x = 0; x < TEST_GRID_X; x++)
        {
            TestEntity_Create(Vector3_New((-TEST_GRID_X / 2.0f) + (float)x + (y % 2 == 1 ? 0.5f : 0.0f),
                                          (-TEST_GRID_Y / 8.0f) + (float)y / 4.0f,
                                          0),
                              Vector3_Zero,
                              Vector3_One,
                              testBatch);
        }
    }
}

void App_Loop(float deltaTime)
{
    Context_Update();
    Input_Update();

    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!window->fullScreen);
    }

    if (Input_GetKey(InputKeyCode_R, InputState_Down))
    {
        TED.camera.isPerspective = !TED.camera.isPerspective;
    }

    if (Input_GetKey(InputKeyCode_LeftArrow, InputState_Down))
    {
        TED.camera.position.x += 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_RightArrow, InputState_Down))
    {
        TED.camera.position.x -= 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_UpArrow, InputState_Down))
    {
        TED.camera.position.y += 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_DownArrow, InputState_Down))
    {
        TED.camera.position.y -= 1.0f;
    }

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Down | InputState_Pressed))
    {
        Vector2Int temp = Input_GetMousePositionDelta();
        TED.camera.position = Vector3_Sum(TED.camera.position, Vector3_Scale(Vector3_New(temp.x, temp.y, 0.0f), TED.camera.speed * TED.camera.size));
    }

    float newSize = TED.camera.size - Input_GetMouseScroll();
    TED.camera.size = Maths_Clamp(newSize, 0.5f, 20.0f);

    Renderer_Update();
    Vector3 mouseWorldPosition = Renderer_ScreenToWorldSpace(Input_GetMousePosition(), Maths_Abs(TED.camera.position.z));
    tedPosition(mark) = Vector3_New(Maths_Round(mouseWorldPosition.x), Maths_Round(mouseWorldPosition.y), 0.0f);

    Renderer_Render();

    char titleBuffer[RJ_TEMP_BUFFER_SIZE] = {0};
    snprintf(titleBuffer, sizeof(titleBuffer),
             "Juliette | FPS: %6.2f | Frame Time: %6.5f ms | mouseWorldPosition: %6.2f, %6.2f, %6.2f",
             1.0f / deltaTime,
             deltaTime * 1000,
             mouseWorldPosition.x,
             mouseWorldPosition.y,
             mouseWorldPosition.z);
    Context_ConfigureTitle(scl(titleBuffer));

    testFrameCount++;
    testTimer += deltaTime;
    if (testTimer > TEST_TIME)
    {
        RJ_DebugLog(true, "TEST", "Test time: %f, Average FPS: %f", testTimer, (float)testFrameCount / testTimer);
    }
}

void App_Terminate(int exitCode, char *exitMessage)
{
    (void)exitCode;
    (void)exitMessage;

    if (Renderer_IsInitialized())
    {
        Renderer_Terminate();
    }

    if (Context_IsInitialized())
    {
        Context_Terminate();
    }
}
