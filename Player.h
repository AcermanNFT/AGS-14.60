#pragma once
#include "Utils.h"
#include "Abilities.h"
#include "Inventory.h"
#include "Looting.h"
#include "XP.h"
template <typename T>
static T* StaticFindObject2(std::string ObjectName)
{
	auto Name = std::wstring(ObjectName.begin(), ObjectName.end()).c_str();

	static UObject* (*StaticFindObjectOriginal)(UClass * Class, UObject * Package, const TCHAR * OrigInName, bool ExactClass) = decltype(StaticFindObjectOriginal)(__int64(GetModuleHandleW(0)) + 0x3774c20);

	return (T*)StaticFindObjectOriginal(T::StaticClass(), nullptr, Name, false);
}

namespace Player
{

	inline void (*ServerMoveOG)(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState);
	inline void ServerMove(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState)
	{
		UPrimitiveComponent* Mesh = (UPrimitiveComponent*)Pawn->RootComponent;

		InState.Rotation.X -= 2.5;
		InState.Rotation.Y /= 0.3;
		InState.Rotation.Z -= -2.0;
		InState.Rotation.W /= -1.2;

		FTransform Transform{};
		Transform.Translation = InState.Translation;
		Transform.Rotation = InState.Rotation;
		Transform.Scale3D = FVector{ 1, 1, 1 };

		Mesh->K2_SetWorldTransform(Transform, false, nullptr, true);
		Mesh->bComponentToWorldUpdated = true;
		Mesh->SetPhysicsLinearVelocity(InState.LinearVelocity, 0, FName());
		Mesh->SetPhysicsAngularVelocity(InState.AngularVelocity, 0, FName());
	}

	APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerController* NewPlayer, AActor* StartSpot)
	{
		auto NewPawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());

		for (auto& StartingItem : GameMode->StartingItems)
		{
			Inventory::GiveItem(NewPlayer, StartingItem.Item, StartingItem.Count, 0, false);
		}
		Inventory::GiveItem(NewPlayer, NewPlayer->CosmeticLoadoutPC.Pickaxe->WeaponDefinition, 1, 0, false);

		return NewPawn;
	}

	void (*ServerAcknowledgePossessionOG)(AFortPlayerControllerAthena* PC, APawn* Pawn);
	inline void ServerAcknowledgePossession(AFortPlayerControllerAthena* PC, APawn* Pawn)
	{
		PC->AcknowledgedPawn = Pawn;
		return ServerAcknowledgePossessionOG(PC, Pawn);
	}

	inline void (*ServerLoadingScreenDroppedOG)(AFortPlayerControllerAthena* PC);
	inline void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC)
	{
		auto Pawn = (AFortPlayerPawn*)PC->Pawn;
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

		UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
		PlayerState->OnRep_CharacterData();

		PlayerState->TeamIndex;
		PlayerState->TeamKillScore;
		PlayerState->TeamScore;

		PlayerState->SquadId = PlayerState->TeamIndex - 2;

		PlayerState->OnRep_SquadId();
		PlayerState->OnRep_TeamIndex(0);
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();

		FGameMemberInfo Info{ -1,-1,-1 };
		Info.MemberUniqueId = PlayerState->UniqueId;
		Info.SquadId = PlayerState->SquadId;
		Info.TeamIndex = PlayerState->TeamIndex;

		GameState->GameMemberInfoArray.Members.Add(Info);
		GameState->GameMemberInfoArray.MarkItemDirty(Info);

		GameState->OnRep_PlayersLeft();

		//PC->XPComponent->bRegisteredWithQuestManager = true;
		//PC->XPComponent->OnRep_bRegisteredWithQuestManager();

		UFortAbilitySet* AbilitySet = Utils::StaticFindObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");

		for (auto& Ability : AbilitySet->GameplayAbilities)
		{
			Abilities::GiveAbility((AFortPlayerPawnAthena*)PC->MyFortPawn, (UGameplayAbility*)Ability.Get()->DefaultObject);
		}

		if (bLategame)
		{

			UFortItemDefinition* RifleDefinition = nullptr;
			UFortItemDefinition* ShotgunDefinition = nullptr;
			UFortItemDefinition* SilverCode = nullptr;
			UFortItemDefinition* Flopper = nullptr;

			RifleDefinition = StaticFindObject2<UFortItemDefinition>("/HighTower/Items/Tomato/Tomato_Rifle/WID_Assault_Stark_Athena_VR_Ore_T03.WID_Assault_Stark_Athena_VR_Ore_T03");
			ShotgunDefinition = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03");
			SilverCode = StaticFindObject2<UFortItemDefinition>("/HighTower/Items/Soy/CoreBR/WID_HighTower_Soy_Boarding_CoreBR.WID_HighTower_Soy_Boarding_CoreBR");
			Flopper = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Consumables/Flopper/Effective/WID_Athena_Flopper_Effective.WID_Athena_Flopper_Effective");

			UFortItemDefinition* Wood = StaticFindObject2<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
			UFortItemDefinition* Stone = StaticFindObject2<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
			UFortItemDefinition* Metal = StaticFindObject2<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
			UFortItemDefinition* Shells = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
			UFortItemDefinition* Medium = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
			UFortItemDefinition* Light = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
			UFortItemDefinition* Heavy = StaticFindObject2<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");


			auto PlayerController = PC;
			Inventory::GiveItem(PlayerController, RifleDefinition, 1, 30, false);
			Inventory::GiveItem(PlayerController, ShotgunDefinition, 1, 5, false);
			Inventory::GiveItem(PlayerController, Wood, 500, 0, true);
			Inventory::GiveItem(PlayerController, Stone, 500, 0, true);
			Inventory::GiveItem(PlayerController, Metal, 500, 0, true);
			Inventory::GiveItem(PlayerController, Shells, 50, 0, true);
			Inventory::GiveItem(PlayerController, Medium, 300, 0, true);
			Inventory::GiveItem(PlayerController, Light, 200, 0, true);
			Inventory::GiveItem(PlayerController, Heavy, 25, 0, true);
			Inventory::GiveItem(PlayerController, SilverCode, 1, 0, true);
			Inventory::GiveItem(PlayerController, Flopper, 3, 0, true);
		}

		if (/*!*/bEnableVehicles)
		{
			bEnableVehicles = true;
			TArray<AActor*> Spawners;
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortAthenaVehicleSpawner::StaticClass(), &Spawners);
			for (size_t i = 0; i < Spawners.Num(); i++)
			{
				AFortAthenaVehicleSpawner* Spawner = Utils::Cast<AFortAthenaVehicleSpawner>(Spawners[i]);
				if (!Spawner)
					continue;
				AActor* Vehicle = UGameplayStatics::FinishSpawningActor(UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Spawner->GetVehicleClass(), (FTransform&)Spawner->GetTransform(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr), (FTransform&)Spawner->GetTransform());
				UFortAthenaVehicleFuelComponent* TestComp = Utils::Cast<UFortAthenaVehicleFuelComponent>(Vehicle->GetComponentByClass(UFortAthenaVehicleFuelComponent::StaticClass()));
				if (TestComp)
				{
					TestComp->ServerFuel = 100;
					TestComp->OnRep_ServerFuel(0);
				}
				Utils::SwapVFTs2(Vehicle, 0xEE, Player::ServerMove, nullptr);// 0xEE o 0xEF si es 0xEE no testees el otro
			}
			Spawners.Free();
		}

		PlayerState->ForceNetUpdate();
		Pawn->ForceNetUpdate();
		PC->ForceNetUpdate();
		return ServerLoadingScreenDroppedOG(PC);
	}



	void (*McpDispatchRequestOG)(__int64, __int64*, int);
	void McpDispatchRequestHook(__int64 a1, __int64* a2, int a3)
	{
		return McpDispatchRequestOG(a1, a2, 3);
	}

	static void(*OnRep_ZiplineState)(AFortPlayerPawn*) = decltype(OnRep_ZiplineState)(Utils::GetOffsetA(0x2b1b650));
	void ServerSendZiplineStateHook(AFortPlayerPawn* Pawn, FZiplinePawnState& InZiplineState)
	{
		if (Pawn)
		{
			Pawn->ZiplineState = InZiplineState;

			if (InZiplineState.bJumped)
			{
				Pawn->LaunchCharacter({ 0,0,1000 }, false, true);
			}

			OnRep_ZiplineState(Pawn);
		}
	}
	inline __int64 (*OnDamageServerOG)(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext);
	__int64 OnDamageServer(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext)
	{
		if (!Actor || !Actor->IsA(ABuildingSMActor::StaticClass()) || !InstigatedBy || !InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) || !DamageCauser->IsA(AFortWeapon::StaticClass()) || !((AFortWeapon*)DamageCauser)->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) || Actor->bPlayerPlaced)
			return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

		auto Def = UFortKismetLibrary::K2_GetResourceItemDefinition(Actor->ResourceType);

		if (Def)
		{
			auto& BuildingResourceAmountOverride = Actor->BuildingResourceAmountOverride;
			if (!BuildingResourceAmountOverride.RowName.ComparisonIndex)
				return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
			auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

			FString CurveTableAssetPath = UKismetStringLibrary::Conv_NameToString(GameState->CurrentPlaylistInfo.BasePlaylist->ResourceRates.ObjectID.AssetPathName);
			static auto CurveTable = Utils::StaticLoadObject<UCurveTable>(CurveTableAssetPath.ToString());
			CurveTableAssetPath.Free();
			if (!CurveTable)
				CurveTable = Utils::StaticLoadObject<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");

			float Average = 1;
			EEvaluateCurveTableResult OutCurveTable;
			UDataTableFunctionLibrary::EvaluateCurveTableRow(CurveTable, BuildingResourceAmountOverride.RowName, 0.f, &OutCurveTable, &Average, FString());
			float FinalResourceCount = round(Average / (Actor->GetMaxHealth() / Damage));

			if (FinalResourceCount > 0)
			{
				InstigatedBy->ClientReportDamagedResourceBuilding(Actor, Actor->ResourceType, FinalResourceCount, false, Damage == 100.f);
				Inventory::GiveItemProper(InstigatedBy, Def, FinalResourceCount, 0);
			}
		}

		return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
	}

	//static bool (*CantBuild)(UWorld*, UObject*, FVector, FRotator, char, void*, char*) = decltype(CantBuild)(Utils::GetOffsetA(0x26983d0));
	//void ServerCreateBuildingActor(AFortPlayerControllerAthena* PC, FCreateBuildingActorData CreateBuildingData)
	//{
	//	if (!PC || PC->IsInAircraft())
	//		return;

	//	UClass* BuildingClass = PC->BroadcastRemoteClientInfo->RemoteBuildableClass.Get();
	//	char a7;
	//	TArray<AActor*> BuildingsToRemove;
	//	if (!CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &BuildingsToRemove, &a7))
	//	{
	//		auto ResDef = UFortKismetLibrary::GetDefaultObj()->K2_GetResourceItemDefinition(((ABuildingSMActor*)BuildingClass->DefaultObject)->ResourceType);
	//		Inventory::RemoveItem2(PC, ResDef, 10);

	//		ABuildingSMActor* NewBuilding = AndreuGayMoment::SpawnActor222<ABuildingSMActor>(CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, PC, BuildingClass);

	//		NewBuilding->bPlayerPlaced = true;
	//		NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PC, true);
	//		NewBuilding->TeamIndex = ((AFortPlayerStateAthena*)PC->PlayerState)->TeamIndex;
	//		NewBuilding->Team = EFortTeam(NewBuilding->TeamIndex);

	//		for (size_t i = 0; i < BuildingsToRemove.Num(); i++)
	//		{
	//			BuildingsToRemove[i]->K2_DestroyActor();
	//		}
	//		BuildingsToRemove.Free();
	//		FGameplayTagContainer Empty{};
	//		bool bor;
	//		SendStatEvent(PC->GetQuestManager(ESubGame::Athena), NewBuilding, Empty, Empty, &bor, &bor, 1, EFortQuestObjectiveStatEvent::Build);
	//	}
	//}
	using CantBuildFn = __int64(*)(UObject*, UObject*, FVector, FRotator, char, TArray<ABuildingSMActor*>*, char*);
	CantBuildFn CantBuild = reinterpret_cast<CantBuildFn>(InSDKUtils::GetImageBase() + 0x26983D0);
	void ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FCreateBuildingActorData& CreateBuildingData) {
		if (!PlayerController || PlayerController->IsInAircraft())
			return;

		auto BuildingClass = PlayerController->BroadcastRemoteClientInfo->RemoteBuildableClass.Get();
		if (!BuildingClass) { std::cout << "BuildingClass is null" << std::endl; return; }

		TArray<ABuildingSMActor*> ExistingBuildings;
		char BuildRestrictionFlag;
		if (CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ExistingBuildings, &BuildRestrictionFlag)) { return; }

		auto NewBuilding = Utils::SpawnActorBuilding<ABuildingSMActor>(CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, BuildingClass);
		if (!NewBuilding) { std::cout << "Failed to spawn NewBuilding" << std::endl; return; }

		NewBuilding->bPlayerPlaced = true;
		auto PlayerState = Utils::Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);
		NewBuilding->TeamIndex = PlayerState->TeamIndex;
		NewBuilding->Team = static_cast<EFortTeam>(PlayerState->TeamIndex);
		NewBuilding->OnRep_Team();
		NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PlayerController, true);

		for (auto& Building : ExistingBuildings) {
			Building->K2_DestroyActor();
		}
		ExistingBuildings.Free();

		auto ItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(NewBuilding->ResourceType);
		auto ItemEntry = Inventory::FindItemEntry(PlayerController, ItemDefinition);
		if (ItemEntry && ItemEntry->ItemGuid.IsValid()) {
			Inventory::RemoveItem(PlayerController, ItemEntry->ItemGuid, 1);
		}
		else {
			std::cout << "[ServerCreateBuildingActor] No se encontr  la entrada de item para el recurso requerido." << std::endl;
			return;
		}

	}

	void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit)
	{
		if (!PlayerController || !BuildingActorToEdit || !PlayerController->MyFortPawn)
			return;

		auto PlayerState = Utils::Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			std::cout << "PlayerState is null" << std::endl;
			return;
		}

		auto EditToolItemDefinition = Utils::StaticFindObject<UFortItemDefinition>(
			"/Game/Items/Weapons/BuildingTools/EditTool.EditTool");

		if (!EditToolItemDefinition)
		{
			std::cout << "Failed to find EditToolItemDefinition" << std::endl;
			return;
		}

		auto CurrentWeapon = PlayerController->MyFortPawn->CurrentWeapon; // FUCK THIS SHIT FR WORKS AND FIXED ALL EDITING BUGS/DELAY!!!!!!!!!!!
		if (!CurrentWeapon || CurrentWeapon->WeaponData != EditToolItemDefinition)
		{
			if (auto ItemEntry = Inventory::FindItemEntry(PlayerController, EditToolItemDefinition))
			{
				PlayerController->ServerExecuteInventoryItem(ItemEntry->ItemGuid);
			}
			CurrentWeapon = PlayerController->MyFortPawn->CurrentWeapon;
		}

		auto EditTool = static_cast<AFortWeap_EditingTool*>(PlayerController->MyFortPawn->CurrentWeapon);
		if (!EditTool)
		{
			std::cout << "EditTool is null or cast failed." << std::endl;
			return;
		}

		EditTool->EditActor = BuildingActorToEdit;
		EditTool->OnRep_EditActor();

		BuildingActorToEdit->EditingPlayer = PlayerState;
		BuildingActorToEdit->OnRep_EditingPlayer();
	}




	//static ABuildingSMActor* (*ReplaceBuildingActorOG)(ABuildingSMActor*, char, UClass*, int, uint8, bool, AController*) = decltype(ReplaceBuildingActorOG)(Utils::GetOffsetA(0x234afe0));
	//void ServerEditBuildingActorHook(AFortPlayerController* PC, ABuildingSMActor* BuildingActorToEdit, UClass* NewBuildingClass, uint8 RotationIterations, bool bMirrored)
	//{
	//	if (PC && BuildingActorToEdit && NewBuildingClass)
	//	{
	//		if (auto NewBuilding = ReplaceBuildingActorOG(BuildingActorToEdit, 1, NewBuildingClass, BuildingActorToEdit->CurrentBuildingLevel, RotationIterations, bMirrored, PC))
	//		{
	//			NewBuilding->bPlayerPlaced = true;

	//			BuildingActorToEdit->EditingPlayer = nullptr;
	//			BuildingActorToEdit->OnRep_EditingPlayer();
	//		}
	//	}
	//}
	using ReplaceBuildingActorFn = ABuildingSMActor * (*)(ABuildingSMActor*, __int64, UClass*, int, int, uint8_t, AFortPlayerController*);
	ReplaceBuildingActorFn ReplaceBuildingActor = reinterpret_cast<ReplaceBuildingActorFn>(InSDKUtils::GetImageBase() + 0x234AFE0);
	void ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored) {
		if (!BuildingActorToEdit || !NewBuildingClass.Get() || BuildingActorToEdit->bDestroyed || BuildingActorToEdit->EditingPlayer != Utils::Cast<AFortPlayerStateAthena>(PlayerController->PlayerState))
			return;

		BuildingActorToEdit->EditingPlayer = nullptr;

		if (auto NewBuilding = ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass.Get(), BuildingActorToEdit->CurrentBuildingLevel, RotationIterations, bMirrored, PlayerController)) {
			NewBuilding->bPlayerPlaced = true;
			NewBuilding->SetTeam(Utils::Cast<AFortPlayerStateAthena>(PlayerController->PlayerState)->TeamIndex);
			NewBuilding->OnRep_Team();
		}
	}

	void ServerEndEditingBuildingActorHook(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToStopEditing) {
		if (!PlayerController || !BuildingActorToStopEditing || BuildingActorToStopEditing->EditingPlayer != Utils::Cast<AFortPlayerStateAthena>(PlayerController->PlayerState) || !PlayerController->MyFortPawn)
			return;

		BuildingActorToStopEditing->SetNetDormancy(ENetDormancy::DORM_DormantAll);
		BuildingActorToStopEditing->EditingPlayer = nullptr;

		auto EditTool = static_cast<AFortWeap_EditingTool*>(PlayerController->MyFortPawn->CurrentWeapon);
		if (!EditTool) { std::cout << "No EditTool" << std::endl; return; }

		EditTool->EditActor = nullptr;
		EditTool->OnRep_EditActor();
	}

	void ServerAttemptInventoryDropHook(AFortPlayerController* PC, FGuid ItemGuid, int Count, bool bTrash)
	{
		if (auto Pawn = PC->Pawn)
		{
			if (auto Entry = Inventory::FindItemEntry(PC, ItemGuid))
			{
				//LOG_("ServerAttemptInventoryDrop Hook");

				Inventory::RemoveItem(PC, Entry->ItemGuid, Count);
				Utils::SpawnPickup(Entry, Pawn->K2_GetActorLocation(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Count);
			}
		}
	}

	void ServerHandlePickupHook(AFortPlayerPawn* Pawn, AFortPickup* Pickup, FFortPickupRequestInfo Params)
	{
		if (auto PlayerController = Utils::Cast<AFortPlayerController>(Pawn->Controller))
		{
			auto& PickupEntry = Pickup->PrimaryPickupItemEntry;

			auto& LocData = Pickup->PickupLocationData;
			LocData.PickupTarget = Pawn;
			LocData.ItemOwner = Pawn;
			LocData.FlyTime = 0.40f;
			LocData.PickupGuid = PickupEntry.ItemGuid;
			Pickup->OnRep_PickupLocationData();

			Inventory::GiveItemProper(PlayerController, &PickupEntry);
		}
	}

	void ServerTeleportToPlaygroundLobbyIslandHook(AFortPlayerController* a1)
	{
		if (a1)
		{

			//LOG_("testAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA REARAERAERAREA");
			auto State = (AFortPlayerStateAthena*)a1->PlayerState;
			if (!State)
				return;
			auto Pawn = a1->GetPlayerPawn();
			static auto FortPlayerStartCreativeClass = FindObject<UClass>(L"/Script/FortniteGame.FortPlayerStartCreative");
			TArray<AActor*> AllCreativePlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), FortPlayerStartCreativeClass, &AllCreativePlayerStarts);

			if (Utils::GetGameState()->IsTeleportToCreativeHubAllowed(State))
			{
				//LOG_("ITS ALLOWED GOOOOOOOOOOOOOOOD SIR\n");
				auto PlayerStartTEST = Utils::GetGameMode()->FindPlayerStart(a1, L"");

				AActor* StartSpot = nullptr;
				if (PlayerStartTEST)
				{
					//LOG_("idk : {}", PlayerStartTEST->GetName());
					StartSpot = PlayerStartTEST;
				}
				else
				{
					TArray<AActor*> idkraraada;
					Utils::GetStatics()->GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartCreative::StaticClass(), &idkraraada);
					//LOG_("TEST: {}", idkraraada.Num());

					StartSpot = idkraraada[rand() % idkraraada.Num()];

					idkraraada.Free();
				}

				if (StartSpot)
				{
					//LOG_("TELEPORTING SIR");

					a1->Pawn->K2_TeleportTo(StartSpot->K2_GetActorLocation(), StartSpot->K2_GetActorRotation());
				}
				else
				{
					//LOG_("CREATIVE IS REAL");
				}
			}
		}
	}
	void (*ServerClientIsReadyToRespawnOG)(AFortPlayerControllerAthena* PC);
	void ServerClientIsReadyToRespawn(AFortPlayerControllerAthena* PC)
	{
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;

		FFortRespawnData& RespawnData = *(FFortRespawnData*)(__int64(PlayerState) + 0xD30);

		if (RespawnData.bRespawnDataAvailable && RespawnData.bServerIsReady)
		{
			RespawnData.bClientIsReady = true;

			FTransform Transform{};
			Transform.Scale3D = FVector{ 1,1,1 };
			Transform.Translation = RespawnData.RespawnLocation;

			AFortPlayerPawnAthena* NewPawn = (AFortPlayerPawnAthena*)Utils::GetGameMode()->SpawnDefaultPawnAtTransform(PC, Transform);
			PC->Possess(NewPawn);

			PC->MyFortPawn->SetMaxHealth(100);
			PC->MyFortPawn->SetMaxShield(100);
			PC->MyFortPawn->SetHealth(100);
			PC->MyFortPawn->SetShield(100);

			PC->MyFortPawn->bCanBeDamaged = true; // nah no need for this

			PC->RespawnPlayerAfterDeath(true);
		}

		return ServerClientIsReadyToRespawnOG(PC);
	}

	void ServerCheat(AFortPlayerControllerAthena* PC, FString msg)
	{
		auto PlayerState = Utils::Cast<AFortPlayerStateAthena>(PC->PlayerState);
		auto PlayerName = PlayerState->GetPlayerName().ToString();
		auto Pawn = (AFortPlayerPawnAthena*)PC->Pawn;
		if (true || PlayerName == "Andreu" || PlayerName == "andreu" /*|| PlayerName == "akos0511" || PlayerName == "jyzo" || PlayerName == "GD"*/)
		{
			std::string MsgStr = msg.ToString();
			if (MsgStr == "StartAircraft")
			{
				UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), TEXT("startaircraft"), nullptr);
			}
			if (MsgStr == "poi")
			{
				TArray<AFortPoiVolume*> Actor;
				UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPoiVolume::StaticClass(), (TArray<AActor*>*) & Actor);
				for (auto poi : Actor)
				{
					printf("size: %g, %g\n", poi->CurrentFortPoiVolumeSize, poi->K2_GetActorLocation().Magnitude());
				}
				Actor.Free();
			}
			if (MsgStr == "thanos")
			{
				static auto wid = Utils::StaticLoadObject<UAthenaGadgetItemDefinition>("/Game/Athena/Items/Gameplay/BackPacks/Ashton/Hippo/AGID_AshtonPack_Hippo.AGID_AshtonPack_Hippo");
				static auto wid2 = Utils::StaticLoadObject<UAthenaGadgetItemDefinition>("/Game/Athena/Items/Gameplay/BackPacks/Ashton/Turbo/AGID_AshtonPack_Turbo.AGID_AshtonPack_Turbo");
				static auto wid3 = Utils::StaticLoadObject<UAthenaGadgetItemDefinition>("/Game/Athena/Items/Gameplay/BackPacks/Ashton/Indigo/AGID_AshtonPack_Indigo.AGID_AshtonPack_Indigo");
				static auto wid4 = Utils::StaticLoadObject<UAthenaGadgetItemDefinition>("/Game/Athena/Items/Gameplay/BackPacks/Ashton/Chicago/AGID_AshtonPack_Chicago.AGID_AshtonPack_Chicago");
				Inventory::GiveItemProper(PC, wid4);
				Inventory::GiveItemProper(PC, wid3);
				Inventory::GiveItemProper(PC, wid2);
				Inventory::GiveItemProper(PC, wid);
			}
		}
	}


	//void (*NetMulticast_Athena_BatchedDamageCuesOG)(AFortPawn* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData);
	//void NetMulticast_Athena_BatchedDamageCues(AFortPawn* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData)
	//{
	//	if (!Pawn || !Pawn->Controller || Pawn->Controller->IsA(ABP_PhoebePlayerController_C::StaticClass()))
	//		return;

	//	if (Pawn->CurrentWeapon)
	//		Inventory::UpdateLoadedAmmo((AFortPlayerController*)Pawn->Controller, ((AFortPlayerPawn*)Pawn)->CurrentWeapon);

	//	return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
	//}

	void (*OnCapsuleBeginOverlapOG)(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult& SweepResult);

	void OnCapsuleBeginOverlapHook(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult& SweepResult)
	{
		if (Pawn && OtherActor && OtherActor->IsA(AFortPickup::StaticClass()))
		{
			AFortPickup* Pickup = (AFortPickup*)OtherActor;

			if (Pickup->PawnWhoDroppedPickup == Pawn)
				return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

			UFortItemDefinition* Def = (UFortItemDefinition*)Pickup->PrimaryPickupItemEntry.ItemDefinition;

			if (Def && (Def->IsA(UFortAmmoItemDefinition::StaticClass()) || Def->IsA(UFortResourceItemDefinition::StaticClass())))
			{
				Pawn->ServerHandlePickup(Pickup, 0.40f, FVector(), true);
			}
		}

		return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}


	void ServerReviveFromDBNO(AFortPlayerPawnAthena* Pawn, AFortPlayerControllerAthena* Instigator)
	{
		float ServerTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
		printf("ServerReviveFromDBNO called\n");
		if (!Pawn || !Instigator)
			return;

		AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
		if (!PC || !PC->PlayerState)
			return;
		auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		auto AbilitySystemComp = (UFortAbilitySystemComponentAthena*)PlayerState->AbilitySystemComponent;

		//Pawn->ReviveFromDBNOTime = 30;
		//Pawn->ServerWorldTimeRevivalTime = 30;
		//Pawn->DBNORevivalStacking = 0;

		FGameplayEventData Data{};
		Data.EventTag = Pawn->EventReviveTag;
		Data.ContextHandle = PlayerState->AbilitySystemComponent->MakeEffectContext();
		Data.Instigator = Instigator;
		Data.Target = Pawn;
		Data.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Pawn);
		Data.TargetTags = Pawn->GameplayTags;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, Pawn->EventReviveTag, Data);

		for (auto& Ability : AbilitySystemComp->ActivatableAbilities.Items)
		{
			if (Ability.Ability->Class == UGAB_AthenaDBNO_C::StaticClass())
			{
				printf("UGAB_AthenaDBNO_C\n");
				AbilitySystemComp->ServerCancelAbility(Ability.Handle, Ability.ActivationInfo);
				AbilitySystemComp->ServerEndAbility(Ability.Handle, Ability.ActivationInfo, Ability.ActivationInfo.PredictionKeyWhenActivated);
				AbilitySystemComp->ClientCancelAbility(Ability.Handle, Ability.ActivationInfo);
				AbilitySystemComp->ClientEndAbility(Ability.Handle, Ability.ActivationInfo);
				break;
			}
		}

		Pawn->bIsDBNO = false;
		Pawn->OnRep_IsDBNO();
		Pawn->SetHealth(30);
		PlayerState->DeathInfo = {};
		PlayerState->OnRep_DeathInfo();

		PC->ClientOnPawnRevived(Instigator);
	}

	inline FGameplayAbilitySpecHandle(*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbilityAndActivateOnce)(InSDKUtils::GetImageBase() + 0xa4bc50);
	static void (*FGameplayAbilitySpecCtor)(FGameplayAbilitySpec*, UGameplayAbility*, int, int, UObject*) = decltype(FGameplayAbilitySpecCtor)(__int64(GetModuleHandleW(0)) + 0xa27b60);
	void ServerPlayEmoteItem(AFortPlayerControllerAthena* PC, UFortMontageItemDefinitionBase* EmoteAsset, float EmoteRandomNumber)
	{
		if (!PC || !EmoteAsset || !PC->MyFortPawn)
			return;

		auto Pawn = PC->MyFortPawn;
		UClass* ToyAbility = Utils::StaticLoadObject<UClass>("/Game/Abilities/Toys/GAB_Toy_Generic.GAB_Toy_Generic_C"); // Nueva línea
		UObject* AbilityToUse = nullptr;
		UGameplayAbility* Ability = nullptr;
		if (EmoteAsset->IsA(UAthenaSprayItemDefinition::StaticClass()))
		{
			AbilityToUse = UGAB_Spray_Generic_C::StaticClass()->DefaultObject;
		}
		else if (EmoteAsset->IsA(UAthenaToyItemDefinition::StaticClass()))
		{
			//GRR
			Ability = (UGameplayAbility*)ToyAbility->DefaultObject;
		}
		else
		{
			AbilityToUse = UGAB_Emote_Generic_C::StaticClass()->DefaultObject;
		}

		if (!AbilityToUse) return;

		if (EmoteAsset->IsA(UAthenaDanceItemDefinition::StaticClass()))
		{
			auto EA = Utils::Cast<UAthenaDanceItemDefinition>(EmoteAsset);
		}

		FGameplayAbilitySpec Spec{};
		FGameplayAbilitySpecCtor(&Spec, (UGameplayAbility*)AbilityToUse, 1, -1, EmoteAsset);
		GiveAbilityAndActivateOnce(PC->MyFortPawn->AbilitySystemComponent, &Spec.Handle, Spec);
	}
}



