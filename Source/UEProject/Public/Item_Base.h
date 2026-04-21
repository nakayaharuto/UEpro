// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Data/ItemData.h"
#include "Item_Base.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UDataTable;


UCLASS()
class UEPROJECT_API AItem_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**当たり判定のカプセル*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<USphereComponent> CollisionComponent;

	/**見た目のカプセル*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<UNiagaraComponent> ItemNiagara;

	/**データテーブルの行を参照*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemRowName;

	/**本体への参照*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UDataTable> ItemDataTable;

	/**プレイヤーが触れた時のイベント*/
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/**データテーブルから情報を反映させる関数*/
	void ApplyItemData();

};
