// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Base.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character_base.h"

// Sets default values
AItem_Base::AItem_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/**当たり判定の設定*/
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(100.f);

	/**メッシュの設定*/
	ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemNiagara"));
	ItemNiagara->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	Super::BeginPlay();
	ApplyItemData();
	
	// オーバーラップイベントの登録
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem_Base::OnOverlapBegin);
}

// Called every frame
void AItem_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem_Base::ApplyItemData()

{
	if (ItemDataTable && !ItemRowName.IsNone())
	{
		static const FString ContextString(TEXT("Item Data Context"));
		FItemData* Data = ItemDataTable->FindRow<FItemData>(ItemRowName, ContextString);

		if (Data && Data->PickupNiagara.IsValid())
		{
			// Niagaraシステムをセット
			ItemNiagara->SetAsset(Data->PickupNiagara.LoadSynchronous());
			ItemNiagara->Activate();
		}
	}
}

void AItem_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// プレイヤーが触れたかチェック
	if (ACharacter_base* Player = Cast<ACharacter_base>(OtherActor))
	{
		// ここでプレイヤーのインベントリに追加する処理を呼ぶ（後述）
		// Player->AddItemToInventory(ItemRowName);

		// 拾ったので消える
		Destroy();
	}
}