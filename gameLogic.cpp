// Function to update bunny's position and speed
void updateBunnyPositionAndSpeed(Bunny bunny) {
    // Increase speed gradually
    bunny.speed += accelerationPerFrame;

    // Update position based on speed
    bunny.position.z += bunny.speed;

    // Add logic for hopping animation, collision detection, etc.
}

void checkCollisions() {
    // Check for collisions with checkpoints
}

void updateModelingMatrixAndCamera() {
    // Assume bunny.speed is a scalar value controlling the forward movement
    // Update bunny's position - assuming the bunny moves along the Z-axis
    bunny.position.z += bunny.speed;

    // Create the modeling matrix for the bunny
    // This is a translation matrix that moves the bunny to its current position
    glm::mat4 bunnyModelMatrix = glm::translate(glm::mat4(1.0f), bunny.position);

    // Update camera position to follow the bunny
    // For simplicity, we are only moving along the Z-axis
    // You can adjust the eyePos.y and eyePos.x to set the camera height and lateral position
    glm::vec3 cameraPosition = glm::vec3(bunny.position.x, bunny.position.y + cameraHeightOffset, bunny.position.z + cameraFollowDistance);

    // Now create the view matrix for the camera, looking at the bunny from the new position
    // Assuming the camera is always looking at the bunny and the up vector is in the positive Y direction
    glm::vec3 cameraTarget = bunny.position; // The camera is looking at the bunny
    glm::vec3 upVector = glm::vec3(0, 1, 0); // Up vector is in the Y direction
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    // Now you can use bunnyModelMatrix to transform the bunny's vertices
    // And use viewMatrix as part of the rendering state for the camera's perspective
}




