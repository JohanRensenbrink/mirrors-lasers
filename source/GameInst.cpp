#include "GameInst.hpp"
#include "GUIManager.hpp"
#include "Helpers.hpp"
#include "block.hpp"
#include "Renderer.hpp"
#include "Player.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

GameInst::GameInst(GUIManager& a_GUIMgr, ResourceManager& a_ResMgr, Renderer& a_Renderer)
:	Scene(a_GUIMgr, a_ResMgr, a_Renderer)
,	m_Running(false)
,	m_Renderer(a_Renderer)
,	m_pQuitMenuButton(sfg::Button::Create("Quit to Main Menu"))
	//
{
	//grab the screen dimensions
	sf::Vector2f windowDim = m_GUIMgr.GetWindowDim();

	//to handle button presses and the like
	m_pSelectListener = new SelectListener(*this);
	/** Create Adjustment.
		* @param value Current value.
		* @param lower Minimum value.
		* @param upper Maximum value.
		* @param minor_step Minor change value (such as clicking on arrow button).
		* @param major_step Major change value (such as clicking on the scroll area).
		* @param page_size Page size (how many entries are visible / slider size).
		* @return Adjustment.
		*/

	//add a systemlist scrollbar
	/*sfg::Scrollbar::Ptr systemScrollBar = sfg::Scrollbar::Create(sfg::Scale::VERTICAL);
	m_pSystemList->Attach(systemScrollBar, sf::Rect<unsigned int>(1,1,1,1), 0, 0);
	sfg::Adjustment::Ptr adjustment = systemScrollBar->GetAdjustment();
	systemScrollBar->SetRequisition( sf::Vector2f( 0.f, windowDim.y/2 ) );

	// Tune the adjustment parameters.
	adjustment->SetLower( 20.f );
	adjustment->SetUpper( 100.f );

	// How much it should change when clicked on the stepper.
	adjustment->SetMinorStep( 1.f );

	// How much it should change when clicked on the trough.
	adjustment->SetMajorStep( 5.f );*/

	//setup gui
	sf::FloatRect allocRect;
	//double invHeightScalar = 10;
	//double invWidthScalar = 5;
	
	//quit to menu button
	sfg::Context::Get().GetEngine().SetProperty("Button", "FontSize", 20.0f);
	m_pQuitMenuButton->SetRequisition( sf::Vector2f(windowDim.x / 20, windowDim.y / 20) );
	allocRect = m_pQuitMenuButton->GetAllocation();
	m_pQuitMenuButton->SetPosition( sf::Vector2f(5, 5) );
	m_pQuitMenuButton->Show(false);
	m_GUIMgr.AddWidget(m_pQuitMenuButton);
	Widgets.push_back(m_pQuitMenuButton);

	//-------------------------------------- chipmunk physics
	// cpVect is a 2D vector and cpv() is a shortcut for initializing them.
	cpVect gravity = cpv(0, -100);
  
	// Create an empty space.
	m_pSpace = cpSpaceNew();
	cpSpaceSetGravity(m_pSpace, gravity);
  
	// Add a static line segment shape for the ground.
	// We'll make it slightly tilted so the ball will roll off.
	// We attach it to space->staticBody to tell Chipmunk it shouldn't be movable.

	//top
	m_WorldBounds.Bottom = cpSegmentShapeNew(m_pSpace->staticBody, cpv(0, windowDim.y), cpv(windowDim.x, windowDim.y), 0);
	cpShapeSetFriction(m_WorldBounds.Bottom, 1);
	cpSpaceAddShape(m_pSpace, m_WorldBounds.Bottom);
	//bottom
	m_WorldBounds.Bottom = cpSegmentShapeNew(m_pSpace->staticBody, cpv(0, 0), cpv(windowDim.x, 0), 0);
	cpShapeSetFriction(m_WorldBounds.Bottom, 1);
	cpSpaceAddShape(m_pSpace, m_WorldBounds.Bottom);
	//left
	m_WorldBounds.Bottom = cpSegmentShapeNew(m_pSpace->staticBody, cpv(0, 0), cpv(0, windowDim.y), 0);
	cpShapeSetFriction(m_WorldBounds.Bottom, 1);
	cpSpaceAddShape(m_pSpace, m_WorldBounds.Bottom);
	//right
	m_WorldBounds.Bottom = cpSegmentShapeNew(m_pSpace->staticBody, cpv(windowDim.x, 0), cpv(windowDim.x, windowDim.y), 0);
	cpShapeSetFriction(m_WorldBounds.Bottom, 1);
	cpSpaceAddShape(m_pSpace, m_WorldBounds.Bottom);

	m_pPlayer = new Player(*m_pSpace);

	/*
	// Now let's make a ball that falls onto the line and rolls off.
	// First we need to make a cpBody to hold the physical properties of the object.
	// These include the mass, position, velocity, angle, etc. of the object.
	// Then we attach collision shapes to the cpBody to give it a size and shape.

	cpFloat radius = 5;
	cpFloat mass = 1;

	// The moment of inertia is like mass for rotation
	// Use the cpMomentFor*() functions to help you approximate it.
	cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

	// The cpSpaceAdd*() functions return the thing that you are adding.
	// It's convenient to create and add an object in one line.
	cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
	cpBodySetPos(ballBody, cpv(0, 15));

	// Now we create the collision shape for the ball.
	// You can create multiple collision shapes that point to the same body.
	// They will all be attached to the body and move around to follow it.
	cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
	cpShapeSetFriction(ballShape, 0.7);
  
	// Now that it's all set up, we simulate all the objects in the space by
	// stepping forward through time in small increments called steps.
	// It is *highly* recommended to use a fixed size time step.
	cpFloat timeStep = 1.0/60.0;
	for(cpFloat time = 0; time < 2; time += timeStep)
	{
		cpVect pos = cpBodyGetPos(ballBody);
		cpVect vel = cpBodyGetVel(ballBody);
		printf(
			"Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
			time, pos.x, pos.y, vel.x, vel.y
		);

		cpSpaceStep(space, timeStep);
	}

	// Clean up our objects and exit!
	cpShapeFree(ballShape);
	cpBodyFree(ballBody);
	cpShapeFree(ground);
	cpSpaceFree(space);
	*/
}

