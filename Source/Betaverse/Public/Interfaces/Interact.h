// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BETAVERSE_API IInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(ACharacterMaster* Character);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void StopInteract(ACharacterMaster* Character);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		FText GetInteractionDetails();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void InteractFromServer(ACharacterMaster* Character);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void StopInteractFromServer(ACharacterMaster* Character);
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIndexButton : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class BETAVERSE_API IIndexButton
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void PressButtonAtIndex(int ButtonIndex);

};