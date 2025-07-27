#pragma once
#include "framework.h"
#include "Utils.h"
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <string>
#include <cstdlib>
#include <ctime>   

struct LootItemInfo {
    UFortItemDefinition* ItemDefinition;
    int32 Probability;
};

struct ConsumableInfo {
    UFortItemDefinition* Definition;
    int Quantity;
};

struct AmoInfo {
    UFortItemDefinition* Definition;
    int Quantity;
};

struct MatsInfo {
    UFortItemDefinition* Definition;
    int Quantity;
};

//floor loot
inline TArray<LootItemInfo> AssaultRifleForFloor() {
    TArray<LootItemInfo> AssaultRifleForFloor;
    //Ar
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_C_Ore_T02.WID_Assault_Auto_Athena_C_Ore_T02"), 65 });
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03"), 7 });
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03"), 3 });

    //burst
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_C_Ore_T02.WID_Assault_SemiAuto_Athena_C_Ore_T02"), 65 });
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_UC_Ore_T03.WID_Assault_SemiAuto_Athena_UC_Ore_T03"), 7 });
    AssaultRifleForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_R_Ore_T03.WID_Assault_SemiAuto_Athena_R_Ore_T03"), 3 });

    return AssaultRifleForFloor;
}

inline TArray<LootItemInfo> ShotgunForFloor() {
    TArray<LootItemInfo> ShotgunForFloor;
    //Pump
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_Common.WID_Shotgun_Standard_Athena_Common"), 65 });
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03"), 7 });
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03"), 3 });

    //Tac
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_UC_Ore_T03.WID_Shotgun_SemiAuto_Athena_UC_Ore_T03"), 65 });
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03"), 7 });
    ShotgunForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03"), 3 });

    return ShotgunForFloor;
}

inline TArray<LootItemInfo> SMGForFloor() {
    TArray<LootItemInfo> SMGForFloor;
    //smg
    SMGForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03"), 65 });
    SMGForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03"), 7 });
    SMGForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03"), 3 });

    return SMGForFloor;
}

inline TArray<LootItemInfo> PistolForFloor() {
    TArray<LootItemInfo> PistolForFloor;
    //Pistol
    PistolForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_C_Ore_T02.WID_Pistol_SemiAuto_Athena_C_Ore_T02"), 65 });
    PistolForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03"), 7 });
    PistolForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03"), 3 });

    return PistolForFloor;
}

inline TArray<LootItemInfo> SniperForFloor() {
    TArray<LootItemInfo> SniperForFloor;
    //Sniper
    SniperForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_UC_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_UC_Ore_T03"), 65 });
    SniperForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03"), 7 });

    //hunting rifle
    SniperForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_UC_Ore_T03.WID_Sniper_NoScope_Athena_UC_Ore_T03"), 7 });
    SniperForFloor.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_R_Ore_T03.WID_Sniper_NoScope_Athena_R_Ore_T03"), 3 });

    return SniperForFloor;
}

// for chests
inline TArray<LootItemInfo> AssaultRifleForChest() {
    TArray<LootItemInfo> AssaultRifleForChest;
    //Ar
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03"), 30 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03"), 25 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03"), 3 });

    //burst
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_UC_Ore_T03.WID_Assault_SemiAuto_Athena_UC_Ore_T03"), 30 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_R_Ore_T03.WID_Assault_SemiAuto_Athena_R_Ore_T03"), 25 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_VR_Ore_T03.WID_Assault_SemiAuto_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_SR_Ore_T03.WID_Assault_SemiAuto_Athena_SR_Ore_T03"), 3 });

    //tac ar
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_R_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_R_Ore_T03"), 30 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03"), 3 });

    return AssaultRifleForChest;
}

inline TArray<LootItemInfo> ShotgunForChest() {
    TArray<LootItemInfo> ShotgunForChest;
    //Pump
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03"), 30 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03"), 25 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03"), 7 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03"), 3 });

    //Tac
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03"), 30 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03"), 25 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03"), 7 });
    ShotgunForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03"), 3 });

    return ShotgunForChest;
}

