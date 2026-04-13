#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "MyUnrealEdEngine.generated.h"

UCLASS()
class YOURMODULENAME_API UMyUnrealEdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()

public:
	void Init(IEngineLoop* InEngineLoop) override;
};
