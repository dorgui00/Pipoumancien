// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WBPSlpte.h"
#include "Components/Image.h"

void UWBPSlote::ChoitMusique(E_Musique Musique)
{
	switch (Musique)
	{
	case E_Musique::Note1:
		if (Image_012 && Texture1)
			Image_012->SetBrushFromTexture(Texture1, true);
		break;

	case E_Musique::Note2:
		if (Image_012 && Texture2)
			Image_012->SetBrushFromTexture(Texture2, true);
		break;

	case E_Musique::Note3:
		if (Image_012 && Texture3)
			Image_012->SetBrushFromTexture(Texture3, true);
		break;

	case E_Musique::Note4:
		if (Image_012 && Texture4)
			Image_012->SetBrushFromTexture(Texture4, true);
		break;

	default:
		break;
	}
}