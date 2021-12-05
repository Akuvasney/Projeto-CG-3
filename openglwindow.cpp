#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>


//Eventos do teclado
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
    if (ev.key.keysym.sym == SDLK_LSHIFT || ev.key.keysym.sym == SDLK_RSHIFT)
      m_modSpeed = 2.0f;
    if (ev.key.keysym.sym == SDLK_ESCAPE && m_gameData.m_state == State::Playing)
      m_gameData.m_state = State::Paused;
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
    if ((ev.key.keysym.sym == SDLK_LSHIFT || ev.key.keysym.sym == SDLK_RSHIFT) &&
        m_modSpeed > 1.0f)
      m_modSpeed = 1.0f;
  }

  // eventos do mouse
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

// função restart que é chamada ao alterar o m_state para PLAYING
void OpenGLWindow::restart() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  m_gameData.m_state = State::Playing;
}

void OpenGLWindow::initializeGL() {

  abcg::glClearColor(0, 0, 0, 1);

  abcg::glEnable(GL_DEPTH_TEST);

  // Criando programa
  m_program = createProgramFromFile(getAssetsPath() + "shaders/texture.vert",
                                    getAssetsPath() + "shaders/texture.frag");

  m_ground.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wood.jpg");
  m_couch.initializeGL(m_program, getAssetsPath() + "models/sofa.obj", getAssetsPath() + "maps/Sofa_albedo.jpg");
  m_lamp1.initializeGL(m_program, getAssetsPath() + "models/lamp.obj", getAssetsPath() + "maps/lamp.jpg");
  m_lamp2.initializeGL(m_program, getAssetsPath() + "models/lamp.obj", getAssetsPath() + "maps/lamp.jpg");
  m_lamp3.initializeGL(m_program, getAssetsPath() + "models/lamp.obj", getAssetsPath() + "maps/lamp.jpg");
  m_lamp4.initializeGL(m_program, getAssetsPath() + "models/lamp.obj", getAssetsPath() + "maps/lamp.jpg");
  m_rug.initializeGL(m_program, getAssetsPath() + "models/rug.obj", getAssetsPath() + "maps/tapete.jpg");
  m_leftwall.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wall.jpg");
  m_rightwall.initializeGL(m_program, getAssetsPath() +"models/wall.obj", getAssetsPath() + "maps/wall.jpg");
  m_frontwall.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wall.jpg");
  m_backwall.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wall.jpg");
  m_roof.initializeGL(m_program, getAssetsPath() + "models/wall.obj", getAssetsPath() + "maps/wood.jpg");
  m_quadro1.initializeGL(m_program, getAssetsPath() + "models/retrato.obj", getAssetsPath() + "maps/retrato.png");
  m_quadro2.initializeGL(m_program, getAssetsPath() + "models/retrato.obj", getAssetsPath() + "maps/retrato2.png");
  m_escr1.initializeGL(m_program, getAssetsPath() + "models/escrivaninha.obj", getAssetsPath() + "maps/escrivaninha.png");
  m_escr2.initializeGL(m_program, getAssetsPath() + "models/escrivaninha.obj", getAssetsPath() + "maps/escrivaninha.png");
  m_table.initializeGL(m_program, getAssetsPath() + "models/central-table.obj", getAssetsPath() + "maps/escrivaninha.png");
  m_tv.initializeGL(m_program, getAssetsPath() + "models/tv.obj", getAssetsPath() + "maps/tv.jpg");
  m_chandelier.initializeGL(m_program, getAssetsPath() + "models/chandelier.obj", getAssetsPath() + "maps/chandelier.jpg");
  m_slender.initializeGL(m_program, getAssetsPath() + "models/slenderman.obj", getAssetsPath() + "maps/slenderman.jpg");
  m_interr1.initializeGL(m_program, getAssetsPath() + "models/interruptor.obj", getAssetsPath() + "maps/interruptor.jpg");
  m_interr2.initializeGL(m_program, getAssetsPath() + "models/interruptor.obj", getAssetsPath() + "maps/interruptor.jpg");
}

