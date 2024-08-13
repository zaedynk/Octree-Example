#version 330 core
in float fragmentDistance;
out vec4 FragColor;

void main() {
    float distanceFactor = clamp(fragmentDistance / 100.0, 0.0, 1.0); // Adjust as needed
    FragColor = vec4(distanceFactor, 1.0 - distanceFactor, 0.0, 1.0); // Color changes with distance
}
