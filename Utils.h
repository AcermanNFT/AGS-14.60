#pragma once
#include "SDK/SDK.hpp"
using namespace SDK;
static bool bEnableVehicles = false;
inline bool bLategame = true;
inline bool bEnableSiphon = true;
namespace Utils
{
#define LOG_(...) { std::cout << "LogAndreu: " << std::format(__VA_ARGS__) << std::endl; WriteLog(std::format(__VA_ARGS__)); }

	template <typename T>
	static T* Cast(UObject* Object) {
		if (Object && Object->IsA(T::StaticClass()))
		{
			return (T*)Object;
		}

		return nullptr;
	}

	inline void HookVTable(void* Base, int Idx, void* Detour, void** OG)
	{
		DWORD oldProtection;

		void** VTable = *(void***)Base;

		if (OG)
		{
			*OG = VTable[Idx];
		}

		VirtualProtect(&VTable[Idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);

		VTable[Idx] = Detour;

		VirtualProtect(&VTable[Idx], sizeof(void*), oldProtection, NULL);
	}

	void SwapVFTs2(void* Base, uintptr_t Index, void* Detour, void** Original)
	{
		auto VTable = (*(void***)Base);
		if (!VTable)
			return;

		if (!VTable[Index])
			return;

		if (Original)
			*Original = VTable[Index];

		DWORD dwOld;
		VirtualProtect(&VTable[Index], 8, PAGE_EXECUTE_READWRITE, &dwOld);
		VTable[Index] = Detour;
		DWORD dwTemp;
		VirtualProtect(&VTable[Index], 8, dwOld, &dwTemp);
	}

	inline void VirtualHook(void* Base, int Index, void* Detour, void** OG = nullptr)
	{
		void** VTable = *(void***)Base;
		if (void** VTable = *(void***)Base)
		{
			if (OG)
				*OG = VTable[Index];

			DWORD OldProtection;
			VirtualProtect(&VTable[Index], 8, PAGE_EXECUTE_READWRITE, &OldProtection);

			VTable[Index] = Detour;

			DWORD Temp;
			VirtualProtect(&VTable[Index], 8, OldProtection, &Temp);
		}
	}

	uintptr_t GetOffsetA(uintptr_t offset)
	{
		return __int64(GetModuleHandleW(0)) + offset;
	}
	template <typename UEType>
	UEType* GetDefObject()
	{
		return (UEType*)UEType::StaticClass()->DefaultObject;
	}

	UGameplayStatics* GetStatics()
	{
		return GetDefObject<UGameplayStatics>();
	}

	UFortKismetLibrary* GetFortKismet()
	{
		return Utils::GetDefObject<UFortKismetLibrary>();
	}

	UKismetStringLibrary* GetStringKismet()
	{
		return GetDefObject<UKismetStringLibrary>();
	}

	UKismetMathLibrary* GetMath()
	{
		return GetDefObject<UKismetMathLibrary>();
	}

	AFortGameStateAthena* GetGameState()
	{
		return (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	}

	UGameplayStatics* GetGameplayStatics()
	{
		return GetDefObject<UGameplayStatics>();
	}

	AFortGameModeAthena* GetGameMode()
	{
		return (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	}

	static void Hook_Internal(uint64_t Address, PVOID Hook, void** OG = nullptr)
	{
		MH_CreateHook(PVOID(Address), Hook, OG);
		MH_EnableHook(PVOID(Address));
	}
#define Hook(...) Hook_Internal(__VA_ARGS__);

	static void HookVFT(void** Vft, uintptr_t Idx, void* Function, void** Original)
	{
		if (Original)
			*Original = Vft[Idx];

		DWORD Protection;
		if (VirtualProtect(&Vft[Idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &Protection))
		{
			Vft[Idx] = Function;
			VirtualProtect(&Vft[Idx], sizeof(void*), Protection, &Protection);
		}
	}

	template <typename T>
	static T* StaticFindObject(std::string ObjectName)
	{
		auto Name = std::wstring(ObjectName.begin(), ObjectName.end()).c_str();

		static UObject* (*StaticFindObjectOriginal)(UClass * Class, UObject * Package, const TCHAR * OrigInName, bool ExactClass) = decltype(StaticFindObjectOriginal)(__int64(GetModuleHandleW(0)) + 0x3774c20);

		return (T*)StaticFindObjectOriginal(T::StaticClass(), nullptr, Name, false);
	}

	/*static UObject* (*StaticLoadObjectOG)(
		UClass* Class,
		UObject* InOuter,
		const TCHAR* Name,
		const TCHAR* Filename,
		uint32_t LoadFlags,
		UObject* Sandbox,
		bool bAllowObjectReconciliation,
		void*
		) = reinterpret_cast<decltype(StaticLoadObjectOG)>(reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr)) + 0x3776110);*/
	static UObject* (*StaticLoadObjectOG)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(StaticLoadObjectOG)(__int64(GetModuleHandleW(0)) + 0x3776110);
	template<typename T>
	T* StaticLoadObject(const std::string& name)
	{
		T* Object = StaticFindObject<T>(name);
		if (!Object)
		{
			std::wstring wname(name.begin(), name.end());
			Object = reinterpret_cast<T*>(StaticLoadObjectOG(T::StaticClass(), nullptr, wname.c_str(), nullptr, 0, nullptr, false, nullptr));
		}

		return Object;
	}


	void sinCos(float* ScalarSin, float* ScalarCos, float Value)
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

	FQuat FRotToQuat(FRotator Rot)
	{
		const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
		const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
		float SP, SY, SR;
		float CP, CY, CR;

		sinCos(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
		sinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
		sinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

		FQuat RotationQuat;
		RotationQuat.X = CR * SP * SY - SR * CP * CY;
		RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
		RotationQuat.Z = CR * CP * SY - SR * SP * CY;
		RotationQuat.W = CR * CP * CY + SR * SP * SY;

		return RotationQuat;
	}

	template<typename T>
	T* SpawnActor(UClass* Class = T::StaticClass(), FVector Loc = {}, FRotator Rot = {}, AActor* Owner = nullptr)
	{
		FTransform Transform{};
		Transform.Scale3D = FVector(1, 1, 1);
		Transform.Translation = Loc;
		Transform.Rotation = FRotToQuat(Rot);

		return (T*)GetStatics()->FinishSpawningActor(GetStatics()->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner), Transform);
	}

	template <class T>
	T* VehiclesSpawnActor(FVector Location, FRotator Rotation = FRotator{ 0, 0, 0 }, UClass* Class = T::StaticClass(), FVector Scale3D = { 1,1,1 })
	{
		FTransform Transform{};
		Transform.Rotation = UKismetMathLibrary::Conv_RotatorToTransform(Rotation).Rotation;
		Transform.Scale3D = Scale3D;
		Transform.Translation = Location;

		auto Actor = UGameplayStatics::GetDefaultObj()->BeginSpawningActorFromClass(UWorld::GetWorld(), Class, Transform, false, nullptr);
		if (Actor)
			UGameplayStatics::GetDefaultObj()->FinishSpawningActor(Actor, Transform);
		return (T*)Actor;
	}

	template <class T>
	T* SpawnActorBuilding(FVector Location, FRotator Rotation = FRotator{ 0, 0, 0 }, UClass* Class = T::StaticClass(), FVector Scale3D = { 1,1,1 })
	{
		FTransform Transform{};
		Transform.Rotation = UKismetMathLibrary::Conv_RotatorToTransform(Rotation).Rotation;
		Transform.Scale3D = Scale3D;
		Transform.Translation = Location;

		auto Actor = UGameplayStatics::GetDefaultObj()->BeginSpawningActorFromClass(UWorld::GetWorld(), Class, Transform, false, nullptr);
		if (Actor)
			UGameplayStatics::GetDefaultObj()->FinishSpawningActor(Actor, Transform);
		return (T*)Actor;
	}


	AFortPickupAthena* SpawnPickup(FFortItemEntry* ItemEntry, FVector Loc, EFortPickupSourceTypeFlag SourceType, EFortPickupSpawnSource Source, int OverrideCount = -1)
	{
		auto SpawnedPickup = SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Loc);
		SpawnedPickup->bRandomRotation = true;

		auto& PickupEntry = SpawnedPickup->PrimaryPickupItemEntry;
		PickupEntry.ItemDefinition = ItemEntry->ItemDefinition;
		PickupEntry.Count = OverrideCount != -1 ? OverrideCount : ItemEntry->Count;
		PickupEntry.LoadedAmmo = ItemEntry->LoadedAmmo;
		SpawnedPickup->OnRep_PrimaryPickupItemEntry();

		SpawnedPickup->TossPickup(Loc, nullptr, -1, true, false, SourceType, Source);

		if (SourceType == EFortPickupSourceTypeFlag::Container)
		{
			SpawnedPickup->bTossedFromContainer = true;
			SpawnedPickup->OnRep_TossedFromContainer();
		}

		return SpawnedPickup;
	}

	AFortPickupAthena* SpawnPickup(UFortItemDefinition* ItemDef, int OverrideCount, int LoadedAmmo, FVector Loc, EFortPickupSourceTypeFlag SourceType, EFortPickupSpawnSource Source)
	{
		auto SpawnedPickup = SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Loc);
		SpawnedPickup->bRandomRotation = true;

		auto& PickupEntry = SpawnedPickup->PrimaryPickupItemEntry;
		PickupEntry.ItemDefinition = ItemDef;
		PickupEntry.Count = OverrideCount;
		PickupEntry.LoadedAmmo = LoadedAmmo;
		SpawnedPickup->OnRep_PrimaryPickupItemEntry();

		SpawnedPickup->TossPickup(Loc, nullptr, -1, true, false, SourceType, Source);

		if (SourceType == EFortPickupSourceTypeFlag::Container)
		{
			SpawnedPickup->bTossedFromContainer = true;
			SpawnedPickup->OnRep_TossedFromContainer();
		}

		return SpawnedPickup;
	}

	template <class T>
	inline TArray<T*> GetAllActorsOfClass() {
		TArray<T*> ResultActors;

		if (UWorld* World = UWorld::GetWorld()) {
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(World, T::StaticClass(), &OutActors);

			for (AActor* Actor : OutActors) {
				if (T* CastedActor = Cast<T>(Actor)) {
					ResultActors.Add(CastedActor);
				}
			}
		}
		return ResultActors;
	}
}

//// Function pointer declaration
//static UObject* (*StaticLoadObjectOG)(
//	UClass* Class,
//	UObject* InOuter,
//	const TCHAR* Name,
//	const TCHAR* Filename,
//	uint32_t LoadFlags,
//	UObject* Sandbox,
//	bool bAllowObjectReconciliation,
//	void*
//	) = reinterpret_cast<decltype(StaticLoadObjectOG)>(reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr)) + 0x3776110);
//
//// Template function to load object by name
//template<typename T>
//T* StaticLoadObject(const std::string& name)
//{
//	T* Object = StaticFindObject<T>(name);
//	if (!Object)
//	{
//		std::wstring wname(name.begin(), name.end());
//		Object = reinterpret_cast<T*>(StaticLoadObjectOG(T::StaticClass(), nullptr, wname.c_str(), nullptr, 0, nullptr, false, nullptr));
//	}
//
//	return Object;
//}


extern inline UObject* (*StaticFindObjectOriginal22)(UClass* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass) = nullptr;
extern inline UObject* (*StaticLoadObjectOriginal22)(UClass*, UObject*, const wchar_t* InName, const wchar_t* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation) = nullptr;

template <typename T = UObject>
static inline T* StaticFindObject22(UClass* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass = false)
{
	// LOG_INFO(LogDev, "StaticFindObjectOriginal: {}", __int64(StaticFindObjectOriginal));
	return (T*)StaticFindObjectOriginal22(Class, InOuter, Name, ExactClass);
}

template <typename T = UObject>
static inline T* FindObject(const TCHAR* Name, UClass* Class = nullptr, UObject* Outer = nullptr)
{
	auto res = (T*)StaticFindObject22/*<T>*/(Class, Outer, Name);
	/*
	if (!res)
	{
		std::wstring NameWStr = std::wstring(Name);
		LOG_WARN(LogDev, "Failed to find object: {}", std::string(NameWStr.begin(), NameWStr.end()));
	}
	*/
	return res;
}


static void ApplySiphonEffectToEveryone()
{
	for (int i = 0; i < UWorld::GetWorld()->NetDriver->ClientConnections.Num(); i++)
	{
		auto PlayerState = (AFortPlayerState*)UWorld::GetWorld()->NetDriver->ClientConnections[i]->PlayerController->PlayerState;

		auto AbilitySystemComponent = PlayerState->AbilitySystemComponent;

		auto Handle = AbilitySystemComponent->MakeEffectContext();
		AbilitySystemComponent->NetMulticast_InvokeGameplayCueAdded(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), Handle);
		AbilitySystemComponent->NetMulticast_InvokeGameplayCueExecuted(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), Handle);
	}
}