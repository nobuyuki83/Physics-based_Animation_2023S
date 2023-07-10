/**
 * @file task10: Simulation of Rigid Body Precession
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <filesystem>
#include <fstream>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"
#include "../src/pba_floor_drawer.h"
#include "../src/pba_eigen_gl.h"
#include "../src/pba_util_eigen.h"

/**
 * compute the volume and the center of gravity of 3D solid triangle mesh
 * @param tri2vtx connectivity of a triangle mesh
 * @param vtx2xyz coordinates
 * @return
 */
auto volume_center_solid_3d_triangle_mesh(
    const Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> &tri2vtx,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz) {
  Eigen::Vector3f center = Eigen::Vector3f::Zero();
  float tot_v = 0.0;
  for (int i_tri = 0; i_tri < tri2vtx.rows(); ++i_tri) {
    const Eigen::Vector3f p0 = vtx2xyz.row(tri2vtx(i_tri, 0));
    const Eigen::Vector3f p1 = vtx2xyz.row(tri2vtx(i_tri, 1));
    const Eigen::Vector3f p2 = vtx2xyz.row(tri2vtx(i_tri, 2));
    float v = p0.dot(p1.cross(p2)) / 6.f;
    center += (v * 0.25f) * (p0 + p1 + p2);
    tot_v += v;
  }
  center /= tot_v;
  return std::make_pair(tot_v, center);
}

/**
 * inertia tensor of 3D solid triangle mesh
 * @param tri2vtx connectivity of a triangle mesh
 * @param vtx2xyz coordinates
 * @return
 */
Eigen::Matrix3f inertia_tensor_solid_3d_triangle_mesh(
    const Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> &tri2vtx,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz) {
  Eigen::Matrix3f res = Eigen::Matrix3f::Zero();
  for (int i_tri = 0; i_tri < tri2vtx.rows(); ++i_tri) {
    const Eigen::Vector3f p0 = vtx2xyz.row(tri2vtx(i_tri, 0));
    const Eigen::Vector3f p1 = vtx2xyz.row(tri2vtx(i_tri, 1));
    const Eigen::Vector3f p2 = vtx2xyz.row(tri2vtx(i_tri, 2));
    Eigen::Vector3f pa = p0 + p1 + p2;
    const Eigen::Matrix3f m0 = p0 * p0.transpose();
    const Eigen::Matrix3f m1 = p1 * p1.transpose();
    const Eigen::Matrix3f m2 = p2 * p2.transpose();
    const Eigen::Matrix3f ma = pa * pa.transpose();
    const Eigen::Matrix3f I0 = m0 + m1 + m2 + ma;
    float tr0 = I0.trace();
    Eigen::Matrix3f I = tr0 * Eigen::Matrix3f::Identity() - I0;
    float v = p0.dot(p1.cross(p2));
    res += (v / 120.f) * I;
  }
  return res;
}

auto load_3d_model() {
  auto[tri2vtx, vtx2xyz] = pba::load_wavefront_obj(std::filesystem::path(PATH_SOURCE_DIR) / "t-rex.obj");
  { // normalize the size
    auto size = (vtx2xyz.colwise().maxCoeff() - vtx2xyz.colwise().minCoeff()).maxCoeff();
    vtx2xyz /= size;
    vtx2xyz *= 2.5;
  }
  // compute volume and the center of gravity
  auto[volume, center] = volume_center_solid_3d_triangle_mesh(tri2vtx, vtx2xyz);
  vtx2xyz.rowwise() -= center.transpose(); // center-ize
  return std::make_pair(tri2vtx, vtx2xyz);
}

int main() {
  const auto[tri2vtx, vtx2xyz_ini] = load_3d_model();
  const Eigen::Matrix3f inertia = inertia_tensor_solid_3d_triangle_mesh(tri2vtx, vtx2xyz_ini);

  GLFWwindow *window = pba::window_initialization("task10: Simulation of Rigid Body Precession");
  pba::FloorDrawer floor(1.0, -1.2);
  ::glEnable(GL_DEPTH_TEST);
  pba::set_some_lighting();

  constexpr float dt = 0.001; // time step
  float time = 0.f;
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> vtx2xyz = vtx2xyz_ini; // rotated mesh
  constexpr unsigned int i_vtx_trajectory = 831;
  std::vector<Eigen::Vector3f> trajectory; // trajectory
  Eigen::Vector3f Omega(0.f, 0.05f, 1.f); // initial angular velocity (\dot{R} = R * Skew(\Omega))
  Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity(); // rotation to optimize

  while (!::glfwWindowShouldClose(window)) {
    if( time < 200.0 ) {
      for (int itr = 0; itr < 100; ++itr) { // sub-stepping
        time += dt;
        // Write some code below to simulate rotation of the rigid body
        // Use the **forward Euler method** to update the rotation matrix and the angular velocity
        // Note that `rotation` should stay a rotational matrix after the update
        // rotation =
        // Omega =
        // Do not change anything else except for the two lines above.
      }
      std::cout << "time: " << time << std::endl;
      // Since we use the forward Euler method for the time integration, the energy will increase slightly over the time.
      std::cout << "   energy: " << Omega.transpose() * inertia * Omega << std::endl;
      // make sure the angular momentum conserves for some extent
      std::cout << "   angular momentum: " << (rotation * inertia * Omega).transpose() << std::endl;
      trajectory.emplace_back(vtx2xyz.row(i_vtx_trajectory));
      vtx2xyz = (rotation * vtx2xyz_ini.transpose()).transpose(); // the rotated mesh's vertices
    }
    //
    pba::default_window_3d(window); // start window for 3D visualization
    // set view angle
    ::glMatrixMode(GL_MODELVIEW);
    ::glTranslatef(0.0, 0.0, 0.0);
    ::glRotatef(50, 1.0, 0.0, 0.0);

    // draw floor with shadow
    floor.draw_shadow_pre();
    pba::draw_triangle_3d_mesh(tri2vtx, vtx2xyz);
    floor.draw_shadow_pos();

    // draw faces
    ::glEnable(GL_LIGHTING);
    ::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, std::array<float, 4>{1.0, 1.0, 1.0, 1.0}.data());
    pba::draw_triangle_3d_mesh(tri2vtx, vtx2xyz);

    // draw trajectory
    ::glDisable(GL_LIGHTING);
    ::glLineWidth(2);
    ::glColor3d(0.0, 0.0, 0.0);
    ::glBegin(GL_LINE_STRIP);
    for (auto p: trajectory) {
      ::glVertex3f(p.x(), p.y(), p.z());
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
