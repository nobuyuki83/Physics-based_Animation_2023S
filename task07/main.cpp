/**
 * @file task07: Dirichlet's Energy Minimization with Gauss-Seidel Method
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"

void solve_laplace_gauss_seidel_on_grid(
    std::vector<float> &vtx2val,
    unsigned int grid_size,
    const std::vector<bool> &vtx2isfix) {
  assert(vtx2val.size() == grid_size * grid_size);
  for (unsigned int ix = 0; ix < grid_size; ++ix) {
    for (unsigned int iy = 0; iy < grid_size; ++iy) {
      unsigned int idx_center = iy * grid_size + ix;
      if (vtx2isfix[idx_center]) { continue; }
      // write some code below to implement Gauss-Seidel method
      // Do not write more than 5 lines of code
    }
  }
}

int main() {

  GLFWwindow *window = pba::window_initialization("task07: Dirichlet's Energy Minimization with Gauss-Seidel Method");

  constexpr float box_size = 1.8;

  unsigned int grid_size = 256; // grid resolution
  std::vector<float> vtx2val(grid_size * grid_size); // grid data array storing distances
  {
    std::mt19937 rand(std::random_device{}());
    std::uniform_real_distribution<float> dist01(0.f, 1.f);
    for (float &val: vtx2val) { val = dist01(rand); }
  }
  std::vector<bool> vtx2isfix(grid_size * grid_size, false);
  {
    for (unsigned int i = 0; i < grid_size; ++i) {
      vtx2isfix[i] = true;
      vtx2isfix[i * grid_size] = true;
      vtx2isfix[i * grid_size + (grid_size - 1)] = true;
      vtx2isfix[(grid_size - 1) * grid_size + i] = true;
      vtx2val[i] = 0.0;
      vtx2val[i * grid_size] = 0.0;
      vtx2val[i * grid_size + (grid_size - 1)] = 0.0;
      vtx2val[(grid_size - 1) * grid_size + i] = 0.0;
    }
    float h = box_size / static_cast<float>(grid_size - 1);
    for (unsigned int ix = 0; ix < grid_size; ++ix) {
      for (unsigned int iy = 0; iy < grid_size; ++iy) {
        const float x = static_cast<float>(ix) * h - box_size * 0.5f;
        const float y = static_cast<float>(iy) * h - box_size * 0.5f;
        const float r = sqrt(x * x + y * y);
        if (r < box_size * 0.4 && r > box_size * 0.3) {
          if (fabs(y) < box_size * 0.2 && x > 0.0) {
            continue;
          }
          vtx2isfix[iy * grid_size + ix] = true;
          vtx2val[iy * grid_size + ix] = 0.9;
        }
      }
    }
  }

  while (!::glfwWindowShouldClose(window)) {
    pba::default_window_2d(window);

    solve_laplace_gauss_seidel_on_grid(
        vtx2val, grid_size, vtx2isfix);

    { // compute energy
      float w = 0.f;
      for (unsigned int iy = 0; iy < grid_size - 1; ++iy) {
        for (unsigned int ix = 0; ix < grid_size; ++ix) {
          float val = vtx2val[iy * grid_size + ix];
          float val_up = vtx2val[(iy + 1) * grid_size + ix];
          w += 0.5f * (val - val_up) * (val - val_up);
        }
      }
      for (unsigned int iy = 0; iy < grid_size; ++iy) {
        for (unsigned int ix = 0; ix < grid_size - 1; ++ix) {
          float val = vtx2val[iy * grid_size + ix];
          float val_right = vtx2val[iy * grid_size + (ix + 1)];
          w += 0.5f * (val - val_right) * (val - val_right);
        }
      }
      std::cout << "Dirichlet's Energy: " << w << std::endl;
    }

    {
      const float h = box_size / static_cast<float>(grid_size - 1);
      const float scale = 1.0;
      ::glColor3f(0.9f, 0.9f, 0.9f);
      ::glBegin(GL_QUADS);
      for (unsigned int iy = 0; iy < grid_size - 1; ++iy) {
        for (unsigned int ix = 0; ix < grid_size - 1; ++ix) {
          pba::colormap_hot(vtx2val[(iy + 0) * grid_size + (ix + 0)], scale);
          glVertex2f((ix + 0) * h - box_size * 0.5f, (iy + 0) * h - box_size * 0.5f);
          pba::colormap_hot(vtx2val[(iy + 0) * grid_size + (ix + 1)], scale);
          glVertex2f((ix + 1) * h - box_size * 0.5f, (iy + 0) * h - box_size * 0.5f);
          pba::colormap_hot(vtx2val[(iy + 1) * grid_size + (ix + 1)], scale);
          glVertex2f((ix + 1) * h - box_size * 0.5f, (iy + 1) * h - box_size * 0.5f);
          pba::colormap_hot(vtx2val[(iy + 1) * grid_size + (ix + 0)], scale);
          glVertex2f((ix + 0) * h - box_size * 0.5f, (iy + 1) * h - box_size * 0.5f);
        }
      }
      ::glEnd();
    }
    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
