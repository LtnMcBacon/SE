#include"PlayState.h"
#include <Profiler.h>
#include <Core\TransformManager.h>
#include <Core\CameraManager.h>
#include <Core\CollisionManager.h>
#include <Core\DebugRenderManager.h>
#include <Core\EntityManager.h>
#include <Core\RenderableManager.h>
using namespace SE;
using namespace Gameplay;

PlayState::PlayState()
{

}

PlayState::PlayState(Window::IWindow* Input)
{
	this->Input = Input;
}

PlayState::~PlayState()
{

}

void PlayState::UpdateInput(PlayerUnit::MovementInput* &Movement)
{

	if (Input->ButtonPressed(1))
	{
		Movement->downW = true;
	}
	if (Input->ButtonPressed(2))
	{
		Movement->downS = true;
	}
	if (Input->ButtonPressed(3))
	{
		Movement->downA = true;
	}
	if (Input->ButtonPressed(4))
	{
		Movement->downD = true;
	}

	int mX, mY;
	Input->GetMousePos(mX, mY);
	Movement->mousePosX = mX;
	Movement->mousePosY = mY;

}

void PlayState::InitializeRooms()
{



}
void PlayState::InitializePlayer()
{

}

IGameState::State PlayState::Update(void* passableInfo)
{
	StartProfile;

	IGameState::State empty = State::PLAY_STATE;
	PlayerUnit::MovementInput* MovementInput = new PlayerUnit::MovementInput;


	UpdateInput(MovementInput);


	delete MovementInput;
	ProfileReturn(empty);

}