#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <memory>

class InputManager {
public:
    using KeyCallback = std::function<void(int key)>;
    using MouseButtonCallback = std::function<void(int button)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;
    using CursorPosCallback = std::function<void(double xpos, double ypos)>;

    // Callback handle type
    template<typename T>
    using CallbackHandle = std::shared_ptr<T>;

    static void initialize(GLFWwindow* window);
    static bool isInitialized();

    static void pollEvents();

    // Keyboard
    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);
    static bool isKeyDown(int key);

    // Mouse
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonReleased(int button);
    static bool isMouseButtonDown(int button);
    static void setMouseLocked(bool state);
    static bool isMouseLocked();

    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();
    static glm::vec2 getScrollOffset();

    // Register callbacks (returns handle for disconnect)
    static CallbackHandle<KeyCallback> onKeyPressed(KeyCallback cb);
    static CallbackHandle<KeyCallback> onKeyReleased(KeyCallback cb);
    static CallbackHandle<MouseButtonCallback> onMouseButtonPressed(MouseButtonCallback cb);
    static CallbackHandle<MouseButtonCallback> onMouseButtonReleased(MouseButtonCallback cb);
    static CallbackHandle<ScrollCallback> onScroll(ScrollCallback cb);
    static CallbackHandle<CursorPosCallback> onCursorPos(CursorPosCallback cb);
    // Disconnect callback by handle
    template<typename T>
    static void disconnectCallback(std::vector<CallbackHandle<T>>& vec, CallbackHandle<T> handle) {
        vec.erase(std::remove(vec.begin(), vec.end(), handle), vec.end());
    }

    // Internal callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static GLFWwindow* s_window;

    static std::unordered_map<int, bool> s_keysDown;
    static std::unordered_map<int, bool> s_keysPressed;
    static std::unordered_map<int, bool> s_keysReleased;

    static std::unordered_map<int, bool> s_mouseButtonsDown;
    static std::unordered_map<int, bool> s_mouseButtonsPressed;
    static std::unordered_map<int, bool> s_mouseButtonsReleased;

    static glm::vec2 s_mousePosition;
    static glm::vec2 s_lastMousePosition;
    static glm::vec2 s_mouseDelta;
    static glm::vec2 s_scrollOffset;

    static bool s_mouseLocked;
    static bool s_isInitialized;

    // Callback lists (store shared_ptrs)
    static std::vector<CallbackHandle<ScrollCallback>> s_scrollCallbacks;
    static std::vector<CallbackHandle<KeyCallback>> s_keyPressedCallbacks;
    static std::vector<CallbackHandle<KeyCallback>> s_keyReleasedCallbacks;
    static std::vector<CallbackHandle<MouseButtonCallback>> s_mouseButtonPressedCallbacks;
    static std::vector<CallbackHandle<MouseButtonCallback>> s_mouseButtonReleasedCallbacks;
    static std::vector<CallbackHandle<CursorPosCallback>> s_cursorPosCallbacks;
};

#endif // INPUT_MANAGER_H