namespace stormShit
{
	inline int Time = 0;
	inline int GetStormDuration()
	{
		int Ret = 30;


		{
			switch (Time)
			{
			case 0:
				Ret = 0.f;
				break;

			case 1:
				Ret = 60;
				break;

			case 2:
				Ret = 30;
				break;

			case 3:
				Ret = 15;
				break;

			default:
				break;
			}
		}

		return Ret;
	}
	//void(__fastcall* stormOG)(__int64 a1, int a2) = nullptr;
	//extern void(__fastcall* stormOG)(__int64 a1, int a2);

	//inline void __fastcall stormHook(__int64 a1, int a2)
	//{
	//	// Call the original function
	//	stormOG(a1, a2);

	//	// Assumes Time is a static or global variable
	//	static int Time = 0;

	//	// Only proceed if the game phase is beyond Aircraft
	//	if (Utils::GetGameState()->GamePhase > EAthenaGamePhase::Aircraft)
	//	{
	//		// Cast mutator
	//		auto* Mutator = (AFortAthenaMutator_DadBro*)Utils::GetGameState()->GetMutatorByClass(
	//			Utils::GetGameMode(), AFortAthenaMutator_DadBro::StaticClass());

	//		// Optional: set center locations if needed
	//		// GetGameState()->SafeZoneIndicator->NextCenter = (FVector_NetQuantize100)Mutator->DadBroSpawnLocation;
	//		// GetGameState()->SafeZoneIndicator->NextNextCenter = (FVector_NetQuantize100)Mutator->DadBroSpawnLocation;

