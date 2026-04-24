// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemRecipeData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemRecipeData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
    // 完成するアイテムのRowName（DT_ItemDataと紐付け）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
    FName ResultItemName;

    // 必要な素材とその個数（TMapで管理）
    // Key: 素材のRowName, Value: 必要数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
    TMap<FName, int32> RequiredMaterials;

    // 進化後の武器アイコン（UI表示用。ResultItemNameから引いても良いが、ここにあると便利）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
    TSoftObjectPtr<UTexture2D> ResultIcon;

    // 合成に必要な「金床レベル」など、将来的な拡張用
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
    int32 RequiredAnvilLevel = 1;*/

};
