// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "Gui.h"
#include <thread>
#include "Player.h"
void AndreuHook(uintptr_t Address, void* Detour, void** OG = nullptr)
{
    MH_CreateHook((LPVOID)Address, Detour, OG);
    MH_EnableHook((LPVOID)Address);
}
static int ReturnFalse()
{
    return false;
}
BYTE* __fastcall GameSessionIdThing(__int64, __int64)
{
    return nullptr;
}
char CanActivateAbilityHook()
{
    return 1;
}
DWORD WINAPI Main(LPVOID) {
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    MH_Initialize();
    Sleep(2000);

    new std::thread(InitImGui);

    *(bool*)(InSDKUtils::GetImageBase() + 0x939a30d) = false;
    *(bool*)(InSDKUtils::GetImageBase() + 0x939a30e) = true;

    Utils::Hook(ImageBase + 0x9f83b0, CanActivateAbilityHook);
    //Utils::Hook(ImageBase + 0x31c3b00, SpawnLoot);
    //Utils::Hook(ImageBase + 0x31bdab0, SpawnLoot);
    //Utils::Hook(ImageBase + 0x31c3b00, ABuildingSMActor_PostUpdate, (LPVOID*)&ABuildingSMActor_PostUpdateOG);
    //Utils::Hook(ImageBase + 0x31bdab0, ABuildingSMActor_PostUpdate, (LPVOID*)&ABuildingSMActor_PostUpdateOG);
    Utils::Hook(ImageBase + 0x26acb20, ReturnFalse);
    Utils::Hook(ImageBase + 0x36e1aed, CollectGarbageHook);
    Utils::Hook(ImageBase + 0x2653520, GameSessionIdThing);
    Utils::Hook(ImageBase + Addresses::GetNetMode, ReturnTrue);
    Utils::Hook(ImageBase + Addresses::ReadyToStartMatch, ReadyToStartMatchHook);
    Utils::Hook(ImageBase + Addresses::SpawnDefaultPawnFor, Player::SpawnDefaultPawnForHook);
    Utils::Hook(ImageBase + Addresses::KickPlayer, ReturnTrue);
    Utils::Hook(ImageBase + Addresses::TickFlush, TickFlushHook, (void**)&TickFlushOriginal);
    /*Utils::Hook(ImageBase + 0x30c6ac0, ServerAttemptInteract, (void**)&ServerAttemptInteractOG);*/
    Utils::Hook(ImageBase + 0x9f83b0, ReturnTrue);
    Utils::Hook(ImageBase + 0x32c36e0, ClientOnPawnDiedHook, (void**)&ClientOnPawnDiedOG);
    Utils::Hook(ImageBase + 0x144cb00, Player::McpDispatchRequestHook, (void**)&Player::McpDispatchRequestOG);
    Utils::Hook(ImageBase + 0x2f3f580, Player::OnDamageServer, (void**)&Player::OnDamageServerOG);
    AndreuHook(ImageBase + 0x2080090, StartNewStormPhaseHook, (void**)&StartNewStormPhaseOriginal);
    AndreuHook(ImageBase + 0x32d3e20, Player::OnCapsuleBeginOverlapHook, (void**)&Player::OnCapsuleBeginOverlapOG);
  //  Utils::Hook_Internal(ImageBase + 0x2060370, OnAircraftEnteredDropZone);
    //Utils::Hook_Internal(ImageBase + 0x30c6ac0, ServerAttemptInteract, (void**)&ServerAttemptInteractOG);
    AndreuHook(ImageBase + 0x30c6ac0, ServerAttemptInteract, (void**)&ServerAttemptInteractOG);
    Utils::Hook(ImageBase + 0x2066c40, stormShit::PickTeam);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x220, Player::ServerAttemptInventoryDropHook);
    //Utils::Hook(ImageBase + 0x3269e40, Player::NetMulticast_Athena_BatchedDamageCues/*, (void**)&Player::NetMulticast_Athena_BatchedDamageCuesOG*/);
    Utils::SwapVFTs2(APlayerPawn_Athena_C::StaticClass()->DefaultObject, 0x1E4, Player::ServerReviveFromDBNO, nullptr);
    Utils::HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x1CC, Player::ServerPlayEmoteItem, nullptr);


    AndreuHook(Utils::GetOffsetA(0x2b69620), GetPlayerViewPointHook, (void**)&GetPlayerViewPointOG);

    AndreuHook(Utils::GetOffsetA(0x32a0480), Player::ServerLoadingScreenDropped, (void**)&Player::ServerLoadingScreenDroppedOG);

    Utils::VirtualHook(Utils::GetDefObject<APlayerPawn_Athena_C>(), 0x1FC, Player::ServerHandlePickupHook);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x4F1, Player::ServerClientIsReadyToRespawn, (void**)&Player::ServerClientIsReadyToRespawnOG);

 //   Utils::HookVTable(AFortPlayerPawnAthena::GetDefaultObj(), 0x119, Player::NetMulticast_Athena_BatchedDamageCues, (LPVOID*)&Player::NetMulticast_Athena_BatchedDamageCuesOG);

    for (int i = 0; i < UObject::GObjects->Num(); i++)
    {
        auto Object = UObject::GObjects->GetByIndex(i);

        if (!Object)
            continue;

        if (Object->IsA(UAbilitySystemComponent::StaticClass()))
        {
            Utils::HookVFT(Object->VTable, 0xfd, Abilities::InternalServerTryActivateAbilityHook, nullptr);
        }
    }

    Utils::HookVFT(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x110, Player::ServerAcknowledgePossession, (LPVOID*)&Player::ServerAcknowledgePossessionOG);
    Utils::VirtualHook(Utils::GetDefObject<UFortControllerComponent_Aircraft>(), 0x8E, ServerAttemptAircraftJumpHook);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x210, ServerExecuteInventoryItemHook);
    Utils::VirtualHook(Utils::GetDefObject<APlayerPawn_Athena_C>(), 0x209, Player::ServerSendZiplineStateHook);

    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x233, Player::ServerCreateBuildingActor);


    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x23A, Player::ServerBeginEditingBuildingActor);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x235, Player::ServerEditBuildingActor);
   // AndreuHook(ImageBase + 0x329f7c0, Player::ServerEditBuildingActor);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x238, Player::ServerEndEditingBuildingActorHook);
    Utils::VirtualHook(Utils::GetDefObject<AAthena_PlayerController_C>(), 0x4F9, Player::ServerTeleportToPlaygroundLobbyIslandHook);
    Utils::SwapVFTs2(AAthena_PlayerController_C::StaticClass()->DefaultObject, 0x1CA, Player::ServerCheat, nullptr);


    UKismetSystemLibrary::GetDefaultObj()->ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    }
    return TRUE;
}

