#include "../Header/Messenger.h"

Event<void(GAME_STATE)> Messenger::OnGamePhase;
Event<void()>  Messenger::OnShot;
Event<void(GameObject*, bool)> Messenger::BallMovement;