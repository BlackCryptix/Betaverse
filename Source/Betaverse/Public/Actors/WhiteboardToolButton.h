// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WhiteboardToolButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWhiteboardButtonPressed, int, ButtonIndex);

UENUM(BlueprintType)
enum EWhiteboardToolType
{
	WhiteboardTool_Color				UMETA(DisplayName = "Color"),
	WhiteboardTool_Strength				UMETA(DisplayName = "Strength"),
	WhiteboardTool_Size					UMETA(DisplayName = "Size")

};

UCLASS()
class BETAVERSE_API UWhiteboardToolButton : public UUserWidget
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		TEnumAsByte<EWhiteboardToolType> ToolType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FLinearColor Color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		float Strength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		float Size;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UMaterial* ColorMat;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnWhiteboardButtonPressed OnWhiteboardButtonPressed;

private:
	UPROPERTY()
		int ButtonIndex;

public:
	UFUNCTION(BlueprintCallable)
		void WhiteboardButtonClicked() { OnWhiteboardButtonPressed.Broadcast(ButtonIndex); };

	/**
	* Return the whiteboard brush setting for this button
	* 
	* @param Value The setting parameter (For Strength and Size the value is inside the R channel)
	* @return EWhiteboardToolType The Setting type
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		TEnumAsByte<EWhiteboardToolType> GetButtonSetting(FLinearColor& Value);

	UFUNCTION(BlueprintCallable)
		void SetButtonIndex(int Index) { ButtonIndex = Index; };
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetButton(int Index) { return ButtonIndex; };

};
