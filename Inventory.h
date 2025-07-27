#pragma once
#include "Utils.h"

namespace Inventory
{
	float GetMaxStackSize(UFortItemDefinition* Def)
	{
		if (auto CurveTable = Def->MaxStackSize.Curve.CurveTable)
		{
			EEvaluateCurveTableResult Result;
			float ReturnValue;
			Utils::GetDefObject<UDataTableFunctionLibrary>()->EvaluateCurveTableRow(CurveTable, Def->MaxStackSize.Curve.RowName, Def->MaxStackSize.Value, &Result, &ReturnValue, FString());
			return ReturnValue;
		}
		return Def->MaxStackSize.Value;
	}

	void Update(AFortPlayerController* Controller, FFortItemEntry* Dirty = nullptr)
	{
		if (!Controller->WorldInventory)
			return;

		Controller->WorldInventory->bRequiresLocalUpdate = true;
		Controller->WorldInventory->HandleInventoryLocalUpdate();

		if (Dirty)
			Controller->WorldInventory->Inventory.MarkItemDirty(*Dirty);
		else
			Controller->WorldInventory->Inventory.MarkArrayDirty();
	}

	FFortItemEntry* FindItemEntry(AFortPlayerController* Controller, FGuid& OtherGuid)
	{
		if (!Controller) return nullptr;
		for (int i = 0; i < Controller->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (Controller->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == OtherGuid)
				return &Controller->WorldInventory->Inventory.ReplicatedEntries[i];
		}
		return nullptr;
	}

	FFortItemEntry* FindItemEntry(AFortPlayerController* Controller, UFortItemDefinition* Def)
	{
		if (Controller && Def)
		{
			for (int i = 0; i < Controller->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
			{
				if (Controller->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
					return &Controller->WorldInventory->Inventory.ReplicatedEntries[i];
			}
		}
		return nullptr;
	}

	void GiveItem(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo, bool bStack)
	{
		if (Count == 0)
			return;

		auto Item = Utils::Cast<UFortWorldItem>(ItemDefinition->CreateTemporaryItemInstanceBP(Count, 0));

		if (!bStack)
		{
			Item->OwnerInventory = PlayerController->WorldInventory;
			Item->SetOwningControllerForTemporaryItem(PlayerController);

			Item->ItemEntry.LoadedAmmo = LoadedAmmo;
			Item->ItemEntry.Count = Count;
			Item->ItemEntry.ItemDefinition = ItemDefinition;

			PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
			PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
			PlayerController->WorldInventory->Inventory.MarkItemDirty(Item->ItemEntry);
			PlayerController->WorldInventory->HandleInventoryLocalUpdate();
		}
		else
		{
			bool bFound = false;

			for (auto& ItemEntry : PlayerController->WorldInventory->Inventory.ReplicatedEntries)
			{
				if (ItemEntry.ItemDefinition == ItemDefinition)
				{
					bFound = true;

					ItemEntry.Count += Count;

					for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
					{
						PlayerController->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo = ItemEntry.LoadedAmmo;
						PlayerController->WorldInventory->Inventory.ReplicatedEntries[i].Count = ItemEntry.Count;
						break;
					}

					PlayerController->WorldInventory->Inventory.MarkItemDirty(ItemEntry);
					PlayerController->WorldInventory->HandleInventoryLocalUpdate();
				}
			}

			if (!bFound)
			{
				GiveItem(PlayerController, ItemDefinition, Count, LoadedAmmo, false);
			}
		}
	}

	void RemoveItem2(AFortPlayerController* PC, FGuid& ItemGuid, int Count = -1)
	{
		for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == ItemGuid)
			{
				if (Count == -1 || PC->WorldInventory->Inventory.ReplicatedEntries[i].Count <= Count)
				{
					PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
					break;
				}
				else
				{
					PC->WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
					Update(PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
					return;
				}
			}
		}

		for (size_t i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == ItemGuid)
			{
				PC->WorldInventory->Inventory.ItemInstances.Remove(i);
				break;
			}
		}

		Update(PC); // MarkArrayDirty
	}

	void RemoveItem2(AFortPlayerController* PC, UFortItemDefinition* ItemDef, int Count = -1)
	{
		for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == ItemDef)
			{
				if (Count == -1 || PC->WorldInventory->Inventory.ReplicatedEntries[i].Count <= Count)
				{
					PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
					break;
				}
				else
				{
					PC->WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
					Update(PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
					return;
				}
			}
		}

