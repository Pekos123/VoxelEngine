#include <Player.h>
#include <World.h>
#include <cmath>
#include <algorithm>

namespace e
{
    // --- Box Implementation ---
    bool Box::intersects(const Box& other) const
    {
        return (min.x < other.max.x && max.x > other.min.x) &&
               (min.y < other.max.y && max.y > other.min.y) &&
               (min.z < other.max.z && max.z > other.min.z);
    }

    // --- Player Implementation ---
    Player::Player(glm::vec3 spawnPos)
        : position(spawnPos), velocity(0.0f), dimensions(0.6f, 1.8f, 0.6f)
    {
    }

    Box Player::GetAABB() const
    {
        // position is feet-center
        return {
            position - glm::vec3(dimensions.x / 2.0f, 0.0f, dimensions.z / 2.0f),
            position + glm::vec3(dimensions.x / 2.0f, dimensions.y, dimensions.z / 2.0f)
        };
    }

    void Player::HandleInput(GLFWwindow* window, glm::vec3 camOrientation)
    {
        // 1. Get the "Flat" Forward vector (ignore vertical tilt)
        // If we don't zero out Y, looking up makes you fly into the air.
        glm::vec3 forward = glm::normalize(glm::vec3(camOrientation.x, 0.0f, camOrientation.z));
        
        // 2. Get the Right vector using a Cross Product
        // The cross product of Forward and World Up (0, 1, 0) gives you "Right"
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    
        // Reset horizontal velocity before recalculating
        glm::vec3 moveDir = glm::vec3(0.0f);
    
        // 3. Accumulate movement direction
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forward;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forward;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;
    
        // 4. Apply Speed
        if (glm::length(moveDir) > 0) {
            moveDir = glm::normalize(moveDir); // Prevents moving faster diagonally
            velocity.x = moveDir.x * speed;
            velocity.z = moveDir.z * speed;
        } else {
            velocity.x = 0;
            velocity.z = 0;
        }
    
        // 5. Jump (Keep Y separate from horizontal logic)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
        {
            velocity.y = jumpForce;
            onGround = false;
        }
    }

    void Player::Update(float deltaTime, World& world)
    {
        // 1. Apply Gravity
        velocity.y += gravity * deltaTime;

        // 2. Axis-by-Axis movement to resolve collisions independently
        // Moving one axis at a time allows us to know exactly which face we hit

        // --- Y AXIS ---
        position.y += velocity.y * deltaTime;
        onGround = false; // Reset before check
        ResolveCollisions(world, 1); // 1 = Y Axis

        // --- X AXIS ---
        position.x += velocity.x * deltaTime;
        ResolveCollisions(world, 0); // 0 = X Axis

        // --- Z AXIS ---
        position.z += velocity.z * deltaTime;
        ResolveCollisions(world, 2); // 2 = Z Axis
    }

    bool Player::CanPlaceBlock(const glm::ivec3& targetBlockPos) const
    {
        // 1. Construct the Player AABB (based on feet position)
        Box playerBox = GetAABB();

        // 2. Construct the Target Block AABB (integer coords)
        // Block is a 1x1x1 cube from targetBlockPos to targetBlockPos + 1
        Box blockBox = {
            glm::vec3(targetBlockPos),
            glm::vec3(targetBlockPos) + glm::vec3(1.0f)
        };

        // 3. Return true if they DO NOT intersect (it's safe to place)
        return !playerBox.intersects(blockBox);
    }

    void Player::ResolveCollisions(World& world, int axis)
    {
        Box playerBox = GetAABB();

        // Broadphase: Only check blocks within the player's potential range
        int minX = static_cast<int>(std::floor(playerBox.min.x));
        int maxX = static_cast<int>(std::floor(playerBox.max.x));
        int minY = static_cast<int>(std::floor(playerBox.min.y));
        int maxY = static_cast<int>(std::floor(playerBox.max.y));
        int minZ = static_cast<int>(std::floor(playerBox.min.z));
        int maxZ = static_cast<int>(std::floor(playerBox.max.z));

        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                for (int z = minZ; z <= maxZ; z++) {
                    
                    if (IsBlockSolid(world, x, y, z)) {
                        Box blockBox = { glm::vec3(x, y, z), glm::vec3(x + 1, y + 1, z + 1) };
                        
                        if (playerBox.intersects(blockBox)) {
                            if (axis == 1) { // Y-Axis Resolution
                                if (velocity.y > 0) {
                                    // Hitting ceiling: snap player's top to block's bottom
                                    position.y = blockBox.min.y - dimensions.y;
                                } else {
                                    // Hitting ground: snap player's feet to block's top
                                    position.y = blockBox.max.y;
                                    onGround = true;
                                }
                                velocity.y = 0;
                            }
                            else if (axis == 0) { // X-Axis Resolution
                                if (velocity.x > 0) 
                                    // Moving positive X: snap to min of block
                                    position.x = blockBox.min.x - dimensions.x / 2.0f;
                                else 
                                    // Moving negative X: snap to max of block
                                    position.x = blockBox.max.x + dimensions.x / 2.0f;
                                velocity.x = 0;
                            }
                            else if (axis == 2) { // Z-Axis Resolution
                                if (velocity.z > 0) 
                                    // Moving positive Z: snap to min of block
                                    position.z = blockBox.min.z - dimensions.z / 2.0f;
                                else 
                                    // Moving negative Z: snap to max of block
                                    position.z = blockBox.max.z + dimensions.z / 2.0f;
                                velocity.z = 0;
                            }
                            // Important: update AABB after each snap to avoid multiple collisions with same block
                            playerBox = GetAABB();
                        }
                    }
                }
            }
        }
    }

    bool Player::IsBlockSolid(World& world, int x, int y, int z)
    {
        int blockId = world.GetBlock(x, y, z);
        e::BlockData bData = e::BlockDatabase::Get(static_cast<BlockID>(blockId));
        return bData.id != BlockID::AIR && bData.type != BlockType::LIQUID;
    }
}
