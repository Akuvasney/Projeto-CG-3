#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "roof.hpp"
#include "model.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  glm::vec3 m_last_mouse_position{0,0,0};

  Ground m_ground;
  Roof m_roof;
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
  
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void update();
};

#endif