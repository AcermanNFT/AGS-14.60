#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "MinHook.h"
#include "Addresses.h"
#include "Funcs.h"
#include "Utils.h"
#include "Inventory.h"
#include "Abilities.h"
#include "Player.h"
#include <thread>
#include <fstream>
#include "Looting.h"
#include "XP.h"
#include "Vehicles.h"
static std::ofstream AAAA("FortniteLogs.log");
#define AndreuLog(...) { std::cout << "GAMESERBIR : " << std::format(__VA_ARGS__) << std::endl; AAAA << std::format(__VA_ARGS__) << std::endl; }
static int ReturnTrue()
{
	return true;
}

static void ReturnHook()
{
	return;
}


template <typename T>
static T* StaticFindObject(std::string ObjectName)
{
	auto Name = std::wstring(ObjectName.begin(), ObjectName.end()).c_str();

	static UObject* (*StaticFindObjectOriginal)(UClass * Class, UObject * Package, const TCHAR * OrigInName, bool ExactClass) = decltype(StaticFindObjectOriginal)(__int64(GetModuleHandleW(0)) + 0x3774c20);

	return (T*)StaticFindObjectOriginal(T::StaticClass(), nullptr, Name, false);
}


void OnAircraftEnteredDropZone(AFortGameModeAthena* GameMode, AFortAthenaAircraft* Aircraft)
{
	if (bLategame)
	{
		auto GameState = GameMode->GameState;
		std::cout << __FUNCTION__ << std::endl;
		FVector AircraftLocation = Aircraft->K2_GetActorLocation();
		FRotator AircraftRotation = Aircraft->K2_GetActorRotation();
		float Pitch = AircraftRotation.Pitch * (3.14159265359f / 180.0f);
		float Yaw = AircraftRotation.Yaw * (3.14159265359f / 180.0f);

		FVector AircraftForward{};
		AircraftForward.X = cos(Yaw) * cos(Pitch);
		AircraftForward.Y = sin(Yaw) * cos(Pitch);
		AircraftForward.Z = sin(Pitch);

		float Length = sqrt(AircraftForward.X * AircraftForward.X +
			AircraftForward.Y * AircraftForward.Y +
			AircraftForward.Z * AircraftForward.Z);

		if (Length > 0.0f) {
			AircraftForward.X /= Length;
			AircraftForward.Y /= Length;
			AircraftForward.Z /= Length;
		}

		FVector SafeZoneCenter = GameMode->SafeZoneLocations[4];
		SafeZoneCenter.Z += 15000;

		FVector NewLocation = SafeZoneCenter;

		Aircraft->K2_SetActorLocation(NewLocation, false, nullptr, true);

		auto& FlightInfo = Aircraft->FlightInfo;

		FlightInfo.FlightStartLocation = FVector_NetQuantize100(NewLocation);
		FlightInfo.FlightSpeed = 2000;
		FlightInfo.TimeTillDropStart = 2.0f;
		FlightInfo.TimeTillFlightEnd = 5.0f;

		Aircraft->FlightInfo = FlightInfo;


		GameState->bGameModeWillSkipAircraft = true;


		GameState->bAircraftIsLocked = false;

		UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"skipaircraft", nullptr);
		GameState->SafeZonesStartTime = 1;
	}
}