	//		// Safely access SafeZoneIndicator
	//		auto* SafeZoneIndicator = Utils::GetGameState()->SafeZoneIndicator;
	//		if (SafeZoneIndicator)
	//		{
	//			switch (Time)
	//			{
	//			case 0:
	//				SafeZoneIndicator->NextRadius = 22000;
	//				break;
	//			case 1:
	//				SafeZoneIndicator->NextRadius = 15000;
	//				break;
	//			case 2:
	//				SafeZoneIndicator->NextRadius = 5000;
	//				break;
	//			case 3:
	//				SafeZoneIndicator->NextRadius = 3000;
	//				break;
	//			case 4:
	//				SafeZoneIndicator->NextRadius = 1500;
	//				break;
	//			case 5:
	//				SafeZoneIndicator->NextRadius = 700;
	//				break;
	//			case 6:
	//				SafeZoneIndicator->NextRadius = 350;
	//				break;
	//			case 7:
	//				SafeZoneIndicator->NextRadius = 150;
	//				break;
	//			default:
	//				SafeZoneIndicator->NextRadius = 90; // fallback
	//				break;
	//			}

	//			float currentTime = Utils::GetStatics()->GetTimeSeconds(UWorld::GetWorld());

	//			SafeZoneIndicator->SafeZoneStartShrinkTime = currentTime;
	//			SafeZoneIndicator->SafeZoneFinishShrinkTime = currentTime + GetStormDuration();

