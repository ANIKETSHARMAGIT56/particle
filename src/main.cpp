#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <csignal>
#include <fstream>
#include <iostream>
#include <iterator>
#include <linux/limits.h>
#include <math.h>
#include <ostream>
#include <random>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <glwrap.h>

#include "glm/trigonometric.hpp"
#include "indexbuffer.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "vertexbufferlayout.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}
struct vertex {
  glm::vec2 position = {0.0f,0.0f};
  float color = 1.0f;
} __attribute__((packed));

struct particle {
  glm::vec2 position = glm::vec2(0,-50);
  float life_time = -1.0f;
  float opacity = 1.0f;
  glm::vec2 initial_velocity = {1.0f, 1.0f};
};

int get_random(int lower , int upper){
	return lower + (rand() % (upper-lower));
}

float length =0.7;
const glm::vec2 particle_geometry[] = {
    glm::vec2{0.0f, length *sqrt(3) / 4},
    glm::vec2{length / 2, -length *sqrt(3) / 4},
    glm::vec2{-length / 2, -length *sqrt(3) / 4},
};
const unsigned int particle_geometry_indices[] = {0, 1, 2};
const unsigned int max_particles = 10000;

glm::vec2 projection_upper_limit ={80, 80};
glm::vec2 projection_lower_limit = {-100, -100};
const int window_height = 1000;
const int window_width = 1000;

        double cursor_pos_x =0;
        double cursor_pos_y =0;

class Particles {
  public:

    vertex vertex_buffer[max_particles][sizeof(particle_geometry)/sizeof(glm::vec2)];
    unsigned int index_buffer[max_particles][sizeof(particle_geometry_indices)/sizeof(unsigned int)];

    particle m_particle_vector[max_particles];
    Particles(){
      for (size_t i = 0; i < max_particles; i++)
      {
        for (size_t j = 0; j < sizeof(particle_geometry_indices)/sizeof(unsigned int); j++)
        {
          index_buffer[i][j]= (i* sizeof(particle_geometry_indices)/sizeof(unsigned int)) + particle_geometry_indices[j];
        }
      }
    }



  public:
    void put(unsigned int index, particle pt){
      m_particle_vector[index] = pt;
      auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(pt.position.x,pt.position.y,1.0f));
      for (size_t i = 0; i < sizeof(particle_geometry)/sizeof(glm::vec2); i++)
      {
        vertex vert;
        vert.color = pt.opacity;
        if (pt.life_time < 0)
        {
          vert.position = glm::vec2(1.0f);
        }
        else{
        vert.position = transform * glm::vec4 (particle_geometry[i],1.0f,1.0f);
        }
        vertex_buffer[index][i] = vert;
      }
    }

    void update(unsigned int index, glm::vec2 velocity, float delta_time){
      particle &pt =m_particle_vector[index];
      pt.position = glm::vec2 ( pt.position.x + (velocity.x*delta_time),pt.position.y + (velocity.y*delta_time));
      pt.life_time = 10;
      put(index, pt);
    }
};


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  // turn the mouse coordinates between 0 to 1
  cursor_pos_x =  xpos/window_width;
  cursor_pos_y = (window_height-ypos)/window_height;

  cursor_pos_x = (cursor_pos_x *( projection_upper_limit.x - projection_lower_limit.x))+ projection_lower_limit.x;
  cursor_pos_y = (cursor_pos_y *( projection_upper_limit.y - projection_lower_limit.y))+ projection_lower_limit.y;
}


void draw_particles(std::vector<particle> &particles) {}

