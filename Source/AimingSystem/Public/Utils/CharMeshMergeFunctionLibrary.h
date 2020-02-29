// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/NoExportTypes.h"
#include "CharMeshMergeFunctionLibrary.generated.h"

/**
* Blueprint equivalent of FSkeleMeshMergeSectionMapping
* Info to map all the sections from a single source skeletal mesh to
* a final section entry in the merged skeletal mesh.
*/

USTRUCT(BlueprintType)
struct FSkelMeshMergeSectionMapping_BP
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
		TArray< int32 > SectionIDs;
};

/**
* Used to wrap a set of UV Transforms for one mesh.
*/
USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransform
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
		TArray < FTransform > UVTransforms;
};

/**
* Blueprint equivalent of FSkelMeshMergeUVTransforms
* Info to map all the sections about how to transform their UVs
*/
USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransformMapping
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
		TArray < FSkelMeshMergeUVTransform > UVTransformsPerMesh;
};

/**
* Struct containing all parameters used to perform a Skeletal Mesh merge.
*/
USTRUCT(BlueprintType)
struct FSkelMeshMergeParams
{
	GENERATED_BODY()

		FSkelMeshMergeParams()
	{
		MeshSectionMappings = TArray<FSkelMeshMergeSectionMapping_BP>();
		UVTransformsPerMesh = TArray<FSkelMeshMergeUVTransformMapping>();
		StripTopLODS = 0;
		bNeedsCpuAccess = false;
		bSkeletonBefore = false;
		Skeleton = nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray < FSkelMeshMergeSectionMapping_BP > MeshSectionMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray < FSkelMeshMergeUVTransformMapping > UVTransformsPerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray < USkeletalMesh* > MeshesToMerge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 StripTopLODS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint32 bNeedsCpuAccess : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint32 bSkeletonBefore : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeleton* Skeleton;
};

/**
 * 
 */
UCLASS()
class AIMINGSYSTEM_API UCharMeshMergeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	* Merges the given meshes into a single mesh.
	* @return The merged mesh (will be invalid if the merge failed).
	*/

	UFUNCTION(BlueprintCallable, Category = "Mesh Merge", meta = (UnsafeDuringActorConstruction = "true"))
		static class USkeletalMesh* MergeMeshes(const FSkelMeshMergeParams& params);
	
};
