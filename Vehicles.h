#pragma once
#include "Utils.h"


#include "framework.h"

void SpawnVehicles() {
    if (bLategame)
        return;
    auto VehicleSpawners = Utils::GetAllActorsOfClass<AFortAthenaVehicleSpawner>();

    for (auto& VehicleSpawner : VehicleSpawners) {
        if (!VehicleSpawner)
            continue;

        auto VehicleClass = VehicleSpawner->GetVehicleClass();
        if (!VehicleClass)
            continue;

        AFortAthenaVehicle* Vehicle = Utils::VehiclesSpawnActor<AFortAthenaVehicle>(
            VehicleSpawner->K2_GetActorLocation(),
            VehicleSpawner->K2_GetActorRotation(),
            VehicleClass
        );

        if (!Vehicle)
            continue;

        auto FuelComponent = Utils::Cast<UFortAthenaVehicleFuelComponent>(
            Vehicle->GetComponentByClass(UFortAthenaVehicleFuelComponent::StaticClass())
        );

        if (FuelComponent) {
            FuelComponent->ServerFuel = 100;
            FuelComponent->OnRep_ServerFuel(0);
        }

        VehicleSpawners.Free();
    }
}
