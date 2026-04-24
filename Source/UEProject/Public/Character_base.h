// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Character_base.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

/*予測結果を保持*/
USTRUCT(BlueprintType)
struct FCraftPrediction
{
	GENERATED_BODY()

	/*何かできるか*/
	UPROPERTY(BlueprintReadOnly)
	FName ResultItemName;

	/*?/?個インベントリにあるか*/
	UPROPERTY(BlueprintReadOnly)
	float Progress;

	/*合成可能か*/
	UPROPERTY(BlueprintReadOnly)
	bool bCanCraft;
};

/*Playerクラス*/
UCLASS()
class UEPROJECT_API ACharacter_base : public ACharacter
{
	GENERATED_BODY()
public:
	/** マッピングコンテキスト  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** 移動アクション  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<UInputAction> MoveAction;

	/** 攻撃アクション  */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))

	//TObjectPtr<UInputAction> AttackAction;

	/** 採取アクション  */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))

	//TObjectPtr<UInputAction> CollectAction;

	/**インベントリ*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> Inventory;

	/**インベントリ関数*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToInventory(FName ItemID);

	/*セット用関数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting",meta = (AllowPrivateAccess = "true"))
	UDataTable* RecieTable;

	// 現在の「予報」リスト（UIはこれを見て表示を変える）
	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	TArray<FCraftPrediction> CraftPredictions;



private:
	/** メインカメラ  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** カメラアーム  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<USpringArmComponent> CameraBoom;
	
public:
	// Sets default values for this character's properties
	ACharacter_base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 入力  */
	void Move(const FInputActionValue& Value);//移動

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// レシピを再計算する関数
	void UpdateCraftingPredictions();

};
