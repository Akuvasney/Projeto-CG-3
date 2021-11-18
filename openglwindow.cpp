#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }

  if (ev.type == SDL_MOUSEMOTION) {
    const float deltaTime{static_cast<float>(getDeltaTime())};

    float velocity_up;
    float velocity_right;

    velocity_up = 0.5f*((float)ev.motion.yrel);
    velocity_right = 0.5f*((float)ev.motion.xrel);

    m_camera.pan(velocity_up*deltaTime, 1);
    m_camera.pan(velocity_right*deltaTime, 0);

  }
}

void OpenGLWindow::initializeGL() {

  SDL_SetRelativeMouseMode(SDL_TRUE);

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

  m_lamp.initializeGL(m_program, getAssetsPath() + "lamp.obj");

  m_ground.initializeGL(m_program);
  m_roof.initializeGL(m_program);
  m_leftwall.initializeGL(m_program);
  m_rightwall.initializeGL(m_program);
  m_backwall.initializeGL(m_program);
  m_frontwall.initializeGL(m_program);
}

void OpenGLWindow::paintGL() {
  
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  

  // Draw ground
  m_ground.paintGL();

  // Draw roof
  m_roof.paintGL();

    // Draw left wall
  m_leftwall.paintGL();

    // Draw right wall
  m_rightwall.paintGL();

    // draw Back wall
  m_backwall.paintGL();
  
  // draw front wall
  m_frontwall.paintGL();

  // draw lamps
  m_lamp.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(-2.0f, 0.0f, -1.8f), 0.006f, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_ground.terminateGL();
  m_roof.terminateGL();
  m_leftwall.terminateGL();
  m_rightwall.terminateGL();
  m_backwall.terminateGL();
  m_frontwall.terminateGL();
  m_lamp.terminateGL();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
}