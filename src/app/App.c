#include "app/App.h"

#include "utilities/Maths.h"

#include "tools/Entity.h"

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

    struct TEST_CAMERA
    {
        RendererCamera cam;
        float speed;
    } camera;

    Entity mark;
} TEST = {0};

float testTimer = 0.0f;
RJ_Size testFrameCount = 0;

ContextWindow window = {0};

void App_Setup(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    window.title = scc(scl("Juliett"));
    window.size = TEST_WINDOW_SIZE;
    window.vSync = TEST_VSYNC;
    window.fullScreen = TEST_FULL_SCREEN;

    RJ_Result result = Context_Initialize(&window);
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to initialize context");
    }

    // todo merge this two optionally. Entity will call context internally

    result = Entity_Initialize(TEST_OBJECT_COUNT);
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to initialize entity system");
    }

    Input_Initialize(&window);

    result = Renderer_Initialize(&window, 4);
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to initialize renderer");
    }

    result = Renderer_ConfigureShaders(scl("shaders/vertex.glsl"), scl("shaders/fragment.glsl"));
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to configure shaders");
    }

    TEST.camera.cam.position = Vector3_New(0.0f, 0.0f, -10.0f);
    TEST.camera.cam.rotation = Vector3_New(0.0f, 90.0f, 0.0f);
    TEST.camera.cam.size = 10.0f;
    TEST.camera.cam.nearClipPlane = 0.01f;
    TEST.camera.cam.farClipPlane = 1000.0f;
    TEST.camera.cam.isPerspective = false;
    TEST.camera.speed = 0.0025f;

    Renderer_SetCamera(&TEST.camera.cam);

    RendererBatch markBatch = 0;
    result = Renderer_BatchCreate(&markBatch, scl("models/Mark.mdl"), NULL, 1);
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to create mark batch");
    }

    RendererBatch testBatch = 0;
    result = Renderer_BatchCreate(&testBatch, scl("models/Test.mdl"), NULL, TEST_OBJECT_COUNT - 1);
    if (result != RJ_OK)
    {
        RJ_DebugError(result, "Failed to create test batch");
    }

    TEST.mark = Entity_Create(Vector3_New(0.0f, 0.0f, -1.0f),
                              Vector3_Zero,
                              Vector3_One);

    Renderer_ComponentCreate(markBatch, TEST.mark);

    for (RJ_Size y = 0; y < TEST_GRID_Y; y++)
    {
        for (RJ_Size x = 0; x < TEST_GRID_X; x++)
        {
            Entity newNTT = Entity_Create(Vector3_New((-TEST_GRID_X / 2.0f) + (float)x + (y % 2 == 1 ? 0.5f : 0.0f),
                                                      (-TEST_GRID_Y / 8.0f) + (float)y / 4.0f,
                                                      0),
                                          Vector3_Zero,
                                          Vector3_One);

            Renderer_ComponentCreate(testBatch, newNTT);
        }
    }
}

void App_Loop(float deltaTime)
{
    if (!Context_Update())
    {
        RJ_DebugInfo("Main window close input received");
        RJ_Terminate(EXIT_SUCCESS, "Main window close input received");
    }

    Input_Update();

    if (Input_GetKey(InputKeyCode_F, InputState_Down))
    {
        Context_ConfigureFullScreen(!window.fullScreen);
    }

    if (Input_GetKey(InputKeyCode_R, InputState_Down))
    {
        TEST.camera.cam.isPerspective = !TEST.camera.cam.isPerspective;
    }

    if (Input_GetKey(InputKeyCode_LeftArrow, InputState_Down))
    {
        TEST.camera.cam.position.x += 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_RightArrow, InputState_Down))
    {
        TEST.camera.cam.position.x -= 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_UpArrow, InputState_Down))
    {
        TEST.camera.cam.position.y += 1.0f;
    }
    else if (Input_GetKey(InputKeyCode_DownArrow, InputState_Down))
    {
        TEST.camera.cam.position.y -= 1.0f;
    }

    if (Input_GetMouseButton(InputMouseButtonCode_Left, InputState_Down | InputState_Pressed))
    {
        Vector2Int temp = Input_GetMousePositionDelta();
        TEST.camera.cam.position = Vector3_Sum(TEST.camera.cam.position, Vector3_Scale(Vector3_New(temp.x, temp.y, 0.0f), TEST.camera.speed * TEST.camera.cam.size));
    }

    float newSize = TEST.camera.cam.size - Input_GetMouseScroll();
    TEST.camera.cam.size = Maths_Clamp(newSize, 0.5f, 20.0f);

    Renderer_Update();
    Vector3 mouseWorldPosition = Renderer_ScreenToWorldSpace(Input_GetMousePosition(), Maths_Abs(TEST.camera.cam.position.z));
    Entity_SetPosition(TEST.mark, Vector3_New(Maths_Round(mouseWorldPosition.x), Maths_Round(mouseWorldPosition.y), 0.0f));

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
