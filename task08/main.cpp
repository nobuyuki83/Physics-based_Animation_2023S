/**
 * @file task08: Controlling Volume of a Mesh using Lagrange-Multiplier Method
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>
#include <filesystem>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"
#include "../src/pba_floor_drawer.h"
#include "../src/pba_eigen_gl.h"
#include "../src/pba_util_eigen.h"

#ifndef M_PI
  #define M_PI 3.14159265358979323846264338327950288
#endif

auto load_my_bunny() {
  auto[tri2vtx, vtx2xyz] = pba::load_wavefront_obj(std::filesystem::path(PATH_SOURCE_DIR) / "bunny_1k.obj");
  { // center-ize
    auto center = (vtx2xyz.colwise().maxCoeff() + vtx2xyz.colwise().minCoeff()) * 0.5;
    vtx2xyz.rowwise() -= center;
  }
  { // normalize the size
    auto size = (vtx2xyz.colwise().maxCoeff() - vtx2xyz.colwise().minCoeff()).maxCoeff();
    vtx2xyz /= size;
    vtx2xyz *= 1.5;
  }
  { // rotate in x-axis
    Eigen::Matrix3f m;
    m = Eigen::AngleAxis<float>(-M_PI * 0.5f, Eigen::Vector3f::UnitX());
    vtx2xyz = (m * vtx2xyz.transpose()).transpose();
  }
  // translate in y-axis
  vtx2xyz.rowwise() += Eigen::Vector3f(0, -0.8, 0).transpose();
  return std::make_pair(tri2vtx, vtx2xyz);
}

/**
 * compute elastic potential energy, energy's gradient, and energy's hessian
 * @param w energy
 * @param dw gradient
 * @param ddw hessian
 * @param node2xyz node's xyz-coordinates
 * @param length_ini initial length
 * @param stiffness spring's stiffness parameter
 */
void wdwddw_spring(
    double &w,
    Eigen::Vector3d dw[2],
    Eigen::Matrix3d ddw[2][2],
    const Eigen::Vector3d node2xyz[2],
    double length_ini,
    double stiffness) {
  double length = (node2xyz[1] - node2xyz[0]).norm();
  double C = length - length_ini;
  w = 0.5 * stiffness * C * C;
  Eigen::Vector3d dC = (node2xyz[0] - node2xyz[1]).normalized();
  Eigen::Matrix3d ddC = (1. / length) * (Eigen::Matrix3d::Identity() - dC * dC.transpose());
  dw[0] = stiffness * C * dC;
  dw[1] = -dw[0];
  ddw[0][0] = stiffness * dC * dC.transpose() + stiffness * C * ddC;
  ddw[1][1] = ddw[0][0];
  ddw[1][0] = -ddw[0][0];
  ddw[0][1] = -ddw[0][0];
}

/**
 * compute volume of a tetrahedron connecting vertices of the triangle and the origin
 * @param w volume
 * @param dw differentiation of volume w.r.t the nodes' xyz coordinates
 * @param node2xyz nodes' xyz coordinates
 */
void wdw_volume_tri_origin(
    double &w,
    Eigen::Vector3d dw[3],
    const Eigen::Vector3d node2xyz[3]) {
  w = 1./6.*node2xyz[0].dot(node2xyz[1].cross(node2xyz[2]));
  // ------------------------------
  // Write some code below to compute differentiation. Keep it simple and understandable
  // dw[0] =
  // dw[1] =
  // dw[2] =
}

