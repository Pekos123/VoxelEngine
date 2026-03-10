#ifndef PLAYER_H
#define PLAYER_H

#include <Camera.h>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace e
{
    class World;

    struct Box
    {
        glm::vec3 min, max;
        bool intersects(const Box& other) const;
    };

    class Player
    {
    public:
        glm::vec3 position;   // Bottom-center of the player (feet position)
        glm::vec3 velocity;
        glm::vec3 dimensions; // Width, Height, Depth (e.g., 0.6, 1.8, 0.6)

        bool onGround = false;
        float speed = 5.0f;
        float jumpForce = 8.0f;
        float gravity = -25.0f;

        Player(glm::vec3 spawnPos);

        Box GetAABB() const;
        void HandleInput(GLFWwindow* window, glm::vec3 camOrientation);
        void Update(float deltaTime, World& world);

        bool CanPlaceBlock(const glm::ivec3& targetBlockPos) const;

    private:
        void ResolveCollisions(World& world, int axis);
        bool IsBlockSolid(World& world, int x, int y, int z);
    };
}

#endif // PLAYER_H
