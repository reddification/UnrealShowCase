#pragma once

struct FPlayerNpcInteractionData
{

	void Reset()
	{
		Npc = nullptr;
	}

	bool IsInteracting() const
	{
		return Npc != nullptr;		
	}
	
	UPROPERTY()
	class ABaseCharacter* Npc;
};