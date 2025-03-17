// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC_AIController.h"

UMyBTTask_FindRandomLocation::UMyBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Find Random Location Using NavMesh";
}

EBTNodeResult::Type UMyBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const cont = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const npc = cont->GetPawn()) 
		{
			auto const Origin = npc->GetActorLocation();

			if(auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld())) 
			{
				FNavLocation Loc;
				if(NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;

			}
		}
	}
	return EBTNodeResult::Failed;
}
