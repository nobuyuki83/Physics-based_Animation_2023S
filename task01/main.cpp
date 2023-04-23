/**
 * @file task01: implicit time integration
 * Let's solve the equation of motion
 * ddr/ddt =  - 1 / r^2
 */

#include <cstdlib>
#include <vector>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"

/**
 * explicit time integration
 * @param p0 input radius and its velocity
 * @param dt time step
 * @return output radius and its velocity
 */
Eigen::Vector2f time_integration_explicit(const Eigen::Vector2f& p0, float dt){
  float r0 = p0.x(); // current radius
  float v0 = p0.y(); // current radius velocity
  float f0 = -1.0f / (r0*r0);
  return {r0 + dt * v0, v0 + dt * f0};
}

/**
 * implicit time integration (assignment)
 * @param p0 input radius and its velocity
 * @param dt time step
 * @return output radius and its velocity
 */
Eigen::Vector2f time_integration_implicit(const Eigen::Vector2f& p0, float dt){
  const float r0 = p0.x(); // current radius
  const float v0 = p0.y(); // current radius velocity
  const float dfdr = 2.f/(r0*r0*r0); // hint!
  float f0 = -1.0f / (r0*r0); // force
  Eigen::Matrix2f A;
  Eigen::Vector2f b;
  // modify the following two lines to implement implicit time integration
  A << 1.f, 0.f, 0.f, 1.f;
  b << r0, v0;
  return A.inverse()*b;
}

/**
 * reflecting ball at the ground
 * @param p0 input radius and its velocity
 * @return output radius and its velocity
 */
Eigen::Vector2f reflection(const Eigen::Vector2f& p0){
  if( p0.x() > 0.5f ){ return p0; }
  float r0 = p0.x();
  float v0 = p0.y();
  float E = 0.5f*v0*v0 - 1.f/r0; // energy before reflection
  return {0.5, std::sqrt(2.f*E+4.f)}; // energy preserving reflection
}

void draw_polyline(const std::vector<Eigen::Vector2f>& ps) {
  ::glBegin(GL_LINE_STRIP);
  for(auto p : ps){
    float r = p.x();
    float t = p.y();
    ::glVertex2f(r*cos(t), r*sin(t));
  }
  ::glEnd();
}

int main()
{
  GLFWwindow* window = pba::window_initialization("task01: implicit integration");

  float time = 0.f;
  float dt = 0.04f; // time step
  Eigen::Vector2f phase_explicit(0.7f, 0.f);
  Eigen::Vector2f phase_implicit(0.7f, 0.f);
  std::vector<Eigen::Vector2f> history_explicit, history_implicit;

  while ( !::glfwWindowShouldClose(window) )
  {
    pba::default_window_2d(window);

    if( history_implicit.size() < 300 ) {
      phase_explicit = time_integration_explicit(phase_explicit, dt);
      phase_explicit = reflection(phase_explicit);

      phase_implicit = time_integration_implicit(phase_implicit, dt);
      phase_implicit = reflection(phase_implicit);
      time += dt;

      history_explicit.emplace_back(phase_explicit.x(), time);
      history_implicit.emplace_back(phase_implicit.x(), time);
    }

    // -----------------------
    // below: draw routine

    ::glLineWidth(2.f);
    ::glColor3d(0.0, 0.0, 1.0);
    pba::draw_circle_solid(phase_explicit.x() * std::cos(time), phase_explicit.x() * std::sin(time), 0.05f);
    draw_polyline(history_explicit);
    ::glColor3d(1.0, 0.0, 0.0);
    pba::draw_circle_solid(phase_implicit.x() * std::cos(time), phase_implicit.x() * std::sin(time), 0.05f);
    draw_polyline(history_implicit);
    ::glColor3d(0.0, 1.0, 0.0);
    pba::draw_circle_wireframe(0.f, 0.f, 0.5f);

    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
