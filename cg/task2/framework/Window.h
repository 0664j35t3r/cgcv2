#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include "Input.h"

struct GLFWwindow;

class Window
{
public:
  virtual ~Window();

  static void init();
  static bool create(unsigned width, unsigned height, const std::string& title);
  static void makeCurrent();
  static int enterMainLoop();

  static void setGLVersion(const unsigned major, const unsigned minor);
  static void setBufferSizes(const unsigned red = 8, const unsigned green = 8,
    const unsigned blue = 8, const unsigned alpha = 8,
    const unsigned depth = 24, const unsigned stencil = 0);

  static void setErrorCallback(void(*errorCallback)(int, const char*));
  static void setDrawCallback(void(*drawCallback)());
  static void setUpdateCallback(void(*updateCallback)(float t, float dt));
  static void setInitCallback(void(*initCallback)());
  static void fireResizeEvent();

  static void setResizeCallback(void(*resizeCallback)(int width, int height));
  static void setKeyboardCallback(void(*keyboardCallback)(Key key, KeyAction action));
  static void setMousePositionCallback(void(*mousePositionCallback)(float x, float y));
  static void setMouseButtonCallback(void(*mouseButtonCallback)(MouseButton button, MouseButtonAction action));

  static void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void glfwKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void glfwMousePositionCallback(GLFWwindow* window, double x, double y);
  static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

protected:
private:
  static GLFWwindow* window_;
  static void(*drawCallback_)();
  static void(*updateCallback_)(float t, float dt);
  static void(*initCallback_)();
  static void(*resizeCallback_)(int width, int height);
  static void(*keyboardCallback_)(Key key, KeyAction action);
  static void(*mousePositionCallback_)(float x, float y);
  static void(*mouseButtonCallback_)(MouseButton button, MouseButtonAction action);
  Window();
};

#endif // WINDOW_H
