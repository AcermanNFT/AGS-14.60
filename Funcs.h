#pragma once
#include "Utils.h"
inline uint64_t ImageBase = uint64_t(GetModuleHandle(0)); 
namespace Funcs {
    inline UNetDriver* (*CreateNetDriver)(SDK::UEngine*, SDK::UWorld*, SDK::FName) = decltype(CreateNetDriver)(ImageBase + Addresses::CreateNetDriver);
    inline bool (*InitListen)(SDK::UNetDriver*, SDK::UWorld*, SDK::FURL&, bool, FString) = decltype(InitListen)(ImageBase + Addresses::InitListen);
    inline void (*ServerReplicateActors)(SDK::UReplicationDriver*, float) = decltype(ServerReplicateActors)(ImageBase + Addresses::ServerReplicateActors);
    inline void (*SetWorld)(SDK::UNetDriver*, SDK::UWorld*) = decltype(SetWorld)(ImageBase + Addresses::SetWorld);
    inline void (*ApplyCharacterCustomization)(SDK::APlayerState*, SDK::APawn*) = decltype(ApplyCharacterCustomization)(ImageBase + Addresses::ApplyCharacterCustomization);
}
