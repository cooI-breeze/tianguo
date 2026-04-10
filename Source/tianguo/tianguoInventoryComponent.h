#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "tianguoItemData.h"
#include "tianguoInventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIANGUO_API UtianguoInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UtianguoInventoryComponent();
	
	// 添加物品（自动找空位）
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(FName ItemID);

	// 移除物品（根据ID）
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(FName ItemID);

	// 查询是否有指定ID的物品
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItem(FName ItemID) const;

	// 获取所有物品
	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<FtianguoInventoryItem> GetAllItems() const;

private:
	// 物品数组（初始为空，自然增长到10）
	UPROPERTY()
	TArray<FtianguoInventoryItem> Items;

	// 最大容量
	static const int32 MAX_SIZE = 10;
};