		for (size_t i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemDefinition == ItemDef)
			{
				PC->WorldInventory->Inventory.ItemInstances.Remove(i);
				break;
			}
		}

		Update(PC); // MarkArrayDirty
	}

	void RemoveItem(AFortPlayerController* PlayerController, FGuid Guid, int Count)
	{
		for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			auto& ReplicatedEntry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[i];

			if (ReplicatedEntry.ItemGuid == Guid)
			{
				ReplicatedEntry.Count -= Count;

				if (ReplicatedEntry.Count > 0)
				{
					PlayerController->WorldInventory->Inventory.MarkItemDirty(ReplicatedEntry);
				}
				else
				{
					PlayerController->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
				}

				break;
			}
		}

		for (int i = 0; i < PlayerController->WorldInventory->Inventory.ItemInstances.Num(); i++)
		{
			auto& ItemInstance = PlayerController->WorldInventory->Inventory.ItemInstances[i];

			if (ItemInstance->ItemEntry.ItemGuid == Guid)
			{
				ItemInstance->ItemEntry.Count -= Count;

				if (ItemInstance->ItemEntry.Count > 0)
				{
					PlayerController->WorldInventory->Inventory.MarkItemDirty(ItemInstance->ItemEntry);
				}
				else
				{
					PlayerController->WorldInventory->Inventory.ItemInstances.Remove(i);
				}

				break;
			}
		}

		PlayerController->WorldInventory->bRequiresLocalUpdate = true;
		PlayerController->WorldInventory->HandleInventoryLocalUpdate();
		PlayerController->WorldInventory->Inventory.MarkArrayDirty();
	}

	UFortWorldItem* CreateItem(AFortPlayerController* PC, UFortItemDefinition* Def, int Count)
	{
		if (!PC || !Def)
			return nullptr;

		if (auto NewItem = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(Count, 0))
		{
			NewItem->SetOwningControllerForTemporaryItem(PC);
			return NewItem;
		}

		return nullptr;
	}

	FFortItemEntry* GiveItemProper(AFortPlayerController* PC, UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = -1)
	{
		if (!Def || !PC || !PC->WorldInventory)
			return nullptr;

		int MaxStackSize = GetMaxStackSize(Def);
		//LOG_("MaxStackSize: {}", MaxStackSize);

		if (auto FoundEntry = FindItemEntry(PC, Def))
		{
		//	LOG_("FOUND ITEM ENTYR");
			int CurrentCount = FoundEntry->Count;
			int CountAfterIncrement = CurrentCount + Count;

			FoundEntry->Count += Count;
			Update(PC, FoundEntry);

			if (CountAfterIncrement > MaxStackSize)
			{
				PC->ServerAttemptInventoryDrop(FoundEntry->ItemGuid, CountAfterIncrement - MaxStackSize, false);
			}

			return FoundEntry;
		}

		if (auto NewItem = CreateItem(PC, Def, Count))
		{
			if (LoadedAmmo != -1)
				NewItem->ItemEntry.LoadedAmmo = LoadedAmmo;

			PC->WorldInventory->Inventory.ItemInstances.Add(NewItem);
			PC->WorldInventory->Inventory.ReplicatedEntries.Add(NewItem->ItemEntry);

			Update(PC);

			return &NewItem->ItemEntry;
		}
		return nullptr;
	}

	FFortItemEntry* GiveItemProper(AFortPlayerController* PC, FFortItemEntry* ItemEntry)
	{
		if (!ItemEntry || !PC || !PC->WorldInventory)
			return nullptr;

		return GiveItemProper(PC, ItemEntry->ItemDefinition, ItemEntry->Count, ItemEntry->LoadedAmmo);
	}

	void ModifyEntry(AFortPlayerControllerAthena* PC, FFortItemEntry& Entry)
	{
		for (size_t i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == Entry.ItemGuid)
			{
				PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry = Entry;
				break;
			}
		}
	}

	void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon)
	{
		for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
			{
				PC->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo = Weapon->AmmoCount;
				ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
				Update((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
				break;
			}
		}
	}

	/*void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon, int AmountToAdd)
	{
		for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
			{
				PC->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo += AmountToAdd;
				ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
				Update((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
				break;
			}
		}*/
}