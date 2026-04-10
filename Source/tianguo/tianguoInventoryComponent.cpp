#include "tianguoInventoryComponent.h"

UtianguoInventoryComponent::UtianguoInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UtianguoInventoryComponent::AddItem(FName ItemID)
{
	if (ItemID.IsNone()) return false;
    
	// 达到最大数量，直接返回false
	if (Items.Num() >= MAX_SIZE) return false;
    
	// 直接添加到数组末尾
	Items.Add(FtianguoInventoryItem(ItemID));
	return true;
}

bool UtianguoInventoryComponent::RemoveItem(FName ItemID)
{
	if (ItemID.IsNone()) return false;
    
	// 查找并移除第一个匹配的物品
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].ItemID == ItemID)
		{
			Items.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool UtianguoInventoryComponent::HasItem(FName ItemID) const
{
	if (ItemID.IsNone()) return false;
    
	for (const FtianguoInventoryItem& Item : Items)
	{
		if (Item.ItemID == ItemID) return true;
	}
	return false;
}

TArray<FtianguoInventoryItem> UtianguoInventoryComponent::GetAllItems() const
{
	return Items;
}
