// Copyright Raymond


#include "Checkpoint/Checkpoint.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
		
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffect();
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (!bReached)
		{
			HandleGlowEffect();
			bReached = true;
		}
		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameMode->SaveWorldState(GetWorld());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
	}
}

void ACheckpoint::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void ACheckpoint::HandleGlowEffect()
{
	//Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ACheckpoint::OnSphereEndOverlap);
}
