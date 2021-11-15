#ifndef LEFT_WALL_HPP_
#define LEFT_WALL_HPP_

#include "abcg.hpp"

class Leftwall {
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