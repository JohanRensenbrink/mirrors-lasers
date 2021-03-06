#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.hpp"

#include "Defs.hpp"

namespace sf
{
	class Sprite;
}

class Animator;

class Block : public GameObject
{
public:
	enum BlockType {
		BLOCK_SOLID = 0,
		BLOCK_GLASS,
		BLOCK_START,
		BLOCK_END
	};
	Block(ResourceManager& a_ResMgr, cpSpace& a_Space, int a_type, sf::Vector2f a_Pos);
	~Block();
	//
	void SetOutput(int a_block);
	const int GetOutput();
	void SetActivated(bool a_state);
	void Update(float a_dt);
	const bool GetActivated();
	const int Type();
	const bool Type(int a_type);
	//
private:
	int m_output;
	int m_blockType;
	bool m_activated;
	sf::Vector2f m_Pos;
	Animator* m_pAnimator;
	//
	StaticRigidQuad m_BoxBounds;
};

#endif // BLOCK_H