/**
 * @file task05: Gradient Descent Method for Mass-Spring Simulation
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_eigen.h"
#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"
#include "../src/pba_floor_drawer.h"
#include "../src/pba_eigen_gl.h"

void wdw_spring_3d(
    float &w,
    Eigen::Vector3f dw[2],
    const Eigen::Vector3f node2xyz[2],
    const float length_ini,
    const float stiffness) {
  constexpr int num_node = 2;
  const float length = (node2xyz[0] - node2xyz[1]).norm(); // distance between p0 and p1
  const float C = length - length_ini; // the length differences.
  w = 0.5f * stiffness * C * C; // Hooke's law. energy is square of length difference W=1/2*k*C*C

  // write a few lines of code below to compute the gradient of elastic energy of this spring
  // with respect to the positions of the two end points.
}

float gradient_descent_energy_minimization(
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz_ini,
    const Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor> &line2vtx,
    float stiffness,
    float mass_point,
    const Eigen::Vector3f &gravity,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &aBCFlag,
    float learning_rate) { // simulation
  const unsigned int num_vtx = vtx2xyz.rows(); // number of vertices
  float W = 0.0; // energy of the system
  Eigen::MatrixX3f gradW = Eigen::MatrixX3f::Zero(num_vtx, 3); // gradient of the energy
  for (int i_line = 0; i_line < line2vtx.rows(); ++i_line) { // loop over springs
    const int i_vtx0 = line2vtx(i_line, 0); // index of vertex 0
    const int i_vtx1 = line2vtx(i_line, 1); // index of vertex 1
    const float length_ini = (vtx2xyz_ini.row(i_vtx0) - vtx2xyz_ini.row(i_vtx1)).norm(); // initial length
    const Eigen::Vector3f node2xyz[2] = {vtx2xyz.row(i_vtx0), vtx2xyz.row(i_vtx1)}; // coordinates of end points
    float w; // energy of one spring
    Eigen::Vector3f dw[2] = { Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero() }; // gradient of the energy of one spring
    wdw_spring_3d( // compute energy and its gradient of a spring
        w, dw,
        node2xyz, length_ini, stiffness);
    W += w;
    // merge gradient
    for (unsigned int i_node = 0; i_node < 2; ++i_node) {
      int i_vtx = line2vtx(i_line, i_node);
      gradW.row(i_vtx) += dw[i_node];
    }
  }
  // adding gravitational potential energy and its gradient
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    gradW.row(i_vtx) -= mass_point * gravity;
    W -= mass_point * vtx2xyz.row(i_vtx).dot(gravity);
  }
  // adding boundary condition
  gradW = gradW.cwiseProduct(aBCFlag);
  vtx2xyz -= gradW * learning_rate;
  return W;
}

int main() {
  constexpr float learning_rate = 6.5e-3f;
  constexpr int num_theta = 64;
  const auto[tri2vtx, vtx2xyz_ini] = pba::generate_mesh_annulus3(0.3, 0.8, 32, num_theta);
  const auto line2vtx = pba::lines_of_mesh(tri2vtx, static_cast<int>(vtx2xyz_ini.rows()));
  auto vtx2xyz = vtx2xyz_ini;

  // whether the DoFs of vertices are fixed or not. Fixed: 0, Free:1
  Eigen::MatrixX3f vtx2isfree = Eigen::MatrixX3f::Ones(vtx2xyz.rows(), 3);
  for (int i = 0; i < num_theta; ++i) {
    vtx2isfree.row(i) = Eigen::Vector3f(0., 0., 0);
  }

  GLFWwindow *window = pba::window_initialization("task05: Gradient Descent Method for Mass-Spring Simulation");
  pba::FloorDrawer floor(1.0, -1.5);

  ::glEnable(GL_DEPTH_TEST);
  pba::set_some_lighting();

  while (!::glfwWindowShouldClose(window)) {
    for (int itr = 0; itr < 40; ++itr) {
      float W = gradient_descent_energy_minimization(
          vtx2xyz, vtx2xyz_ini, line2vtx, 60.f, 1.f, {0., -0.1, 0}, vtx2isfree, learning_rate);
      if (itr == 0) {
        std::cout << "energy of the system " << W << std::endl;
      }
    }

    pba::default_window_3d(window); // start window for 3D visualization

     // set view angle
    ::glMatrixMode(GL_MODELVIEW);
    ::glTranslatef(0.0, 0.8, 0.0);
    ::glRotatef(50, 1.0, 0.0, 0.0);

    // draw floor with shadow
    floor.draw_shadow_pre();
    pba::draw_triangle_3d_mesh(tri2vtx, vtx2xyz);
    floor.draw_shadow_pos();

    // draw edges
    ::glDisable(GL_LIGHTING);
    ::glColor3d(0.0, 0.0, 0.0);
    pba::draw_line_3d_mesh(line2vtx, vtx2xyz);

    // draw faces
    ::glEnable(GL_LIGHTING);
    ::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, std::array<float,4>{0.0, 1.0, 1.0, 1.0}.data());
    pba::draw_triangle_3d_mesh(tri2vtx, vtx2xyz);

    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
