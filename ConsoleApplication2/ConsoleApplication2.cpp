#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "BirdsGroup.h"
using namespace std;

#include "olcConsoleGameEngine.h"

constexpr int numOfBirds = 10;
constexpr float gameSpeed = 1.0f;


class OneLoneCoder_FlappyBird : public olcConsoleGameEngine
{
public:
	OneLoneCoder_FlappyBird():fGravity(130.0f), birds(numOfBirds, ScreenHeight() / 2.0f, 0.0f, fGravity)
	{
		m_sAppName = L"Flappy Bird";
	}

private:

	float fGravity;
	BirdsGroup birds;
	float fLevelPosition = 0.0f;
	int birdX = (int)(ScreenWidth() / 3.0f);

	float fSectionWidth;
	deque<int> listSection = { 0, 0, 0, 0, 0, 0 };

	int nAttemptCount = 0;

	void resetGame() {
		listSection = { 0, 0, 0, 0, 0, 0 };
		birds.reborn(ScreenHeight() / 2.0f, 0.0f);
		nAttemptCount++;
	}

	void drawDisplay() {
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
	}

	//Section Service
	void updateSectionsPositions(float elapsedTime) {
		fLevelPosition += gameSpeed * 12.0f * elapsedTime;

		if (fLevelPosition > fSectionWidth)
		{
			fLevelPosition -= fSectionWidth;
			listSection.pop_front();
			int i = rand() % (ScreenHeight() - 20);
			if (i <= 10) i = 0;
			listSection.push_back(i);
		}
	}

	void drawSections() {
		int nSection = 0;
		for (auto s : listSection)
		{
			if (s != 0)
			{
				Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s, nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(), PIXEL_SOLID, FG_GREEN);
				Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0, nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 15, PIXEL_SOLID, FG_GREEN);
			}
			nSection++;
		}
	}

	void drawNewFitnessPointLine() {
		const int nSection = 1;
		auto s = listSection[nSection];
		{
			if (s != 0)
			{
				Fill(nSection * fSectionWidth + 14 - fLevelPosition, ScreenHeight() - s - 15, nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s, PIXEL_SOLID, FG_RED);
			}
		}
	}

	//Bird Service
	void killBirdIfHasCollision() {
		for (int i = 0; i < numOfBirds; ++i) {
			int birdY = birds[i].getPosition();
			if (birds[i].isAlive()) {
				bool birdHasCollision = birds[i].getPosition() < 2 || birds[i].getPosition() > ScreenHeight() - 2 ||
					m_bufScreen[(int)(birdY + 0) * ScreenWidth() + birdX].Char.UnicodeChar != L' ' ||
					m_bufScreen[(int)(birdY + 1) * ScreenWidth() + birdX].Char.UnicodeChar != L' ' ||
					m_bufScreen[(int)(birdY + 0) * ScreenWidth() + birdX + 3].Char.UnicodeChar != L' ' ||
					m_bufScreen[(int)(birdY + 1) * ScreenWidth() + birdX + 3].Char.UnicodeChar != L' ';
				if (birdHasCollision) {
					birds.birdDie(i);
				}
			}
		}
	}
	bool birdIsOnCheckpoint() {
		const int nSection = 1;
		int s = listSection[nSection];
		{
			if (s != 0)
			{
				return birdX == int(nSection * fSectionWidth + 15 - fLevelPosition);
			}
		}
		return false;
	}

	void incrementFitnessIfBirdPassCheckpoint(float elapsedTime) {
		for (int i = 0; i < numOfBirds; ++i) {
			if (birds[i].isAlive()) {
				if (birds[i].checkIfBirdHasMovedFromLastCheck(fLevelPosition)) {
					if (birdIsOnCheckpoint()) {
						birds[i].incrementFitnessPoint();
					}
				}
			}
		}
	}

	void drawBirdMovesUp(int birdX, int birdY) {
		DrawString(birdX, birdY + 0, L"\\");
		DrawString(birdX, birdY + 1, L"<\\Q");
	}

	void drawBirdMovesDown(int birdX, int birdY) {
		DrawString(birdX, birdY + 0, L"</Q");
		DrawString(birdX, birdY + 1, L"/");
	}

	void drawLivingBirds() {
		for (int i = 0; i < numOfBirds; ++i) {
			if (birds[i].isAlive()) {
				int birdY = birds[i].getPosition();
				if (birds[i].getVelocity() > 0)
				{
					drawBirdMovesUp(birdX, birdY);
				}
				else
				{
					drawBirdMovesDown(birdX, birdY);
				}
			}
		}
	}
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		resetGame();
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Game
		if (birds.allBirdsDied())
		{
			// Do nothing until user releases space
			if (m_keys[VK_SPACE].bReleased)
				resetGame();
		}
		else
		{
			if (m_keys[VK_SPACE].bPressed )//&& fBirdVelocity >= fGravity / 10.0f)
			{
				birds.flap();
			}

			drawDisplay();

			updateSectionsPositions(fElapsedTime);
			drawSections();
			//drawNewFitnessPointLine();

			birds.updatePosition(fElapsedTime);
			killBirdIfHasCollision();
			incrementFitnessIfBirdPassCheckpoint(fElapsedTime);
			drawLivingBirds();

			DrawString(1, 1, L"Attempt: " + to_wstring(nAttemptCount) + L" Best Bird Score " + to_wstring(birds.getBestFitnessPoint()) + L" Best High Score: " + to_wstring(birds.getMaxBestFitnessPoint()) );
		}

		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_FlappyBird game;
	game.ConstructConsole(80, 52, 16, 16);
	game.Start();

	return 0;
}