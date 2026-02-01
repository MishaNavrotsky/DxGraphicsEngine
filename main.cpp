#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // important for DX12

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "DX12 Engine", nullptr, nullptr
    );

    if (!window)
        return -1;

    // Get HWND when you need it (for DXGI swapchain)
    HWND hwnd = glfwGetWin32Window(window);
    std::cout << "HWND = " << hwnd << "\n";

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}