#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "abcg.hpp"
#include <vector>

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
  }
};

class Model {
 public:

  void initializeGL(GLuint program, std::string_view path, std::string_view diffuseTexture_path = "NULL");
  void loadDiffuseTexture(std::string_view path);
  void loadFromFile(std::string_view path, GLuint program);
  void render(int numTriangles = -1) const;
  void setupVAO(GLuint program);
  void terminateGL();

  float m_max_x;
  float m_max_z;
  float m_min_x;
  float m_min_z;

  glm::vec3 m_position{};
  glm::vec3 m_rotation_axis{};
  float m_angle;
  float m_scale;
  bool m_far{true};
  
  int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  glm::vec4 getKa() const { return m_Ka; }
  glm::vec4 getKd() const { return m_Kd; }
  glm::vec4 getKs() const { return m_Ks; }
  float getShininess() const { return m_shininess; }

  bool isUVMapped() const { return m_hasTexCoords; }

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};


  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess;
  GLuint m_diffuseTexture{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  void computeNormals();

  void createBuffers();
  void standardize();
};

#endif