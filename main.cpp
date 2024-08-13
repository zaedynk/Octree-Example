#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <algorithm> // For std::find_if

#include "Input.h"
#include "Shader.h"

int Window_Width = 1920;
int Window_Height = 1080;

GLuint VAO;
GLuint VBO;
GLuint EBO;

std::vector<glm::vec3> vertices;
std::vector<unsigned int> indices;

// Cube vertices for a unit cube centered at the origin
const std::vector<glm::vec3> cubeVertices = {
    glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.5f, -0.5f,  0.5f),
    glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f)
};

// Indices for the cube's triangles
const std::vector<unsigned int> cubeIndices = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    7, 6, 5, 5, 4, 7,
    4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3
};

class OctreeNode {
public:
    bool isLeaf;
    glm::vec3 position; // Center position of the node
    float size; // Length of the edge of the node's cubic volume
    std::unique_ptr<OctreeNode> children[8]; // Pointers to child nodes
    OctreeNode(bool leaf, glm::vec3 pos, float s) : isLeaf(leaf), position(pos), size(s) {}
    // Add scalar value for marching cubes
    float value;
};

void generateCubeMeshData(const OctreeNode* node, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    if (node->isLeaf) {
        // Calculate the base index for this cube's vertices
        unsigned int baseIndex = vertices.size();

        // Scale and translate the unit cube vertices to the correct position and size
        for (const auto& v : cubeVertices) {
            vertices.push_back(node->position + (v * node->size));
        }

        // Add indices for the cube, adjusted for the current base index
        for (const auto& i : cubeIndices) {
            indices.push_back(baseIndex + i);
        }
        return;
    }

    for (auto& child : node->children) {
        if (child) generateCubeMeshData(child.get(), vertices, indices);
    }
}

void createOctree(OctreeNode* node, int maxDepth, glm::vec3 cameraPos, float initialDetailThreshold, int currentDepth = 0) {
    // Calculate distance from the camera to the node center
    float distance = glm::distance(cameraPos, node->position);

    // Dynamically adjust detail threshold based on distance
    // Closer nodes have a smaller detail threshold, causing more subdivisions
    float detailThreshold = initialDetailThreshold / (currentDepth + 1); // Adjust this formula as needed

    if (distance > detailThreshold || currentDepth >= maxDepth) {
        node->isLeaf = true;
        // Assign a constant value or another method to decide the value
        node->value = 0.5f; // Example constant value
        return;
    }

    // Subdivide the node
    node->isLeaf = false;
    float newSize = node->size / 2.0f;
    for (int i = 0; i < 8; ++i) {
        glm::vec3 newPos = node->position + glm::vec3(i & 1 ? newSize / 2 : -newSize / 2, i & 2 ? newSize / 2 : -newSize / 2, i & 4 ? newSize / 2 : -newSize / 2);
        node->children[i] = std::make_unique<OctreeNode>(false, newPos, newSize);
        // Recursively create children with adjusted detail threshold
        createOctree(node->children[i].get(), maxDepth, cameraPos, detailThreshold, currentDepth + 1);
    }
}

void updateOctreeForCameraPosition(OctreeNode* node, glm::vec3 cameraPos, int currentDepth, int maxDepth, float detailThreshold) {
    if (!node) return; // Safety check

    float distance = glm::distance(cameraPos, node->position);
    float dynamicThreshold = detailThreshold * pow(2, -currentDepth); // Adjusted for demonstration purposes

    // Subdivide if within threshold, not a leaf, and under max depth
    if (distance < dynamicThreshold && currentDepth < maxDepth) {
        if (node->isLeaf) {
            node->isLeaf = false;
            float newSize = node->size / 2.0f;
            for (int i = 0; i < 8; ++i) {
                glm::vec3 childPos = node->position + newSize * glm::vec3(i & 1 ? 0.5f : -0.5f, i & 2 ? 0.5f : -0.5f, i & 4 ? 0.5f : -0.5f);
                node->children[i] = std::make_unique<OctreeNode>(true, childPos, newSize);
                // Initialize leaf nodes
                node->children[i]->value = 0.5f; // Example constant value
            }
        }
    }
    else if (!node->isLeaf) {
        // Attempt to merge children if they are all leaves and far enough from the camera
        bool allChildrenAreLeaves = std::all_of(std::begin(node->children), std::end(node->children), [](const std::unique_ptr<OctreeNode>& child) {
            return child && child->isLeaf;
            });

        if (allChildrenAreLeaves) {
            // Additional conditions for merging can be added here (e.g., similarity in scalar values)
            // For now, we'll simply merge all children into the parent node
            for (int i = 0; i < 8; ++i) {
                node->children[i].reset(); // This effectively deletes the child node
            }
            node->isLeaf = true; // Mark the parent as a leaf
        }
    }

    // Recursively update children if not a leaf
    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                updateOctreeForCameraPosition(node->children[i].get(), cameraPos, currentDepth + 1, maxDepth, detailThreshold);
            }
        }
    }
}

void clearMeshData(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear();
    indices.clear();
}

int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(Window_Width, Window_Height, "Zengine - 3D Editor", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    Shader shader("VertexShader.glsl", "FragmentShader.glsl");

    // GLFW window setup
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, Window_Width, Window_Height);

    // Create the root octree node and populate the octree
    glm::vec3 rootPos(0.0f, 0.0f, 0.0f);
    float rootSize = 36.0f; // Size for the entire terrain block
    OctreeNode rootNode(false, rootPos, rootSize);
    int octreeDepth = 4; // Adjust depth based on the level of detail required
    float initialDetailThreshold = 50.0f; // Starting detail threshold, adjust based on your scene's scale
    createOctree(&rootNode, 8, cameraPos, initialDetailThreshold);

    float scale = 0.5f; // Noise scale
    float isovalue = 0.0f; // Surface threshold
    int octaves = 6;
    float lacunarity = 2.0;
    float persistence = 0.5;

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {

        // Keyboard and Mouse
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Model, view, and projection matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(Window_Width) / Window_Height, 0.1f, 1000.0f);

        // Shaders
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", cameraPos);

        // Scene matrix
        glm::vec3 scenePosition(0.0f, 0.0f, 0.0f);
        glm::mat4 scene = glm::translate(glm::mat4(1.0f), scenePosition);
        shader.setMat4("model", scene);

        // Update the octree based on the current camera position
        updateOctreeForCameraPosition(&rootNode, cameraPos, 0, 8, initialDetailThreshold);
        clearMeshData(vertices, indices); // Clear existing mesh data
        generateCubeMeshData(&rootNode, vertices, indices); // Generate new mesh data from the octree

        // Update VBO and EBO with the new mesh data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW); // Update VBO

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); // Update EBO

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}