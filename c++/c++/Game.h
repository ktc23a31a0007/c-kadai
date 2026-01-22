#pragma once
#include <memory>
#include <vector>
#include <windows.h>
#include <random>

#include "SimpleMath.h"
#include "GeometricPrimitive.h"
#include "CommonStates.h"
#include "DeviceResources.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum class GameState
{
    Title,
    Playing,
    Clear,
    Difficulty
};

class Game
{
public:
    Game(std::shared_ptr<DX::DeviceResources> dr);

    void Initialize(HWND, int, int);
    void Update();
    void Render();

    void OnMouseDown(int x, int y, bool left);
    void OnMouseUp(bool left);
    void OnMouseMove(int x, int y);

private:
    void GenerateMaze();
    void ResetPlayer();

    std::shared_ptr<DX::DeviceResources> m_dr;

    std::unique_ptr<CommonStates> m_states;
    std::unique_ptr<GeometricPrimitive> m_box;
    std::unique_ptr<GeometricPrimitive> m_sphere;

    std::vector<BoundingBox> m_walls;

    Vector3 m_player;
    Vector3 m_goal;

    Matrix m_view;
    Matrix m_proj;

    GameState m_state = GameState::Title;

    bool m_left = false;
    bool m_right = false;
    POINT m_prev{};

    int m_difficulty = 1;
    float m_time = 0.f;
    ULONGLONG m_lastTick = 0;

    int MazeSize() const { return 11 + m_difficulty * 4; }
};
