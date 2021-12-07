#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>
#include <list>

#include "abcg.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "gamedata.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:

  void render_model(Model *item, float angle, glm::vec3 axis, glm::vec3 position, float scale_size, bool lamp = false);
  void checkLight();
  bool checkCollision_individual(Model* model, glm::vec3 currentPos, float offset = 0.175f);
  bool checkCollisions(glm::vec3 currentPos);
  GLuint m_program{};

  GLint m_diffuseTexLoc{};
  GLint m_mappingModeLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_normalMatrixLoc{};
  GLint m_lightDirLoc{};
  GLint m_shininessLoc{};
  GLint m_IaLoc{};
  GLint m_IdLoc{};
  GLint m_IsLoc{};
  GLint m_KaLoc{};
  GLint m_KdLoc{};
  GLint m_KsLoc{};
  GLint m_colorLoc{};

  bool m_on{false};
  
  int m_viewportWidth{};
  int m_viewportHeight{};
  
  GameData m_gameData;
  
  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_modSpeed{1.0f};

  Model m_ground;
  Model m_roof;
  Model m_leftwall;
  Model m_rightwall;
  Model m_backwall;
  Model m_frontwall;
  Model m_lamp1;
  Model m_lamp2;
  Model m_lamp3;
  Model m_lamp4;
  Model m_couch;
  Model m_table;
  Model m_tv;
  Model m_rug;
  Model m_slender;
  Model m_quadro1;
  Model m_quadro2;
  Model m_escr1;
  Model m_escr2;
  Model m_chandelier;
  Model m_interr1;
  Model m_interr2;

  //lista de interruptores
  std::list<Model*> m_collisions {&m_table, &m_tv, &m_escr1, &m_escr2, &m_couch, &m_slender,
                                  &m_frontwall, &m_backwall, &m_rightwall, &m_leftwall};
  std::list<Model*> m_interruptors {&m_interr1, &m_interr2};

  // Propriedades da luz e materiais:
  glm::vec4 m_lightDir{0.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 1.0f, 1.0f};

  void update();
  void restart();
};

#endif