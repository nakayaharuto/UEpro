// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_base.h"
#include "Camera/CameraComponent.h"//カメラ
#include "GameFramework/SpringArmComponent.h"//カメラアーム
#include "Components/CapsuleComponent.h"//当たり判定
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ItemRecipeData.h"
#include "InputActionValue.h"

// Sets default values
ACharacter_base::ACharacter_base()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	/** キャラが回った時、カメラも回るのを止める */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/** カメラアーム */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;//俯瞰視点
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	/** カメラ本体 */
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	/** プレイヤーの向き */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

///////////////////////////////////////////////////////////////////////////
/** アクション関数 */
// Called when the game starts or when spawned
void ACharacter_base::BeginPlay()
{
	Super::BeginPlay();

	/** マッピングコンテキスト */
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

/** 移動アクション */
void ACharacter_base::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		//ワールドのx,y
		const FVector ForwardDir = FVector::ForwardVector;
		const FVector RightDir = FVector::RightVector;

		AddMovementInput(ForwardDir, MovementVector.Y);
		AddMovementInput(RightDir, MovementVector.X);
	}
}

// Called every frame
void ACharacter_base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacter_base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/** 移動アクション */
		if(MoveAction) EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacter_base::Move);
	}

}

/**インベントリの構成*/
void ACharacter_base::AddItemToInventory(FName ItemID)
{
	if (ItemID.IsNone()) return;

	//持っていれば個数を増やす
	if (Inventory.Contains(ItemID))
	{
		Inventory[ItemID]++;
	}
	else
	{
		Inventory.Add(ItemID, 1);
	}

	int32& Count = Inventory.FindOrAdd(ItemID);
	Count++;

	/*拾うたびに予測結果の更新*/
	UpdateCraftingPredictions();

	UE_LOG(LogTemp, Log, TEXT("Inventory Update: %s (Count: %d)"), *ItemID.ToString(), Inventory[ItemID]);
}

/**インベントリ内での予測結果*/
void ACharacter_base::UpdateCraftingPredictions()
{
	if (!RecieTable) return;

	CraftPredictions.Empty();//レシピのリセット

	/*レシピを走査*/
	TArray<FItemRecipeData*> AllReipes;
	RecieTable->GetAllRows<FItemRecipeData>(TEXT("RecipeContext"), AllReipes);

	for (FItemRecipeData* Recipe : AllReipes)
	{
		int32 Total = 0;
		int32 CurrentOwned = 0;
		bool component = false;

		//チェック
		for (auto& Elem : Recipe->RequiredMaterials)
		{
			FName MatName = Elem.Key;
			int32 NeedCount = Elem.Value;

			Total += NeedCount;

			if (Inventory.Contains(MatName))
			{
				// 持っている数（必要数を超えて持っている場合は必要数で止める）
				CurrentOwned += FMath::Min(Inventory[MatName], NeedCount);
				component = true;
			}
		}

		// 1つでも素材を持っていれば予報リストに加える
		if (component)
		{
			FCraftPrediction Prediction;
			Prediction.ResultItemName = Recipe->ResultItemName;
			Prediction.Progress = (float)CurrentOwned / (float)Total;
			Prediction.bCanCraft = (CurrentOwned >= Total);

			CraftPredictions.Add(Prediction);
		}
	}

	// 進捗（Progress）が高い順にソートする（あと少しで完成するものを上に）
	CraftPredictions.Sort([](const FCraftPrediction& A, const FCraftPrediction& B) {
		return A.Progress > B.Progress;
		});
}
