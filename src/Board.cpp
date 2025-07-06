#include "../include/Board.hpp"
#include <algorithm>
#include <chrono>
#include <queue>

Board::Board(int w, int h) : width(w), height(h), 
    rng(std::chrono::steady_clock::now().time_since_epoch().count()),
    colorDist(0, 5),  // 6 kolorów: 0-5
    selectedX(-1), selectedY(-1), hasBallSelected(false), blinkState(false),
    lineAnimationActive(false), animationPhase(0), fastBlinkState(false),
    score(0), comboMultiplier(1), gameOver(false), ballsToAdd(2)
{
    initialize();
    initializeGraphics();
    generateBalls(); // Generuj kulki po inicjalizacji
    generateNextBalls(); // Przygotuj następne kulki
}

Board::~Board() 
{
    // std::vector automatycznie zwalnia pamięć - nie trzeba nic robić
}

void Board::initialize()
{
    grid.resize(height);
    balls.resize(height);
    lineMarked.resize(height);
    for (int i = 0; i < height; ++i)
    {
        grid[i].resize(width);
        balls[i].resize(width);
        lineMarked[i].resize(width);
        for (int j = 0; j < width; ++j)
        {
            grid[i][j] = 0;
            balls[i][j] = nullptr;
            lineMarked[i][j] = false;
        }
    }
}

void Board::initializeGraphics()
{
    // Stałe dla rysowania
    cellSize = 50.0f;
    offsetX = 100.0f;
    offsetY = 50.0f;

    // Utwórz kształty dla pól planszy
    cells.resize(height);
    for (int i = 0; i < height; ++i)
    {
        cells[i].resize(width);
        for (int j = 0; j < width; ++j)
        {
            // Każde pole to prostokąt
            cells[i][j].setSize({cellSize - 2.0f, cellSize - 2.0f});
            cells[i][j].setPosition({offsetX + j * cellSize + 1.0f,
                                     offsetY + i * cellSize + 1.0f});
            cells[i][j].setFillColor(sf::Color(40, 40, 40));
            cells[i][j].setOutlineThickness(1.0f);
            cells[i][j].setOutlineColor(sf::Color(100, 100, 100));
        }
    }
}

void Board::reset()
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            grid[i][j] = 0;
        }
    }
}

void Board::draw(sf::RenderWindow &window)
{
    // Rysuj pola planszy
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // Ustaw kolor w zależności od zawartości pola
            if (grid[i][j] == 0)
            {
                // Puste pole
                cells[i][j].setFillColor(sf::Color(40, 40, 40));
            }
            else
            {
                // Pole z kulką - nieco ciemniejsze tło
                cells[i][j].setFillColor(sf::Color(30, 30, 30));
            }

            window.draw(cells[i][j]);
        }
    }

    // Rysuj linie siatki
    drawGrid(window);
    
    // Rysuj kulki na wierzchu
    drawBalls(window);

    // Rysuj latające punkty
    drawFloatingScores(window);
    
    // Rysuj preview następnych kulek
    drawNextBalls(window);
}

void Board::drawGrid(sf::RenderWindow &window)
{
    sf::RectangleShape line;
    line.setFillColor(sf::Color(80, 80, 80));

    // Linie pionowe
    for (int j = 0; j <= width; ++j)
    {
        line.setSize({1.0f, height * cellSize});
        line.setPosition({offsetX + j * cellSize, offsetY});
        window.draw(line);
    }

    // Linie poziome
    for (int i = 0; i <= height; ++i)
    {
        line.setSize({width * cellSize, 1.0f});
        line.setPosition({offsetX, offsetY + i * cellSize});
        window.draw(line);
    }
}

sf::Color Board::getBallColor(int ballType)
{
    switch (ballType)
    {
    case 1:
        return sf::Color::Red;
    case 2:
        return sf::Color::Green;
    case 3:
        return sf::Color::Blue;
    case 4:
        return sf::Color::Yellow;
    case 5:
        return sf::Color::Magenta;
    case 6:
        return sf::Color::Cyan;
    case 7:
        return sf::Color(255, 165, 0); // Orange
    default:
        return sf::Color(40, 40, 40); // Puste pole
    }
}

void Board::generateBalls()
{
    // Parametry generowania
    const float fillRate = 0.30f; // 30% wypełnienie - łatwiejsza gra
    std::uniform_real_distribution<float> fillDist(0.0f, 1.0f);
    
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // Losuj czy umieścić kulkę w tym miejscu
            if (fillDist(rng) < fillRate)
            {
                BallColor color = getRandomColor();
                placeBallAt(j, i, color);
            }
        }
    }
}

