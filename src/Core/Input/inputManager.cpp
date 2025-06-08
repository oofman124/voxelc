#include "InputManager.h"
#include <algorithm>
#include <iostream>
GLFWwindow* InputManager::s_window = nullptr;
std::unordered_map<int, bool> InputManager::s_keysDown;
std::unordered_map<int, bool> InputManager::s_keysPressed;
std::unordered_map<int, bool> InputManager::s_keysReleased;
std::unordered_map<int, bool> InputManager::s_mouseButtonsDown;
std::unordered_map<int, bool> InputManager::s_mouseButtonsPressed;
std::unordered_map<int, bool> InputManager::s_mouseButtonsReleased;
glm::vec2 InputManager::s_mousePosition = glm::vec2(0.0f);
glm::vec2 InputManager::s_lastMousePosition = glm::vec2(0.0f);
glm::vec2 InputManager::s_mouseDelta = glm::vec2(0.0f);
glm::vec2 InputManager::s_scrollOffset = glm::vec2(0.0f);
bool InputManager::s_mouseLocked = false;
bool InputManager::s_isInitialized = false;


std::vector<InputManager::CallbackHandle<InputManager::ScrollCallback>> InputManager::s_scrollCallbacks;
std::vector<InputManager::CallbackHandle<InputManager::KeyCallback>> InputManager::s_keyPressedCallbacks;
std::vector<InputManager::CallbackHandle<InputManager::KeyCallback>> InputManager::s_keyReleasedCallbacks;
std::vector<InputManager::CallbackHandle<InputManager::MouseButtonCallback>> InputManager::s_mouseButtonPressedCallbacks;
std::vector<InputManager::CallbackHandle<InputManager::MouseButtonCallback>> InputManager::s_mouseButtonReleasedCallbacks;
std::vector<InputManager::CallbackHandle<InputManager::CursorPosCallback>> InputManager::s_cursorPosCallbacks;

void InputManager::initialize(GLFWwindow* window) {
    s_window = window;
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    s_isInitialized = true;
}
bool InputManager::isInitialized() {
    return s_isInitialized;
}

void InputManager::pollEvents() {
    // Call key pressed callbacks
    for (const auto& [key, pressed] : s_keysPressed) {
        if (pressed) {
            for (const auto& cbHandle : s_keyPressedCallbacks)
                if (cbHandle && *cbHandle) (*cbHandle)(key);
        }
    }
    for (const auto& [key, released] : s_keysReleased) {
        if (released) {
            for (const auto& cbHandle : s_keyReleasedCallbacks)
                if (cbHandle && *cbHandle) (*cbHandle)(key);
        }
    }
    for (const auto& [button, pressed] : s_mouseButtonsPressed) {
        if (pressed) {
            for (const auto& cbHandle : s_mouseButtonPressedCallbacks)
                if (cbHandle && *cbHandle) (*cbHandle)(button);
        }
    }
    for (const auto& [button, released] : s_mouseButtonsReleased) {
        if (released) {
            for (const auto& cbHandle : s_mouseButtonReleasedCallbacks)
                if (cbHandle && *cbHandle) (*cbHandle)(button);
        }
    }

    // Reset pressed/released states and scroll offset each frame
    s_keysPressed.clear();
    s_keysReleased.clear();
    s_mouseButtonsPressed.clear();
    s_mouseButtonsReleased.clear();
    s_scrollOffset = glm::vec2(0.0f);

    s_mouseDelta = s_mousePosition - s_lastMousePosition;
    s_lastMousePosition = s_mousePosition;
}

bool InputManager::isKeyPressed(int key) {
    return s_keysPressed[key];
}
bool InputManager::isKeyReleased(int key) {
    return s_keysReleased[key];
}
bool InputManager::isKeyDown(int key) {
    return s_keysDown[key];
}

bool InputManager::isMouseButtonPressed(int button) {
    return s_mouseButtonsPressed[button];
}
bool InputManager::isMouseButtonReleased(int button) {
    return s_mouseButtonsReleased[button];
}
bool InputManager::isMouseButtonDown(int button) {
    return s_mouseButtonsDown[button];
}
void InputManager::setMouseLocked(bool state) {
    s_mouseLocked = state;
    glfwSetInputMode(s_window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
bool InputManager::isMouseLocked() {
    return s_mouseLocked;
}

glm::vec2 InputManager::getMousePosition() {
    return s_mousePosition;
}
glm::vec2 InputManager::getMouseDelta() {
    return s_mouseDelta;
}
glm::vec2 InputManager::getScrollOffset() {
    return s_scrollOffset;
}

// Register callbacks (return handle)
InputManager::CallbackHandle<InputManager::KeyCallback> InputManager::onKeyPressed(KeyCallback cb) {
    auto handle = std::make_shared<KeyCallback>(std::move(cb));
    s_keyPressedCallbacks.push_back(handle);
    return handle;
}
InputManager::CallbackHandle<InputManager::KeyCallback> InputManager::onKeyReleased(KeyCallback cb) {
    auto handle = std::make_shared<KeyCallback>(std::move(cb));
    s_keyReleasedCallbacks.push_back(handle);
    return handle;
}
InputManager::CallbackHandle<InputManager::MouseButtonCallback> InputManager::onMouseButtonPressed(MouseButtonCallback cb) {
    auto handle = std::make_shared<MouseButtonCallback>(std::move(cb));
    s_mouseButtonPressedCallbacks.push_back(handle);
    return handle;
}
InputManager::CallbackHandle<InputManager::MouseButtonCallback> InputManager::onMouseButtonReleased(MouseButtonCallback cb) {
    auto handle = std::make_shared<MouseButtonCallback>(std::move(cb));
    s_mouseButtonReleasedCallbacks.push_back(handle);
    return handle;
}
InputManager::CallbackHandle<InputManager::CursorPosCallback> InputManager::onCursorPos(CursorPosCallback cb) {
    auto handle = std::make_shared<CursorPosCallback>(std::move(cb));
    s_cursorPosCallbacks.push_back(handle);
    return handle;
}

// GLFW Callbacks
void InputManager::keyCallback(GLFWwindow*, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        s_keysDown[key] = true;
        s_keysPressed[key] = true;
    } else if (action == GLFW_RELEASE) {
        s_keysDown[key] = false;
        s_keysReleased[key] = true;
    }
}
void InputManager::mouseButtonCallback(GLFWwindow*, int button, int action, int) {
    if (action == GLFW_PRESS) {
        s_mouseButtonsDown[button] = true;
        s_mouseButtonsPressed[button] = true;
    } else if (action == GLFW_RELEASE) {
        s_mouseButtonsDown[button] = false;
        s_mouseButtonsReleased[button] = true;
    }
}

InputManager::CallbackHandle<InputManager::ScrollCallback> InputManager::onScroll(ScrollCallback cb) {
    auto handle = std::make_shared<ScrollCallback>(std::move(cb));
    s_scrollCallbacks.push_back(handle);
    return handle;
}

void InputManager::cursorPosCallback(GLFWwindow*, double xpos, double ypos) {
    s_mousePosition = glm::vec2((float)xpos, (float)ypos);
    for (const auto& cbHandle : s_cursorPosCallbacks) {
        if (cbHandle && *cbHandle) (*cbHandle)(xpos, ypos);
    }
}
void InputManager::scrollCallback(GLFWwindow*, double xoffset, double yoffset) {
    s_scrollOffset = glm::vec2((float)xoffset, (float)yoffset);
    for (const auto& cbHandle : s_scrollCallbacks) {
        if (cbHandle && *cbHandle) (*cbHandle)(s_scrollOffset.x, s_scrollOffset.y);
    }
}