bool GameInst::Start()
{
	m_Running = true;
	
	// Load the level
	LoadLevel();

	// Set up the laser sprites
	for (int i = 0; i < 1000; ++i) {
		SpriteID laser;
		m_ResMgr.CreateSprite("media/laser.png", &laser);
		m_laserSprites.push_back(laser);
	}

	// Add them to the draw list (has to be separate for some reason)
	for (auto it = m_laserSprites.begin(); it != m_laserSprites.end(); ++it) {
		m_Renderer.AddDrawableSprite(&(*it));
	}

	return true;
}

void GameInst::LoadLevel()
{
	/*Block *block = new Block(m_ResMgr);
	m_blocks.push_back(block);
	m_Renderer.AddDrawableSprite(block->Sprite());*/
	std::fstream file;
	file.open(("media/level1.txt"));
	std::string line;
	int curLine = 0;
	while( std::getline(file,line) )
	{
		//
		for(unsigned int i = 0; i < line.size();i++)
		{
			char c = line[i];
			switch(c)
			{
			case('#'):
				{
					Block *block = new Block(m_ResMgr,Block::BLOCK_SOLID);
					m_blocks.push_back(block);
					block->GetSprite()->sprite->setPosition(float(i)*32,float(curLine)*32);
					m_Renderer.AddDrawableSprite(block->GetSprite());
					break;
				}
			case('>'):
				{
					startX = i * 32;
					startY = curLine * 32 + 8;
					break;
				}
			}
			//
		}
		curLine++;
	}
	file.close();

	file.open(("media/level1_binds.txt"));
	while( std::getline(file,line) )
	{//
		char buffer[32];
		char* tokenBuffer;
		sprintf_s(buffer, 32, line.c_str());

		// Get the block IDs to link them
		int block1 = atoi(strtok_s(buffer,",", &tokenBuffer)) + atoi(strtok_s(NULL ,"=", &tokenBuffer)) * 32;
		int block2 = atoi(strtok_s(NULL,",", &tokenBuffer)) + atoi(strtok_s(NULL,",", &tokenBuffer)) * 32;
		
		// Link them
		m_blocks[block1]->SetOutput(block2);
	}
	file.close();
}