void Board::drawBalls(sf::RenderWindow &window)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (balls[i][j] != nullptr)
            {
                bool shouldDraw = true;
                
                // Obsługa migania wybranej kulki
                if (hasBallSelected && selectedX == j && selectedY == i && !blinkState)
                {
                    shouldDraw = false;
                }
                
                // Obsługa animacji linii
                if (lineAnimationActive && lineMarked[i][j])
                {
                    if (animationPhase == 0) // Highlight - zawsze rysuj
                    {
                        shouldDraw = true;
                        // Można dodać efekt highlight (np. pulsowanie)
                    }
                    else if (animationPhase == 1) // Fast blink
                    {
                        shouldDraw = fastBlinkState;
                    }
                    else // Remove phase
                    {
                        shouldDraw = false;
                    }
                }
                
                if (shouldDraw)
                {
                    balls[i][j]->draw(window);
                }
            }
        }
    }
    
    // Rysuj latające punkty
    drawFloatingScores(window);
}

void Board::placeBallAt(int x, int y, BallColor color)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        sf::Vector2f position = getCellPosition(x, y);
        balls[y][x] = std::make_unique<Ball>(color, position);
        grid[y][x] = static_cast<int>(color) + 1; // 1-6 dla kolorów
    }
}

BallColor Board::getRandomColor()
{
    int colorValue = colorDist(rng);
    return static_cast<BallColor>(colorValue);
}

sf::Vector2f Board::getCellPosition(int x, int y) const
{
    // Pozycja środka komórki
    float centerX = offsetX + x * cellSize + cellSize / 2.0f - 20.0f; // 20.0f = radius kulki
    float centerY = offsetY + y * cellSize + cellSize / 2.0f - 20.0f;
    return sf::Vector2f(centerX, centerY);
}