void OpenGLWindow::paintGL() {
  

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
  
  // Desenhando chão
  render_model(&m_ground, -90.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 0.0f, 0.0f), 2.8f);

  // Desenhando sofá
  render_model(&m_couch, 270.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 0.2f, -1.65f), 0.56f);

  // Desenhando lampadas
  render_model(&m_lamp1, 270.0f, glm::vec3(1, 0, 0), glm::vec3(1.8f, 0.35f, -1.8f), 0.35f);
  render_model(&m_lamp2, 270.0f, glm::vec3(1, 0, 0), glm::vec3(-1.8f, 0.35f, -1.8f), 0.35f);
  render_model(&m_lamp3, 270.0f, glm::vec3(1, 0, 0), glm::vec3(1.8f, 0.35f, 1.8f), 0.35f);
  render_model(&m_lamp4, 270.0f, glm::vec3(1, 0, 0), glm::vec3(-1.8f, 0.35f, 1.8f), 0.35f);

  // Desenhando tapete
  render_model(&m_rug, 270.0f, glm::vec3(1, 0, 0), glm::vec3(0.0f, 0.0f, -1.2f), 0.9f);

  // Desenhando paredes
  render_model(&m_backwall, 0.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 1.0f, -1.9f), 2.8f);
  render_model(&m_leftwall, 90.0f, glm::vec3(0,1,0), glm::vec3(-1.9f, 1.0f, 0.0f), 2.8f);
  render_model(&m_rightwall, -90.0f, glm::vec3(0,1,0), glm::vec3(1.9f, 1.0f, 0.0f), 2.8f);
  render_model(&m_frontwall, 180.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 1.0f, 1.9f), 2.8f);

  // Desenhando teto
  render_model(&m_roof, 90.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 3.0f, 0.0f), 2.8f);

  // Desenhando quadro
  render_model(&m_quadro1, -90.0f, glm::vec3(1,0,0), glm::vec3(0.75f, 0.4f, -1.7f), 0.1f);
  render_model(&m_quadro2, -90.0f, glm::vec3(1,0,0), glm::vec3(-0.75f, 0.4f, -1.7f), 0.1f);

  // Desenhando mesas de canto
  render_model(&m_escr1, 0.0f, glm::vec3(1,0,0), glm::vec3(0.75f, 0.15f, -1.7f), 0.3f);
  render_model(&m_escr2, 0.0f, glm::vec3(1,0,0), glm::vec3(-0.75f, 0.15f, -1.7f), 0.3f);

  // Desenhando mesa central
  render_model(&m_table, 90.0f, glm::vec3(0,1,0), glm::vec3(0.0f, 0.1f, -0.5f), 0.3f);

  // Desenhando TV
  render_model(&m_tv, 180.0f, glm::vec3(0,1,0), glm::vec3(0.0f, 0.4f, -0.5f), 0.4f);

  // Desenhando lustre
  render_model(&m_chandelier, 0.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 2.7f, 0.0f), 0.6f, true);

  // Desenhando figura humana
  render_model(&m_slender, 0.0f, glm::vec3(1,0,0), glm::vec3(0.0f, 0.93f, -1.1f), 1.0f);

  // Desenhando primeiro interruptor
  render_model(&m_interr1, -90.0f, glm::vec3(0,0,1), glm::vec3(-1.9f, 0.5f, 0.0f), 0.1f);
  
  //Desenhando segundo interruptor
  render_model(&m_interr2, 90.0f, glm::vec3(0,0,1), glm::vec3(1.9f, 0.5f, 0.0f), 0.1f);

  abcg::glUseProgram(0);

  update();
}

