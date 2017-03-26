
#include "ShooterLibrary.h"

Texture *Projectile::s_pTexture = nullptr;
Vector2 Projectile::s_textureOrigin = Vector2::Zero;

Projectile::Projectile()
{
	m_speed = 500;
	m_damage = 1;
	m_direction.Set(0, -1);
	SetCollisionRadius(8);
}

void Projectile::Update(const GameTime *pGameTime)
{
	if (IsActive())
	{
		Vector2 translation = m_direction * m_speed * pGameTime->GetTimeElapsed();
		TranslatePosition(translation);

		if (GetPosition().Y < s_textureOrigin.Y) Deactivate();
	}
}

void Projectile::Draw(const GameTime *pGameTime)
{
	if (s_pTexture)
	{
		GetSpriteBatch()->Draw(s_pTexture, GetPosition(), Color::White, s_textureOrigin);
	}
}

void Projectile::Activate(const Vector2 &position, bool isShotByPlayer)
{
	GameObject::Activate();

	SetPosition(position);
}

void Projectile::SetTexture(Texture *pTexture)
{
	s_pTexture = pTexture;
	s_textureOrigin.Set(s_pTexture->GetSize() / 2);
}