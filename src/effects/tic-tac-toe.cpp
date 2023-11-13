#include "effects.h"
#include "elapsedMillis.h"

enum GameState
{
    P1 = 0,
    P2 = 1,
    Win = 2,
    Tie = 3
};

struct PlayerColors
{
    CRGB P1;
    CRGB P2;
};

// Defines three indexes that make a tic tac toe win line
struct WinLine
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

WinLine topRow = {0, 1, 2};
WinLine middleRow = {3, 4, 5};
WinLine bottomRow = {6, 7, 8};
WinLine firstCol = {0, 5, 6};
WinLine middleCol = {1, 4, 7};
WinLine lastCol = {2, 3, 8};
WinLine oneCross = {0, 4, 8};
WinLine otherCross = {2, 4, 6};

const uint8_t lineCount = 8;

WinLine *lines[lineCount] = {&topRow, &middleRow, &bottomRow, &firstCol, &middleCol, &lastCol, &oneCross, &otherCross};

const uint16_t defStepDelay = 1000;
const uint16_t winStepDelay = 2500;
const uint16_t tieStepDelay = 1100;
const uint16_t partialWinStepDelay = 1500;

const uint8_t ledCount = 9;

PlayerColors pColors = {CRGB::Red, CRGB::Blue};

// XXX We can only use colors that have the same value in channles, otherwise the fade out is ugly
const PlayerColors pOptions[3] = {{CRGB::Blue, CRGB::Red}, {CRGB::Yellow, CRGB::Green}, {CRGB::Cyan, CRGB::Orange}};

// Move timer
static elapsedMillis stamp;

// Game progress tracker
static GameState gameState = GameState::P1;

// Le game board state
static uint8_t gameBoard[ledCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// Values we wish to fade in/out to
static CRGB targetValues[ledCount] = {CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black};

static int8_t winningIndexes[3] = {-1, -1, -1};

static CRGB winner = CRGB::Black;

static void switchColors()
{
    // uint8_t idx = random(0, 3);
    // pColors.P1 = pOptions[idx].P1;
    // pColors.P2 = pOptions[idx].P2;
}

static bool isEmpty()
{
    bool result = true;

    for (size_t i = 0; i < ledCount; i++)
    {
        if (gameBoard[i] != 0)
        {
            result = false;
        }
    }

    return result;
}

static void place(uint8_t num)
{
    long idx = random(0, ledCount);

    if (gameBoard[idx] != 0)
    {
        place(num);
    }
    else
    {
        gameBoard[idx] = num;
    }
}

static void smartPlace(uint8_t num)
{
    // If board is empty - simply place it randomly
    if (isEmpty())
    {
        long idx = random(0, ledCount);
        gameBoard[idx] = num;
        return;
    }

    // Check if opponent has double, if so ruin it
    for (size_t i = 0; i < lineCount; i++)
    {
        WinLine *line = lines[i];

        uint8_t result = 0;
        uint8_t target = 0;

        // Not equal to other and not zero
        if (gameBoard[line->x] != num && gameBoard[line->x] != 0)
        {
            result++;
        }

        if (gameBoard[line->y] != num && gameBoard[line->y] != 0)
        {
            result++;
        }

        if (gameBoard[line->z] != num && gameBoard[line->z] != 0)
        {
            result++;
        }

        if (result == 2)
        {
            if (gameBoard[line->x] == 0)
            {
                target = line->x;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->y] == 0)
            {
                target = line->y;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->z] == 0)
            {
                target = line->z;
                gameBoard[target] = num;
                return;
            }
        }
    }

    // Check if we have double somewhere, if so place it there
    for (size_t i = 0; i < lineCount; i++)
    {
        WinLine *line = lines[i];

        uint8_t result = 0;
        uint8_t target = 0;

        // Equal to this
        if (gameBoard[line->x] == num)
        {
            result++;
        }

        if (gameBoard[line->y] == num)
        {
            result++;
        }

        if (gameBoard[line->z] == num)
        {
            result++;
        }

        if (result == 2)
        {
            if (gameBoard[line->x] == 0)
            {
                target = line->x;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->y] == 0)
            {
                target = line->y;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->z] == 0)
            {
                target = line->z;
                gameBoard[target] = num;
                return;
            }
        }
    }

    // Choose a line with atleaset 1 and make it two
    for (size_t i = 0; i < lineCount; i++)
    {
        WinLine *line = lines[i];

        uint8_t result = 0;
        uint8_t target = 0;

        // Equal to this
        if (gameBoard[line->x] == num)
        {
            result++;
        }

        if (gameBoard[line->y] == num)
        {
            result++;
        }

        if (gameBoard[line->z] == num)
        {
            result++;
        }

        if (result == 1)
        {
            if (gameBoard[line->x] == 0)
            {
                target = line->x;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->y] == 0)
            {
                target = line->y;
                gameBoard[target] = num;
                return;
            }

            if (gameBoard[line->z] == 0)
            {
                target = line->z;
                gameBoard[target] = num;
                return;
            }
        }
    }

    // Place randomly otherwise
    place(num);
}

