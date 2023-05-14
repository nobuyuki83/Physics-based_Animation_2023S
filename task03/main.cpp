/**
 * @file task03: Acceleration of N-Body Simulation
 * Collision between particle and ball, simulating Brownian motion
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
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
  Eigen::Vector2f force; //! force from all the other particles
};

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

/**
 * position to grid coordinate
 * @param [in] pos input position
 * @param [in] box_size size of square box
 * @param [in] num_div number of division for grid
 * @return grid coordinate
 */
std::array<unsigned int,2> pos2grid(
    const Eigen::Vector2f &pos,
    float box_size,
    unsigned int num_div) {
  const auto fndiv = static_cast<float>(num_div);
  const auto ix = static_cast<unsigned int>((pos.x() / box_size + 0.5f) * fndiv);
  const auto iy = static_cast<unsigned int>((pos.y() / box_size + 0.5f) * fndiv);
  return {ix, iy};
}

/**
 * Gravitational force with softening
 * @param d relative position
 * @return force
 */
Eigen::Vector2f gravitational_force(const Eigen::Vector2f &d) {
  constexpr float eps = 2.0e-3; // softening coefficient
  float r = sqrt(d.squaredNorm() + eps * eps);
  return (1.f / (r * r * r)) * d;
}

/**
 * For each particle, set summation of gravitational forces from all the other particles in a brute-force way O(N^2)
 * @param [in,out] particles
 */
void set_force_bruteforce(std::vector<Particle> &particles) {
  for (unsigned int ip = 0; ip < particles.size(); ++ip) {
    particles[ip].force.setZero();
    for (unsigned int jp = 0; jp < particles.size(); ++jp) {
      if (ip == jp) { continue; }
      particles[ip].force += gravitational_force(particles[jp].pos - particles[ip].pos);
    }
  }
}

unsigned int abs_diff(unsigned int a, unsigned b) {
  return a > b ? a - b : b - a;
}

// pair of particle index and grid index
struct ParticleGridIndex {
  unsigned int particle_idx;
  unsigned int grid_idx;
};

// data structure for acceleration
class Acceleration {
 public:
  Acceleration(unsigned int num_particle, unsigned int num_div) {
    idx2pgi.resize(num_particle);
    grid2idx.resize(num_div * num_div + 1);
    grid2cg.resize(num_div * num_div);
  }
  std::vector<ParticleGridIndex> idx2pgi; // data of jagged array
  std::vector<unsigned int> grid2idx; // index of jagged array
  std::vector<Eigen::Vector2f> grid2cg; // the center of gravity of each grid
};

/**
 * For each particle, set summation of gravitational forces from all the other particles in an accelerated way
 * @param [in,out] particles particles
 * @param [in,out] acc acceleration data structure
 * @param [in] box_size box size
 * @param [in] num_div number of division of grid
 */