	//			Utils::GetGameState()->SafeZonePhase = 5;
	//			Utils::GetGameMode()->SafeZonePhase = 5;
	//			Utils::GetGameState()->OnRep_SafeZonePhase();

	//			Time++;
	//		}
	//	}
	//}


	EFortTeam PickTeam()
	{
		static int MaxTeamSize = 2;
		static int Team = 3;
		static int CurrentPlayers = 0;

		int Ret = Team;

		CurrentPlayers++;
		if (CurrentPlayers == MaxTeamSize)
		{
			CurrentPlayers = 0;
			Team++;
		}
		return EFortTeam(Ret);
	}


}

//inline void (*ServerAttemptInteractOG)(UFortControllerComponent_Interaction* Comp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalData, EInteractionBeingAttempted InteractionBeingAttempted);
//inline void ServerAttemptInteract(UFortControllerComponent_Interaction* Comp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalData, EInteractionBeingAttempted InteractionBeingAttempted)
//{
//	static std::map<AFortPlayerControllerAthena*, int> ChestsSearched{};
//	ServerAttemptInteractOG(Comp, ReceivingActor, InteractComponent, InteractType, OptionalData, InteractionBeingAttempted);
//
//	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Comp->GetOwner();
//
//	if (!ReceivingActor || !PC) return;
//
//	if (ReceivingActor->IsA(AFortAthenaSupplyDrop::StaticClass()))
//	{
//		if (ReceivingActor->GetName().starts_with("AthenaSupplyDrop_Llama_C_"))
//		{
//			SpawnllamaLoot((ABuildingContainer*)ReceivingActor);
//		}
//		else
//		{
//			SpawnSupplyLoot((ABuildingContainer*)ReceivingActor);
//		}
//	}
//	else if (ReceivingActor->IsA(ABuildingContainer::StaticClass()))
//	{
//		ABuildingContainer* Container = (ABuildingContainer*)ReceivingActor;
//		SpawnLoot(Container);
//	}
//}