void OpenGLWindow::paintUI() { 
  abcg::OpenGLWindow::paintUI(); 
  // caso o m_state seja INITIAL, criar os elementos do menu:
  if (m_gameData.m_state != State::Playing) {
      //Zerando velocidades para evitar bugs
      m_dollySpeed = 0.0f;
      m_truckSpeed = 0.0f;
      m_modSpeed = 1.0f;
      SDL_SetRelativeMouseMode(SDL_FALSE);
      bool show = true;
      ImGuiWindowFlags window_flags = 0;
      window_flags |= ImGuiWindowFlags_NoMove;
      ImGui::SetNextWindowSize(ImVec2(m_viewportWidth, m_viewportHeight));
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin("*The Room 2.0!*", &show, window_flags);
      ImGui::Text("Mexa o mouse para virar a camera.");
      ImGui::Text("Ande com as teclas W-A-S-D.");
      ImGui::Text("Corra com a tecla SHIFT.");
      ImGui::Text("Se aproxime de um interruptor (nas paredes laterais) p/ acender/apagar a luz.");
      ImGui::Text("Aperte F11 para entrar/sair da tela cheia.");
      ImGui::Text("Aperte ALT+F4 para fechar a aplicação.");

      if(m_gameData.m_state == State::Initial){
        ImGui::Text("Clique em INICIAR para jogar.");

        ImGui::Button("INICIAR", ImVec2(-1, 50));
      }else{
        ImGui::Text("Clique em RETOMAR para voltar a jogar.");

        ImGui::Button("RETOMAR", ImVec2(-1, 50));
      }

      if (ImGui::IsItemClicked()) {
        restart();
      }
      ImGui::End();

  }
  }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_ground.terminateGL();
  m_couch.terminateGL();
  m_lamp1.terminateGL();
  m_lamp2.terminateGL();
  m_lamp3.terminateGL();
  m_lamp4.terminateGL();
  m_frontwall.terminateGL();
  m_backwall.terminateGL();
  m_leftwall.terminateGL();
  m_rightwall.terminateGL();
  m_roof.terminateGL();
  m_quadro1.terminateGL();
  m_quadro2.terminateGL();
  m_escr1.terminateGL();
  m_escr2.terminateGL();  
  m_table.terminateGL();
  m_tv.terminateGL();
  m_chandelier.terminateGL();
  m_slender.terminateGL();
  m_interr1.terminateGL();
  m_interr2.terminateGL();

  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Atualiza a camera
  m_camera.dolly(m_modSpeed * m_dollySpeed * deltaTime);
  m_camera.truck(m_modSpeed * m_truckSpeed * deltaTime);
  checkLight();
}

//Renderiza um modelo
void OpenGLWindow::render_model(Model *item, float angle, glm::vec3 axis, glm::vec3 position, float scale_size, bool lamp){
  
  //Uso de vetor para definir iluminação da lampada (que deve estar iluminada olhando de baixo para cima se estiver acesa)
  glm::vec4 usedLight;

  if(lamp && m_on){
    usedLight = glm::vec4{m_lightDir.x, 1.0f, m_lightDir.z, 0.0f};
  }else{
    usedLight = glm::vec4{m_lightDir.x, m_lightDir.y, m_lightDir.z, 0.0f};
  }
  item->m_position = position;
  item->m_scale = scale_size;

  glm::mat4 model{1.0f};
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(angle), axis);
  model = glm::scale(model, glm::vec3(scale_size));

  glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

  glUniform1i(m_diffuseTexLoc, 0);
  glUniform1i(m_mappingModeLoc, 3);  // mesh
  glUniform4f(m_colorLoc, 1.0f, 0.25f, 0.25f, 1.0f);

  glUniform4fv(m_lightDirLoc, 1, &usedLight.x);
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

//Checa se câmera está proxima a interruptor e atualiza estado de iluminação
void OpenGLWindow::checkLight() {
  //Percorre interruptores para checar se está proximo a algum deles
  for (Model* model : m_interruptors){

    if(!checkCollision_individual(model, m_camera.m_eye)){
      model->m_far = true;
    }else{
      if(model->m_far && m_on){
        m_on = false;
        model->m_far = false;

      }else if(model->m_far && !m_on){
        m_on = true;
        model->m_far = false;
      }
    }
  }
  
  //Atualiza direção da luz de acordo com o vetor forward para causar máxima impressão de
  //Iluminação na sala inteira (por uma lâmpada)
  if (m_on){
      glm::vec3 forward = glm::vec3(m_camera.m_at-m_camera.m_eye);
      m_lightDir = glm::vec4(forward.x, -1.0f, forward.z, 0.0f);
  }else{
      m_lightDir = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

bool OpenGLWindow::checkCollisions(glm::vec3 currentPos){
  for (Model* model : m_collisions){
    if(checkCollision_individual(model, currentPos)){
      m_lightDir = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
      return true;
    }
  }
  return false;
}

bool OpenGLWindow::checkCollision_individual(Model* model, glm::vec3 currentPos){
    float profundidade = (model->m_max_z - model->m_min_z)*model->m_scale;
    float comprimento = (model->m_max_x - model->m_min_x)*model->m_scale;

    float pos_x = model->m_position.x + comprimento/2;
    float neg_x = model->m_position.x - comprimento/2;

    float pos_z = model->m_position.z + profundidade/2;
    float neg_z = model->m_position.z - profundidade/2;

    if(pos_z == neg_z){
      pos_z = pos_z + 0.15f;
      neg_z = neg_z - 0.15f;
    }

  if((currentPos.x > neg_x && currentPos.x < pos_x) && (currentPos.z > neg_z && currentPos.z < pos_z)) {
    return true;
  }else{
    return false;
  }
}