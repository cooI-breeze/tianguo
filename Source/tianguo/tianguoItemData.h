#pragma once

#include "CoreMinimal.h"
#include "tianguoItemData.generated.h"

USTRUCT(BlueprintType)
struct FtianguoInventoryItem
{
	GENERATED_BODY()

	// 物品唯一ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FName ItemID;

	// 图标
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	// TSoftObjectPtr<UTexture2D> Icon;

	// 构造函数
	FtianguoInventoryItem() {}
	FtianguoInventoryItem(FName InItemID) : ItemID(InItemID) {}
    
	// 是否有效
	bool IsValid() const { return !ItemID.IsNone(); }
};