//void (*ServerAttemptInteractOG)(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);
//void ServerAttemptInteract(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId) {
//
//	ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
//
//	if (!ControllerComp || !ReceivingActor)
//		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
//
//	AFortPlayerControllerAthena* PlayerController = Utils::Cast<AFortPlayerControllerAthena>(ControllerComp->GetOwner());
//	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)ControllerComp->GetOwner();
//	static std::map<AFortPlayerControllerAthena*, int> ChestsSearched{};
//	static UClass* AthenaQuestBGAClass = Utils::StaticLoadObject<UClass>("/Game/Athena/Items/QuestInteractablesV2/Parents/AthenaQuest_BGA.AthenaQuest_BGA_C");
//
//	if (!PlayerController) {
//		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
//	}
//	std::cout << "ReceivingActor: " << ReceivingActor->GetFullName() << '\n';
//	if (ReceivingActor->Class->GetName() == "Tiered_Chest") {
//		std::cout << "ReceivingActor: " << ReceivingActor->GetFullName() << '\n';
//		printf("[+] Interacted with a chest:");
//		ABuildingContainer* Chest = (ABuildingContainer*)ReceivingActor;
//		SpawnLoot(Chest);
//	}
//	else if (ReceivingActor->IsA(AFortAthenaSupplyDrop::StaticClass()))
//	{
//		std::cout << "ReceivingActor: " << ReceivingActor->GetFullName() << '\n';
//		printf("[+] Interacted with a SupplyDrop:");
//	}
//
//}