void GameInst::UnloadLevel() {
	// Clear blocks
	for (auto it = m_blocks.begin(); it != m_blocks.end();) {
		it = m_blocks.erase(it);
	}

	// Clear laser sprites
	for (auto it = m_laserSprites.begin();it != m_laserSprites.end();) {
		m_Renderer.RemoveDrawableSprite(&(*it));
		m_ResMgr.DeleteSprite((*it).ID);
		it = m_laserSprites.erase(it);
	}
}

void GameInst::Stop()
{
	m_Running = false;
	UnloadLevel();
}

void GameInst::Update(float a_dt)
{
	if(m_Running)
	{
		// Update block stuff
		for (auto it = m_blocks.begin(); it != m_blocks.end();++it)
		{
			//Check if a block is able to trigger.
			if ((*it)->Type(Block::BLOCK_DOOR) || (*it)->Type(Block::BLOCK_BUTTON) || (*it)->Type(Block::BLOCK_END)) {
				for (auto it2 = m_blocks.begin(); it2 != m_blocks.end();++it2) {
					sf::Vector2f sourcePos = (*it)->GetSprite()->sprite->getPosition();
					sf::Vector2f targetPos = (*it2)->GetSprite()->sprite->getPosition();
					
					// NOTE: Bounds are intentionally not matched with either tile size or bounding sizes to allow for fine tuning.
					//       Current values should be optimal.
					
					// Activate door if player is mostly on the same tile
					if ((*it)->Type(Block::BLOCK_DOOR) && (*it2)->Type(Block::BLOCK_PLAYER)) {
						if (targetPos.x + 8 > sourcePos.x && targetPos.x - 8 < sourcePos.x && targetPos.y < sourcePos.y && targetPos.y + 32 > sourcePos.y) {
							// Complete level
							(*it2)->SetActivated(true);
						}
					}

					// Press button if player is standing anywhere above the tile.
					if ((*it)->Type(Block::BLOCK_BUTTON) && (*it2)->Type(Block::BLOCK_PLAYER)) {
						if (targetPos.x + 63 > sourcePos.x && targetPos.x - 31 < sourcePos.x && targetPos.y < sourcePos.y && targetPos.y + 8 > sourcePos.y) {
							// Press button
							(*it)->SetActivated(true);
						}
					}

					// if (BLOCK_END && LASER) activate door --- make the laser first!
				}
			}

			// Check if a block has triggered.
			if ((*it)->GetActivated()) {
				int targetBlock = (*it)->GetOutput();

				// If the block has an output, activate it.
				if (targetBlock != -1) m_blocks[targetBlock]->SetActivated(true);

				// Depress buttons
				if ((*it)->Type(Block::BLOCK_BUTTON)) (*it)->SetActivated(false);
			}
		}
	}

	// Calculate and create new laser path
	sf::Vector2f laserPos = sf::Vector2f((float)startX,(float)startY);
	sf::Vector2f laserDir = sf::Vector2f(4.0f,0.0f);
	unsigned int iter = 0;
	while (laserPos.x >= 0 && laserPos.x <= 1024 && laserPos.y >= 0 && laserPos.y <= 768 && iter < m_laserSprites.size()) {
		m_laserSprites[iter].sprite->setPosition(laserPos);
		
		laserPos += laserDir;
		++iter;
	}
}

void GameInst::UpdateGUISizes()
{
	//
}

void GameInst::Select(void* a_pSelectee, SelectListener::Type a_SelectedType)
{
	//
}