int main() {
  	srand((unsigned) time(NULL));
  // glfw setup
  GLFWwindow *window;
  glfwSetErrorCallback([](int, const char *err_str) {
    std::cout << "GLFW Error: " << err_str << std::endl;
  });
  if (!glfwInit()) {
    std::cout << "glfw failed to initialize" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);

  if (!window) {
    glfwTerminate();
    std::cout << "failed to create window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);
  std::cout << "openGL version:" << glGetString(GL_VERSION) << std::endl;
  glfwSwapInterval(1);

  // glfwSetKeyCallback(window, key_callback);
glfwSetCursorPosCallback(window, cursor_position_callback);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // imgui styling
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char *glsl_version = "#version 130";
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // glEnable(GL_DEBUG_OUTPUT);
  // glDebugMessageCallback(MessageCallback, 0);

  glewInit();
 // define the range

  std::vector<particle> particles_vector;


  Particles particles;


  GLwrap::VertexBuffer vbo(nullptr, sizeof(particles.vertex_buffer));

  vbo.Bind();

  GLwrap::IndexBuffer ibo(nullptr, sizeof(particles.index_buffer)/sizeof(unsigned int));
  ibo.Bind();

  GLwrap::VertexBufferLayout layout;
  layout.Push(2);
  layout.Push(1);

  GLwrap::VertexArray vao;
  vao.Bind();
  vao.AddVertexBuffer(vbo, layout);

  GLwrap::Shader shaderCode;
  shaderCode.LoadShader(GL_VERTEX_SHADER,
                        "/home/aniket/code/particle/shader.vert");
  shaderCode.LoadShader(GL_FRAGMENT_SHADER,
                        "/home/aniket/code/particle/shader.frag");
  shaderCode.CreateShader();
  shaderCode.Bind();

  GLwrap::Shader::uniform u_projection(shaderCode, "u_projection");
  u_projection = glm::ortho(projection_lower_limit.x, projection_upper_limit.x, projection_lower_limit.y, projection_upper_limit.y);

  GLwrap::Renderer renderer;



  int particle_no =0;
  float delta_time =0.017;
  float milliseconds_elapsed =0.0f;
  int no_of_particles_spawned = 0;
  float time_to_spawn_all_particles = 1.0f;
  
  glm::vec2 rand_x_velocity_range = {-100.0f, 100.0f};
  glm::vec2 rand_y_velocity_range = {-20.0f, 60.0f};
    srand((unsigned) time(NULL));
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();



    milliseconds_elapsed += delta_time*1000;
  int particles_per_frame = (max_particles/io.Framerate)/time_to_spawn_all_particles; 



      for( int i = 0;(i < particles_per_frame) && (no_of_particles_spawned < max_particles); i++)
      {
        particles.m_particle_vector[no_of_particles_spawned].position = {cursor_pos_x,cursor_pos_y};
      particles.update(no_of_particles_spawned,{get_random(rand_x_velocity_range.x,rand_x_velocity_range.y ),get_random(rand_y_velocity_range.x,rand_y_velocity_range.y )} , delta_time);
      no_of_particles_spawned++;
      milliseconds_elapsed = 0; 
      }
      // std::cout << no_of_particles_spawned<< std::endl;
    
    for (size_t i = 0; i < no_of_particles_spawned; i++)
    {
      if (particles.m_particle_vector[i].opacity < 0)
      {
        particles.m_particle_vector[i].position = {cursor_pos_x,cursor_pos_y};
        particles.m_particle_vector[i].opacity =1;
      }
        particles.update(i,{get_random(rand_x_velocity_range.x,rand_x_velocity_range.y ),get_random(rand_y_velocity_range.x, rand_x_velocity_range.y)} , delta_time);
        particles.m_particle_vector[i].life_time --;
        particles.m_particle_vector[i].opacity -= 1 *delta_time;
    }


    renderer.Clear(); 



    vbo.Update(&particles.vertex_buffer[0][0],sizeof(particles.vertex_buffer));


    ibo.Update(&particles.index_buffer, max_particles*sizeof(particle_geometry_indices)/sizeof(unsigned int));
    // ibo.Update(&particle_geometry_indices, 3);


    renderer.Draw(vao, ibo, shaderCode);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {

      ImGui::Begin("debug window");

      ImGui::Text("view transform");

      ImGui::Text("cursor pos %f,  %f",cursor_pos_x,cursor_pos_y);
      ImGui::Text("particles spawned %d",no_of_particles_spawned);


      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1/io.Framerate, io.Framerate);
      ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

// i++;
  
    delta_time = 1/io.Framerate;
  }
}