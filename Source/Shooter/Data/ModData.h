#pragma once

#include "CoreMinimal.h"
#include "Shooter/Types/ModType.h"
#include "ModData.generated.h"

class AMod;

USTRUCT()
struct FModData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EModType ModType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMod> ModClass;

};