bool Board::isValidPosition(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::isEmpty(int x, int y) const
{
    return isValidPosition(x, y) && grid[y][x] == 0;
}

std::pair<int, int> Board::getGridPosition(float mouseX, float mouseY) const
{
    int gridX = static_cast<int>((mouseX - offsetX) / cellSize);
    int gridY = static_cast<int>((mouseY - offsetY) / cellSize);
    
    if (isValidPosition(gridX, gridY))
    {
        return {gridX, gridY};
    }
    return {-1, -1}; // Nieprawidłowa pozycja
}

void Board::handleMouseClick(float mouseX, float mouseY)
{
    auto [gridX, gridY] = getGridPosition(mouseX, mouseY);
    
    if (gridX == -1 || gridY == -1) // Kliknięcie poza planszą
    {
        deselectBall();
        return;
    }
    
    // Jeśli nie ma wybranej kulki
    if (!hasBallSelected)
    {
        // Sprawdź czy kliknięto na kulkę
        if (!isEmpty(gridX, gridY))
        {
            selectBall(gridX, gridY);
        }
    }
    else // Mamy wybraną kulkę
    {
        // Jeśli kliknięto na tę samą kulkę - odznacz
        if (gridX == selectedX && gridY == selectedY)
        {
            deselectBall();
        }
        // Jeśli kliknięto na pustą pozycję - spróbuj przesunąć
        else if (isEmpty(gridX, gridY))
        {
            if (canMoveTo(selectedX, selectedY, gridX, gridY))
            {
                moveBall(selectedX, selectedY, gridX, gridY);
                deselectBall();
            }
            else
            {
                deselectBall(); // Nie można dotrzeć - odznacz kulkę
            }
        }
        // Jeśli kliknięto na inną kulkę - wybierz ją
        else
        {
            selectBall(gridX, gridY);
        }
    }
}

void Board::selectBall(int x, int y)
{
    selectedX = x;
    selectedY = y;
    hasBallSelected = true;
    blinkClock.restart();
    blinkState = true;
}

void Board::deselectBall()
{
    selectedX = -1;
    selectedY = -1;
    hasBallSelected = false;
    blinkState = false;
}

std::vector<std::pair<int, int>> Board::findPath(int fromX, int fromY, int toX, int toY)
{
    if (!isValidPosition(fromX, fromY) || !isValidPosition(toX, toY))
        return {};
    
    if (!isEmpty(toX, toY))
        return {};
    
    // BFS pathfinding
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<std::pair<int, int>>> parent(height, std::vector<std::pair<int, int>>(width, {-1, -1}));
    std::queue<std::pair<int, int>> queue;
    
    queue.push({fromX, fromY});
    visited[fromY][fromX] = true;
    
    // Kierunki: góra, dół, lewo, prawo
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    while (!queue.empty())
    {
        auto [x, y] = queue.front();
        queue.pop();
        
        if (x == toX && y == toY)
        {
            // Odtwórz ścieżkę
            std::vector<std::pair<int, int>> path;
            int currX = toX, currY = toY;
            
            while (currX != -1 && currY != -1)
            {
                path.push_back({currX, currY});
                auto [px, py] = parent[currY][currX];
                currX = px;
                currY = py;
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Sprawdź sąsiadów
        for (int i = 0; i < 4; i++)
        {
            int newX = x + dx[i];
            int newY = y + dy[i];
            
            if (isValidPosition(newX, newY) && !visited[newY][newX])
            {
                // Pozwól przejść przez pozycję startową lub puste pola
                if ((newX == fromX && newY == fromY) || isEmpty(newX, newY))
                {
                    visited[newY][newX] = true;
                    parent[newY][newX] = {x, y};
                    queue.push({newX, newY});
                }
            }
        }
    }
    
    return {}; // Nie znaleziono ścieżki
}

bool Board::canMoveTo(int fromX, int fromY, int toX, int toY)
{
    auto path = findPath(fromX, fromY, toX, toY);
    return !path.empty();
}

void Board::moveBall(int fromX, int fromY, int toX, int toY)
{
    if (!isValidPosition(fromX, fromY) || !isValidPosition(toX, toY))
        return;
    
    if (balls[fromY][fromX] == nullptr || !isEmpty(toX, toY))
        return;
    
    // Przenieś kulkę
    balls[toY][toX] = std::move(balls[fromY][fromX]);
    balls[fromY][fromX] = nullptr;
    
    // Zaktualizuj pozycję kulki
    sf::Vector2f newPosition = getCellPosition(toX, toY);
    balls[toY][toX]->setPosition(newPosition);
    
    // Zaktualizuj grid
    grid[toY][toX] = grid[fromY][fromX];
    grid[fromY][fromX] = 0;
    
    // Sprawdź linie po ruchu
    auto lines = findAllLines();
    if (!lines.empty())
    {
        markLinesForRemoval(lines);
        startLineAnimation();
        // Nie dodajemy nowych kulek jeśli są linie do usunięcia
    }
    else
    {
        comboMultiplier = 1; // Reset combo jeśli nie ma linii
        // Dodaj nowe kulki tylko jeśli nie ma linii do usunięcia
        addNewBalls();
        
        // Sprawdź linie po dodaniu nowych kulek
        auto newLines = findAllLines();
        if (!newLines.empty())
        {
            markLinesForRemoval(newLines);
            startLineAnimation();
        }
        else
        {
            // Sprawdź czy gra się skończyła
            checkGameOver();
        }
    }
}

void Board::update()
{
    // Obsługa migania wybranej kulki
    if (hasBallSelected && blinkClock.getElapsedTime().asMilliseconds() > 500) // Miganie co 500ms
    {
        blinkState = !blinkState;
        blinkClock.restart();
    }
    
    // Obsługa animacji linii
    if (lineAnimationActive)
    {
        updateLineAnimation();
    }
    
    // Aktualizuj latające punkty
    updateFloatingScores();
}

std::vector<std::vector<std::pair<int, int>>> Board::findAllLines()
{
    std::vector<std::vector<std::pair<int, int>>> allLines;
    std::vector<std::vector<bool>> checked(height, std::vector<bool>(width, false));
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (grid[y][x] != 0 && !checked[y][x])
            {
                BallColor color = static_cast<BallColor>(grid[y][x] - 1);
                
                // Sprawdź wszystkie kierunki: →, ↓, ↘, ↙
                std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {1, 1}, {-1, 1}};
                
                for (auto [dx, dy] : directions)
                {
                    auto line = checkDirection(x, y, dx, dy, color);
                    if (line.size() >= 3)
                    {
                        allLines.push_back(line);
                        // Oznacz jako sprawdzone
                        for (auto [px, py] : line)
                        {
                            checked[py][px] = true;
                        }
                    }
                }
            }
        }
    }
    
    return allLines;
}