// TODO refactor to use winlines
static bool winCheck(uint8_t target)
{
    // Top Line
    if (gameBoard[0] == target && gameBoard[1] == target && gameBoard[2] == target)
    {
        winningIndexes[0] = 0;
        winningIndexes[1] = 1;
        winningIndexes[2] = 2;

        return true;
    }

    // Middle Line
    if (gameBoard[3] == target && gameBoard[4] == target && gameBoard[5] == target)
    {
        winningIndexes[0] = 3;
        winningIndexes[1] = 4;
        winningIndexes[2] = 5;

        return true;
    }

    // Bottom Line
    if (gameBoard[6] == target && gameBoard[7] == target && gameBoard[8] == target)
    {
        winningIndexes[0] = 6;
        winningIndexes[1] = 7;
        winningIndexes[2] = 8;

        return true;
    }

    // First col
    if (gameBoard[0] == target && gameBoard[5] == target && gameBoard[6] == target)
    {
        winningIndexes[0] = 0;
        winningIndexes[1] = 5;
        winningIndexes[2] = 6;

        return true;
    }

    // Middle col
    if (gameBoard[1] == target && gameBoard[4] == target && gameBoard[7] == target)
    {
        winningIndexes[0] = 1;
        winningIndexes[1] = 4;
        winningIndexes[2] = 7;

        return true;
    }

    // Last col
    if (gameBoard[2] == target && gameBoard[3] == target && gameBoard[8] == target)
    {
        winningIndexes[0] = 2;
        winningIndexes[1] = 3;
        winningIndexes[2] = 8;

        return true;
    }

    // Cross one
    if (gameBoard[0] == target && gameBoard[4] == target && gameBoard[8] == target)
    {
        winningIndexes[0] = 0;
        winningIndexes[1] = 4;
        winningIndexes[2] = 8;

        return true;
    }

    // Cross other
    if (gameBoard[2] == target && gameBoard[4] == target && gameBoard[6] == target)
    {
        winningIndexes[0] = 2;
        winningIndexes[1] = 4;
        winningIndexes[2] = 6;

        return true;
    }

    return false;
}

static bool isOver()
{
    bool isOver = true;
    for (size_t i = 0; i < ledCount; i++)
    {
        if (gameBoard[i] == 0)
        {
            isOver = false;
            break;
        }
    }

    return isOver;
}

static void performEndOfTurnChecks()
{
    if (winCheck(1))
    {
        winner = pColors.P1;
        gameState = GameState::Win;
    }
    else if (winCheck(2))
    {
        winner = pColors.P2;
        gameState = GameState::Win;
        switchColors();
    }
    else if (isOver())
    {
        gameState = GameState::Tie;
        switchColors();
    }
    else
    {
        if (gameState == GameState::P1)
        {
            gameState = GameState::P2;
        }
        else
        {
            gameState = GameState::P1;
        }
    }
}

static void updateBoard(CRGB leds[])
{
    for (size_t i = 0; i < ledCount; i++)
    {
        if (gameBoard[i] == 1)
        {
            targetValues[i] = pColors.P1;
        }
        else if (gameBoard[i] == 2)
        {
            targetValues[i] = pColors.P2;
        }
        else
        {
            targetValues[i] = CRGB::Black;
        }
    }
}

static void brightnessPass(CRGB leds[])
{
    for (size_t i = 0; i < ledCount; i++)
    {
        CRGB *l = &leds[i];

        if (l->red != targetValues[i].red)
        {

            if (l->red > targetValues[i].red)
            {
                l->red -= 1;
            }
            else if (l->red < targetValues[i].red)
            {
                l->red += 1;
            }
        }

        if (l->green != targetValues[i].green)
        {
            if (l->green > targetValues[i].green)
            {
                l->green -= 1;
            }
            else if (l->green < targetValues[i].green)
            {
                l->green += 1;
            }
        }

        if (l->blue != targetValues[i].blue)
        {
            if (l->blue > targetValues[i].blue)
            {
                l->blue -= 1;
            }
            else if (l->blue < targetValues[i].blue)
            {
                l->blue += 1;
            }
        }
    }
}

static void playTicTacToe(CRGB leds[], uint8_t num_leds)
{
    brightnessPass(leds);

    uint16_t delay = defStepDelay;
    if (gameState == GameState::Win)
    {
        delay = winStepDelay;
    }
    else if (gameState == GameState::Tie)
    {
        delay = tieStepDelay;
    }

    // Perform game step
    if (stamp > delay)
    {
        switch (gameState)
        {
        case GameState::P1:
            smartPlace(1);
            updateBoard(leds);
            performEndOfTurnChecks();
            break;
        case GameState::P2:
            smartPlace(2);
            updateBoard(leds);
            performEndOfTurnChecks();
            break;
        case GameState::Win:
        case GameState::Tie:
            gameState = GameState::P1;

            for (size_t i = 0; i < ledCount; i++)
            {
                gameBoard[i] = 0;
                targetValues[i] = CRGB::Black;
            }

            winningIndexes[0] = -1;
            winningIndexes[1] = -1;
            winningIndexes[2] = -1;

            break;
        }

        stamp = 0;
    }

    if (gameState == GameState::Win)
    {
        if (stamp < partialWinStepDelay)
        {
            targetValues[winningIndexes[0]].fadeToBlackBy(10);
            targetValues[winningIndexes[1]].fadeToBlackBy(10);
            targetValues[winningIndexes[2]].fadeToBlackBy(10);
        }
        else
        {
            fadeToBlackBy(targetValues, num_leds, 1);
        }
    }
    else if (gameState == GameState::Tie)
    {
        fadeToBlackBy(targetValues, num_leds, 1);
    }

    EVERY_N_MILLISECONDS(400)
    {
        if (gameState == GameState::Win)
        {
            if (stamp < partialWinStepDelay)
            {
                targetValues[winningIndexes[0]] = winner;
                targetValues[winningIndexes[1]] = winner;
                targetValues[winningIndexes[2]] = winner;

                leds[winningIndexes[0]] = winner;
                leds[winningIndexes[1]] = winner;
                leds[winningIndexes[2]] = winner;
            }
        }
    }
}

Effect ticTacToe = {EffectId::TicTacToe, &playTicTacToe};