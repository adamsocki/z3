//
// Created by socki on 3/2/2025.
//

#include <iostream>
#include "InputManager.h"
#include "Engine.h"
#include "imgui_impl_glfw.h"

//#include "../external/imgui/backends/imgui_impl_glfw.h"

namespace Zayn {

    void AllocateInputManager(InputManager* inputManager, MemoryArena* arena, int32 deviceCapacity)
    {
        inputManager->events = MakeDynamicArray<InputEvent>(arena, deviceCapacity);

        inputManager->deviceCount = deviceCapacity;
        inputManager->devices = (InputDevice*)malloc(sizeof(InputDevice) * deviceCapacity);
        memset(inputManager->devices, 0, inputManager->deviceCount * sizeof(InputDevice));

    }

    void AllocateInputDevice(InputDevice* device, InputDeviceType type, int32 discreteCount, int32 analogueCount)
    {
        device->type = type;
        device->discreteCount = discreteCount;

        device->framesHeld = (int32 *)malloc(sizeof(int32) * discreteCount);
        device->released = (bool *)malloc(sizeof(bool) * discreteCount);
        device->pressed = (bool *)malloc(sizeof(bool) * discreteCount);
        device->timePressed = (real32 *)malloc(sizeof(real32) * discreteCount);

        device->analogueCount = analogueCount;
        device->prevAnalogue = (real32 *)malloc(sizeof(real32) * analogueCount);
        device->analogue = (real32 *)malloc(sizeof(real32) * analogueCount);

        memset(device->framesHeld, -1, sizeof(int32) * discreteCount);
        memset(device->pressed, 0, sizeof(bool) * discreteCount);
        memset(device->released, 0, sizeof(bool) * discreteCount);
        memset(device->timePressed, 0, sizeof(real32) * discreteCount);

        memset(device->analogue, 0, sizeof(real32) * analogueCount);
        memset(device->prevAnalogue, 0, sizeof(real32) * analogueCount);
    }

    InputKeyboardDiscrete glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LAST + 1];


    void InitializeKeyMap()
    {
        for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
            Zayn::glfwKeyToInputKeyboardDiscrete[i] = Input_KeyboardDiscreteCount;
        }

        // Map GLFW keys to InputKeyboardDiscrete enum values
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_A] = Input_A;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_B] = Input_B;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_C] = Input_C;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_D] = Input_D;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_E] = Input_E;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F] = Input_F;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_G] = Input_G;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_H] = Input_H;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_I] = Input_I;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_J] = Input_J;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_K] = Input_K;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_L] = Input_L;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_M] = Input_M;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_N] = Input_N;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_O] = Input_O;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_P] = Input_P;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Q] = Input_Q;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_R] = Input_R;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_S] = Input_S;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_T] = Input_T;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_U] = Input_U;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_V] = Input_V;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_W] = Input_W;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_X] = Input_X;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Y] = Input_Y;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Z] = Input_Z;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_0] = Input_0;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_1] = Input_1;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_2] = Input_2;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_3] = Input_3;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_4] = Input_4;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_5] = Input_5;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_6] = Input_6;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_7] = Input_7;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_8] = Input_8;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_9] = Input_9;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_EQUAL] = Input_Equal;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_MINUS] = Input_Minus;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_BRACKET] = Input_RightBracket;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_BRACKET] = Input_LeftBracket;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_APOSTROPHE] = Input_Quote;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SEMICOLON] = Input_Semicolon;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSLASH] = Input_Backslash;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_COMMA] = Input_Comma;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SLASH] = Input_Slash;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PERIOD] = Input_Period;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ENTER] = Input_Return;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_TAB] = Input_Tab;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SPACE] = Input_Space;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSPACE] = Input_Backspace;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ESCAPE] = Input_Escape;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_GRAVE_ACCENT] = Input_Tick;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SUPER] = Input_Win;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SHIFT] = Input_Shift;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_CAPS_LOCK] = Input_CapsLock;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_ALT] = Input_Alt;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_CONTROL] = Input_Control;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SUPER] = Input_RightWin;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SHIFT] = Input_RightShift;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_ALT] = Input_RightAlt;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_CONTROL] = Input_RightControl;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F1] = Input_F1;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F2] = Input_F2;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F3] = Input_F3;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F4] = Input_F4;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F5] = Input_F5;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F6] = Input_F6;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F7] = Input_F7;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F8] = Input_F8;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F9] = Input_F9;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F10] = Input_F10;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F11] = Input_F11;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F12] = Input_F12;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F13] = Input_F13;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F14] = Input_F14;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F15] = Input_F15;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F16] = Input_F16;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F17] = Input_F17;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F18] = Input_F18;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F19] = Input_F19;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F20] = Input_F20;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F21] = Input_F21;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F22] = Input_F22;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F23] = Input_F23;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F24] = Input_F24;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_HOME] = Input_Home;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_INSERT] = Input_Insert;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_UP] = Input_PageUp;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DELETE] = Input_ForwardDelete;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_END] = Input_End;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_DOWN] = Input_PageDown;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT] = Input_LeftArrow;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT] = Input_RightArrow;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DOWN] = Input_DownArrow;
        Zayn::glfwKeyToInputKeyboardDiscrete[GLFW_KEY_UP] = Input_UpArrow;
    }

    enum InputMouseDiscrete : uint32 {
        Input_MouseLeft,
        Input_MouseRight,
        Input_MouseMiddle,

        Input_MouseDiscreteCount,
    };




    void InitInputManager(Engine* engine)
    {
        InputManager* inputManager = &engine->inputManager;
        MemoryArena* permanentMemory = &engine->permanentMemory;

        std::cout << "initInputManager: Done" << '\n';

        AllocateInputManager(inputManager, permanentMemory, 5);
        // inputManager->windowReference = *windowReference;/**/

        inputManager->keyboard = &inputManager->devices[0];
        inputManager->mouse = &inputManager->devices[1];
        AllocateInputDevice(inputManager->keyboard, InputDeviceType_Keyboard, Input_KeyboardDiscreteCount, 0);
        AllocateInputDevice(inputManager->mouse, InputDeviceType_Mouse, Input_MouseDiscreteCount, Input_MouseAnalogueCount);

        InitializeKeyMap();
    }


    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Zayn::InputManager* inputManager = static_cast<Zayn::InputManager*>(glfwGetWindowUserPointer(window));

        Zayn::InputKeyboardDiscrete inputKey = Zayn::glfwKeyToInputKeyboardDiscrete[key];
        if (action == GLFW_PRESS)
        {
            Zayn::InputEvent event = {};
            event.device = inputManager->keyboard;
            event.index = inputKey;
//            if (key == GLFW_KEY_A)
//            {
//                // std::cout << "A Is for Asshole: " << key << std::endl;
//
//            }
            event.discreteValue = true;

            PushBack(&inputManager->events, event);

            // std::cout << "Key pressed: " << key << std::endl;
        }
        else if (action == GLFW_RELEASE)
        {
            Zayn::InputEvent event = {};
            event.device = inputManager->keyboard;
            event.index = inputKey;
            event.discreteValue = false;
            std::cout << "Key released: " << key << std::endl;

            PushBack(&inputManager->events, event);

        }

