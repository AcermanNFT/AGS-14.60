#pragma once
#include "framework.h"
#include <Windows.h>
namespace Addresses {
    inline uintptr_t GetNetMode = 0x5139DE0;
    inline uintptr_t GIsClient = 0x939A30D;
    inline uintptr_t GIsServer = 0x939A30E;
    inline uintptr_t KickPlayer = 0x4CDE3C0;
    inline uintptr_t InitListen = 0xC61890;
    inline uintptr_t SetWorld = 0x4E3E980;
    inline uintptr_t ActorNetMode = 0x4A3FAA0;//No Need to worry if this is 0x0
    inline uintptr_t ChangeGameSessionId = 0x2653520;//No Need to worry if this is 0x0
    inline uintptr_t ApplyCharacterCustomization = 0x2BEEAD0;//No Need to worry if this is 0x0
    inline uintptr_t CreateNetDriver = 0x50D2BF0;
    inline uintptr_t ServerReplicateActors = 0x16F2410;
    inline uintptr_t TickFlush = 0x4E3FEA0;
    inline uintptr_t ReadyToStartMatch = 0x206E160;
    inline uintptr_t SpawnDefaultPawnFor = 0x2079A10;
    inline uintptr_t ServerAcknowledgePossesion = 0x4F42640;
}
