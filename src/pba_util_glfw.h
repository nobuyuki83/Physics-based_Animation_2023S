#ifndef PBA_GLFW_UTIL_H_
#define PBA_GLFW_UTIL_H_

#include <GLFW/glfw3.h>

namespace pba {

// print out error
static void error_callback(
    [[maybe_unused]] int error,
    const char *description) {
  fputs(description, stderr);
}

GLFWwindow *window_initialization(const char* title) {
  ::glfwSetErrorCallback(error_callback);
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // make window
  GLFWwindow *window = ::glfwCreateWindow(
      640, 480, title,
      nullptr, nullptr);
  if (!window) { // exit if failed to create window
    ::glfwTerminate();
    exit(EXIT_FAILURE);
  }
  ::glfwMakeContextCurrent(window); // working on this window
  return window;
}

void default_window_2d(GLFWwindow *window) {
  // get windowsize
  int width, height;
  ::glfwGetFramebufferSize(window, &width, &height);
  // get aspect ratio of the window
  const float ratio = (float) width / (float) height;
  ::glViewport(0, 0, width, height);
  ::glClearColor(1.f, 1.f, 1.f, 1.f);
  ::glClear(GL_COLOR_BUFFER_BIT);
  // matrix mode: projection
  ::glMatrixMode(GL_PROJECTION);
  ::glLoadIdentity();
  ::glOrtho(-ratio, ratio, -1.f, 1.f, -1.f, +1.f);
  // matrix mode : model view
  ::glMatrixMode(GL_MODELVIEW);
  ::glLoadIdentity();
}

}

#endif //PBA_GLFW_UTIL_H_
