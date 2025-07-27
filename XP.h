#pragma once
#include "Utils.h"
#include "framework.h"
static bool QuestEnabled = true;

bool ContainsTag(FGameplayTagContainer Container, FName Tag)
{
	for (auto tag : Container.GameplayTags)
	{
		if (tag.TagName.ComparisonIndex == Tag.ComparisonIndex)
			return true;
	}
	return false;
}


inline int GetOffset(UObject* Object, std::string name)
{
	FProperty* Property = nullptr;

	for (UStruct* Cls = Object->Class; Cls; Cls = Cls->Super)
	{
		FField* ChildProperties = Cls->ChildProperties;
		if (ChildProperties)
		{
			Property = (FProperty*)ChildProperties;
			std::string PropStr = ChildProperties->Name.ToString();
			while (Property)
			{
				if (PropStr == name)
					return Property->Offset;

				Property = (FProperty*)Property->Next;
				PropStr = Property ? Property->Name.ToString() : "Invalid Property";
			}
		}
	}
	if (!Property)
		return 0;
	return Property->Offset;
}


void GiveAccolade(AFortPlayerControllerAthena* PC, UFortAccoladeItemDefinition* AccoladeDef, UFortQuestItemDefinition* QuestDef, EXPEventPriorityType Priority)
{
	if (!PC || !PC->XPComponent || !AccoladeDef)
		return;

	FXPEventInfo Info{};
	Info.Accolade = UKismetSystemLibrary::GetPrimaryAssetIdFromObject(AccoladeDef);
	Info.EventXpValue = AccoladeDef->GetAccoladeXpValue();//idk if proepr functiosn
	Info.Priority = Priority;
	Info.QuestDef = QuestDef;
	Info.RestedXPRemaining = PC->XPComponent->RestXP - Info.EventXpValue;
	Info.RestedValuePortion = 100;
	Info.SeasonBoostValuePortion = PC->XPComponent->CachedSeasonMatchXpBoost;
	Info.TotalXpEarnedInMatch = PC->XPComponent->TotalXpEarned += Info.EventXpValue;
	Info.SimulatedText = AccoladeDef->ShortDescription;
	PC->XPComponent->OnXPEvent(Info);
	//FXPEventEntry Entry{};
	//Entry.Accolade = UKismetSystemLibrary::GetPrimaryAssetIdFromObject(AccoladeDef);
	//Entry.EventXpValue = AccoladeDef->GetAccoladeXpValue();
	//Entry.QuestDef = QuestDef;
	//Entry.Time = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
	//Entry.TotalXpEarnedInMatch = Entry.EventXpValue;
	//Entry.SimulatedXpEvent = AccoladeDef->ShortDescription;
	//PC->XPComponent->EventArray.Entries.Add(Entry);
	//PC->XPComponent->EventArray.MarkItemDirty(Entry);
	//PC->XPComponent->HighPrioXPEvent(Entry);
}

void AddStatObjective(AFortPlayerControllerAthena* PC, FFortUpdatedObjectiveStat& Obj)
{
	for (auto& UpdatedObj : PC->UpdatedObjectiveStats)
	{
		if (UpdatedObj.BackendName.ComparisonIndex == Obj.BackendName.ComparisonIndex)
		{
			UpdatedObj.CurrentStage = Obj.CurrentStage;
			UpdatedObj.ShadowStatValue = Obj.ShadowStatValue;
			UpdatedObj.StatDelta = Obj.StatDelta;
			UpdatedObj.StatValue = Obj.StatValue;
			PC->OnRep_UpdatedObjectiveStats();
			return;
		}
	}
	PC->UpdatedObjectiveStats.Add(Obj);
	PC->OnRep_UpdatedObjectiveStats();
}

