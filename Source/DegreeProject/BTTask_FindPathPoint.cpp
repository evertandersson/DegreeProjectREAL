// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPathPoint.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPathPoint::UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("find Path Point");
}

EBTNodeResult::Type UBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const cont = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const bc = OwnerComp.GetBlackboardComponent())
		{
			auto const index = bc->GetValueAsInt(GetSelectedBlackboardKey());

			if (auto* npc = Cast<ANPC>(cont->GetPawn()))
			{
				auto const Point = npc->GetPatrolPath()->GetPatrolPoint(index);

				auto const GlobalPoint = npc->GetPatrolPath()->GetActorTransform().TransformPosition(Point);
				bc->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName, GlobalPoint);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
