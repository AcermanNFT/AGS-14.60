#pragma once
#include "framework.h"
#include "Utils.h"

#include "LootTypes.h"
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <string>
#include <cstdlib>
#include <ctime> 
namespace AndreuGayMoment
{ 
inline void sinCos222(float* ScalarSin, float* ScalarCos, float Value)
{
    float quotient = (0.31830988618f * 0.5f) * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f * 3.1415926535897932f) * quotient;

    float sign;
    if (y > 1.57079632679f)
    {
        y = 3.1415926535897932f - y;
        sign = -1.0f;
    }
    else if (y < -1.57079632679f)
    {
        y = -3.1415926535897932f - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    *ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ScalarCos = sign * p;
}

inline FQuat FRotToQuat222(FRotator Rot)
{
    const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
    const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

   sinCos222(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
   sinCos222(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
   sinCos222(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

template<typename T>
T* SpawnActor222(SDK::FVector Loc, SDK::FRotator Rot = SDK::FRotator(), SDK::AActor* Owner = nullptr, SDK::UClass* Class = T::StaticClass(), ESpawnActorCollisionHandlingMethod Handle = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{
    SDK::FTransform Transform{};
    Transform.Scale3D = SDK::FVector{ 1,1,1 };
    Transform.Translation = Loc;
    Transform.Rotation = FRotToQuat222(Rot);
    return (T*)SDK::UGameplayStatics::GetDefaultObj()->FinishSpawningActor(SDK::UGameplayStatics::GetDefaultObj()->BeginDeferredActorSpawnFromClass(SDK::UWorld::GetWorld(), Class, Transform, Handle, Owner), Transform);
}
inline void SpawnPickup222222(FVector Loc, UFortItemDefinition* Def, EFortPickupSourceTypeFlag Flag, EFortPickupSpawnSource SpawnSource, int Count = 1, int LoadedAmmo = 0, AFortPawn* Owner = nullptr);
inline void SpawnPickup222222(FVector Loc, UFortItemDefinition* Def, EFortPickupSourceTypeFlag Flag, EFortPickupSpawnSource SpawnSource, int Count, int LoadedAmmo, AFortPawn* Owner)
{
    AFortPickupAthena* Pickup = SpawnActor222<AFortPickupAthena>(Loc);
    Pickup->bRandomRotation = true;
    Pickup->PrimaryPickupItemEntry.ItemDefinition = Def;
    Pickup->PrimaryPickupItemEntry.Count = Count;
    Pickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
    Pickup->OnRep_PrimaryPickupItemEntry();

    if (Flag == EFortPickupSourceTypeFlag::Container)
    {
        Pickup->bTossedFromContainer = true;
        Pickup->OnRep_TossedFromContainer();
    }

    Pickup->TossPickup(Loc, nullptr, -1, true, false, Flag, SpawnSource);
}
}
inline FVector PickSupplyDropLocation(AFortAthenaMapInfo* MapInfo, FVector Center, float Radius)
{
    static FVector* (*PickSupplyDropLocationOriginal)(AFortAthenaMapInfo * MapInfo, FVector * outLocation, __int64 Center, float Radius) = decltype(PickSupplyDropLocationOriginal)(__int64(GetModuleHandleA(0)) + 0x18848f0);

    if (!PickSupplyDropLocationOriginal)
        return FVector(0, 0, 0);

    FVector Out = FVector(0, 0, 0);
    auto ahh = PickSupplyDropLocationOriginal(MapInfo, &Out, __int64(&Center), Radius);
    return Out;
}

inline FVector FindllamaSpawn(AFortAthenaMapInfo* MapInfo, FVector Center, float Radius)
{
    static FVector* (*PickSupplyDropLocationOriginal)(AFortAthenaMapInfo * MapInfo, FVector * outLocation, __int64 Center, float Radius) = decltype(PickSupplyDropLocationOriginal)(__int64(GetModuleHandleA(0)) + 0x18848f0);

    if (!PickSupplyDropLocationOriginal)
        return FVector(0, 0, 0);


    FVector Out = FVector(0, 0, 0);
    auto ahh = PickSupplyDropLocationOriginal(MapInfo, &Out, __int64(&Center), Radius);
    return Out;
}

inline UFortItemDefinition* GetRandomItemByProbability(TArray<LootItemInfo>& Items) {
    int32 TotalWeight = 0;
    for (int32 i = 0; i < Items.Num(); i++) {
        TotalWeight += Items[i].Probability;
    }

    int32 RandomWeight = std::rand() % TotalWeight;
    int32 CumulativeWeight = 0;

    for (int32 i = 0; i < Items.Num(); i++) {
        CumulativeWeight += Items[i].Probability;
        if (RandomWeight < CumulativeWeight) {
            return Items[i].ItemDefinition;
        }
    }
}

inline char __fastcall SpawnllamaLoot(ABuildingContainer* Object)
{
    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);
    UFortItemDefinition* AmmoItemHeavy = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
    if (AmmoItemHeavy) {
        AndreuGayMoment::SpawnPickup222222(Loc, AmmoItemHeavy, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 90, 1);
    }
    UFortItemDefinition* AmmoItemAR = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    if (AmmoItemAR) {
        AndreuGayMoment::SpawnPickup222222(Loc, AmmoItemAR, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 150, 1);
    }
    UFortItemDefinition* AmmoItemSMG = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
    if (AmmoItemSMG) {
        AndreuGayMoment::SpawnPickup222222(Loc, AmmoItemSMG, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 180, 1);
    }
    UFortItemDefinition* AmmoItemShotGun = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    if (AmmoItemShotGun) {
        AndreuGayMoment::SpawnPickup222222(Loc, AmmoItemShotGun, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 30, 1);
    }
    static auto WoodDef = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
    static auto StoneDef = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
    static auto MetalDef = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
    AndreuGayMoment::SpawnPickup222222(Loc, WoodDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    AndreuGayMoment::SpawnPickup222222(Loc, StoneDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    AndreuGayMoment::SpawnPickup222222(Loc, MetalDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    TArray<ConsumableInfo> GetConsumable = Consumable();
    for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn 
    {
        int32 ConsumableIndex = std::rand() % GetConsumable.Num();
        ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
        UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
        int Quantity = RandomConsumable.Quantity;
        AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);
    }
    return 1;
}

inline char __fastcall SpawnLoot(ABuildingContainer* Object)
{
   std::string ClassName = Object->Class->GetName();

    if (ClassName.contains("Tiered_Chest_Apollo_IceBox"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<LootItemInfo> GetIceBox = IceBox();
        UFortItemDefinition* Coal = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Seasonal/WID_Athena_Bucket_Coal.WID_Athena_Bucket_Coal"); // i hate you vs
        UFortItemDefinition* SnowMan = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/SneakySnowmanV2/AGID_SneakySnowmanV2.AGID_SneakySnowmanV2"); // i hate you vs

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 items spawn
        {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetIceBox);
            if (RandomItem)
            {
                if (RandomItem == Coal || RandomItem == SnowMan)
                {
                    AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1, 5);
                }
                else
                {
                    AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1, 1);
                }
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else if (ClassName.contains("Tiered_Chest"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

        TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForChest();
        TArray<LootItemInfo> GetShotgun = ShotgunForChest();
        TArray<LootItemInfo> GetSMG = SMGForChest();
        TArray<LootItemInfo> GetPistol = PistolForChest();
        TArray<LootItemInfo> GetSniper = SniperForChest();
        TArray<LootItemInfo> GetRPG = RPGForChest();

        int32_t RandomCategory = std::rand() % 6 + 1; //really proper


        if (RandomCategory == 1) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 30);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 20, 30);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 2) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 5);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 5);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 3) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 4) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 5) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 4, 1);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 6) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetRPG);
            if (RandomItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
                UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
                if (AmmoItem) {
                    AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 9, 1);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else if (ClassName.contains("Tiered_Ammo"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<AmoInfo> GetAmo = Amo();

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 types of amo spawn
        {
            int32 AmoIndex = std::rand() % GetAmo.Num();
            AmoInfo& RandomAmo = GetAmo[AmoIndex];
            UFortItemDefinition* RandomAmu = RandomAmo.Definition;
            int Quantity = RandomAmo.Quantity;
            AndreuGayMoment::SpawnPickup222222(Loc, RandomAmu, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, Quantity, 1);
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else  if (ClassName.contains("Barrel_FishingRod"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<LootItemInfo> GetBarrel = Barrel();
        UFortItemDefinition* Harpon = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/HappyGhost/WID_Athena_HappyGhost.WID_Athena_HappyGhost"); // i hate you vs

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 items spawn
        {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetBarrel);
            if (RandomItem)
            {
                if (RandomItem == Harpon) // if i directly do smth like "WID_Athena_HappyGhost" it gives me Error C2446	'==': no conversion from 'const char [22] to 'SDK::UFortItemDefinition * but this workes some how
                {
                    AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 10);
                }
                else
                {
                    AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 9999); // idk what amo i should set it??? and nullptr doesnt work (this is for fishing rod)
                }
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

}

inline char __fastcall SpawnSupplyLoot(ABuildingContainer* Object)
{

    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

    TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForSupply();
    TArray<LootItemInfo> GetShotgun = ShotgunForSupply();
    TArray<LootItemInfo> GetSMG = SMGForSupply();
    TArray<LootItemInfo> GetPistol = PistolForSupply();
    TArray<LootItemInfo> GetSniper = SniperForSupply();
    TArray<LootItemInfo> GetRPG = RPGForSupply();

    int32_t RandomCategory = std::rand() % 6 + 1; //really proper


    if (RandomCategory == 1) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 30);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 20, 30);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 2) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 5);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 5, 5);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 3) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 28);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 5, 18);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 4) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 5) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 4, 1);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 6) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetRPG);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 9, 1);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            };
        }
    }

    //these are handled by the game and i dont need to do these (it actaully crashes if i do)
    //Object->bAlreadySearched = true;
    //Object->SearchBounceData.SearchAnimationCount++;
    //Object->BounceContainer();
    //Object->OnRep_bAlreadySearched();

    return 1;
}

