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
	CollisionComponent->ShapeColor = FColor::Red; //当たり判定の線の色を赤にする
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger")); //トリガーとして設定
	CollisionComponent->SetGenerateOverlapEvents(true);         // オーバーラップを許可
	CollisionComponent->bHiddenInGame = false;

	/**メッシュの設定*/
	ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemNiagara"));
	ItemNiagara->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	// オーバーラップイベントの登録
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem_Base::OnOverlapBegin);
	}

	Super::BeginPlay();
	ApplyItemData();
	
}

// Called every frame
void AItem_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem_Base::ApplyItemData()
{
	// ハンドルから直接行データを取得
	FItemData* Data = ItemDataHandle.GetRow<FItemData>(TEXT("Item Data Context"));

	if (Data)
	{
		// ハンドルに保存されている「行の名前」を取得（インベントリ追加用など）
		ItemRowName = ItemDataHandle.RowName;

		if (Data->PickupNiagara.IsValid())
		{
			(void)Data->PickupNiagara.LoadSynchronous();
			ItemNiagara->SetAsset(Data->PickupNiagara.Get());
			ItemNiagara->Activate(true);
		}
	}
}

void AItem_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// コンパイラの警告（未使用の引数）を回避
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;

	// 1. 当たった相手がプレイヤー（ACharacter_base）か確認
	ACharacter_base* Player = Cast<ACharacter_base>(OtherActor);

	if (Player)
	{
		// 2. ログを出力（デバッグ用）
		UE_LOG(LogTemp, Warning, TEXT("Item Collected: %s"), *ItemRowName.ToString());

		// 3. プレイヤーのインベントリに追加
		// ※戻り値がある場合は (void) で受け流す
		Player->AddItemToInventory(ItemRowName);

		// 4. Niagaraの演出を切り離して残す
		if (ItemNiagara)
		{
			// 戻り値 void への対応として (void) キャスト
			(void)ItemNiagara->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			ItemNiagara->Deactivate();
			ItemNiagara->SetAutoDestroy(true);
		}

		// 5. アイテム（当たり判定）自体を消去
		(void)Destroy();
	}
}