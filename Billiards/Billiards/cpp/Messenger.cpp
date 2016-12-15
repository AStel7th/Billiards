#include "../Header/Messenger.h"

Event<void()> Messenger::OnGameStart;
Event<void(GAME_STATE)> Messenger::OnGamePhase;
Event<void(GAME_STATE)> Messenger::OnGameStateRequest;
Event<void(GAME_STATE , int)> Messenger::OnTurnChange;
Event<void(int)> Messenger::OnGameFinish;
Event<void(GameObject*, bool)> Messenger::BallMovement;
Event<void(GameObject*)> Messenger::BallInPocket;
Event<void(GameObject*)> Messenger::FirstHitBall;