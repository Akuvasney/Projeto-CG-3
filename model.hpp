#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "abcg.hpp"
#include <vector>

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class Model {
 public:
  void initializeGL(GLuint program, std::string_view path);
  void paintGL(float angle, glm::vec3 axis, glm::vec3 position, float scale_size, glm::vec4 color);
  void terminateGL();
  void loadModelFromFile(std::string_view path);

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
};

#endif