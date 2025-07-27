#pragma once
#include "Utils.h"


namespace Abilities
{
	void GiveAbility(AFortPlayerPawnAthena* Pawn, UGameplayAbility* GameplayAbility)
	{
		FGameplayAbilitySpec Spec;

		static void (*ConstructAbilitySpec)(FGameplayAbilitySpec * Spec, UGameplayAbility * Ability, int Level, int InputID, UObject * Source) = decltype(ConstructAbilitySpec)(InSDKUtils::GetImageBase() + 0xa27b60);
		static FGameplayAbilitySpecHandle* (*InternalGiveAbility)(UAbilitySystemComponent * Comp, FGameplayAbilitySpecHandle * Handle, const FGameplayAbilitySpec & Spec) = decltype(InternalGiveAbility)(InSDKUtils::GetImageBase() + 0xa4bb20);

		ConstructAbilitySpec(&Spec, GameplayAbility, 1, -1, nullptr);
		InternalGiveAbility(Pawn->AbilitySystemComponent, &Spec.Handle, Spec);
	}

	void InternalServerTryActivateAbilityHook(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
	{
		FGameplayAbilitySpec* Spec = nullptr;

		for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); ++i)
		{
			auto& ItemHandle = AbilitySystemComponent->ActivatableAbilities.Items[i].Handle;

			if (ItemHandle.Handle == Handle.Handle)
			{
				Spec = &AbilitySystemComponent->ActivatableAbilities.Items[i];
			}
		}

		if (!Spec)
		{
			AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
			return;
		}

		Spec->InputPressed = true;

		UGameplayAbility* InstancedAbility = nullptr;

		static bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility * *OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData * TriggerEventData) = decltype(InternalTryActivateAbility)(InSDKUtils::GetImageBase() + 0xa52940);

		if (!InternalTryActivateAbility(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
		{
			AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
			Spec->InputPressed = false;
			AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
		}
	}

}