// for lategame
inline int64(*StartNewStormPhaseOriginal)(AFortGameModeAthena* GameMode, int ZoneIndex);
int64 StartNewStormPhaseHook(AFortGameModeAthena* GameMode, int ZoneIndex)
{
	auto GameStateAsFort = (AFortGameStateAthena*)GameMode->GameState;
	if (!GameStateAsFort)
		return StartNewStormPhaseOriginal(GameMode, ZoneIndex);

	auto MapInfo = GameStateAsFort->MapInfo;

	if (!MapInfo)
		return StartNewStormPhaseOriginal(GameMode, ZoneIndex);

	static int LategameSafeZonePhase = 2;

	static bool bHasBeenSetup = false;
	auto& WaitTimes = MapInfo->SafeZoneDefinition.WaitTimes();
	auto& Durations = MapInfo->SafeZoneDefinition.Durations();

	if (!bHasBeenSetup)
	{
		bHasBeenSetup = true;

		static auto GameData = StaticFindObject<UCurveTable>(UKismetStringLibrary::Conv_NameToString(GameStateAsFort->CurrentPlaylistInfo.BasePlaylist->GameData.ObjectID.AssetPathName).ToString());

		if (!GameData)
			GameData = UObject::FindObject<UCurveTable>("AthenaGameData.AthenaGameData");

		static auto ShrinkTime_NAME = UKismetStringLibrary::Conv_StringToName(L"Default.SafeZone.ShrinkTime");
		static auto WaitTime_NAME = UKismetStringLibrary::Conv_StringToName(L"Default.SafeZone.WaitTime");

		for (size_t i = 0; i < WaitTimes.Num(); i++)
		{
			float Out;
			EEvaluateCurveTableResult Res;
			UDataTableFunctionLibrary::EvaluateCurveTableRow(GameData, WaitTime_NAME, i, &Res, &Out, FString());
			WaitTimes[i] = Out;
		}
		for (size_t i = 0; i < Durations.Num(); i++)
		{
			float Out;
			EEvaluateCurveTableResult Res;
			UDataTableFunctionLibrary::EvaluateCurveTableRow(GameData, ShrinkTime_NAME, i, &Res, &Out, FString());
			Durations[i] = Out;
		}
	}

	static auto Accolade = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle");
	for (auto PC : GameMode->AlivePlayers)
	{
		GiveAccolade(PC, Accolade, nullptr, EXPEventPriorityType::NearReticle);
		bool bruh;
		FGameplayTagContainer Empty{};
		FGameplayTagContainer Empty2{};
		SendStatEvent(PC->GetQuestManager(ESubGame::Athena), nullptr, Empty, Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::StormPhase);
	}

	if (bLategame == true)
	{
		GameMode->SafeZonePhase = LategameSafeZonePhase;
		GameStateAsFort->SafeZonePhase = LategameSafeZonePhase;
		StartNewStormPhaseOriginal(GameMode, ZoneIndex);
		LategameSafeZonePhase++;
	}
	else
	{
		//StartNewStormPhaseOriginal(GameMode, ZoneIndex);
		GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) + WaitTimes[GameMode->SafeZonePhase];
		GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime + Durations[GameMode->SafeZonePhase];
		StartNewStormPhaseOriginal(GameMode, ZoneIndex);
	}

	float WaitTime = 0;

	if (GameMode->SafeZonePhase >= 0 && GameMode->SafeZonePhase < WaitTimes.Num())
		WaitTime = WaitTimes[GameMode->SafeZonePhase];

	GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds() + WaitTime;

	float ZoneDuration = 0;
	if (GameMode->SafeZonePhase >= 0 && GameMode->SafeZonePhase < Durations.Num())
		ZoneDuration = Durations[GameMode->SafeZonePhase];

	GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime + ZoneDuration;

	static FVector ZoneLocation = GameMode->SafeZoneLocations[4];
	GameMode->SafeZoneIndicator->NextCenter.X = ZoneLocation.X;
	GameMode->SafeZoneIndicator->NextCenter.Y = ZoneLocation.Y;
	GameMode->SafeZoneIndicator->NextCenter.Z = ZoneLocation.Z;

	static FVector_NetQuantize100 ZoneLocationQuantize = FVector_NetQuantize100{ ZoneLocation.X, ZoneLocation.Y, ZoneLocation.Z };

	std::cout << "SafeZonePhase: " << GameMode->SafeZonePhase << std::endl;

	if (GameMode->SafeZonePhase == 2 || GameMode->SafeZonePhase == 3)
	{
		if (GameMode->SafeZoneIndicator)
		{
			GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds();
			GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds() + 0.3;
		}
	}

	if (GameMode->SafeZonePhase == 4)
	{
		GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
		GameMode->SafeZoneIndicator->NextRadius = 10000.f;
		GameMode->SafeZoneIndicator->LastRadius = 20000.f;
	}

	if (GameMode->SafeZonePhase == 5)
	{
		GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
		GameMode->SafeZoneIndicator->NextRadius = 5000.f;
		GameMode->SafeZoneIndicator->LastRadius = 10000.f;
	}

	if (GameMode->SafeZonePhase == 6)
	{
		GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
		GameMode->SafeZoneIndicator->NextRadius = 2500.f;
		GameMode->SafeZoneIndicator->LastRadius = 5000.f;
	}

	if (GameMode->SafeZonePhase == 7)
	{
		GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
		GameMode->SafeZoneIndicator->NextRadius = 1650.2f;
		GameMode->SafeZoneIndicator->LastRadius = 2500.f;
	}

	if (GameMode->SafeZonePhase == 8)
	{
		GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
		GameMode->SafeZoneIndicator->NextRadius = 1090.12f;
		GameMode->SafeZoneIndicator->LastRadius = 1650.2f;
	}
}
//fixed for full map
//inline int64(*StartNewStormPhaseOriginal)(AFortGameModeAthena* GameMode, int ZoneIndex);
//int64 StartNewStormPhaseHook(AFortGameModeAthena* GameMode, int ZoneIndex)
//{
//	auto GameStateAsFort = (AFortGameStateAthena*)GameMode->GameState;
//
//	if (!GameStateAsFort)
//		return StartNewStormPhaseOriginal(GameMode, ZoneIndex);
//
//	auto MapInfo = GameStateAsFort->MapInfo;
//	if (!MapInfo)
//		return StartNewStormPhaseOriginal(GameMode, ZoneIndex);
//
//	static int LategameSafeZonePhase = 2;
//	static bool bHasBeenSetup = false;
//
//	auto& WaitTimes = MapInfo->SafeZoneDefinition.WaitTimes();
//	auto& Durations = MapInfo->SafeZoneDefinition.Durations();
//
//	if (!bHasBeenSetup)
//	{
//		bHasBeenSetup = true;
//
//		static auto GameData = StaticFindObject<UCurveTable>(UKismetStringLibrary::Conv_NameToString(GameStateAsFort->CurrentPlaylistInfo.BasePlaylist->GameData.ObjectID.AssetPathName).ToString());
//
//		if (!GameData)
//			GameData = UObject::FindObject<UCurveTable>("AthenaGameData.AthenaGameData");
//
//		static auto ShrinkTime_NAME = UKismetStringLibrary::Conv_StringToName(L"Default.SafeZone.ShrinkTime");
//		static auto WaitTime_NAME = UKismetStringLibrary::Conv_StringToName(L"Default.SafeZone.WaitTime");
//
//		for (size_t i = 0; i < WaitTimes.Num(); i++)
//		{
//			float Out;
//			EEvaluateCurveTableResult Res;
//			UDataTableFunctionLibrary::EvaluateCurveTableRow(GameData, WaitTime_NAME, i, &Res, &Out, FString());
//			WaitTimes[i] = Out;
//		}
//		for (size_t i = 0; i < Durations.Num(); i++)
//		{
//			float Out;
//			EEvaluateCurveTableResult Res;
//			UDataTableFunctionLibrary::EvaluateCurveTableRow(GameData, ShrinkTime_NAME, i, &Res, &Out, FString());
//			Durations[i] = Out;
//		}
//	}
//
//	static auto Accolade = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle");
//	for (auto PC : GameMode->AlivePlayers)
//	{
//		GiveAccolade(PC, Accolade, nullptr, EXPEventPriorityType::NearReticle);
//		bool bruh;
//		FGameplayTagContainer Empty{};
//		FGameplayTagContainer Empty2{};
//		SendStatEvent(PC->GetQuestManager(ESubGame::Athena), nullptr, Empty, Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::StormPhase);
//	}
//
//	if (bLategame)
//	{
//		GameMode->SafeZonePhase = LategameSafeZonePhase;
//		GameStateAsFort->SafeZonePhase = LategameSafeZonePhase;
//		StartNewStormPhaseOriginal(GameMode, ZoneIndex);
//		LategameSafeZonePhase++;
//	}
//	else
//	{
//		StartNewStormPhaseOriginal(GameMode, ZoneIndex);
//	}
//
//	float WaitTime = 0;
//
//	if (GameMode->SafeZonePhase >= 0 && GameMode->SafeZonePhase < WaitTimes.Num())
//		WaitTime = WaitTimes[GameMode->SafeZonePhase];
//
//	GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds() + WaitTime;
//
//	float ZoneDuration = 0;
//	if (GameMode->SafeZonePhase >= 0 && GameMode->SafeZonePhase < Durations.Num())
//		ZoneDuration = Durations[GameMode->SafeZonePhase];
//
//	GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime + ZoneDuration;
//
//	static FVector ZoneLocation = GameMode->SafeZoneLocations[4];
//	static FVector_NetQuantize100 ZoneLocationQuantize = FVector_NetQuantize100{ ZoneLocation.X, ZoneLocation.Y, ZoneLocation.Z };
//
//	//GameMode->SafeZoneIndicator->NextCenter = ZoneLocation;
//	GameMode->SafeZoneIndicator->NextCenter = FVector_NetQuantize100(ZoneLocation);
//
//	std::cout << "SafeZonePhase: " << GameMode->SafeZonePhase << std::endl;
//
//	// Apply fixed zone behavior ONLY if NOT lategame
//	if (!bLategame)
//	{
//		switch (GameMode->SafeZonePhase)
//		{
//		case 2:
//		case 3:
//			GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds();
//			GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameStateAsFort->GetServerWorldTimeSeconds() + 0.3f;
//			break;
//
//		case 4:
//			GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
//			GameMode->SafeZoneIndicator->NextRadius = 10000.f;
//			GameMode->SafeZoneIndicator->LastRadius = 20000.f;
//			break;
//
//		case 5:
//			GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
//			GameMode->SafeZoneIndicator->NextRadius = 5000.f;
//			GameMode->SafeZoneIndicator->LastRadius = 10000.f;
//			break;
//
//		case 6:
//			GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
//			GameMode->SafeZoneIndicator->NextRadius = 2500.f;
//			GameMode->SafeZoneIndicator->LastRadius = 5000.f;
//			break;
//
//		case 7:
//			GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
//			GameMode->SafeZoneIndicator->NextRadius = 1650.2f;
//			GameMode->SafeZoneIndicator->LastRadius = 2500.f;
//			break;
//
//		case 8:
//			GameMode->SafeZoneIndicator->NextCenter = ZoneLocationQuantize;
//			GameMode->SafeZoneIndicator->NextRadius = 1090.12f;
//			GameMode->SafeZoneIndicator->LastRadius = 1650.2f;
//			break;
//
//		default:
//			break;
//		}
//	}
//}

bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
	//SetConsoleTitleA("14.60 || Setting up");
	auto GameState = (AFortGameStateAthena*)GameMode->GameState;
	auto World = UWorld::GetWorld();

	if (!GameState)
		return false;

	static bool bInitPlaylist = false;

	if (!bInitPlaylist)
	{
		UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_ShowdownAlt_Solo.Playlist_ShowdownAlt_Solo");///Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2
		if (!Playlist)
			return false;
		GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
		GameState->CurrentPlaylistInfo.MarkArrayDirty();
		GameState->OnRep_CurrentPlaylistInfo();

		GameState->CurrentPlaylistId = Playlist->PlaylistId;
		GameState->OnRep_CurrentPlaylistId();

		//GameState->CachedSafeZoneStartUp = Playlist->SafeZoneStartUp;

		GameState->AirCraftBehavior = Playlist->AirCraftBehavior;

		GameMode->CurrentPlaylistName = Playlist->PlaylistName;
		GameMode->CurrentPlaylistId = Playlist->PlaylistId;

		if (auto BotManager = (UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode))
		{
			GameMode->ServerBotManager = BotManager;
			BotManager->CachedGameState = GameState;
			BotManager->CachedGameMode = GameMode;
			printf("BotManager is Working!");
		}
		else
		{
			printf("BotManager is nullptr!");
		}
		/*static void (*CreateAIDirector)(AFortGameModeAthena * GameMode) = decltype(CreateAIDirector)(ImageBase + 0x31b4200);
		CreateAIDirector(GameMode);

		if (GameMode->AIDirector)
		{
			GameMode->AIDirector->Activate();
		}*/

		if (Playlist->AISettings)
		{
			if (Playlist->AISettings->bAllowAIGoalManager)
				GameMode->AIGoalManager = AndreuGayMoment::SpawnActor222<AFortAIGoalManager>({ 0, 0, -99999 }, {});
		}

		if (Playlist->bIsTournament) {
			GameState->EventTournamentRound = EEventTournamentRound::Open;
			GameState->OnRep_EventTournamentRound();
		}

		GameMode->WarmupRequiredPlayerCount = 1;
		GameMode->DefaultWarmupEarlyRequiredPlayerPercent = 100.f;

		GameState->CachedSafeZoneStartUp = ESafeZoneStartUp::StartsWithAirCraft;
		bInitPlaylist = true;
	}

	if (!GameState->MapInfo)
		return false;

	static bool bListening = false;

	if (!bListening)
	{
		static UNetDriver* (*CreateNetDriver)(UEngine * Engine, UWorld * World, FName Name) = decltype(CreateNetDriver)(InSDKUtils::GetImageBase() + 0x50d2bf0);
		static bool (*InitListen)(UNetDriver * NetDriver, void* InNotify, FURL & ListenURL, bool bReuseAddressAndPort, FString & Error) = decltype(InitListen)(InSDKUtils::GetImageBase() + 0xc61890);
		static void (*SetWorld)(UNetDriver * NetDriver, UWorld * World) = decltype(SetWorld)(InSDKUtils::GetImageBase() + 0x4e3e980);

		World->NetDriver = CreateNetDriver(UFortEngine::GetEngine(), World, UKismetStringLibrary::Conv_StringToName(L"GameNetDriver"));
		World->NetDriver->NetDriverName = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
		World->NetDriver->World = World;

		FString Error;
		FURL ListenURL{};
		ListenURL.Port = 7777;

		if (!InitListen(World->NetDriver, World, ListenURL, false, Error))
		{
			std::cout << "InitListen has failed.\n";
		}

		SetWorld(World->NetDriver, World);

		GameState->OnRep_CurrentPlaylistInfo();

		for (auto& LevelCollection : World->LevelCollections)
		{
			LevelCollection.NetDriver = World->NetDriver;
		}

		SetConsoleTitleA("AGS 14.60 | Listening");
		GameState->PlayersLeft--;
		GameState->OnRep_PlayersLeft();
		GameState->DefaultRebootMachineHotfix = 1;


		GameMode->bWorldIsReady = true;
		bListening = true;
	}

	//SpawnVehicles();

	return GameMode->AlivePlayers.Num() > 0;
}