inline TArray<LootItemInfo> SMGForChest() {
    TArray<LootItemInfo> SMGForChest;
    //smg
    SMGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03"), 30 });
    SMGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03"), 25 });
    SMGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03"), 7 });
    SMGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03"), 3 });

    return SMGForChest;
}

inline TArray<LootItemInfo> PistolForChest() {
    TArray<LootItemInfo> PistolForChest;
    //Pistol
    PistolForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03"), 30 });
    PistolForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03"), 25 });
    PistolForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_VR.WID_Pistol_Standard_Athena_VR"), 7 });
    PistolForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_SR.WID_Pistol_Standard_Athena_SR"), 3 });

    return PistolForChest;
}

inline TArray<LootItemInfo> SniperForChest() {
    TArray<LootItemInfo> SniperForChest;
    //Sniper
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_UC_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_UC_Ore_T03"), 30 });
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03"), 25 });
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03"), 7 });
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03"), 3 });

    //hunting rifle
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_UC_Ore_T03.WID_Sniper_NoScope_Athena_UC_Ore_T03"), 30 });
    SniperForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_R_Ore_T03.WID_Sniper_NoScope_Athena_R_Ore_T03"), 25 });

    return SniperForChest;
}

inline TArray<LootItemInfo> RPGForChest() {
    TArray<LootItemInfo> RPGForChest;
    //RPG
    RPGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_UC_Ore_T03.WID_Launcher_Rocket_Athena_UC_Ore_T03"), 30 });
    RPGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_R_Ore_T03.WID_Launcher_Rocket_Athena_R_Ore_T03"), 27 });
    RPGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Launcher_Rocket_Athena_VR_Ore_T03"), 7 });
    RPGForChest.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03"), 3 });

    return RPGForChest;
}

//for supplydrops
inline TArray<LootItemInfo> AssaultRifleForSupply() {
    TArray<LootItemInfo> AssaultRifleForSupply;
    //Ar
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03"), 5 });

    //burst
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_VR_Ore_T03.WID_Assault_SemiAuto_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_SR_Ore_T03.WID_Assault_SemiAuto_Athena_SR_Ore_T03"), 5 });

    //tac ar
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03"), 7 });
    AssaultRifleForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03"), 5 });
    return AssaultRifleForSupply;
}

inline TArray<LootItemInfo> ShotgunForSupply() {
    TArray<LootItemInfo> ShotgunForSupply;
    //Pump
    ShotgunForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03"), 7 });
    ShotgunForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03"), 5 });

    //Tac
    ShotgunForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03"), 7 });
    ShotgunForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03"), 5 });

    return ShotgunForSupply;
}

inline TArray<LootItemInfo> SMGForSupply() {
    TArray<LootItemInfo> SMGForSupply;
    //smg
    SMGForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03"), 7 });
    SMGForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03"), 5 });

    return SMGForSupply;
}

inline TArray<LootItemInfo> PistolForSupply() {
    TArray<LootItemInfo> PistolForSupply;
    //Pistol
    PistolForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_VR.WID_Pistol_Standard_Athena_VR"), 7 });
    PistolForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_SR.WID_Pistol_Standard_Athena_SR"), 5 });

    return PistolForSupply;
}

inline TArray<LootItemInfo> SniperForSupply() {
    TArray<LootItemInfo> SniperForSupply;
    //Sniper
    SniperForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03"), 7 });
    SniperForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03"), 5 });

    return SniperForSupply;
}

inline TArray<LootItemInfo> RPGForSupply() {
    TArray<LootItemInfo> RPGForSupply;
    //RPG
    RPGForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Launcher_Rocket_Athena_VR_Ore_T03"), 7 });
    RPGForSupply.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03"), 5 });

    return RPGForSupply;
}

//for barrels duh!!!
inline TArray<LootItemInfo> Barrel() {
    TArray<LootItemInfo> Barrel;
    Barrel.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/FloppingRabbit/WID_Athena_FloppingRabbit.WID_Athena_FloppingRabbit"), 75 });
    Barrel.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/HappyGhost/WID_Athena_HappyGhost.WID_Athena_HappyGhost"), 25 });
    return Barrel;
}

