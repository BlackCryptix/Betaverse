// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WhiteboardToolButton.h"
#include "Character/CharacterMaster.h"


void UWhiteboardToolButton::NativeConstruct()
{
	Super::NativeConstruct();

}

TEnumAsByte<EWhiteboardToolType> UWhiteboardToolButton::GetButtonSetting(FLinearColor& Value)
{
	switch (ToolType)
	{
	case WhiteboardTool_Color:
		Value = Color;
		break;
	case WhiteboardTool_Size:
		Value.R = Size;
		break;
	case WhiteboardTool_Strength:
		Value.R = Strength;
		break;
	}
	return ToolType;
}


