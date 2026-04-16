/**ItemData.h*/
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

// アイテムのカテゴリー（武器、素材、消費アイテムなど）
UENUM(BlueprintType)
enum class EItemType : uint8
{
    Health      UMETA(DisplayName = "Health"),     //回復系統
    Weapon      UMETA(DisplayName = "Weapon"),     //ジョブの元になる武器
    Material    UMETA(DisplayName = "Material"),   //強化用の素材
    Permanent   UMETA(DisplayName = "Permanent")   //永続スキルに関連する特殊アイテム
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

public:

    FItemData()
        : ItemName(FText::GetEmpty())
        , ItemType(EItemType::Material)
        , PowerValue(0.0f)
    {
    }

    /** アイテム名 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    /** アイテムの種類 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    /** 強化値や攻撃力への加算値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float PowerValue;

    /** アイテムのアイコン（UI表示用） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TSoftObjectPtr<UTexture2D> ItemIcon;

    /** フィールドに落ちている時の見た目（StaticMesh） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TObjectPtr<UStaticMesh> PickupMesh;

    /** アイテムの説明文 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText Description;

};