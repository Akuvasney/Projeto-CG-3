#ifndef BACK_WALL_HPP_
#define BACK_WALL_HPP_

#include "abcg.hpp"

class Backwall {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};
};

#endif