//for fish fridge duh!!!
inline TArray<LootItemInfo> IceBox() {
    TArray<LootItemInfo> IceBox;
    IceBox.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Flopper/Small/WID_Athena_FlopperSmall.WID_Athena_FlopperSmall"), 30 });
    IceBox.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Flopper/Effective/WID_Athena_Flopper_Effective.WID_Athena_Flopper_Effective"), 30 });
    IceBox.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Flopper/WID_Athena_Flopper.WID_Athena_Flopper"), 30 });
    IceBox.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Seasonal/WID_Athena_Bucket_Coal.WID_Athena_Bucket_Coal"), 30 });
    IceBox.Add({ Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/SneakySnowmanV2/AGID_SneakySnowmanV2.AGID_SneakySnowmanV2"), 30 });
    return IceBox;
}

//amo/consumable/mats are all equal in spawning rate
inline TArray<ConsumableInfo> Consumable() {

    TArray<ConsumableInfo> Consumable;

    ConsumableInfo SmallShieldInfo;
    SmallShieldInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall");
    SmallShieldInfo.Quantity = 3;
    Consumable.Add(SmallShieldInfo);

    ConsumableInfo MedkitInfo;
    MedkitInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Medkit/Athena_Medkit.Athena_Medkit");
    MedkitInfo.Quantity = 1;
    Consumable.Add(MedkitInfo);

    ConsumableInfo BandageInfo;
    BandageInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Bandage/Athena_Bandage.Athena_Bandage");
    BandageInfo.Quantity = 5;
    Consumable.Add(BandageInfo);

    ConsumableInfo ShieldsInfo;
    ShieldsInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields");
    ShieldsInfo.Quantity = 1;
    Consumable.Add(ShieldsInfo);

    ConsumableInfo GernadeInfo;
    GernadeInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Grenade/Athena_Grenade.Athena_Grenade");
    GernadeInfo.Quantity = 3;
    Consumable.Add(GernadeInfo);

    //ConsumableInfo TrapInfo;
    //TrapInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Traps/Grenade/TID_ContextTrap_Athena.TID_ContextTrap_Athena");  //crashes the game wtf?
   // TrapInfo.Quantity = 1;
   // Consumable.Add(TrapInfo);

    ConsumableInfo RodInfo;
    RodInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/FloppingRabbit/WID_Athena_FloppingRabbit.WID_Athena_FloppingRabbit");
    RodInfo.Quantity = 1;
    Consumable.Add(RodInfo);

    return Consumable;
}

inline TArray<AmoInfo> Amo() {

    TArray<AmoInfo> Amo;

    AmoInfo SniperAmo;
    SniperAmo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
    SniperAmo.Quantity = 6;
    Amo.Add(SniperAmo);

    AmoInfo SMGAmo;
    SMGAmo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
    SMGAmo.Quantity = 18;
    Amo.Add(SMGAmo);

    AmoInfo AssaultAmo;
    AssaultAmo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    AssaultAmo.Quantity = 10;
    Amo.Add(AssaultAmo);

    AmoInfo ShotgunInfo;
    ShotgunInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    ShotgunInfo.Quantity = 4;
    Amo.Add(ShotgunInfo);

    AmoInfo RocketInfo;
    RocketInfo.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
    RocketInfo.Quantity = 2;
    Amo.Add(RocketInfo);

    return Amo;
}

inline TArray<MatsInfo> Mats() {

    TArray<MatsInfo> Mats;

    MatsInfo Wood;
    Wood.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
    Wood.Quantity = 30;
    Mats.Add(Wood);

    MatsInfo Brick;
    Brick.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
    Brick.Quantity = 30;
    Mats.Add(Brick);

    MatsInfo Metal;
    Metal.Definition = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
    Metal.Quantity = 30;
    Mats.Add(Metal);

    return Mats;
}