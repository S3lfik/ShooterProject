// Fill out your copyright notice in the Description page of Project Settings.


#include "CharMeshMergeFunctionLibrary.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"

namespace
{
	static void ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray< FSkelMeshMergeSectionMapping>& OutSectionMappings)
	{
		if (InSectionMappings.Num() > 0)
		{
			OutSectionMappings.AddUninitialized(InSectionMappings.Num());
			for (int32 i = 0; i < InSectionMappings.Num(); ++i)
			{
				OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
			}
		}
	}

	static void ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
	{
		if (InUVTransformsPerMesh.Num() > 0)
		{
			OutUVTransformsPerMesh.Empty();
			OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
			for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
			{
				TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
				const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
				if (InUVTransforms.Num() > 0)
				{
					OutUVTransforms.Empty();
					OutUVTransforms.AddUninitialized(InUVTransforms.Num());

					for (int32 j = 0; j < InUVTransforms.Num(); ++j)
					{
						OutUVTransforms[j] = InUVTransforms[j].UVTransforms;
					}
				}
			}
		}
	}
}

USkeletalMesh* UCharMeshMergeFunctionLibrary::MergeMeshes(const FSkelMeshMergeParams& Params)
{
	TArray <USkeletalMesh*> MeshesToMerge = Params.MeshesToMerge;
	MeshesToMerge.RemoveAll([](USkeletalMesh* InMesh)
	{
		return InMesh == nullptr;
	});
	if (MeshesToMerge.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Must contain several meshes to merge"));
	}

	EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
		EMeshBufferAccess::ForceCPUAndGPU :
		EMeshBufferAccess::Default;
	TArray<FSkelMeshMergeSectionMapping> SectionMapping;
	TArray<FSkelMeshMergeUVTransforms> UVTransforms;
	ToMergeParams(Params.MeshSectionMappings, SectionMapping);
	ToMergeParams(Params.UVTransformsPerMesh, UVTransforms);

	bool bRunDuplicatesCheck = false;
	USkeletalMesh* baseMesh = NewObject<USkeletalMesh>();
	if (Params.Skeleton && Params.bSkeletonBefore)
	{
		baseMesh->Skeleton = Params.Skeleton;
		bRunDuplicatesCheck = true;
		for (USkeletalMeshSocket* socket : baseMesh->GetMeshOnlySocketList())
		{
			if (socket)
			{
				UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(socket->SocketName.ToString()));
			}
		}
		for (auto socket : baseMesh->Skeleton->Sockets)
		{
			if (socket)
			{
				UE_LOG(LogTemp, Warning, TEXT("SkelBaseMeshSocket: %s"), *(socket->SocketName.ToString()));
			}
		}
	}

	FSkeletalMeshMerge Merger{ baseMesh, MeshesToMerge, SectionMapping, Params.StripTopLODS,
		BufferAccess, UVTransforms.GetData() };

	if (!Merger.DoMerge())
	{
		UE_LOG(LogTemp, Error, TEXT("Meshes merge failed!"));
		return nullptr;
	}

	if (Params.Skeleton && !Params.bSkeletonBefore)
	{
		baseMesh->Skeleton = Params.Skeleton;
	}
	if (bRunDuplicatesCheck)
	{
		TArray<FName> SkelMeshSockets;
		TArray<FName> SkelSockets;
		for (USkeletalMeshSocket* socket : baseMesh->GetMeshOnlySocketList())
		{
			if (socket)
			{
				SkelMeshSockets.Add(socket->GetFName());
				UE_LOG(LogTemp, Warning, TEXT("MeshSocketlist socket: %s"), *(socket->SocketName.ToString()));
			}
		}
		for (auto socket : baseMesh->Skeleton->Sockets)
		{
			if (socket)
			{
				SkelSockets.Add(socket->GetFName());
				UE_LOG(LogTemp, Warning, TEXT("skeleton socket: %s"), *(socket->SocketName.ToString()));
			}
		}
		TSet<FName> UniqSkelMeshSockets;
		TSet<FName> UniqSkelSockets;
		UniqSkelMeshSockets.Append(SkelMeshSockets);
		UniqSkelSockets.Append(SkelSockets);
		int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
		int32 UniqueTotal = UniqSkelMeshSockets.Num() + UniqSkelSockets.Num();
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqSkelMeshSockets.Num(), UniqSkelSockets.Num(), UniqueTotal);
		UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
	}

	return baseMesh;
}