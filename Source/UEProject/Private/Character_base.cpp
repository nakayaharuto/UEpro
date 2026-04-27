// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_base.h"
#include "Camera/CameraComponent.h"//カメラ
#include "GameFramework/SpringArmComponent.h"//カメラアーム
#include "Components/CapsuleComponent.h"//当たり判定
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/ItemData.h"
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

	/** 速度の反映 */
	GetCharacterMovement()->MaxWalkSpeed = ACharacter_base::BaseSpeed;


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

	/** 開始時に倍率を適用 */
	UpdateMovementSpeed();

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

/** マッピングコンテキスト */
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

void ACharacter_base::UpdateMovementSpeed()
{
	if (GetCharacterMovement())
	{
		/** 基本速度 * 倍率で最終的な速度の決定 */
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;

		UE_LOG(LogTemp, Log, TEXT("Current Speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
}

///////////////////////////////////////////////////////////////////////
/**インベントリ*/

/**インベントリの構成*/
void ACharacter_base::AddItemToInventory(FName ItemID)
{
	if (ItemID.IsNone()) return;

	//持っていれば個数を増やす
	// FindOrAdd 1回で「追加」と「加算」を同時に行う（最も安全で速い書き方）
	int32& CurrentCount = Inventory.FindOrAdd(ItemID);
	CurrentCount++;

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

TArray<FCraftPrediction> ACharacter_base::GetAvailableWeaponList()
{
	TArray<FCraftPrediction> DisplayList;

	// 1. 全てのレシピをチェックして「作れるもの」を探す
	// UpdateCraftingPredictions() の結果（CraftPredictions）を利用
	for (const FCraftPrediction& Prediction : CraftPredictions)
	{
		if (Prediction.bCanCraft)
		{
			FCraftPrediction Info;
			Info.ResultItemName = Prediction.ResultItemName;
			Info.bOwned = Inventory.Contains(Info.ResultItemName) && Inventory[Info.ResultItemName] > 0;
			Info.bCanCraft = true;
			DisplayList.Add(Info);
		}
	}

	// 2. 「すでに持っている武器」の中で、上記リストに入っていないものを追加
	for (auto& Elem : Inventory)
	{
		FName ItemID = Elem.Key;
		// ※ここで ItemDataTable を参照して「武器フラグ」が True のものだけを抽出する
		if (IsWeapon(ItemID))
		{
			// すでにリストにある（＝作れるし持っている）場合はスキップ
			bool bAlreadyAdded = DisplayList.ContainsByPredicate([&](const FCraftPrediction& W) { return W.ResultItemName == ItemID; });

			if (!bAlreadyAdded && Elem.Value > 0)
			{
				FCraftPrediction Info;
				Info.ResultItemName = ItemID;
				Info.bOwned = true;
				Info.bCanCraft = false; // すでに持っているので「素材から作る」判定は不要
				DisplayList.Add(Info);
			}
		}
	}

	return DisplayList;
}


/** 武器の切り替え */
void ACharacter_base::ChangeWeapon(FName NewWeaponID)
{
	/** 持っていない武器には代えられない */
	if (!Inventory.Contains(NewWeaponID) || Inventory[NewWeaponID] <= 0) return;

	WeaponID = NewWeaponID;

	/** 武器のメッシュを変えたり、攻撃力を呼び出す処理 */
	UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon Changed to: %s"), *NewWeaponID.ToString());
}

/** 指定したIDが武器かどうかを判定する内部関数 */
bool ACharacter_base::IsWeapon(FName ItemID)
{
	if (!ItemDataTable) return false;

	// 前に作ったアイテムデータの構造体（FItemData）を取得
	static const FString ContextString(TEXT("Weapon Check Context"));
	FItemData* Data = ItemDataTable->FindRow<FItemData>(ItemID, ContextString);

	if (Data)
	{
		return Data->ItemType == EItemType::Weapon;
	}
	return false;
}