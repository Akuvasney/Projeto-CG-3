#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(int width, int height) {
  m_projMatrix = glm::mat4(1.0f);
  const auto aspect{static_cast<float>(width) / static_cast<float>(height)};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(float speed) {
  // Calcular vetor direto (direção da vista)
  glm::vec3 forward{glm::normalize(m_at - m_eye)};
  forward.y = 0;

  // Mova o observador e o centro para frente (velocidade> 0) ou para trás (velocidade <0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Calcular vetor direto (direção da vista)
  glm::vec3 forward{glm::normalize(m_at - m_eye)};
  forward.y = 0;
  // Calcule o vetor à esquerda
  const glm::vec3 left{glm::cross(m_up, forward)};

  // Mova o observador e o centro para a esquerda (velocidade <0) ou para a direita (velocidade> 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

void Camera::pan(float speed, int direction) {
  glm::mat4 transform{glm::mat4(1.0f)};
  glm::vec3 axis;

  // Girar a câmera em torno de seu eixo y local
  if(direction){
    axis = glm::cross(glm::vec3(m_at.x-m_eye.x, 0, m_at.z-m_eye.z), glm::vec3(0,1,0));
  }else{
    axis = m_up;
  }
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, axis);
  transform = glm::translate(transform, -m_eye);

  if(direction){
    glm::vec3 new_point = transform * glm::vec4(m_at, 1.0f);
    glm::vec3 vector(m_at-m_eye);
    glm::vec3 protection_axis;
    if(vector.z>0){
      protection_axis = glm::vec3(0,0,1);
    }else{
      protection_axis = glm::vec3(0,0,-1);
    }
    float vec_product = glm::dot(glm::normalize(new_point-m_eye),glm::normalize(protection_axis));
    float angle = glm::degrees(glm::acos(vec_product));
    if(angle < 90.0f){
      m_at = transform * glm::vec4(m_at, 1.0f);
    }

  }else{
    m_at = transform * glm::vec4(m_at, 1.0f);
  }
    


  

  computeViewMatrix();
}