#if IMGUI
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {


#if IMGUI
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Zayn::InputManager* inputManager = static_cast<Zayn::InputManager*>(glfwGetWindowUserPointer(window));
        inputManager->lastMousePosWindow = inputManager->nowMousePosWindow;
        inputManager->mousePosUpdated = true;
        inputManager->nowMousePosWindow.x = xpos;
        inputManager->nowMousePosWindow.y = ypos;

        inputManager->mousePos.x = (int32)xpos;
        inputManager->mousePos.y = (int32)ypos;

        real32 xOffset = (real32)(xpos - inputManager->lastMousePosWindow.x);
        real32 yOffset = (real32)(ypos - inputManager->lastMousePosWindow.y);

        inputManager->mouse->analogue[Input_MousePositionX] = (real32)xpos;
        inputManager->mouse->analogue[Input_MousePositionY] = (real32)ypos;
        inputManager->mouse->analogue[Input_MousePositionXOffset] = xOffset;
        inputManager->mouse->analogue[Input_MousePositionYOffset] = yOffset;


        InputEvent eventX = {};
        eventX.device = inputManager->mouse;
        eventX.index = Input_MousePositionX;
        eventX.value = (real32)xpos;

        InputEvent eventY = {};
        eventY.device = inputManager->mouse;
        eventY.index = Input_MousePositionY;
        eventY.value = (real32)ypos;

        InputEvent eventXOffset = {};
        eventXOffset.device = inputManager->mouse;
        eventXOffset.index = Input_MousePositionXOffset;
        eventXOffset.value = (real32)(xpos - inputManager->lastMousePosWindow.x);

        InputEvent eventYOffset = {};
        eventYOffset.device = inputManager->mouse;
        eventYOffset.index = Input_MousePositionYOffset;
        eventYOffset.value = (real32)(ypos - inputManager->lastMousePosWindow.y);

        PushBack(&inputManager->events, eventX);
        PushBack(&inputManager->events, eventY);
        PushBack(&inputManager->events, eventXOffset);
        PushBack(&inputManager->events, eventYOffset);


//        std::cout << "X Pos: " << xpos << std::endl;
//        std::cout << "Y Pos: " << ypos << std::endl;


        // std::cout << "Key pressed: " << xpos << std::endl;
#if IMGUI
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif

    }




    void UpdateInputManager(Engine* engine)
    {

        InputManager* inputManager = &engine->inputManager;
        WindowManager* windowManager = &engine->windowManager;
        TimeManager* timeManager = &engine->timeManager;

        glfwPollEvents();


        glfwSetKeyCallback(windowManager->glfwWindow, keyCallback);
        glfwSetMouseButtonCallback(windowManager->glfwWindow, mouseButtonCallback);
        glfwSetCursorPosCallback(windowManager->glfwWindow, cursorPosCallback);

        for (int d = 0; d < inputManager->deviceCount; d++)
        {
            InputDevice* device = &inputManager->devices[d];

            if (device->type == InputDeviceType_Invalid) { continue; }

            for (int i = 0; i < device->discreteCount; i++)
            {
                device->released[i] = false;
                //std::cout << "device->discretecount: " << i << std::endl;

                if (device->framesHeld[i] >= 0)
                {
                    //std::cout << "device->discretecount: " << i << std::endl;
                    //std::cout << "device->framesHeld[i]++: " << device->framesHeld[i] << std::endl;
                    device->framesHeld[i]++;
                    device->pressed[i] = false;
                }
            }
        }

        for (int i = 0; i < inputManager->events.count; i++)
        {
            // std::cout << "inputManager->events.count: " << i << std::endl;

            InputEvent event = inputManager->events[i];
            int32 index = event.index;

            InputDevice* device = event.device;

            if (!event.discreteValue)
            {
                if (device->framesHeld[index] > 0)
                {
                    device->released[index] = true;
                }
                device->timePressed[index] = -1;
                device->framesHeld[index] = -1;
                device->pressed[index] = false;
            }
            else
            {
                if (device->framesHeld[index] < 0)
                {
                    printf("pressed\n");
                    std::cout << "device->framesHeld" << std::endl;

                    device->timePressed[index] = timeManager->time.totalTime;
                    device->framesHeld[index] = 0;
                    device->pressed[index] = true;
                    device->released[index] = false;
                }
                else
                {
                    // @NOTE: we shouldnt even get a pressed event when the key is being held
                }
            }
        }

        if (inputManager->mousePosUpdated)
        {
//            UpdateMouseInputs();

            int width, height;
            glfwGetWindowSize(windowManager->glfwWindow, &width, &height);
            inputManager->mousePosNorm.x = inputManager->nowMousePosWindow.x / (real32)width;
            inputManager->mousePosNorm.y = inputManager->nowMousePosWindow.y / (real32)height;

            inputManager->mousePosNormSigned.x = (inputManager->mousePosNorm.x * 2.0f) - 1.0f;
            inputManager->mousePosNormSigned.y = (inputManager->mousePosNorm.y * 2.0f) - 1.0f;

            inputManager->mouse->analogue[Input_MousePositionXNorm] = inputManager->mousePosNorm.x;
            inputManager->mouse->analogue[Input_MousePositionYNorm] = inputManager->mousePosNorm.y;

            inputManager->mouse->analogue[Input_MousePositionX] = inputManager->nowMousePosWindow.x;
            inputManager->mouse->analogue[Input_MousePositionY] = inputManager->nowMousePosWindow.y;

            inputManager->mousePosUpdated = false;

            inputManager->lastMousePosWindow = inputManager->nowMousePosWindow;
        }
    }

    void ClearInputManager(Engine* engine) {
        InputManager* inputManager = &engine->inputManager;

        DynamicArrayClear(&inputManager->events);

        inputManager->mouse->analogue[Input_MousePositionXOffset] = 0.0f;
        inputManager->mouse->analogue[Input_MousePositionYOffset] = 0.0f;


        inputManager->charCount = 0;
        memset(inputManager->inputChars, 0, inputManager->charSize);
    }

    bool InputHeld(InputDevice* device, int32 inputID)
    {
        return device->framesHeld[inputID] > 0;
    }

    vec2 GetMousePosition(InputManager* inputManager) {
        return inputManager->nowMousePosWindow;
    }

    vec2 GetMouseDelta(InputManager* inputManager) {
        vec2 delta = {};
        delta.x = inputManager->mouse->analogue[Input_MousePositionXOffset];
        delta.y = inputManager->mouse->analogue[Input_MousePositionYOffset];
        return delta;
    }

    vec2 GetMousePositionNormalized(InputManager* inputManager) {
        return inputManager->mousePosNorm;
    }

    vec2 GetMousePositionNormalizedSigned(InputManager* inputManager) {
        return inputManager->mousePosNormSigned;
    }


} // Zayn