void set_force_accelerated(
    std::vector<Particle> &particles,
    Acceleration &acc,
    float box_size,
    unsigned int num_div) {
  // computation of acceleration data structure
  acc.idx2pgi.clear();
  for (unsigned int ip = 0; ip < particles.size(); ++ip) {
    auto[ix, iy] = pos2grid(particles[ip].pos, box_size, num_div);
    acc.idx2pgi.push_back({ip, iy * num_div + ix});
  }
  std::sort(acc.idx2pgi.begin(), acc.idx2pgi.end(),
            [](const ParticleGridIndex &lhs, const ParticleGridIndex &rhs) { return lhs.grid_idx < rhs.grid_idx; });
  std::fill(acc.grid2idx.begin(), acc.grid2idx.end(), 0);
  // construct index of the jagged array
  for (auto &pg: acc.idx2pgi) {
    acc.grid2idx[pg.grid_idx + 1]++;
  }
  for (unsigned int i = 0; i < num_div * num_div; ++i) {
    acc.grid2idx[i + 1] += acc.grid2idx[i];
  }
  // compute the center of the gravity
  for (unsigned int ix = 0; ix < num_div; ++ix) {
    for (unsigned int iy = 0; iy < num_div; ++iy) {
      acc.grid2cg[iy * num_div + ix].setZero();
      for (unsigned int idx = acc.grid2idx[iy * num_div + ix]; idx < acc.grid2idx[iy * num_div + ix + 1]; ++idx) {
        unsigned int ip = acc.idx2pgi[idx].particle_idx;
        acc.grid2cg[iy * num_div + ix] += particles[ip].pos;
      }
      unsigned int np = acc.grid2idx[iy * num_div + ix + 1] - acc.grid2idx[iy * num_div + ix];
      if (np == 0) { continue; }
      acc.grid2cg[iy * num_div + ix] /= static_cast<float>(np);
    }
  }
  //
  for (unsigned int ip = 0; ip < particles.size(); ++ip) {
    auto[ix, iy] = pos2grid(particles[ip].pos, box_size, num_div); // grid coordinate of particle with index `ip`
    particles[ip].force.setZero(); // initialize force
    // loop over all the grid set force to the particle with index `ip`
    for (unsigned int jx = 0; jx < num_div; ++jx) {
      for (unsigned int jy = 0; jy < num_div; ++jy) {
        if (abs_diff(ix, jx) <= 1 && abs_diff(iy, jy) <= 1) { // this grid is near to the particle as the grid indexes are close
          for (unsigned int idx = acc.grid2idx[jy * num_div + jx]; idx < acc.grid2idx[jy * num_div + jx + 1]; ++idx) {
            assert(acc.idx2pgi[idx].grid_idx == jy * num_div + jx);
            unsigned int jp = acc.idx2pgi[idx].particle_idx; // particle index
            if (ip == jp) { continue; }
            assert(pos2grid(particles[jp].pos, box_size, num_div)[0] == jx);
            assert(pos2grid(particles[jp].pos, box_size, num_div)[1] == jy);
            particles[ip].force += gravitational_force(particles[jp].pos - particles[ip].pos); // adding up forces from near particles
          }
        } else { // far field approximation
          // write a few lines of code here to compute the force from far grid.
          // use the center for the gravity of the grid : `acc.grid2cg[jy * num_div + jx]`
        }
      }
    }
  }
}

int main() {

  GLFWwindow *window = pba::window_initialization("task03: acceleration of n-body simulation");

  const float box_size = 1.8;
  constexpr unsigned int num_div = 30;

  // particle information
  std::vector<Particle> particles(5000); // change the number of particles here
  for (auto &p: particles) {
    // initialization
    p.pos.setRandom();
    p.pos *= box_size * 0.4f;
    // velocity is random but its magnitude is 1
    p.velo = {200 * p.pos.y(), -200 * p.pos.x()};
    // random RGB color
    p.color.setRandom();
    p.color = p.color * 0.5 + Eigen::Vector3f(0.5f, 0.5f, 0.5f);
  }

  Acceleration acceleration(particles.size(), num_div);
  constexpr float dt = 0.00002f; // time step

  unsigned int i_step = 0;
  constexpr int num_step = 200;

  std::chrono::system_clock::time_point start = std::chrono::system_clock::now(); // record starting time
  while (!::glfwWindowShouldClose(window)) {
    i_step++;
    pba::default_window_2d(window);
    if (i_step < num_step) {
      if( i_step % 20 == 0 ){ std::cout << i_step << " steps in " << num_step << " steps computed" << std::endl; }

      // switch brute-force/accelerated computation here by uncomment/comment below
      set_force_bruteforce(particles);
      // set_force_accelerated(particles, acceleration, box_size, num_div);

      for (auto &p: particles) {
        // leap frog time integration
        p.velo += p.force * dt; // update velocity
        p.pos += p.velo * dt; // update position
        // collision between particle and walls
        collision_circle_plane(p.pos, p.velo, 0.f, {-box_size * 0.5f, 0.f}, {+1.f, 0.f}); // left wall
        collision_circle_plane(p.pos, p.velo, 0.f, {0.f, -box_size * 0.5f}, {0.f, 1.f});  // bottom wall
        collision_circle_plane(p.pos, p.velo, 0.f, {+box_size * 0.5f, 0.f}, {-1.f, 0.f}); // right wall
        collision_circle_plane(p.pos, p.velo, 0.f, {0.f, +box_size * 0.5f}, {0.f, -1.f}); // top wall
      }
    } else if (i_step == num_step) {
      std::chrono::system_clock::time_point end = std::chrono::system_clock::now(); // record end time
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      std::cout << "total computation time: " << elapsed << "ms" << std::endl;
    }

    // -----------------------
    // below: draw routine

    ::glColor3f(0.0f, 0.0f, 0.0f);
    pba::draw_box_wireframe(0.f, 0.f, box_size, box_size);

    ::glPointSize(2);
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
