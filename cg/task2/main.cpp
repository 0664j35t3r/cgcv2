#include <GL/gl3w.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "Window.h"
#include "Model.h"
#include "IQMImporter.h"
#include "IModelDrawer.h"
#include "ModelDrawer.h"
#include "InFile.h"
#include "Shader.h"
#include "Camera.h"
#include "Input.h"
#include "PointLight.h"
#include "Config.h"
#include "Spline.h"
#include "SplineDrawer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::string;

void errorCallback(int, const char*);
void drawCallback();
void updateCallback(float time, float dt);
void initCallback();
void resizeCallback(int width, int height);
void keyboardCallback(Key key, KeyAction action);
void mousePositionCallback(float x, float y);
void mouseButtonCallback(MouseButton button, MouseButtonAction action);

Config* config;
Spline spline;
SplineDrawer* spline_drawer;
Model* model;
IModelDrawer* drawer;
Shader* shader;
Camera* camera = new Camera();
PointLight* light = new PointLight(glm::vec3(0), glm::vec4(1, 1, 1, 1));
MouseButton mouse_button_pressed = MouseButton::UNKNOWN;
glm::vec2 mouse_pos_prev(-1.f);
float animation_time = 0.f;
float spline_time = 0.f;
std::vector<float> screenshot_frames;
bool generateScreenshots = false;

int MODE_MESH = 1 << 0;
int MODE_JOINTS = 1 << 1;
int MODE_BONES = 1 << 2;
int MODE_WIREFRAME = 1 << 3;
int MODE_SPLINE = 1 << 4;
int mode = MODE_MESH;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " config.xml [-screenshots]" << endl;
    exit(1);
  }
  if (argc == 3)
  {
    if (std::string(argv[2]).compare("-screenshots") == 0)
      generateScreenshots = true;
  }
  config = new Config();
  if (!config->load(argv[1]))
  {
    cerr << "Loading of config file failed." << endl;
    exit(1);
  }
  mode = config->getRenderMode();
  if (config->hasSpline())
  {
    spline = config->getSpline();
  }
  if (config->hasScreenshotFrames())
  {
    screenshot_frames = config->getScreenshotFrames();
  }

  Window::setErrorCallback(errorCallback);
  Window::init();
  Window::setGLVersion(2, 1);
  Window::setBufferSizes(8, 8, 8, 8, 24, 0);
  Window::create(800, 600, "CG2 Skeletal Animation");
  Window::makeCurrent();

  Window::setDrawCallback(drawCallback);
  Window::setUpdateCallback(updateCallback);
  Window::setInitCallback(initCallback);
  Window::setResizeCallback(resizeCallback);
  Window::setKeyboardCallback(keyboardCallback);
  Window::setMousePositionCallback(mousePositionCallback);
  Window::setMouseButtonCallback(mouseButtonCallback);
  Window::fireResizeEvent();

  IQMImporter importer;
  model = importer.loadModel(config->getModelFileName(),
      config->getAnimationFileNames(), config->getAnimationRepeatTime(),
      config->getAnimationRelativeFlags());
  if (!model)
    cerr << "Error loading model." << endl;

  return Window::enterMainLoop();
}

void errorCallback(int error, const char* description)
{
  cerr << "GLFW Error: " << description << " (" << error << ")." << endl;
}

void drawCallback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  if (config->hasSpline())
  {
    SplineInterpolationResult interplation_result =
        spline.interpolate(spline_time);
    drawer->moveTo(interplation_result.getPosition());
    drawer->orientate(interplation_result.getOrientation());
  }

  shader->bind();
  shader->setUniformMatrix4f("proj_mat", camera->getProjMatrix());
  shader->setUniformMatrix4f("view_mat", camera->getViewMatrix());
  shader->setUniform3f("light_position", light->getPosition());
  shader->setUniform4f("light_diffuse", light->getDiffuse());

  if (config->hasSpline() && mode & MODE_SPLINE)
  {
    shader->setUniform1i("light_enabled", 0);
    spline_drawer->draw();
  }

  if (mode & MODE_WIREFRAME)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader->setUniform1i("light_enabled", 0);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader->setUniform1i("light_enabled", 1);
  }

  if (mode & MODE_JOINTS)
    drawer->drawJoints();

  if (mode & MODE_BONES)
    drawer->drawBones();

  if (mode & MODE_MESH)
    drawer->draw();

  if (config->hasScreenshotFrames() && generateScreenshots)
  {
    if (screenshot_frames.empty())
    {
      exit(0);
    }

    static int screenshot_number = 0;
    screenshot_number++;
    Image screenshot = drawer->makeScreenshot();
    std::stringstream ss;
    ss << config->getScreenshotsFolder() << "/" << screenshot_number << ".png";
    screenshot.save(ss.str());

    if (config->exportJointTransformations())
    {
        ss.clear();
        ss << config->getScreenshotsFolder() << "/" << config->getJointTransformationsFileName() << screenshot_number << ".png";
        drawer->exportJointTransformations(ss.str());
    }
  }

  shader->unbind();
}