void (*ServerAttemptInteractOG)(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);
void ServerAttemptInteract(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId) {

	ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);

	if (!ControllerComp || !ReceivingActor)
		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);

	AFortPlayerControllerAthena* PlayerController = Utils::Cast<AFortPlayerControllerAthena>(ControllerComp->GetOwner());
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)ControllerComp->GetOwner();
	static std::map<AFortPlayerControllerAthena*, int> ChestsSearched{};
	//static UClass* AthenaQuestBGAClass = Utils::StaticLoadObject<UClass>("/Game/Athena/Items/QuestInteractablesV2/Parents/AthenaQuest_BGA.AthenaQuest_BGA_C");
	if (!PlayerController) {
		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
	}
	
	if (ReceivingActor->Class->GetName().contains("Tiered_"))
	{
		static auto Accolade = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_OpenChest.AccoladeID_OpenChest");

		ChestsSearched[PC]++;

		GiveAccolade(PC, Accolade, nullptr, EXPEventPriorityType::NearReticle);

		bool bruh;
		FGameplayTagContainer Empty{};
		FGameplayTagContainer Empty2{};
		SendStatEvent(PC->GetQuestManager(ESubGame::Athena), nullptr, Empty, Empty2, &bruh, &bruh, 1, EFortQuestObjectiveStatEvent::Interact);
	}
}