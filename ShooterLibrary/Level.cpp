

#include "ShooterLibrary.h"


Level::Level()
{
	m_sectorSize.X = 32;
	m_sectorSize.Y = 32;

	m_sectorCount.X = (Game::GetScreenWidth() / (int)m_sectorSize.X) + 1;
	m_sectorCount.Y = (Game::GetScreenHeight() / (int)m_sectorSize.Y) + 1;

	m_totalSectorCount = m_sectorCount.X * m_sectorCount.Y;

	m_pSectors = new std::vector<GameObject *>[m_totalSectorCount];
}

Level::~Level()
{
	delete[] m_pSectors;
}

void Level::LoadContent()
{
	AddGameObject(GetPlayerShip());
}

void Level::HandleInput(InputState *pInput)
{
	Vector2 direction = Vector2::Zero;
	if (pInput->IsKeyDown(ALLEGRO_KEY_DOWN)) direction.Y++;
	if (pInput->IsKeyDown(ALLEGRO_KEY_UP)) direction.Y--;
	if (pInput->IsKeyDown(ALLEGRO_KEY_RIGHT)) direction.X++;
	if (pInput->IsKeyDown(ALLEGRO_KEY_LEFT)) direction.X--;

	// Normalize the direction
	if (direction.X != 0 && direction.Y != 0)
	{
		direction *= Math::NORMALIZE_45;
	}

	// gamepad overrides keyboard input
	//Vector2 thumbstick = pInput->GetGamePadState(0).Thumbsticks.Left;
	//if (thumbstick != Vector2::Zero()) direction = thumbstick;

	GetPlayerShip()->SetDesiredDirection(direction);

	if (pInput->IsKeyDown(ALLEGRO_KEY_SPACE)) GetPlayerShip()->Fire();
}

void Level::Update(const GameTime *pGameTime)
{
	//m_particleManager.Update(pGameTime);

	for (unsigned int i = 0; i < m_totalSectorCount; i++)
	{
		m_pSectors[i].clear();
	}

	m_gameObjectIt = m_gameObjects.begin();
	for (; m_gameObjectIt != m_gameObjects.end(); m_gameObjectIt++)
	{
		GameObject *pGameObject = (*m_gameObjectIt);
		pGameObject->Update(pGameTime);
	}

	for (unsigned int i = 0; i < m_totalSectorCount; i++)
	{
		if (m_pSectors[i].size() > 1)
		{
			CheckCollisions(m_pSectors[i]);
		}
	}
}

void Level::CheckCollisions(std::vector<GameObject *> &gameObjects)
{
	const unsigned int objectCount = (unsigned int)gameObjects.size();

	GameObject::CollisionInstructions inst1, inst2;

	GameObject *pI, *pJ;

	for (unsigned int i = 0; i < objectCount - 1; i++)
	{
		pI = gameObjects[i];
		if (pI->IsActive())
		{
			for (unsigned int j = i + 1; i < objectCount; i++)
			{
				if (!pI->IsActive()) continue;

				pJ = gameObjects[j];
				if (pJ->IsActive())
				{
					if (GameObject::AreObjectsColliding(pI, pJ, &inst1, &inst2))
					{
						if (inst1.removeObject) pI->Deactivate();
						if (inst2.removeObject) pJ->Deactivate();

						if (inst1.damageToObject) pI->Hit(inst1.damageToObject);
						if (inst2.damageToObject) pJ->Hit(inst2.damageToObject);
					}
				}
			}
		}
	}
}


void Level::Draw(const GameTime *pGameTime)
{
	//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
	//if (m_pBackground) m_pBackground->Draw(pGameTime);

	//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	//m_particleManager.Draw(pGameTime);

	//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	GetSpriteBatch()->Begin();

	m_gameObjectIt = m_gameObjects.begin();
	for (; m_gameObjectIt != m_gameObjects.end(); m_gameObjectIt++)
	{
		GameObject *pGameObject = (*m_gameObjectIt);
		if (pGameObject->IsActive())
		{
			pGameObject->Draw(pGameTime);
		}
	}

	GetSpriteBatch()->End();

	/**
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
	m_explosionIt = m_explosions.begin();
	for (; m_explosionIt != m_explosions.end(); m_explosionIt++)
	{
		Explosion *pExplosion = (*m_explosionIt);
		if (pExplosion->IsActive())
		{
			pExplosion->Draw(pGameTime);
		}
	}
	/**/

	//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void Level::AddGameObject(GameObject *pGameObject)
{
	pGameObject->SetSpriteBatch(GetSpriteBatch());
	m_gameObjects.push_back(pGameObject);
}

void Level::UpdateSectorPosition(GameObject *pGameObject)
{
	Vector2 position = pGameObject->GetPosition();
	Vector2 halfDimensions = pGameObject->GetHalfDimensions();

	int minX = (int)(position.X - halfDimensions.X) / (int)m_sectorSize.X;
	int maxX = (int)(position.X + halfDimensions.X) / (int)m_sectorSize.X;
	int minY = (int)(position.Y - halfDimensions.Y) / (int)m_sectorSize.Y;
	int maxY = (int)(position.Y + halfDimensions.Y) / (int)m_sectorSize.Y;

	minX = (int)Math::Clamp(0, m_sectorCount.X - 1, minX);
	minY = (int)Math::Clamp(0, m_sectorCount.Y - 1, minY);
	maxX = (int)Math::Clamp(0, m_sectorCount.X - 1, maxX);
	maxY = (int)Math::Clamp(0, m_sectorCount.Y - 1, maxY);

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			int index = y * (int)m_sectorCount.Y + x;

			m_pSectors[index].push_back(pGameObject);
		}
	}
}