void inflate(
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz,
    double &lambda,
    double volume_trg,
    const Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> &tri2vtx,
    const Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor> &line2vtx,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz_ini) {
  double stiffness = 1.0;
  unsigned int num_vtx = vtx2xyz.rows();
  Eigen::MatrixXd ddW(num_vtx * 3 + 1, num_vtx * 3 + 1);
  Eigen::VectorXd dW(num_vtx * 3 + 1);
  ddW.setZero();
  dW.setZero();
  // setting mass-spring system as a regularizer
  double elastic_energy = 0.0;
  for (unsigned int i_line = 0; i_line < line2vtx.rows(); ++i_line) {
    const int node2vtx[2] = {  // index of two end points
        line2vtx(i_line, 0),
        line2vtx(i_line, 1)};
    float length_ini = (vtx2xyz_ini.row(node2vtx[0]) - vtx2xyz_ini.row(node2vtx[1])).norm();
    const Eigen::Vector3d node2xyz[3] = {  // xyz-coordinates of two nodes
        vtx2xyz.row(node2vtx[0]).cast<double>(),
        vtx2xyz.row(node2vtx[1]).cast<double>()};
    double w = 0;
    Eigen::Vector3d dw[2];
    Eigen::Matrix3d ddw[2][2];
    wdwddw_spring(w, dw, ddw,
                  node2xyz, length_ini, stiffness);
    elastic_energy += w;
    for (unsigned int inode = 0; inode < 2; ++inode) {
      for (unsigned int idim = 0; idim < 3; ++idim) {
        for (unsigned int jnode = 0; jnode < 2; ++jnode) {
          for (unsigned int jdim = 0; jdim < 3; ++jdim) {
            ddW(node2vtx[inode] * 3 + idim, node2vtx[jnode] * 3 + jdim) += ddw[inode][jnode](idim, jdim);
          }
        }
        dW(node2vtx[inode] * 3 + idim) += dw[inode](idim);
      }
    }
  }
  // setting constraint
  double volume = 0.0;
  for (unsigned int i_tri = 0; i_tri < tri2vtx.rows(); ++i_tri) {
    const int node2vtx[3] = {
        tri2vtx(i_tri, 0),
        tri2vtx(i_tri, 1),
        tri2vtx(i_tri, 2)};
    const Eigen::Vector3d node2xyz[3] = {
        vtx2xyz.row(node2vtx[0]).cast<double>(),
        vtx2xyz.row(node2vtx[1]).cast<double>(),
        vtx2xyz.row(node2vtx[2]).cast<double>()};
    double w = 0.0;
    Eigen::Vector3d dw[3];
    wdw_volume_tri_origin(
        w, dw,
        node2xyz);
    volume += w;
    // -------------------------
    // write some code below to set values in the linear system to set constraint to specify volume
    // write some code including 'w'
    for (unsigned int inode = 0; inode < 3; ++inode) {
      for (unsigned int idim = 0; idim < 3; ++idim) {
        // write some code including `dw` and `lambda`
      }
    }
  }
  // Do not forget to write one line of code here
  // -------------------------------------------------
  // Do not change below
  // damping for stable convergence
  for (unsigned int i = 0; i < ddW.rows(); ++i) {
    ddW(i, i) += 0.1;
  }
  std::cout << "   elastic_energy (write down in the README.md): " << elastic_energy << std::endl;
  std::cout << "   current volume: " << volume << "  target volume: " << volume_trg << std::endl;
  std::cout << "   residual (make sure this number get smaller in each iteration): " << dW.squaredNorm() << std::endl;
  // solve linear system
  Eigen::VectorXd upd = ddW.inverse() * dW;
  // update solution
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    for (unsigned int idim = 0; idim < 3; ++idim) {
      vtx2xyz(i_vtx, idim) -= static_cast<float>(upd(i_vtx * 3 + idim));
    }
  }
  lambda -= upd(num_vtx*3);
  std::cout << "   lambda (make sure this number converges): " << lambda << std::endl;
}

int main() {
  const auto[tri2vtx, vtx2xyz_ini] = load_my_bunny();
  const auto line2vtx = pba::lines_of_mesh(tri2vtx, static_cast<int>(vtx2xyz_ini.rows()));
  auto vtx2xyz = vtx2xyz_ini;

  double volume_ini = 0.0;
  for (unsigned int i_tri = 0; i_tri < tri2vtx.rows(); ++i_tri) {
    const Eigen::Vector3d node2xyz[3] = {
        vtx2xyz.row(tri2vtx(i_tri, 0)).cast<double>(),
        vtx2xyz.row(tri2vtx(i_tri, 1)).cast<double>(),
        vtx2xyz.row(tri2vtx(i_tri, 2)).cast<double>()};
    double w = 0.0;
    Eigen::Vector3d dw[3];
    wdw_volume_tri_origin(
        w, dw,
        node2xyz);
    volume_ini += w;
  }
  const double volume_trg = volume_ini * 2.0;
  double lambda = 0.0;

  for(unsigned int itr=0;itr<10;++itr){
    std::cout << "iteration: " << itr << std::endl;
    inflate(vtx2xyz, lambda, volume_trg, tri2vtx, line2vtx, vtx2xyz_ini);
  }

  GLFWwindow *window = pba::window_initialization("task08: Controlling Volume of a Mesh using Lagrange-Multiplier Method");
  pba::FloorDrawer floor(1.0, -1.5);

  ::glEnable(GL_DEPTH_TEST);
  pba::set_some_lighting();

  while (!::glfwWindowShouldClose(window)) {
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
