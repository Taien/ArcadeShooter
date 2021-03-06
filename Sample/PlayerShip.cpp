
/*   .oooooo..o       .o.       ooo        ooooo ooooooooo.   ooooo        oooooooooooo
	d8P'    `Y8      .888.      `88.       .888' `888   `Y88. `888'        `888'     `8
	Y88bo.          .8"888.      888b     d'888   888   .d88'  888          888
	 `"Y8888o.     .8' `888.     8 Y88. .P  888   888ooo88P'   888          888oooo8
		 `"Y88b   .88ooo8888.    8  `888'   888   888          888          888    "
	oo     .d8P  .8'     `888.   8    Y     888   888          888       o  888       o
	8""88888P'  o88o     o8888o o8o        o888o o888o        o888ooooood8 o888ooooood8

					        Sample � 2017 - Shuriken Studios LLC                          */

#include "Sample.h"

using ShooterLibrary::Projectile;

namespace Sample
{
	PlayerShip::PlayerShip()
	{
		SetSpeed(450);
		m_isAIControlled = false;
	}


	void PlayerShip::HandleInput(const InputState *pInput)
	{
		if (!m_isAIControlled) ShooterLibrary::PlayerShip::HandleInput(pInput);
	}


	void PlayerShip::Update(const GameTime *pGameTime)
	{
		if (m_pThrusterAnimation) m_pThrusterAnimation->Update(pGameTime);

		if (m_isAIControlled)
		{
			Vector2 targetBefore = m_targetPosition - GetPosition();
			Vector2 normalized = targetBefore;
			normalized.Normalize();

			SetDesiredDirection(normalized);
			ShooterLibrary::PlayerShip::Update(pGameTime);

			Vector2 targetAfter = m_targetPosition - GetPosition();
			if (targetBefore.LengthSquared() < targetAfter.LengthSquared())
			{
				SetPosition(m_targetPosition);
				m_isAIControlled = false;
				ConfineToScreen();
			}
		}
		else
		{
			ShooterLibrary::PlayerShip::Update(pGameTime);
		}
	}


	void PlayerShip::Draw(SpriteBatch *pSpriteBatch)
	{
		if (m_pThrusterAnimation)
		{
			Vector2 origin(m_pThrusterAnimation->GetCurrentFrame()->Width / 2, 0);
			pSpriteBatch->Draw(m_pThrusterAnimation, GetPosition() + Vector2(-2, 14), Color::White, origin);
		}

		ShooterLibrary::PlayerShip::Draw(pSpriteBatch);
	}


	void PlayerShip::Hit(const float damage)
	{
		ShooterLibrary::PlayerShip::Hit(damage);

		if (!IsActive())
		{
			Level *pLevel = (Level *)GetCurrentLevel();
			pLevel->SpawnExplosion(GetPosition(), 1);
		}
	}
}