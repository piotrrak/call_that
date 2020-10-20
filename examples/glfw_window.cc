/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#include <GLFW/glfw3.h>

#include "call_that/that_call.hh"
#include "call_that/make_dispatch.hh"
#include "call_that/ext/glfw_cbs.hh"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <iostream>

using namespace call_that;
using namespace call_that::ext_glfw;

void report_glfw_error(const char* message)
{
   const char* err = "<Unknown reason>";
   glfwGetError(&err);

   std::cerr << message << ": " << err << std::endl;
}

void report_fatal_glfw_error(const char* message)
{
   report_glfw_error(message);
   std::exit(EXIT_FAILURE);
}

auto setup_dummy_window()
{
   glfwDefaultWindowHints();

   auto monitor = glfwGetPrimaryMonitor();

   int width = 800; int height = 600;

   if (monitor == nullptr)
      report_glfw_error("Failed to obtain primary monitor");
   else
   {
      const auto* video_mode = glfwGetVideoMode(monitor);

      if (video_mode)
      {
         width = int(video_mode->width * 0.8);
         height = int(video_mode->height * 0.8);
      }
      else
         report_glfw_error("Failed to obtain video mode");
   }

   auto window = glfwCreateWindow(width, height, "Call that!", nullptr , nullptr);

   if (!window)
      report_fatal_glfw_error("Failed to create window");

   return window;
}

struct wnd_dispatch : decltype(make_dispatch_table(
  wnd::mouse_button_cb,
  wnd::cursor_pos_cb,
  wnd::cursor_enter_cb,
  wnd::fbsize_cb,
  wnd::size_cb))
{
};

int main()
{
   // Init glfw
   auto ok = glfwInit();

   if (!ok)
      report_fatal_glfw_error("Failed to initialize GLFW3");

   // Create window - todays test subject.
   auto window = setup_dummy_window();

   // Create dispatch table for callbacks (same as allocate_dispatch_table(ids...))
   auto dispatch = std::make_unique<wnd_dispatch>();

   assert(has_cb(*dispatch, wnd::fbsize_cb) == false && "Should have none");

   // Add callback that logs window fb_size changes dispatch table
   on(*dispatch, wnd::fbsize_cb, [wnd = window] (auto w, auto h) {

      std::cout << "Framebuffer of "<< (void*)wnd
         << ": w = " << w << "; h = " << h << std::endl;
   });

   assert(has_cb(*dispatch, wnd::fbsize_cb) && "Should have one");

   // Add callback that logs window fb_size changes dispatch table
   on(*dispatch, wnd::size_cb, [] (size_t w, size_t h) {
      std::cout << "Window size w = " << w <<  "; h = " << h << std::endl;
   });

   assert(has_cb(*dispatch, wnd::size_cb) && "Should have one");

   // Remove fb_size callback with index 1
   auto prev = remove_cb(*dispatch, wnd::fbsize_cb);

   // Add dummy callback for fb_size
   on(*dispatch, wnd::fbsize_cb, [] (auto, auto) {});
   assert(has_cb(*dispatch, wnd::fbsize_cb) && "Should have one");

   on(*dispatch, wnd::fbsize_cb, std::move(prev));
   assert(has_cb(*dispatch, wnd::fbsize_cb) && "Should have one");

   // Attach all callbacks and dispatch table to window instance
   attach_all_cbs(*dispatch, window);

   // Helper toggler for size/fb_size
   auto toggle_size_cbs = [&](bool toggle) {
     if (toggle)
     {
        attach_cb(*dispatch, wnd::size_cb, window);
        attach_cb(*dispatch, wnd::fbsize_cb, window);
     }
     else
     {
        detach_cb(*dispatch, wnd::size_cb, window);
        detach_cb(*dispatch, wnd::fbsize_cb, window);
     }
   };

   // Toggle on/off window size/fbsize callbacks on mouse button click.
   on(*dispatch, wnd::mouse_button_cb,
      [&, on=true] (int button, int action, int mods) mutable {
         if (action != GLFW_RELEASE) return;

         toggle_size_cbs(on = !on);
   });

   for(;;)
   {
      if (glfwWindowShouldClose(window)) break;

      glfwWaitEvents();
   }

   // Attach all callbacks and dispatch table from window instance
   detach_all_cbs(*dispatch, window);

   // Cleanup...
   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}
