#pragma once

struct FCharacterInteractionParameters
{
	UAnimMontage* InteractionMontage = nullptr;
	int CharacterInteractionOptionIndex = 0;

	bool IsValid() const
	{
		return InteractionMontage != nullptr;
	}
};