inline char __fastcall SpawnFloorLoot(ABuildingContainer* Object)
{
    if (Object->bAlreadySearched) return 0;

    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

    TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForFloor();
    TArray<LootItemInfo> GetShotgun = ShotgunForFloor();
    TArray<LootItemInfo> GetSMG = SMGForFloor();
    TArray<LootItemInfo> GetPistol = PistolForFloor();
    TArray<LootItemInfo> GetSniper = SniperForFloor();

    int32_t RandomCategory = std::rand() % 10 + 1; // Increased range to 10 for better control its cause for mats

    if (RandomCategory >= 1 && RandomCategory <= 2) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 30);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 20, 30);
            }
        }
    }
    else if (RandomCategory == 3 || RandomCategory == 4) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 5);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 5);
            }
        }
    }
    else if (RandomCategory == 5 || RandomCategory == 6) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
        }
    }
    else if (RandomCategory == 7) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
        }
    }
    else if (RandomCategory == 8) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
        if (RandomItem) {
            AndreuGayMoment::SpawnPickup222222(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
            if (AmmoItem) {
                AndreuGayMoment::SpawnPickup222222(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 4, 1);
            }
        }
    }
    else if (RandomCategory == 9)
    {
        TArray<MatsInfo> GetMats = Mats();

        int32 MatsIndex = std::rand() % GetMats.Num();
        MatsInfo& RandomMats = GetMats[MatsIndex];
        UFortItemDefinition* RandomMat = RandomMats.Definition;
        int QuantityM = RandomMats.Quantity;
        AndreuGayMoment::SpawnPickup222222(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
    }
    else if (RandomCategory == 10)
    {
        TArray<ConsumableInfo> GetConsumable = Consumable();

        int32 ConsumableIndex = std::rand() % GetConsumable.Num();
        ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
        UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
        int Quantity = RandomConsumable.Quantity;
        AndreuGayMoment::SpawnPickup222222(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);
    }
    return 1;
}
