/**
 * @file task06: dynamic mass-spring system using variational Euler time integration
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
#include "../src/pba_block_sparse_matrix.h"

/**
 * compute the elastic potential energy, its gradient and its hessian of a 3D spring.
 * @param w elastic potential energy
 * @param dw gradient of energy
 * @param ddw hessian of energy
 * @param node2xyz xyz coordinates of two end points
 * @param length_ini initial length
 * @param stiffness Hock's spring coefficient
 */
void WdWddW_Spring3(
    double &w,
    Eigen::Vector3d dw[2],
    Eigen::Matrix3d ddw[2][2],
    const Eigen::Vector3d node2xyz[2],
    const double length_ini,
    const double stiffness) {
  constexpr int num_node = 2; // number of two end points
  const double length = (node2xyz[0] - node2xyz[1]).norm(); // distance between p0 and p1
  const double C = length - length_ini; // the length differences.
  w = 0.5f * stiffness * C * C; // Hooke's law. energy is square of length difference W=1/2*k*C*C
  //
  const Eigen::Vector3d u01 = (node2xyz[1] - node2xyz[0]).normalized();
  const Eigen::Vector3d dC[num_node] = {-u01, u01};
  for (int ino = 0; ino < num_node; ++ino) {
    dw[ino] = stiffness * dC[ino] * C; // dW = k*dC*C
  }
  // write code to correctly compute hessian of a spring.
  // ddw[i_node][j_node] stands for derivative of dw[i_node] w.r.t the end-point's position node2xyz[j_node]
  // the current hessian computed by the code below is not very accurate, so the simulation is unstable.
  const Eigen::Matrix3d n = stiffness * u01 * u01.transpose();
  ddw[0][0] = n;
  ddw[1][1] = n;
  ddw[0][1] = -n;
  ddw[1][0] = -n;
}

float step_time_mass_spring_system_with_variational_integration(
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz,
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2velocity,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz_ini,
    const Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor> &line2vtx,
    float stiffness,
    float mass_point,
    const Eigen::Vector3f &gravity,
    const Eigen::MatrixX3d &vtx2isfree,
    float dt,
    pba::BlockSparseMatrix<3> &sparse) { // simulation
  const unsigned int num_vtx = vtx2xyz.rows(); // number of vertices
  double W = 0.0; // energy of the system
  Eigen::MatrixX3d gradW = Eigen::MatrixX3d::Zero(num_vtx, 3); // gradient of the energy
  sparse.setZero();
  // step position using velocity
  vtx2xyz += vtx2velocity * dt;
  for (int i_line = 0; i_line < line2vtx.rows(); ++i_line) { // loop over springs
    const int i_vtx0 = line2vtx(i_line, 0); // index of vertex 0
    const int i_vtx1 = line2vtx(i_line, 1); // index of vertex 1
    const double length_ini = (vtx2xyz_ini.row(i_vtx0) - vtx2xyz_ini.row(i_vtx1)).norm(); // initial length
    const Eigen::Vector3d node2xyz[2] = { // coordinates of end points
        vtx2xyz.row(i_vtx0).cast<double>(),
        vtx2xyz.row(i_vtx1).cast<double>()};
    double w; // energy of one spring
    Eigen::Vector3d dw[2]; // gradient of the energy of one spring
    Eigen::Matrix3d ddw[2][2]; // hessian of energy of one spring
    WdWddW_Spring3(
        w, dw, ddw,
        node2xyz, length_ini, stiffness);
    // merge energy
    W += w;
    // merge gradient
    for (unsigned int i_node = 0; i_node < 2; ++i_node) {
      const int i_vtx = line2vtx(i_line, i_node);
      gradW.row(i_vtx) += dw[i_node];
    }
    // merge hessian
    for (unsigned int i_node = 0; i_node < 2; ++i_node) {
      for (unsigned int j_node = 0; j_node < 2; ++j_node) {
        const int i_vtx = line2vtx(i_line, i_node);
        const int j_vtx = line2vtx(i_line, j_node);
        sparse.coeff(i_vtx, j_vtx) += ddw[i_node][j_node];
      }
    }
  }
  // adding the dynamic effect
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    sparse.coeff(i_vtx, i_vtx) += Eigen::Matrix3d::Identity() * (mass_point / (dt * dt));
  }
  // adding gravitational potential energy and its gradient
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    gradW.row(i_vtx) -= (mass_point * gravity).cast<double>();
    W -= mass_point * vtx2xyz.row(i_vtx).dot(gravity);
  }
  // set free/fix
  gradW = gradW.cwiseProduct(vtx2isfree);
  sparse.set_is_free(vtx2isfree);
  // solve
  Eigen::MatrixX3d x = sparse.solve_conjugate_gradient(gradW);
  // step position and velocity
  vtx2velocity += -x.cast<float>() / dt;
  vtx2xyz -= x.cast<float>();
  return static_cast<float>(W);
}

int main() {
  // make geometry
  constexpr int num_theta = 64;
  const auto[tri2vtx, vtx2xyz_ini] = pba::generate_mesh_annulus3(0.3, 0.8, 32, num_theta);
  const auto line2vtx = pba::lines_of_mesh(tri2vtx, static_cast<int>(vtx2xyz_ini.rows()));
  auto vtx2xyz = vtx2xyz_ini;

  // initialize velocity
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> vtx2velocity(vtx2xyz.rows(), 3);
  vtx2velocity.setZero();

  // specify free DoF and fixed DoF
  Eigen::MatrixX3d vtx2isfree = Eigen::MatrixX3d::Ones(vtx2xyz.rows(), 3);
  for (int i = 0; i < num_theta; ++i) {
    vtx2isfree.row(i) = Eigen::Vector3d(0., 0., 0);
  }

  // block sparse matrix
  pba::BlockSparseMatrix<3> sparse_matrix;
  sparse_matrix.initialize(tri2vtx, vtx2xyz.rows());

  GLFWwindow *window = pba::window_initialization("task06: dynamic mass-spring system using variational Euler time integration");
  pba::FloorDrawer floor(1.0, -1.5);

  ::glEnable(GL_DEPTH_TEST);
  pba::set_some_lighting();

  const float dt = 0.13f;
  float current_time = 0.f;

  while (!::glfwWindowShouldClose(window)) {

    if(current_time < 40.0) {
      float W = step_time_mass_spring_system_with_variational_integration(
          vtx2xyz, vtx2velocity, vtx2xyz_ini, line2vtx, 60.f, 1.f, {0., -0.1, 0}, vtx2isfree, dt,
          sparse_matrix);
      current_time += dt;
      std::cout << "time: " << current_time << "   elastic_energy: " << W << std::endl;
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
    ::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, std::array<float, 4>{0.0, 1.0, 1.0, 1.0}.data());
    pba::draw_triangle_3d_mesh(tri2vtx, vtx2xyz);

    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
