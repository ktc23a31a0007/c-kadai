#include "Game.h"

Game::Game(std::shared_ptr<DX::DeviceResources> dr) : m_dr(dr) {}

void Game::Initialize(HWND, int w, int h)
{
    auto dev = m_dr->GetD3DDevice();
    auto ctx = m_dr->GetD3DDeviceContext();

    m_states = std::make_unique<CommonStates>(dev);
    m_box = GeometricPrimitive::CreateBox(ctx, Vector3(1, 1, 1));
    m_sphere = GeometricPrimitive::CreateSphere(ctx, 0.7f);

    m_proj = Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(60), float(w) / h, 0.1f, 200.f);

    GenerateMaze();
    ResetPlayer();
    m_lastTick = GetTickCount64();
}

void Game::GenerateMaze()
{
    m_walls.clear();
    int s = MazeSize();

    std::vector<std::vector<int>> maze(s, std::vector<int>(s, 1));
    std::mt19937 rng{ std::random_device{}() };

    auto dig = [&](auto&& self, int x, int z)->void
        {
            maze[x][z] = 0;
            int dx[4]{ 2,-2,0,0 }, dz[4]{ 0,0,2,-2 };
            std::vector<int> d{ 0,1,2,3 };
            std::shuffle(d.begin(), d.end(), rng);

            for (int i : d)
            {
                int nx = x + dx[i], nz = z + dz[i];
                if (nx > 0 && nz > 0 && nx < s - 1 && nz < s - 1 && maze[nx][nz])
                {
                    maze[x + dx[i] / 2][z + dz[i] / 2] = 0;
                    self(self, nx, nz);
                }
            }
        };
    dig(dig, 1, 1);

    for (int x = 0; x < s; x++)
        for (int z = 0; z < s; z++)
            if (maze[x][z])
                m_walls.emplace_back(Vector3(x, 0.5f, z), Vector3(0.5f));

    m_goal = Vector3(s - 2, 0.5f, s - 2);
}

void Game::ResetPlayer()
{
    m_player = Vector3(1, 0.5f, 1);
    m_time = 0.f;
}

void Game::OnMouseDown(int x, int y, bool left)
{
    if (left) m_left = true;
    else m_right = true;
    m_prev = { x,y };
}

void Game::OnMouseUp(bool left)
{
    if (left) m_left = false;
    else m_right = false;
}

void Game::OnMouseMove(int x, int y)
{
    if (m_state != GameState::Playing || !m_left) return;

    float speed = m_right ? 0.004f : 0.02f;

    int dx = x - m_prev.x;
    int dy = y - m_prev.y;

    Vector3 next = m_player;
    next.x += dx * speed;
    next.z -= dy * speed; // 上下のみ反転

    BoundingSphere s(next, 0.35f);
    for (auto& w : m_walls)
        if (s.Intersects(w))
        {
            ResetPlayer();
            return;
        }

    m_player = next;
    m_prev = { x,y };
}

void Game::Update()
{
    ULONGLONG now = GetTickCount64();
    if (m_state == GameState::Playing)
        m_time += (now - m_lastTick) * 0.001f;
    m_lastTick = now;

    if (m_state == GameState::Title && m_left)
        m_state = GameState::Playing;

    if (m_state == GameState::Playing &&
        Vector3::Distance(m_player, m_goal) < 1.f)
        m_state = GameState::Clear;

    m_view = Matrix::CreateLookAt(
        m_player + Vector3(0, 25, -1),
        m_player,
        Vector3::UnitZ);
}

void Game::Render()
{
    auto ctx = m_dr->GetD3DDeviceContext();
    ctx->RSSetState(m_states->CullCounterClockwise());
    ctx->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    for (auto& w : m_walls)
        m_box->Draw(
            Matrix::CreateScale(w.Extents * 2) *
            Matrix::CreateTranslation(w.Center),
            m_view, m_proj, Colors::Red);

    m_sphere->Draw(Matrix::CreateTranslation(m_player),
        m_view, m_proj, Colors::Yellow);

    m_sphere->Draw(Matrix::CreateTranslation(m_goal),
        m_view, m_proj, Colors::Green);
}