void UpdateChallange(UFortQuestManager* QuestManager, UFortQuestItemDefinition* QuestItem, FName BackendName, int Count)
{
	static FName map = UKismetStringLibrary::Conv_StringToName(L"Apollo_Terrain");
	if (!UWorld::GetWorld() || !UWorld::GetWorld()->GameState || UWorld::GetWorld()->Name.ComparisonIndex != map.ComparisonIndex || ((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->GamePhase <= EAthenaGamePhase::Aircraft)
		return;
	auto PC = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();

	if (!QuestManager || !PC || !Count || QuestManager->HasCompletedQuest(QuestItem))
		return;
	int DeltaChange = 1;
	UFortQuestItem* CurrentQuest = nullptr;
	UFortQuestObjectiveInfo* CurrentObjective = nullptr;
	for (auto bruh : QuestManager->CurrentQuests)
	{
		for (auto obj : bruh->Objectives)
		{
			if (obj->BackendName.ComparisonIndex == BackendName.ComparisonIndex)
			{
				CurrentQuest = bruh;
				CurrentObjective = obj;
				break;
			}
		}
	}

	if (!CurrentQuest || !CurrentObjective)
		return;
	int OldAchievedCount = CurrentObjective->AchievedCount;
	//DeltaChange = CurrentObjective->RequiredCount - CurrentObjective->AchievedCount;
	//printf("DeltaChange: %d %s\n", DeltaChange, BackendName.ToString().c_str());
	//printf("Completion Count: %d\n", Count);
	CurrentObjective->AchievedCount += Count;
	DeltaChange = CurrentObjective->AchievedCount - OldAchievedCount;
	if (CurrentQuest->CurrentStage == -1)
		CurrentQuest->CurrentStage = 0;
	bool ObjCompleted = CurrentObjective->AchievedCount >= CurrentObjective->RequiredCount;
	//if (ObjCompleted)
	//	CurrentQuest->CurrentStage++;
	FFortUpdatedObjectiveStat Stat{};
	Stat.BackendName = BackendName;
	Stat.CurrentStage = CurrentQuest->CurrentStage;
	Stat.Quest = QuestItem;
	Stat.ShadowStatValue = CurrentObjective->AchievedCount;
	Stat.StatDelta = DeltaChange;
	Stat.StatValue = Stat.ShadowStatValue;
	AddStatObjective(PC, Stat);
	//CurrentObjective->DisplayDynamicQuestUpdate();

	printf("CurrentStage: %d ObjCompleted: %p\n", Stat.CurrentStage, ObjCompleted);

	//TArray<FFortDisplayQuestUpdateData> UpdateData;
	//FFortDisplayQuestUpdateData Data{};
	//Data.QuestOwner = (AFortPlayerState*)PC->PlayerState;
	//Data.ObjectiveUpdated = Stat;
	//PC->Client_DisplayQuestUpdate(UpdateData);
	//UpdateData.Free();

	QuestManager->HandleQuestUpdated(PC, QuestItem, BackendName, Stat.StatValue, DeltaChange, nullptr, ObjCompleted, /*CurrentQuest->GetNumObjectivesComplete() >= CurrentQuest->Objectives.Num()*/false);
}

void SendStatEvent(UFortQuestManager* ManagerComp, UObject* TargetObject, FGameplayTagContainer& AdditionalSourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count, EFortQuestObjectiveStatEvent StatEvent)
{
	if (!QuestEnabled)
		return;
	if (!ManagerComp)
		return;

	FGameplayTagContainer Source;
	FGameplayTagContainer Context;
	ManagerComp->GetSourceAndContextTags(&Source, &Context);
	ManagerComp->AppendTemporaryRelevancyTags(Source, Context, TargetTags);

	for (auto tag : Source.GameplayTags)
	{
		bool contains = false;
		for (auto aTag : AdditionalSourceTags.GameplayTags)
		{
			if (aTag.TagName.ComparisonIndex == tag.TagName.ComparisonIndex)
			{
				contains = true;
				break;
			}
		}
		if (!contains)
			AdditionalSourceTags.GameplayTags.Add(tag);
	}

	for (auto tag : Source.ParentTags)
	{
		bool contains = false;
		for (auto aTag : AdditionalSourceTags.ParentTags)
		{
			if (aTag.TagName.ComparisonIndex == tag.TagName.ComparisonIndex)
			{
				contains = true;
				break;
			}
		}
		if (!contains)
			AdditionalSourceTags.ParentTags.Add(tag);
	}

	Source.GameplayTags.Free();
	Source.ParentTags.Free();
	Context.GameplayTags.Free();
	Context.ParentTags.Free();

	if (StatEvent == EFortQuestObjectiveStatEvent::Kill)
	{
		int Kills = ((AFortPlayerStateAthena*)ManagerComp->GetPlayerControllerBP()->PlayerState)->KillScore + 1;

		static UFortAccoladeItemDefinition* ElimDef = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_012_Elimination.AccoladeId_012_Elimination");
		static UFortAccoladeItemDefinition* ElimDef1 = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze");
		static UFortAccoladeItemDefinition* ElimDef4 = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Silver.AccoladeId_014_Elimination_Silver");
		static UFortAccoladeItemDefinition* ElimDef8 = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Gold.AccoladeId_014_Elimination_Gold");

		GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), ElimDef, nullptr, EXPEventPriorityType::NearReticle);

		if (Kills == 1)
		{
			GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), ElimDef1, nullptr, EXPEventPriorityType::NearReticle);
		}
		if (Kills == 4)
		{
			GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), ElimDef4, nullptr, EXPEventPriorityType::NearReticle);
		}
		if (Kills == 8)
		{
			GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), ElimDef8, nullptr, EXPEventPriorityType::NearReticle);
		}
	}

	if (StatEvent == EFortQuestObjectiveStatEvent::Interact)
	{
		static FName ChestTag = UKismetStringLibrary::Conv_StringToName(TEXT("Building.Type.Container.TreasureChest"));
		static FName AmmoBoxTag = UKismetStringLibrary::Conv_StringToName(TEXT("Building.Type.Container.Ammobox"));
		bool IsChest = ContainsTag(TargetTags, ChestTag);
		bool IsAmmoBox = ContainsTag(TargetTags, AmmoBoxTag);
		if (IsChest)
		{
			static auto SearchDef = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_007_SearchChests.AccoladeId_007_SearchChests");
			GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), SearchDef, nullptr, EXPEventPriorityType::NearReticle);
		}
		else if (IsAmmoBox)
		{
			static auto SearchDef = Utils::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_011_SearchAmmoBox.AccoladeId_011_SearchAmmoBox");
			GiveAccolade((AFortPlayerControllerAthena*)ManagerComp->GetPlayerControllerBP(), SearchDef, nullptr, EXPEventPriorityType::NearReticle);
		}
	}

	static FName HomebaseClassName = UKismetStringLibrary::Conv_StringToName(TEXT("Homebase.Class"));
	static FName PawnBudName = UKismetStringLibrary::Conv_StringToName(TEXT("Pawn.Athena.NPC.Bud"));
	for (auto Quest : ManagerComp->CurrentQuests)
	{
		auto QuestDef = Quest->GetQuestDefinitionBP();
		if (!QuestDef || ManagerComp->HasCompletedQuest(QuestDef))
			continue;
		auto PrereqQuest = QuestDef->GetPrerequisiteQuest();

		if (PrereqQuest && !ManagerComp->HasCompletedQuest(PrereqQuest))
			continue;

		if (QuestDef->PrerequisiteObjective.DataTable && QuestDef->PrerequisiteObjective.RowName.ComparisonIndex && !ManagerComp->HasCompletedObjective(QuestDef, QuestDef->PrerequisiteObjective))
			return;

		for (auto& Objective : QuestDef->Objectives)
		{
			if (ManagerComp->HasCompletedObjective(QuestDef, Objective.ObjectiveStatHandle) || Objective.InlineObjectiveStats.Num() <= 0)
				continue;
			auto& InlineStat = Objective.InlineObjectiveStats;
			auto& Stat = InlineStat[0];
			if (Stat.Type != StatEvent)
				continue;
			//Ill implement context later
			bool hasSource = false;
			bool hasTarget = false;
			if (!Stat.bHasInclusiveSourceTags && !Stat.bHasInclusiveTargetTags)
			{
				hasSource = true;
				hasTarget = true;
			}
			for (auto& cond : Stat.TagConditions)
			{
				//if (cond.Require == 0)
				//	continue;
				if (cond.Tag.TagName.ComparisonIndex == HomebaseClassName.ComparisonIndex)
					continue;
				if (cond.Type == EInlineObjectiveStatTagCheckEntryType::Source)
				{
					if (Stat.bHasInclusiveSourceTags)
					{
						if (Stat.Condition.IsValid() && Stat.Condition.ToString().contains("Source"))
						{
							hasSource = true;
							break;
						}
						for (auto& tag : AdditionalSourceTags.GameplayTags)
						{
							if (tag.TagName.ComparisonIndex == cond.Tag.TagName.ComparisonIndex)
							{
								hasSource = true;
								break;
							}
						}
						for (auto& tag : AdditionalSourceTags.ParentTags)
						{
							if (tag.TagName.ComparisonIndex == cond.Tag.TagName.ComparisonIndex)
							{
								hasSource = true;
								break;
							}
						}
					}
				}
				if (cond.Type == EInlineObjectiveStatTagCheckEntryType::Target)
				{
					if (cond.Tag.TagName.ComparisonIndex == PawnBudName.ComparisonIndex)
					{
						hasTarget = true;
						break;
					}
					if (Stat.bHasInclusiveTargetTags)
					{
						for (auto& tag : TargetTags.GameplayTags)
						{
							if (tag.TagName.ComparisonIndex == cond.Tag.TagName.ComparisonIndex)
							{
								hasTarget = true;
								break;
							}
						}
						for (auto& tag : TargetTags.ParentTags)
						{
							if (tag.TagName.ComparisonIndex == cond.Tag.TagName.ComparisonIndex)
							{
								hasTarget = true;
								break;
							}
						}
					}
				}
			}
			if ((!Stat.bHasInclusiveTargetTags || (Stat.bHasInclusiveTargetTags && hasTarget)) && (!Stat.bHasInclusiveSourceTags || (Stat.bHasInclusiveSourceTags && hasSource)))
			{
				UpdateChallange(ManagerComp, QuestDef, Objective.BackendName, Count);
			}
		}
	}
	TargetTags.GameplayTags.Free();
	TargetTags.ParentTags.Free();
	AdditionalSourceTags.GameplayTags.Free();
	AdditionalSourceTags.ParentTags.Free();
}


