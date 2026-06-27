#include "player.h"
#include "raymath.h"
#include <cmath>
#include <algorithm>

Player CreatePlayer(int spawnRow, int spawnCol, const Maze& maze)
{
    Player p;
    p.yaw   = 0.0f;
    p.pitch = 0.0f;

    float ox = -maze.width  * 0.5f;
    float oz = -maze.depth  * 0.5f;

    float x = ox + (spawnCol + 0.5f) * maze.cellSize;
    float z = oz + (spawnRow + 0.5f) * maze.cellSize;

    p.camera.position   = { x, Player::HEIGHT, z };
    p.camera.target     = { x, Player::HEIGHT, z + 1.0f };
    p.camera.up         = { 0.0f, 1.0f, 0.0f };
    p.camera.fovy       = 60.0f;
    p.camera.projection = CAMERA_PERSPECTIVE;

    return p;
}

void UpdatePlayer(Player& player, const std::vector<Wall>& walls, float dt)
{
    // ── Mouse look ──
    Vector2 md = GetMouseDelta();
    const float sensitivity = 0.003f;
    player.yaw   += -md.x * sensitivity;
    player.pitch += -md.y * sensitivity;
    player.pitch = std::clamp(player.pitch, -1.47f, 1.47f);

    // ── Forward vector ──
    Vector3 forward;
    forward.x = cosf(player.pitch) * sinf(player.yaw);
    forward.y = sinf(player.pitch);
    forward.z = cosf(player.pitch) * cosf(player.yaw);

    // ── Flat (horizontal) direction for movement ──
    Vector3 flat = { forward.x, 0.0f, forward.z };
    float len = sqrtf(flat.x*flat.x + flat.z*flat.z);
    if (len > 0.0f) { flat.x /= len; flat.z /= len; }

    Vector3 right = Vector3Normalize(Vector3CrossProduct(flat, player.camera.up));

    // ── Movement input ──
    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? Player::SPRINT : Player::SPEED;
    Vector3 move = { 0, 0, 0 };
    if (IsKeyDown(KEY_W)) { move.x += flat.x * speed * dt; move.z += flat.z * speed * dt; }
    if (IsKeyDown(KEY_S)) { move.x -= flat.x * speed * dt; move.z -= flat.z * speed * dt; }
    if (IsKeyDown(KEY_A)) { move.x -= right.x * speed * dt; move.z -= right.z * speed * dt; }
    if (IsKeyDown(KEY_D)) { move.x += right.x * speed * dt; move.z += right.z * speed * dt; }

    // ── Sliding collision (full → X only → Z only) ──
    Vector3 newPos = player.camera.position;

    auto collides = [&](Vector3 pos) {
        for (auto& w : walls)
            if (CheckCollisionBoxSphere(w.box, pos, Player::RADIUS)) return true;
        return false;
    };

    Vector3 attempt = newPos; attempt.x += move.x; attempt.z += move.z;
    if (!collides(attempt)) {
        newPos = attempt;
    } else {
        Vector3 attemptX = player.camera.position; attemptX.x += move.x;
        if (!collides(attemptX)) {
            newPos = attemptX;
        } else {
            Vector3 attemptZ = player.camera.position; attemptZ.z += move.z;
            if (!collides(attemptZ)) newPos = attemptZ;
        }
    }

    player.camera.position.x = newPos.x;
    player.camera.position.z = newPos.z;
    player.camera.target = {
        player.camera.position.x + forward.x,
        player.camera.position.y + forward.y,
        player.camera.position.z + forward.z
    };
}
