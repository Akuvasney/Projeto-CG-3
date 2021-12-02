#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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
  m_program = createProgramFromFile(getAssetsPath() + "shaders/lookat.vert",
                                    getAssetsPath() + "shaders/lookat.frag");

  m_ground.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wood.jpg");
  m_couch.initializeGL(m_program, getAssetsPath() + "models/sofa.obj", getAssetsPath() + "maps/Sofa_albedo.jpg");

}

void OpenGLWindow::paintGL() {
  
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};

  m_diffuseTexLoc = glGetUniformLocation(m_program, "diffuseTex");
  m_mappingModeLoc = glGetUniformLocation(m_program, "mappingMode");
  m_modelMatrixLoc = glGetUniformLocation(m_program, "modelMatrix");
  m_normalMatrixLoc = glGetUniformLocation(m_program, "normalMatrix");
  m_lightDirLoc = glGetUniformLocation(m_program, "lightDirWorldSpace");
  m_shininessLoc = glGetUniformLocation(m_program, "shininess");
  m_IaLoc = glGetUniformLocation(m_program, "Ia");
  m_IdLoc = glGetUniformLocation(m_program, "Id");
  m_IsLoc = glGetUniformLocation(m_program, "Is");
  m_KaLoc = glGetUniformLocation(m_program, "Ka");
  m_KdLoc = glGetUniformLocation(m_program, "Kd");
  m_KsLoc = glGetUniformLocation(m_program, "Ks");
  m_colorLoc = glGetUniformLocation(m_program, "color");

  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                     &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                     &m_camera.m_projMatrix[0][0]);
  
  // Draw floor
  render_model(&m_ground, -90.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 0.0f, 0.0f), 5.0f);

  // Draw couch
  render_model(&m_couch, 270.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 0.2f, -1.7f), 0.56f);


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
  m_couch.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
}

void OpenGLWindow::render_model(Model *item, float angle, glm::vec3 axis, glm::vec3 position, float scale_size){
  glm::mat4 model{1.0f};
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(angle), axis);
  model = glm::scale(model, glm::vec3(scale_size));

  glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

  glUniform1i(m_diffuseTexLoc, 0);
  glUniform1i(m_mappingModeLoc, 3);  // mesh
  glUniform4f(m_colorLoc, 1.0f, 0.25f, 0.25f, 1.0f);

  glUniform4fv(m_lightDirLoc, 1, &m_lightDir.x);
  glUniform4fv(m_IaLoc, 1, &m_Ia.x);
  glUniform4fv(m_IdLoc, 1, &m_Id.x);
  glUniform4fv(m_IsLoc, 1, &m_Is.x);

  auto modelViewMatrixTexture{glm::mat3(m_camera.m_viewMatrix * model)};
  glm::mat3 textureMatrix{glm::inverseTranspose(modelViewMatrixTexture)};
  glUniformMatrix3fv(m_normalMatrixLoc, 1, GL_FALSE, &textureMatrix[0][0]);

  auto ka = item->getKa();
  auto kd = item->getKd();
  auto ks = item->getKs();

  glUniform1f(m_shininessLoc, item->getShininess());
  glUniform4fv(m_KaLoc, 1, &ka.x);
  glUniform4fv(m_KdLoc, 1, &kd.x);
  glUniform4fv(m_KsLoc, 1, &ks.x);

  item->render(-1);
}