void SendComplexCustomStatEvent(UFortQuestManager* ManagerComp, UObject* TargetObject, FGameplayTagContainer& AdditionalSourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count)
{
	if (!ManagerComp || !Count)
		return;

	SendStatEvent(ManagerComp, TargetObject, AdditionalSourceTags, TargetTags, QuestActive, QuestCompleted, Count, EFortQuestObjectiveStatEvent
		::ComplexCustom);
}

bool SendDistanceUpdate(UGameplayAbility* Ability)
{
	return true;
	static FName map = UKismetStringLibrary::Conv_StringToName(L"Apollo_Terrain");
	if (!UWorld::GetWorld() || !UWorld::GetWorld()->GameState || UWorld::GetWorld()->Name.ComparisonIndex != map.ComparisonIndex || ((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->GamePhase <= EAthenaGamePhase::Aircraft)
		return true;
	static UClass* SandTunnelClass = Utils::StaticLoadObject<UClass>("/BattlepassS15/Items/QuestAbilities/Quest_Distance_SandTunneling/GA_Quest_Distance_SandTunneling.GA_Quest_Distance_SandTunneling_C");
	static UClass* OnFootClass = Utils::StaticLoadObject<UClass>("/BattlepassS15/Items/QuestAbilities/Quest_Distance_OnFoot/GA_Quest_Distance_OnFoot.GA_Quest_Distance_OnFoot_C");
	static UClass* SwimDistanceClass = Utils::StaticLoadObject<UClass>("/BattlepassS15/Items/QuestAbilities/Milestone/SwimDistance/GA_Quest_Milestone_SwimDistance.GA_Quest_Milestone_SwimDistance_C");
	static UClass* VehicleDistanceClass = Utils::StaticLoadObject<UClass>("/BattlepassS15/Items/QuestAbilities/Milestone/VehicleDistance/GA_Quest_Milestone_VehicleDistance.GA_Quest_Milestone_VehicleDistance_C");
	static UClass* GlideDistanceClass = Utils::StaticLoadObject<UClass>("/BattlepassS15/Items/QuestAbilities/Milestone/GlideDistance/GA_Quest_Milestone_GlideDistance.GA_Quest_Milestone_GlideDistance_C");

	static bool First = false;
	static FGameplayTagContainer SandTunnelSourceTags;
	static FGameplayTagContainer OnFootSourceTags;
	static FGameplayTagContainer SwimDistanceTags;
	static FGameplayTagContainer VehicleDistanceTags;
	static FGameplayTagContainer GlideDistanceTags;
	if (!First)
	{
		First = true;
		SandTunnelSourceTags.GameplayTags.Add({ UKismetStringLibrary::Conv_StringToName(TEXT("Athena.Quests.Distance.SandTunneling")) });
		OnFootSourceTags.GameplayTags.Add({ UKismetStringLibrary::Conv_StringToName(TEXT("Athena.Quests.Distance.OnFoot")) });
		SwimDistanceTags.GameplayTags.Add({ UKismetStringLibrary::Conv_StringToName(TEXT("Athena.Quests.SwimDistance")) });
		VehicleDistanceTags.GameplayTags.Add({ UKismetStringLibrary::Conv_StringToName(TEXT("Athena.Quests.VehicleDistance")) });
		GlideDistanceTags.GameplayTags.Add({ UKismetStringLibrary::Conv_StringToName(TEXT("Athena.Quests.GlideDistance")) });
	}

	if (Ability->Class == SandTunnelClass)
	{
		auto AbilitySystemComp = Ability->GetAbilitySystemComponentFromActorInfo();
		if (!AbilitySystemComp || !AbilitySystemComp->GetOwner())
			return false;
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)AbilitySystemComp->GetOwner();
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)PlayerState->Owner;
		if (!PlayerController)
			return false;
		static auto MetersTravelledOffset = GetOffset(Ability, "MetersTravelled");
		int& MetersTravelled = *(int*)(__int64(Ability) + MetersTravelledOffset);
		bool bruh;
		bool isOnFoot = false;
		FGameplayTagContainer Empty{};
		SendComplexCustomStatEvent(PlayerController->GetQuestManager(ESubGame::Athena), nullptr, isOnFoot ? OnFootSourceTags : SandTunnelSourceTags, Empty, &bruh, &bruh, MetersTravelled);
	}
	else if (Ability->Class == OnFootClass)
	{
		auto AbilitySystemComp = Ability->GetAbilitySystemComponentFromActorInfo();
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)AbilitySystemComp->GetOwner();
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)PlayerState->Owner;
		static auto MetersTravelledOffset = GetOffset(Ability, "MetersTravelled");
		int& MetersTravelled = *(int*)(__int64(Ability) + MetersTravelledOffset);
		if (!PlayerController->MyFortPawn)
			return false;
		if (PlayerController->MyFortPawn->IsSkydiving())
		{
			MetersTravelled = 0;
			return false;
		}
		bool bruh;
		FGameplayTagContainer Empty{};
		SendComplexCustomStatEvent(PlayerController->GetQuestManager(ESubGame::Athena), nullptr, OnFootSourceTags, Empty, &bruh, &bruh, MetersTravelled);
		MetersTravelled = 0;
	}
	else if (Ability->Class == SwimDistanceClass)
	{
		auto AbilitySystemComp = Ability->GetAbilitySystemComponentFromActorInfo();
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)AbilitySystemComp->GetOwner();
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)PlayerState->Owner;
		if (!PlayerController->MyFortPawn)
			return false;
		static auto MetersTravelledOffset = GetOffset(Ability, "MetersTraveled");
		int& MetersTravelled = *(int*)(__int64(Ability) + MetersTravelledOffset);
		bool bruh;
		FGameplayTagContainer Empty{};
		SendComplexCustomStatEvent(PlayerController->GetQuestManager(ESubGame::Athena), nullptr, SwimDistanceTags, Empty, &bruh, &bruh, MetersTravelled);
		MetersTravelled = 0;
	}
	else if (Ability->Class == VehicleDistanceClass)
	{
		auto AbilitySystemComp = Ability->GetAbilitySystemComponentFromActorInfo();
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)AbilitySystemComp->GetOwner();
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)PlayerState->Owner;
		if (!PlayerController->MyFortPawn)
			return false;
		static auto MetersTravelledOffset = GetOffset(Ability, "MetersTraveled");
		int& MetersTravelled = *(int*)(__int64(Ability) + MetersTravelledOffset);
		if (!PlayerController->MyFortPawn)
			return false;
		if (!PlayerController->MyFortPawn->IsInVehicle())
		{
			MetersTravelled = 0;
			return false;
		}
		bool bruh;
		FGameplayTagContainer Empty{};
		SendComplexCustomStatEvent(PlayerController->GetQuestManager(ESubGame::Athena), nullptr, VehicleDistanceTags, Empty, &bruh, &bruh, MetersTravelled);
		MetersTravelled = 0;
	}
	else if (Ability->Class == GlideDistanceClass)
	{
		auto AbilitySystemComp = Ability->GetAbilitySystemComponentFromActorInfo();
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)AbilitySystemComp->GetOwner();
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)PlayerState->Owner;
		if (!PlayerController->MyFortPawn)
			return false;
		static auto MetersTravelledOffset = GetOffset(Ability, "MetersTraveled");
		int& MetersTravelled = *(int*)(__int64(Ability) + MetersTravelledOffset);
		if (!PlayerController->MyFortPawn)
			return false;
		if (!PlayerController->MyFortPawn->IsSkydiving())
		{
			MetersTravelled = 0;
			return false;
		}
		bool bruh;
		FGameplayTagContainer Empty{};
		SendComplexCustomStatEvent(PlayerController->GetQuestManager(ESubGame::Athena), nullptr, GlideDistanceTags, Empty, &bruh, &bruh, MetersTravelled);
		MetersTravelled = 0;
	}
	return true;
}