inline void (*TickFlushOriginal)(UNetDriver* NetDriver);
void TickFlushHook(UNetDriver* NetDriver)
{
	if (!NetDriver || !NetDriver->ReplicationDriver)
		return TickFlushOriginal(NetDriver);

	static void (*ServerReplicateActors)(UReplicationDriver * ReplicationDriver) = decltype(ServerReplicateActors)(InSDKUtils::GetImageBase() + 0x16f2410);

	if (auto ReplicationDriver = NetDriver->ReplicationDriver)
	{
		ServerReplicateActors(ReplicationDriver);
	}

	return TickFlushOriginal(NetDriver);
}

void Siphon(AFortPlayerControllerAthena* PC)
{
	if (!PC || !PC->MyFortPawn || !PC->WorldInventory || !bEnableSiphon)
		return;
	static auto Wood = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
	static auto Stone = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
	static auto Metal = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

	Inventory::GiveItemProper(PC, Wood, 50);
	Inventory::GiveItemProper(PC, Stone, 50);
	Inventory::GiveItemProper(PC, Metal, 50);
}
void OnKilled(AFortPlayerControllerAthena* DeadPC, AFortPlayerControllerAthena* KillerPC, UFortWeaponItemDefinition* KillerWeapon)
{
	Siphon(KillerPC);
	bool bruh;
	FGameplayTagContainer Empty{};
	FGameplayTagContainer Empty2{};
	/*XP::Challanges::*/SendStatEvent(KillerPC->GetQuestManager(ESubGame::Athena), DeadPC, KillerWeapon ? KillerWeapon->GameplayTags : (KillerPC->MyFortPawn->CurrentWeapon ? KillerPC->MyFortPawn->CurrentWeapon->WeaponData->GameplayTags : Empty), Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::Kill);
	Empty.GameplayTags.Free();
	Empty.ParentTags.Free();
	Empty2.ParentTags.Free();
	Empty2.GameplayTags.Free();
	auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	for (auto PC : GameMode->AlivePlayers)
	{
		/*XP::Challanges::*/SendStatEvent(PC->GetQuestManager(ESubGame::Athena), nullptr, Empty, Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::AthenaOutlive);
	}

	auto KillerState = (AFortPlayerStateAthena*)KillerPC->PlayerState;
	if (!KillerState)
		return;
	auto DeadState = (AFortPlayerStateAthena*)DeadPC->PlayerState;
	if (!DeadState)
		return;

	static FGameplayTag EarnedElim = { UKismetStringLibrary::Conv_StringToName(TEXT("Event.EarnedElimination")) };
	FGameplayEventData Data{};
	Data.EventTag = EarnedElim;
	Data.ContextHandle = KillerState->AbilitySystemComponent->MakeEffectContext();
	Data.Instigator = KillerPC;
	Data.Target = DeadState;
	Data.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(DeadState);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(KillerPC->Pawn, Data.EventTag, Data);
}