std::vector<std::pair<int, int>> Board::checkDirection(int startX, int startY, int dx, int dy, BallColor color)
{
    std::vector<std::pair<int, int>> line;
    int x = startX, y = startY;
    
    // Sprawdź w jedną stronę
    while (isValidPosition(x, y) && grid[y][x] == static_cast<int>(color) + 1)
    {
        line.push_back({x, y});
        x += dx;
        y += dy;
    }
    
    // Sprawdź w drugą stronę (bez startowej pozycji)
    x = startX - dx;
    y = startY - dy;
    while (isValidPosition(x, y) && grid[y][x] == static_cast<int>(color) + 1)
    {
        line.insert(line.begin(), {x, y});
        x -= dx;
        y -= dy;
    }
    
    return line;
}

void Board::markLinesForRemoval(const std::vector<std::vector<std::pair<int, int>>>& lines)
{
    // Wyczyść poprzednie oznaczenia
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            lineMarked[y][x] = false;
        }
    }
    
    // Oznacz nowe linie
    for (const auto& line : lines)
    {
        for (auto [x, y] : line)
        {
            lineMarked[y][x] = true;
        }
    }
}

void Board::startLineAnimation()
{
    lineAnimationActive = true;
    animationPhase = 0; // Zaczynamy od highlight
    lineAnimationClock.restart();
    fastBlinkState = true;
}

void Board::updateLineAnimation()
{
    auto elapsed = lineAnimationClock.getElapsedTime().asMilliseconds();
    
    switch (animationPhase)
    {
        case 0: // Highlight phase - 300ms
            if (elapsed > 300)
            {
                animationPhase = 1;
                lineAnimationClock.restart();
                blinkTimer.restart(); // Uruchom timer migania
            }
            break;
            
        case 1: // Fast blink phase - 1000ms (150ms intervals)
            // Obsługa migania co 150ms
            if (blinkTimer.getElapsedTime().asMilliseconds() > 150)
            {
                fastBlinkState = !fastBlinkState;
                blinkTimer.restart();
            }
            
            // Sprawdź czy minęła 1 sekunda od rozpoczęcia fazy 1
            if (elapsed > 1000)
            {
                animationPhase = 2;
                lineAnimationClock.restart();
            }
            break;
            
        case 2: // Remove phase
            removeLinesAndUpdateScore();
            lineAnimationActive = false;
            animationPhase = 0;
            break;
    }
}

void Board::removeLinesAndUpdateScore()
{
    int totalPoints = 0;
    int linesRemoved = 0;
    
    // Debug: sprawdź ile kulek ma być usuniętych
    int markedCount = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (lineMarked[y][x]) markedCount++;
        }
    }
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (lineMarked[y][x])
            {
                if (balls[y][x] != nullptr)
                {
                    // Dodaj latające punkty w pozycji kulki
                    sf::Vector2f pos = getCellPosition(x, y);
                    addScore(10 * comboMultiplier, pos);
                    totalPoints += 10 * comboMultiplier;
                    
                    // Usuń kulkę
                    balls[y][x] = nullptr;
                    grid[y][x] = 0;
                    linesRemoved++;
                }
                // Zawsze odznacz pole po przetworzeniu
                lineMarked[y][x] = false;
            }
        }
    }
    
    // Bonus za długość linii i combo
    if (linesRemoved >= 3)
    {
        int bonus = (linesRemoved - 2) * 30 * comboMultiplier;
        totalPoints += bonus;
        score += bonus;
    }
    
    score += totalPoints;
    comboMultiplier++;
    
    // Sprawdź czy powstały nowe linie (chain reaction)
    auto newLines = findAllLines();
    if (!newLines.empty())
    {
        markLinesForRemoval(newLines);
        startLineAnimation();
    }
    else
    {
        comboMultiplier = 1; // Reset combo
        
        // Dodaj nowe kulki tylko po zakończeniu wszystkich chain reactions
        addNewBalls();
        
        // Sprawdź czy po dodaniu nowych kulek powstały linie
        auto finalLines = findAllLines();
        if (!finalLines.empty())
        {
            markLinesForRemoval(finalLines);
            startLineAnimation();
        }
        else
        {
            checkGameOver();
        }
    }
}

int Board::calculateLineScore(int lineLength)
{
    // 5=50, 6=100, 7=200, 8+=500
    if (lineLength == 5) return 50;
    if (lineLength == 6) return 100;
    if (lineLength == 7) return 200;
    return 500;
}

