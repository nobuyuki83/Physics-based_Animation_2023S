/**
 * @file task02: Linear Momentum Conservation
 * Collision between particle and ball, simulating Brownian motion
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"

/**
 * particle class (radius = 0)
 */
class Particle {
 public:
  Eigen::Vector2f pos; //! position
  Eigen::Vector2f velo; //! velocity
  Eigen::Vector3f color; //! color
};

/**
 * collision between a particle and a ball
 * @param [in,out] p particle class (position & velocity etc..)
 * @param [in] particle_mass particle mass
 * @param [in,out] ball_pos ball position
 * @param [in,out] ball_velo ball velocity
 * @param [in] ball_mass ball mass
 * @param [in] ball_rad ball rad
 */
void collide_particle_ball(
    Particle &p,
    float particle_mass,
    Eigen::Vector2f &ball_pos,
    Eigen::Vector2f &ball_velo,
    float ball_mass,
    float ball_rad) {
  if ((p.pos - ball_pos).norm() > ball_rad) { return; }
  const Eigen::Vector2f plane_norm = (p.pos - ball_pos).normalized();
  const Eigen::Vector2f plane_org = ball_pos + plane_norm * ball_rad;
  float height = (p.pos - plane_org).dot(plane_norm);
  p.pos -= height * 2 * plane_norm;

  //--------------------------------------------------------------------- //
  // ASSIGNMENT: implement the collision between ball and particle.
  //             The coefficient of restitution is 1, e.g., the magnitude
  //             of relative velocity does not change after the collision.
  //             The linear momentum should be conserved.
  //             The force between ball and particle should only in the normal
  //             direction (e.g, `plane_norm`). In other words, there is
  //             no friction. You do not need to change the positions.

  // comment out the line below
  p.velo -= 2.f * (p.velo - ball_velo).dot(plane_norm) * plane_norm;

  // write a few lines of code to compute the velocity of ball and particle
  // please uncomment the lines below
  // const Eigen::Vector2f impulse =
  // p.velo +=
  // ball_velo +=
}

/**
 * collision between a circle and plane
 * @param [in,out] pos  circle position
 * @param [in, out] velo  circle velocity
 * @param [in] rad  circle radius
 * @param [in] plane_org  plane origin
 * @param [in] plane_nrm  plane normal
 */
void collision_circle_plane(
    Eigen::Vector2f &pos,
    Eigen::Vector2f &velo,
    const float rad,
    const Eigen::Vector2f &plane_org,
    const Eigen::Vector2f &plane_nrm) {
  const float height = (pos - plane_org).dot(plane_nrm) - rad;
  if (height > 0.f) { return; }
  pos -= height * 2 * plane_nrm;
  const float velo_perp = velo.dot(plane_nrm);
  velo -= 2.f * velo_perp * plane_nrm;
}

int main() {
  GLFWwindow *window = pba::window_initialization("task02: Linear Momentum Conservation");

  const float box_size = 1.5;

  // ball information
  const float ball_rad = 0.3;
  const float ball_mass = 10.f;
  Eigen::Vector2f ball_pos(0.f, 0.f);
  Eigen::Vector2f ball_velo(0.f, 0.f);
  std::vector<Eigen::Vector2f> trajectory;

  // particle information
  std::vector<Particle> particles(100);
  for (auto &p: particles) {
    // initialization
    // particle should be put outside the ball at the beginning
    for (unsigned int itr = 0; itr < 100; ++itr) {
      p.pos.setRandom();
      p.pos *= box_size * 0.5f;
      if (p.pos.norm() > ball_rad) { break; }
    }
    // velocity is random but its magnitude is 1
    p.velo.setRandom();
    p.velo.normalize();
    // random RGB color
    p.color.setRandom();
    p.color = p.color * 0.5 + Eigen::Vector3f(0.5f, 0.5f, 0.5f);
  }
  const float particle_mass = 1.0f;

  float dt = 0.01f;

  while (!::glfwWindowShouldClose(window)) {
    pba::default_window_2d(window);

    if (trajectory.size() < 3000) {
      ball_pos += ball_velo * dt; // step time for ball
      // collision between ball and the walls
      collision_circle_plane(ball_pos, ball_velo, ball_rad, {-box_size * 0.5f, 0.f}, {+1.f, 0.f}); // left wall
      collision_circle_plane(ball_pos, ball_velo, ball_rad, {0.f, -box_size * 0.5f}, {0.f, 1.f});  // bottom wall
      collision_circle_plane(ball_pos, ball_velo, ball_rad, {+box_size * 0.5f, 0.f}, {-1.f, 0.f}); // right wall
      collision_circle_plane(ball_pos, ball_velo, ball_rad, {0.f, +box_size * 0.5f}, {0.f, -1.f}); // top wall
      for (auto &p: particles) {
        p.pos += p.velo * dt; // step time for a particle
        // collision between a particle and the walls
        collision_circle_plane(p.pos, p.velo, 0.f, {-box_size * 0.5f, 0.f}, {+1.f, 0.f}); // left wall
        collision_circle_plane(p.pos, p.velo, 0.f, {0.f, -box_size * 0.5f}, {0.f, 1.f});  // bottom wall
        collision_circle_plane(p.pos, p.velo, 0.f, {+box_size * 0.5f, 0.f}, {-1.f, 0.f}); // right wall
        collision_circle_plane(p.pos, p.velo, 0.f, {0.f, +box_size * 0.5f}, {0.f, -1.f}); // top wall
        collide_particle_ball(p, particle_mass,
                              ball_pos, ball_velo, ball_mass, ball_rad);
      }
      trajectory.push_back(ball_pos);
    }

    // -----------------------
    // below: draw routine

    ::glLineWidth(2);
    ::glColor3d(1.0, 0.0, 0.0);
    pba::draw_circle_wireframe(ball_pos.x(), ball_pos.y(), ball_rad);
    ::glLineWidth(1);
    pba::draw_polyline(trajectory);

    ::glColor3f(0.0f, 0.0f, 0.0f);
    pba::draw_box_wireframe(0.f, 0.f, box_size, box_size);

    ::glPointSize(5);
    ::glBegin(GL_POINTS);
    for (const auto &p: particles) {
      ::glColor3f(p.color.x(), p.color.y(), p.color.z());
      ::glVertex2f(p.pos.x(), p.pos.y());
    }
    ::glEnd();

    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