static void (*RemoveFromAlivePlayerOG)(void*, void*, void*, void*, void*, EDeathCause, char) = decltype(RemoveFromAlivePlayerOG)(Utils::GetOffsetA(0x206ecf0));
void (*ClientOnPawnDiedOG)(AFortPlayerControllerZone* a1, FFortPlayerDeathReport a2);
void ClientOnPawnDiedHook(AFortPlayerControllerZone* DeadPlayer, FFortPlayerDeathReport& DeathReport, AFortPlayerPawnAthena* Pawn)
{
	auto DeadPawn = (AFortPlayerPawnAthena*)DeadPlayer->Pawn;
	auto DeadPlayerState = (AFortPlayerStateAthena*)DeadPlayer->PlayerState;
	auto KillerPlayerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	auto KillerPawn = (AFortPlayerPawnAthena*)DeathReport.KillerPawn;

	if (!DeadPawn || !DeadPlayerState)
		return ClientOnPawnDiedOG(DeadPlayer, DeathReport);

	EDeathCause DeathCause = DeadPlayerState->ToDeathCause(DeathReport.Tags, DeadPawn->bIsDBNO);
	FDeathInfo& DeathInfo = DeadPlayerState->DeathInfo;
	DeathInfo.bInitialized = true;
	DeathInfo.bDBNO = DeadPawn->bIsDBNO;
	DeathInfo.DeathCause = DeathCause;
	DeathInfo.FinisherOrDowner = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
	DeathInfo.Distance = DeathCause == EDeathCause::FallDamage ? DeadPawn->LastFallDistance : DeadPawn->GetDistanceTo(KillerPawn);
	DeathInfo.DeathLocation = DeadPawn ? DeadPawn->K2_GetActorLocation() : FVector{};

	DeadPlayerState->PawnDeathLocation = DeathInfo.DeathLocation;
	DeadPlayerState->OnRep_DeathInfo();

	if (KillerPlayerState && KillerPlayerState != DeadPlayerState)
	{
		KillerPlayerState->KillScore++;
		KillerPlayerState->TeamKillScore++;
		KillerPlayerState->ClientReportKill(DeadPlayerState);
		KillerPlayerState->OnRep_Kills();
		KillerPlayerState->OnRep_TeamScore();

		KillerPlayerState->Score++;
		KillerPlayerState->TeamScore++;
		KillerPlayerState->OnRep_Score();

		if (bEnableSiphon)
		{
			if (KillerPawn)
			{
				auto PC = (AFortPlayerControllerAthena*)Pawn->Controller;

				float Health = KillerPawn->GetHealth();
				float NewHealthAmount = Health + 100;

				KillerPawn->SetHealth(NewHealthAmount);

				if (NewHealthAmount > 100)
				{
					float ShieldToGive = (NewHealthAmount - 100) + KillerPawn->GetShield();

					KillerPawn->SetHealth(100);
					KillerPawn->SetShield(ShieldToGive);

					if (KillerPawn->GetShield() > 100) { KillerPawn->SetShield(100); }

					/*ApplySiphonEffectToEveryone();*/
					static auto Wood = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
					static auto Stone = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
					static auto Metal = Utils::StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

					Inventory::GiveItemProper(PC, Wood, 50);
					Inventory::GiveItemProper(PC, Stone, 50);
					Inventory::GiveItemProper(PC, Metal, 50);

					static auto KillAccolade = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_Elimination.AccoladeID_Elimination");
					if (KillAccolade)
					{
						GiveAccolade(PC, KillAccolade, nullptr, EXPEventPriorityType::NearReticle);

						bool bruh;
						FGameplayTagContainer Empty{};
						FGameplayTagContainer Empty2{};
						SendStatEvent(PC->GetQuestManager(ESubGame::Athena), nullptr, Empty, Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::DownOrElim);
					}

					ApplySiphonEffectToEveryone();
				}
			}


		}

		if (!Utils::GetGameState()->IsRespawningAllowed(DeadPlayerState))
		{
			if (!DeadPawn->IsDBNO())
			{
				if (DeadPlayer->WorldInventory)
				{
					for (int i = 0; i < DeadPlayer->WorldInventory->Inventory.ItemInstances.Num(); i++)
					{
						if (DeadPlayer->WorldInventory->Inventory.ItemInstances[i]->CanBeDropped())
						{
							auto UWU = DeadPlayer->WorldInventory->Inventory.ItemInstances[i]->ItemEntry;
							Utils::SpawnPickup(UWU.ItemDefinition, UWU.Count, UWU.LoadedAmmo, DeadPawn->K2_GetActorLocation(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination);
						}
					}
				}

				UFortItemDefinition* WeaponDef = nullptr;
				auto DamageCauser = DeathReport.DamageCauser;
				if (DamageCauser)
				{
					if (auto WEAPON = Utils::Cast<AFortWeapon>(DamageCauser))
					{
						WeaponDef = WEAPON->WeaponData;
					}
				}

				RemoveFromAlivePlayerOG(Utils::GetGameMode(), DeadPlayer, KillerPlayerState == DeadPlayerState ? nullptr : KillerPlayerState, KillerPawn, WeaponDef, DeathInfo.DeathCause, 0);
			}
		}

		return ClientOnPawnDiedOG(DeadPlayer, DeathReport);
	}
}

void (*GetPlayerViewPointOG)(AFortPlayerController* a1, FVector a2, FRotator a3);
void GetPlayerViewPointHook(AFortPlayerController* a1, FVector& a2, FRotator& a3)
{
	if (auto Pawn = a1->Pawn)
	{
		a2 = Pawn->K2_GetActorLocation();
		a3 = a1->GetControlRotation();
		return;
	}

	return GetPlayerViewPointOG(a1, a2, a3);
}

void ServerAttemptAircraftJumpHook(UFortControllerComponent_Aircraft* Component)
{
	if (auto PlayerController = Utils::Cast<AFortPlayerControllerAthena>(Component->GetOwner()))
	{
		Utils::GetGameMode()->RestartPlayer(PlayerController);

		if (PlayerController->MyFortPawn)
		{
			PlayerController->MyFortPawn->BeginSkydiving(true);
			if (bLategame)
			{
				PlayerController->MyFortPawn->SetHealth(100);
				PlayerController->MyFortPawn->SetShield(100);
			}
		}
	}
}


void ServerExecuteInventoryItemHook(AFortPlayerController* PC, FGuid ItemGuid)
{
	if (auto Pawn = (AFortPlayerPawn*)PC->Pawn)
	{
		if (auto ItemEntry = Inventory::FindItemEntry(PC, ItemGuid))
		{
			UFortWeaponItemDefinition* WeaponDefToEquip = (UFortWeaponItemDefinition*)ItemEntry->ItemDefinition;
			if (auto TrapDef = Utils::Cast<UFortDecoItemDefinition>(ItemEntry->ItemDefinition))
			{
				Pawn->PickUpActor(nullptr, TrapDef);
				Pawn->CurrentWeapon->ItemEntryGuid = ItemGuid;

				if (auto ContextTrap = Utils::Cast<AFortDecoTool_ContextTrap>(Pawn->CurrentWeapon))
				{
					//LOG_("CONTEXT!");
					ContextTrap->ContextTrapItemDefinition = (UFortContextTrapItemDefinition*)TrapDef;
				}

				return;
			}
			if (auto Gadget = Utils::Cast<UFortGadgetItemDefinition>(ItemEntry->ItemDefinition))
			{
				//LOG_("GADGET!");
				WeaponDefToEquip = Gadget->GetWeaponItemDefinition();
			}
			if (WeaponDefToEquip)
				Pawn->EquipWeaponDefinition(WeaponDefToEquip, ItemEntry->ItemGuid);
		}
	}
}


void CollectGarbageHook()
{
	return;
}

inline void (*ABuildingSMActor_PostUpdateOG)(ABuildingSMActor*);
inline void __fastcall ABuildingSMActor_PostUpdate(ABuildingSMActor* Actor)
{
	if (Actor->IsA(ABuildingContainer::StaticClass()) && ((ABuildingContainer*)Actor)->bStartAlreadySearched_Athena == 1)
	{
		SpawnFloorLoot((ABuildingContainer*)Actor);
	}

	return ABuildingSMActor_PostUpdateOG(Actor);
}