// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBase/GameMode_Base.h"
#include "Character_base.h"
#include "UObject/ConstructorHelpers.h"


AGameMode_Base::AGameMode_Base()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_Character_base"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}
