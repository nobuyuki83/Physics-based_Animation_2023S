/**
 * @file task09: Rotation and Energy Minimization
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
 *
 * @param [out] rotation rotation matrix
 * @param [out] translation translation vector
 * @param [out] vtx2xyz the transformed coordinates of the mesh's vertices
 * @param [in] i_vtx_fix the index of the fixed vertex
 * @param [in] vtx2xyz_ini the initial coordinates of the mesh's vertices
 */
void step(
    Eigen::Matrix3f& rotation,
    Eigen::Vector3f& translation,
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& vtx2xyz,
    unsigned int i_vtx_fix,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& vtx2xyz_ini){
  Eigen::Vector3f gravity(0.f,-10.f,0.f);
  constexpr float penalty = 1.0e+6;
  constexpr float learning_rate = 1.0e-6;
  //
  vtx2xyz = ((rotation * vtx2xyz_ini.transpose()).colwise() + translation).transpose();
  // energy = 0.5*stiffness*||[R]*{X_fix}+{t}-{X_fix}||^2 - sum<{gravity}^T([R]*{X}+{t})>
  float E = 0.5f*penalty*(vtx2xyz.row(i_vtx_fix) - vtx2xyz_ini.row(i_vtx_fix)).squaredNorm() - (vtx2xyz * gravity).sum();
  std::cout << "energy: " << E << std::endl;
  Eigen::Vector3f dEdo = Eigen::Vector3f::Zero();
  Eigen::Vector3f dEdt = Eigen::Vector3f::Zero();
  {
    Eigen::Vector3f t0 = vtx2xyz.row(i_vtx_fix) -  vtx2xyz_ini.row(i_vtx_fix);
    dEdt += penalty*t0;
    dEdo += penalty*vtx2xyz_ini.row(i_vtx_fix).cross(t0.transpose() * rotation);
  }
  for(unsigned int i_vtx=0; i_vtx<vtx2xyz.rows(); ++i_vtx){
    // Write some code below to compute gradient of gravitational potential energy for each vertex
    // Code differentiation of energy w.r.t. translation and rotation for one line each.
    // For the differentiation w.r.t. rotation, observe how the rotation matrix will be updated at the line #83
    // dEdt +=
    // dEdo +=
    // do not change anything else except for the lines above.
  }
  translation -= learning_rate * dEdt;
  rotation = rotation * Eigen::AngleAxisf(-dEdo.norm()*learning_rate, dEdo.stableNormalized());
  vtx2xyz = ((rotation * vtx2xyz_ini.transpose()).colwise() + translation).transpose();
}

int main() {
  const auto[tri2vtx, vtx2xyz_ini] = load_my_bunny();
  const auto line2vtx = pba::lines_of_mesh(tri2vtx, static_cast<int>(vtx2xyz_ini.rows()));
  constexpr unsigned int ivtx_fix = 0;

  Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity(); // rotation to optimize
  Eigen::Vector3f translation = Eigen::Vector3f::Zero(); // translation to optimize
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> vtx2xyz = vtx2xyz_ini; // position after transformation

  GLFWwindow *window = pba::window_initialization("task09: Rotation and Energy Minimization");
  pba::FloorDrawer floor(1.0, -1.5);

  ::glEnable(GL_DEPTH_TEST);
  pba::set_some_lighting();

  while (!::glfwWindowShouldClose(window)) {

    step(rotation, translation, vtx2xyz,
         ivtx_fix, vtx2xyz_ini);
    //
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
    ::glColor3d(0.0, 0.0, 1.0);
    pba::draw_line_3d_mesh(line2vtx, vtx2xyz);
    ::glColor3d(1.0, 0.0, 0.0);
    pba::draw_line_3d_mesh(line2vtx, vtx2xyz_ini);

    // draw fixed point
    ::glDisable(GL_LIGHTING);
    ::glColor3d(0.0, 0.0, 1.0);
    pba::draw_sphere_at(32, 32, 0.05, vtx2xyz(ivtx_fix,0), vtx2xyz(ivtx_fix,1), vtx2xyz(ivtx_fix,2));
    ::glColor3d(1.0, 0.0, 0.0);
    pba::draw_sphere_at(32, 32, 0.05, vtx2xyz_ini(ivtx_fix,0), vtx2xyz_ini(ivtx_fix,1), vtx2xyz_ini(ivtx_fix,2));

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
