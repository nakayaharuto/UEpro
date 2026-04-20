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

};
