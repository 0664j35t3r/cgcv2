#include "Window.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

GLFWwindow* Window::window_ = 0;
void(*Window::drawCallback_)() = 0;
void(*Window::updateCallback_)(float t, float dt) = 0;
void(*Window::initCallback_)() = 0;
void(*Window::resizeCallback_)(int width, int height) = 0;
void(*Window::keyboardCallback_)(Key key, KeyAction action) = 0;
void(*Window::mousePositionCallback_)(float x, float y);
void(*Window::mouseButtonCallback_)(MouseButton button, MouseButtonAction action);

void Window::fireResizeEvent()
{
  if (resizeCallback_)
  {
    int size[2];
    glfwGetWindowSize(window_, &size[0], &size[1]);
    resizeCallback_(size[0], size[1]);
  }
}

Window::Window()
{
}

Window::~Window()
{
}

void Window::setGLVersion(const unsigned major, const unsigned minor)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
}

void Window::setErrorCallback(void(*errorCallback)(int, const char*))
{
  glfwSetErrorCallback(errorCallback);
}

void Window::init()
{
  if (!glfwInit())
  {
    std::cout << "Failed to initialize GLFW. Maybe it's already initialized."
      << std::endl;
  }
}

bool Window::create(unsigned width, unsigned height, const std::string& title)
{
  window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window_)
  {
    std::cerr << "Failed to create window with the supplied hints."
      << std::endl;
    glfwTerminate();
    return false;
  }
  return true;
}

void Window::setBufferSizes(const unsigned red, const unsigned green,
  const unsigned blue, const unsigned alpha, const unsigned depth,
  const unsigned stencil)
{
  glfwWindowHint(GLFW_RED_BITS, red);
  glfwWindowHint(GLFW_GREEN_BITS, green);
  glfwWindowHint(GLFW_BLUE_BITS, blue);
  glfwWindowHint(GLFW_ALPHA_BITS, alpha);
  glfwWindowHint(GLFW_DEPTH_BITS, depth);
  glfwWindowHint(GLFW_STENCIL_BITS, stencil);
}

void Window::makeCurrent()
{
  glfwMakeContextCurrent(window_);

  if (gl3wInit())
    std::cout << "Failed to initialize GL3W. Maybe it's "
    "already been initialized." << std::endl;
}

int Window::enterMainLoop()
{
  if (initCallback_)
    initCallback_();

  float time_prev = static_cast<float>(glfwGetTime());

  while (!glfwWindowShouldClose(window_))
  {
    float time_curr = static_cast<float>(glfwGetTime());
    float passedSecs = time_curr - time_prev;
    time_prev = time_curr;

    if (updateCallback_)
      updateCallback_(time_curr, passedSecs);

    if (drawCallback_)
      drawCallback_();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
  return EXIT_SUCCESS;
}

void Window::setDrawCallback(void(*drawCallback)())
{
  drawCallback_ = drawCallback;
}

void Window::setUpdateCallback(void(*updateCallback)(float t, float dt))
{
  updateCallback_ = updateCallback;
}

void Window::setInitCallback(void(*initCallback)())
{
  initCallback_ = initCallback;
}

void Window::setResizeCallback(void(*resizeCallback)(int width, int height))
{
  glfwSetFramebufferSizeCallback(window_, glfwFramebufferSizeCallback);
  resizeCallback_ = resizeCallback;
}

void Window::setKeyboardCallback(void(*keyboardCallback)(Key key, KeyAction action))
{
  glfwSetKeyCallback(window_, glfwKeyboardCallback);
  keyboardCallback_ = keyboardCallback;
}

void Window::setMousePositionCallback(void(*mousePositionCallback)(float x, float y))
{
  glfwSetCursorPosCallback(window_, glfwMousePositionCallback);
  mousePositionCallback_ = mousePositionCallback;
}

void Window::setMouseButtonCallback(void(*mouseButtonCallback)(MouseButton button, MouseButtonAction action))
{
  glfwSetMouseButtonCallback(window_, glfwMouseButtonCallback);
  mouseButtonCallback_ = mouseButtonCallback;
}

void Window::glfwFramebufferSizeCallback(GLFWwindow* window, int width,
  int height)
{
  if (resizeCallback_)
    resizeCallback_(width, height);
}

void Window::glfwKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  Key k;
  if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
  {
    k = static_cast<Key>(static_cast<int>(Key::ZERO) + (key - GLFW_KEY_0));
  }
  else if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
  {
    k = static_cast<Key>(static_cast<int>(Key::A) + (key - GLFW_KEY_A));
  }
  else
  {
    switch (key)
    {
    case GLFW_KEY_LEFT:
      k = Key::LEFT;
      break;
    case GLFW_KEY_RIGHT:
      k = Key::RIGHT;
      break;
    case GLFW_KEY_UP:
      k = Key::UP;
      break;
    case GLFW_KEY_DOWN:
      k = Key::DOWN;
      break;
    case GLFW_KEY_SPACE:
      k = Key::SPACE;
      break;
    case GLFW_KEY_BACKSPACE:
      k = Key::BACKSPACE;
      break;
    default:
      k = Key::UNKNOWN;
      break;
    }
  }

  KeyAction a = KeyAction::PRESS;
  if (action == GLFW_RELEASE)
  {
    a = KeyAction::RELEASE;
  }
  else if (action == GLFW_REPEAT)
  {
    a = KeyAction::REPEAT;
  }

  if (keyboardCallback_)
    keyboardCallback_(k, a);
}

void Window::glfwMousePositionCallback(GLFWwindow* window, double x, double y)
{
  if (mousePositionCallback_)
    mousePositionCallback_(static_cast<float>(x), static_cast<float>(y));
}

void Window::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  MouseButton b;
  switch (button)
  {
  case GLFW_MOUSE_BUTTON_RIGHT:
    b = MouseButton::RIGHT;
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    b = MouseButton::MIDDLE;
    break;
  case GLFW_MOUSE_BUTTON_LEFT:
    b = MouseButton::LEFT;
    break;
  default:
    b = MouseButton::UNKNOWN;
    break;
  }

  MouseButtonAction a = MouseButtonAction::PRESS;
  if (action == GLFW_RELEASE)
  {
    a = MouseButtonAction::RELEASE;
  }

  if (mouseButtonCallback_)
    mouseButtonCallback_(b, a);
}
