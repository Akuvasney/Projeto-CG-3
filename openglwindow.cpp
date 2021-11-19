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

  m_lamp1.initializeGL(m_program, getAssetsPath() + "lamp.obj");
  m_lamp2.initializeGL(m_program, getAssetsPath() + "lamp.obj");
  m_lamp3.initializeGL(m_program, getAssetsPath() + "lamp.obj");
  m_lamp4.initializeGL(m_program, getAssetsPath() + "lamp.obj");
  m_couch.initializeGL(m_program, getAssetsPath() + "sofa.obj");
  m_table.initializeGL(m_program, getAssetsPath() + "CoffeeTable.obj");
  m_tv.initializeGL(m_program, getAssetsPath() + "TV.obj");
  m_rug.initializeGL(m_program, getAssetsPath() + "rug.obj");
  m_slender.initializeGL(m_program, getAssetsPath() + "Slenderman Model.obj");

  m_ground.initializeGL(m_program);
  m_roof.initializeGL(m_program);
  m_leftwall.initializeGL(m_program, getAssetsPath() + "wall.obj");
  m_rightwall.initializeGL(m_program, getAssetsPath() + "wall.obj");
  m_backwall.initializeGL(m_program, getAssetsPath() + "wall.obj");
  m_frontwall.initializeGL(m_program, getAssetsPath() + "wall.obj");
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
  m_leftwall.paintGL(90.0f, glm::vec3(0, 1, 0), glm::vec3(-2.0f, 0.0f, 2.0f), 1.5f, glm::vec4(0.67f, 0.13f, 0.13f, 1.0f));

    // Draw right wall
  m_rightwall.paintGL(90.0f, glm::vec3(0, 1, 0), glm::vec3(2.0f, 0.0f, 2.0f), 1.5f, glm::vec4(0.67f, 0.13f, 0.13f, 1.0f));

    // draw Back wall
  m_backwall.paintGL(0.0f, glm::vec3(1, 0, 0), glm::vec3(-2.0f, 0.0f, 2.0f), 1.5f, glm::vec4(0.67f, 0.13f, 0.13f, 1.0f));
  
  // draw front wall
  m_frontwall.paintGL(0.0f, glm::vec3(1, 0, 0), glm::vec3(-2.0f, 0.0f, -2.0f), 1.5f, glm::vec4(0.67f, 0.13f, 0.13f, 1.0f));

  // draw lamps
  m_lamp1.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(-2.0f, 0.0f, -1.8f), 0.006f, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));
  m_lamp2.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(-2.0f, 0.0f, 1.8f), 0.006f, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));
  m_lamp3.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(1.7f, 0.0f, -1.8f), 0.006f, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));
  m_lamp4.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(1.7f, 0.0f, 1.8f), 0.006f, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));

  // draw couch
  m_couch.paintGL(270.0f, glm::vec3(1, 0, 0), glm::vec3(0.0f, 0.2f, -1.7f), 0.006f, glm::vec4(0.45f, 0.5f, 0.55f, 1.0f));

  // draw table
  m_table.paintGL(90.0f, glm::vec3(0, 1, 0), glm::vec3(0.0f, 0.14f, -0.2f), 0.28f, glm::vec4(0.54f, 0.26f, 0.074f, 1.0f));

  // draw tv
  m_tv.paintGL(90.0f, glm::vec3(0, 1, 0), glm::vec3(-0.2f, 0.3f, -0.0f), 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

  //draw rug
  m_rug.paintGL(90.0f, glm::vec3(1, 0, 0), glm::vec3(-0.0f, 0.002f, -0.9f), 0.009f, glm::vec4(0.67f, 0.13f, 0.13f, 1.0f));

  //draw slender
  m_slender.paintGL(0.0f, glm::vec3(1, 0, 0), glm::vec3(-0.0f, 0.82f, -0.9f), 0.0025f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));


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
  m_lamp1.terminateGL();
  m_lamp2.terminateGL();
  m_lamp3.terminateGL();
  m_lamp4.terminateGL();
  m_couch.terminateGL();
  m_table.terminateGL();
  m_tv.terminateGL();
  m_rug.terminateGL();
  m_slender.terminateGL();

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