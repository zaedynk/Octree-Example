#include "Input.h"

// Define the variables
glm::vec3 cameraPos = glm::vec3(0.0f, 25.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.2f;
float cameraSensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
bool cursorLocked = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;
float lastX = 1920.0f / 2.0;
float lastY = 1080.0f / 2.0;
float fov = 80.0f;
double lastEscapePressTime = 0;
double escapePressDelay = 0.2;  // 200 ms delay

void processInput(GLFWwindow* window)
{
    double currentTime = glfwGetTime();

    // Camera movement speed based on whether Shift is pressed or not
    float currentCameraSpeed = cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        currentCameraSpeed *= 2.0f; // Move twice as fast when Shift is held

    // Camera movement based on key inputs
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += currentCameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= currentCameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * currentCameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * currentCameraSpeed;

    // Camera movement up and down with Space and Shift
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS) // Move down only if not holding forward key
            cameraPos -= cameraSpeed * cameraUp;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
        // Handle mouse movement only when the cursor is locked
        if (firstMouse)
        {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos; // Reversed since y-coordinates go from bottom to top
        lastMouseX = xpos;
        lastMouseY = ypos;

        xoffset *= cameraSensitivity;
        yoffset *= cameraSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
}