void Board::addScore(int points, sf::Vector2f position)
{
    floatingScores.push_back({position, points});
    scoreAnimationClock.restart();
}

void Board::updateFloatingScores()
{
    auto elapsed = scoreAnimationClock.getElapsedTime().asSeconds();
    
    // Usuń stare latające punkty (po 2 sekundach)
    floatingScores.erase(
        std::remove_if(floatingScores.begin(), floatingScores.end(),
            [elapsed](const std::pair<sf::Vector2f, int>& score) {
                return elapsed > 2.0f;
            }),
        floatingScores.end()
    );
}

void Board::drawFloatingScores(sf::RenderWindow& window)
{
    // Implementacja w następnym kroku - potrzebujemy sf::Font
    // Na razie zostawiamy pustą metodę
}

bool Board::hasMarkedLines()
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (lineMarked[y][x]) return true;
        }
    }
    return false;
}

void Board::generateNextBalls()
{
    nextBalls.clear();
    for (int i = 0; i < 2; ++i) // Generuj tylko 2 kulki
    {
        nextBalls.push_back(getRandomColor());
    }
}

void Board::addNewBalls()
{
    if (gameOver) return;
    
    auto emptyPositions = getEmptyPositions();
    
    // Sprawdź czy jest miejsce na nowe kulki
    if (emptyPositions.size() < static_cast<size_t>(ballsToAdd))
    {
        // Dodaj tyle ile się da
        ballsToAdd = static_cast<int>(emptyPositions.size());
    }
    
    if (ballsToAdd == 0)
    {
        checkGameOver();
        return;
    }
    
    // Wymieszaj pozycje
    std::shuffle(emptyPositions.begin(), emptyPositions.end(), rng);
    
    // Dodaj kulki z nextBalls
    for (int i = 0; i < ballsToAdd && i < 2; ++i) // Maksymalnie 2 kulki
    {
        auto [x, y] = emptyPositions[i];
        BallColor color = nextBalls[i];
        placeBallAt(x, y, color);
    }
    
    // Wygeneruj nowe nextBalls
    generateNextBalls();
    ballsToAdd = 2; // Reset na następny ruch
}

std::vector<std::pair<int, int>> Board::getEmptyPositions()
{
    std::vector<std::pair<int, int>> emptyPos;
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (isEmpty(x, y))
            {
                emptyPos.push_back({x, y});
            }
        }
    }
    
    return emptyPos;
}

bool Board::hasAvailableMoves()
{
    // Sprawdź czy istnieje przynajmniej jedna kulka, która może się ruszyć
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (!isEmpty(x, y))
            {
                // Sprawdź czy ta kulka może się ruszyć w którymkolwiek kierunku
                for (int ty = 0; ty < height; ++ty)
                {
                    for (int tx = 0; tx < width; ++tx)
                    {
                        if (isEmpty(tx, ty) && canMoveTo(x, y, tx, ty))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Board::checkGameOver()
{
    auto emptyPositions = getEmptyPositions();
    
    // Jeśli nie ma miejsca na nowe kulki lub nie ma dostępnych ruchów
    if (emptyPositions.size() < 2 || !hasAvailableMoves()) // Sprawdź miejsce na 2 kulki
    {
        gameOver = true;
    }
}

void Board::drawNextBalls(sf::RenderWindow& window)
{
    // Rysuj preview następnych kulek w prawym górnym rogu
    float startX = 650.0f;
    float startY = 150.0f;
    float ballRadius = 15.0f;
    float spacing = 40.0f;
    
    for (size_t i = 0; i < nextBalls.size(); ++i)
    {
        sf::CircleShape previewBall(ballRadius);
        previewBall.setFillColor(getSFMLColorFromBallColor(nextBalls[i]));
        previewBall.setPosition({startX, startY + i * spacing});
        window.draw(previewBall);
    }
}

sf::Color Board::getSFMLColorFromBallColor(BallColor ballColor) const
{
    switch (ballColor)
    {
        case BallColor::Red:
            return sf::Color::Red;
        case BallColor::Green:
            return sf::Color::Green;
        case BallColor::Blue:
            return sf::Color::Blue;
        case BallColor::Yellow:
            return sf::Color::Yellow;
        case BallColor::Purple:
            return sf::Color::Magenta;
        case BallColor::Orange:
            return sf::Color(255, 165, 0);
        default:
            return sf::Color::White;
    }
}