void updateCallback(float t, float dt)
{
  if (config->hasScreenshotFrames() && generateScreenshots)
  {
    t = screenshot_frames.front();
    screenshot_frames.erase(screenshot_frames.begin());
    animation_time = t;
    spline_time = t;
  }
  else
  {
    animation_time += dt;
    spline_time += dt;
  }

  drawer->update(animation_time);

  camera->update(dt);
}

void initCallback()
{
  glClearColor(0.3f, 0.4f, 0.2f, 1.f);
  glClearDepth(1.f);

  string vsh_src = InFile("data/shaders/shader.vsh").toString();
  string fsh_src = InFile("data/shaders/shader.fsh").toString();

  shader = new Shader();
  shader->create();
  shader->addShader(Shader::ShaderType::VERTEX_SHADER, vsh_src);
  shader->addShader(Shader::ShaderType::FRAGMENT_SHADER, fsh_src);
  shader->link();

  camera->setPosition(config->getCameraPosition());
  camera->setOrientation(
      config->getCameraHorizontalAngle(), config->getCameraVerticalAngle());

  drawer = new ModelDrawer(model);
  drawer->init();
  drawer->setConfig(config);
  drawer->setShader(shader);
  drawer->setCamera(camera);
  drawer->setJointSize(config->getJointSize());
  drawer->setBoneSize(config->getBoneSize());
  if (!config->getAnimationFileNames().empty())
    drawer->startAction(0);
  drawer->update(0.f);

  if (config->hasSpline())
  {
    spline_drawer = new SplineDrawer(spline, *shader);
    spline_drawer->init();
  }
}

void resizeCallback(int width, int height)
{
  glViewport(0, 0, width, height);

  float aspect = (float)width / (float)height;
  camera->setProjection(config->getCameraFOV(), aspect, 0.1f, 1000.f);
}

void keyboardCallback(Key key, KeyAction action)
{
  static const float speed = config->getCameraSpeed();
  if (action == KeyAction::PRESS || action == KeyAction::REPEAT)
  {
    switch (key)
    {
    case Key::W:
      camera->move(glm::vec3(0, 0, speed));
      break;
    case Key::S:
      camera->move(glm::vec3(0, 0, -speed));
      break;
    case Key::A:
      camera->move(glm::vec3(-speed, 0, 0));
      break;
    case Key::D:
      camera->move(glm::vec3(speed, 0, 0));
      break;
    case Key::R:
      camera->move(glm::vec3(0, speed, 0));
      break;
    case Key::F:
      camera->move(glm::vec3(0, -speed, 0));
      break;
    case Key::ONE:
      mode = mode & MODE_MESH ? mode & ~MODE_MESH : mode | MODE_MESH;
      break;
    case Key::TWO:
      mode = mode & MODE_JOINTS ? mode & ~MODE_JOINTS : mode | MODE_JOINTS;
      break;
    case Key::THREE:
      mode = mode & MODE_BONES ? mode & ~MODE_BONES : mode | MODE_BONES;
      break;
    case Key::FOUR:
      mode = mode & MODE_WIREFRAME ? mode & ~MODE_WIREFRAME
                                   : mode | MODE_WIREFRAME;
      break;
    case Key::FIVE:
      mode = mode & MODE_SPLINE ? mode & ~MODE_SPLINE : mode | MODE_SPLINE;
      break;
    case Key::EIGHT:
      spline_time = 0.f;
      break;
    case Key::BACKSPACE:
      animation_time = 0.f;
      break;
    case Key::NINE:
      camera->setPosition(config->getCameraPosition());
      camera->resetOrientation();
      break;
    case Key::ZERO:
    {
      Image image = drawer->makeScreenshot();
      std::stringstream ss;
      ss << config->getScreenshotsFolder() << "/custom.png";
      image.save(ss.str());
    }
    break;
    default:
      break;
    }
  }
}

void mousePositionCallback(float x, float y)
{
  glm::vec2 mouse_pos_curr = glm::vec2(x, y);
  if (mouse_pos_prev != glm::vec2(-1.f))
  {
    glm::vec2 mouse_pos_diff = mouse_pos_curr - mouse_pos_prev;
    if (mouse_button_pressed == MouseButton::MIDDLE)
    {
      static const float speed = 1.5f;
      camera->move(
          glm::vec3(-mouse_pos_diff.x * speed, mouse_pos_diff.y * speed, 0));
    }
    else if (mouse_button_pressed == MouseButton::LEFT)
    {
      static const float speed = 0.6f;
      camera->orientate(-mouse_pos_diff * speed);
    }
  }
  mouse_pos_prev = mouse_pos_curr;
}

void mouseButtonCallback(MouseButton button, MouseButtonAction action)
{
  switch (action)
  {
  case MouseButtonAction::PRESS:
    mouse_button_pressed = button;
    break;
  case MouseButtonAction::RELEASE:
    mouse_button_pressed = MouseButton::UNKNOWN;
    break;
  }
}
