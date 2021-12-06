#include "model.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <filesystem>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Criação namespace std
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    std::size_t h2{std::hash<glm::vec3>()(vertex.normal)};
    std::size_t h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return h1 ^ h2 ^ h3;
  }
};
}  // namespace std

void Model::terminateGL() {
  glDeleteTextures(1, &m_diffuseTexture);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void Model::initializeGL(GLuint program, std::string_view path, std::string_view diffuseTexture_path){
  loadFromFile(path, program);
  
  if(diffuseTexture_path != "NULL"){
    loadDiffuseTexture(diffuseTexture_path);
  }
}

void Model::computeNormals() {
  // Limpando vertex.normal anteriores
  for (auto& vertex : m_vertices) {
    vertex.normal = glm::zero<glm::vec3>();
  }

  //Calcular normais de face
  for (const auto offset : iter::range<int>(0, m_indices.size(), 3)) {
    // Pegando vértices de face
    Vertex& a{m_vertices.at(m_indices.at(offset + 0))};
    Vertex& b{m_vertices.at(m_indices.at(offset + 1))};
    Vertex& c{m_vertices.at(m_indices.at(offset + 2))};

    // Computando normal
    const auto edge1{b.position - a.position};
    const auto edge2{c.position - b.position};
    glm::vec3 normal{glm::cross(edge1, edge2)};

    // Acumulando nos vértices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalizar
  for (auto& vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Model::createBuffers() {
  // Deletando buffers anteriores
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);

  // VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::loadDiffuseTexture(std::string_view path) {
  if (!std::filesystem::exists(path)) return;

  glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::opengl::loadTexture(path);
}

void Model::loadFromFile(std::string_view path, GLuint program) {
  auto basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath;  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};
  const auto& materials{reader.GetMaterials()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

  std::unordered_map<Vertex, GLuint> hash{};

  // Loop sob as formas
  for (const auto& shape : shapes) {
    // Loop sob indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Acesso ao vertex
      tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Coordenadas de vértice
      std::size_t startIndex{static_cast<size_t>(3 * index.vertex_index)};
      float vx{attrib.vertices.at(startIndex + 0)};
      float vy{attrib.vertices.at(startIndex + 1)};
      float vz{attrib.vertices.at(startIndex + 2)};

      // Vertex normal
      float nx{};
      float ny{};
      float nz{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        startIndex = 3 * index.normal_index;
        nx = attrib.normals.at(startIndex + 0);
        ny = attrib.normals.at(startIndex + 1);
        nz = attrib.normals.at(startIndex + 2);
      }

      // Vertex - coordenadas textura
      float tu{};
      float tv{};
      if (index.texcoord_index >= 0) {
        m_hasTexCoords = true;
        startIndex = 2 * index.texcoord_index;
        tu = attrib.texcoords.at(startIndex + 0);
        tv = attrib.texcoords.at(startIndex + 1);
      }
    
      Vertex vertex{};
      vertex.position = {vx, vy, vz};
      vertex.normal = {nx, ny, nz};
      vertex.texCoord = {tu, tv};

      if (hash.count(vertex) == 0) {
        hash[vertex] = m_vertices.size();
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  // Use propriedades do primeiro material, se disponível
  if (!materials.empty()) {
    const auto& mat{materials.at(0)};  // First material
    m_Ka = glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1);
    m_Kd = glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1);
    m_Ks = glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1);
    m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(basePath + mat.diffuse_texname);
  } else {
    // Valores default
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 35.0f;
  }


  this->standardize();


  if (!m_hasNormals) {
    computeNormals();
  }

  createBuffers();

  setupVAO(program);
}

void Model::render(int numTriangles) const {
  glBindVertexArray(m_VAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

  // Definir parâmetros de minificação e ampliação
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Definir parâmetros de envolvimento de textura
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLsizei numIndices = (numTriangles < 0) ? m_indices.size() : numTriangles * 3;

  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void Model::setupVAO(GLuint program) {

  glDeleteVertexArrays(1, &m_VAO);

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  GLint positionAttribute = glGetAttribLocation(program, "inPosition");
  if (positionAttribute >= 0) {
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), nullptr);
  }

  GLint normalAttribute = glGetAttribLocation(program, "inNormal");
  if (normalAttribute >= 0) {
    glEnableVertexAttribArray(normalAttribute);
    GLsizei offset{sizeof(glm::vec3)};
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offset));
  }

  GLint texCoordAttribute{glGetAttribLocation(program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    glEnableVertexAttribArray(texCoordAttribute);
    GLsizei offset{sizeof(glm::vec3) + sizeof(glm::vec3)};
    glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offset));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Model::standardize() {
  // Centraliza para originar e normalizar o maior limite para [-1, 1]

  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Centraliza e escala, além de armazenar os valores máximos/mínimos das componentes
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
    m_max_x = (max.x - center.x) * scaling;
    m_max_z = (max.z - center.z) * scaling;
    m_min_x = (min.x - center.x) * scaling;
    m_min_z = (min.z - center.z) * scaling;
  }
}