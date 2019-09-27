#pragma once
#include <Windows.h>
#include <iostream>

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_internal.h"

#include "Offsets.h"
#include "Sdk.h"
#include "Renderer.h"


void createPlayerTreeNode(Player* player)
{
	if (ImGui::TreeNode((char*)player->pName))
	{
		ImGui::Text("Player: %x", player);
		ImGui::Text("Wood: %.f", player->Ressources->wood);
		ImGui::Text("Food: %.f", player->Ressources->food);
		ImGui::Text("Gold: %.f", player->Ressources->gold);
		ImGui::Text("Stone: %.f", player->Ressources->stone);
		ImGui::Text("Villigers: %.f", player->Ressources->villagerCount);
		ImGui::Text("Pop: %.f/%.f", player->Ressources->CurrentPop, player->Ressources->CurrentPop + player->Ressources->popSpaceLeft);

		if (ImGui::TreeNode("Units"))
		{
			int buildingCount = 0;
			int infantryCount = 0;
			int calavaryCount = 0;
			for (int i = 0; i < player->objectManager->iObjectCount; i++)
			{
				Unit* unit = player->objectManager->untis[i];
				if (!unit) { continue; }

				if (unit->pOwner == player)
				{
					ImGui::Text("%x", unit);
					ImGui::SameLine();
					ImGui::Text("%s", unit->pUnitData->name);
		
					if (unit->pUnitData->Class == EnumUnitDataClass::Building)
					{
						buildingCount++;
					}
					if (unit->pUnitData->Class == EnumUnitDataClass::Infantry)
					{
						infantryCount++;
					}
					if (unit->pUnitData->Class == EnumUnitDataClass::Cavalry)
					{
						calavaryCount++;
					}
				}
			}
			ImGui::Text("Buildings %.d", buildingCount);
			ImGui::Text("Infantry %.d", infantryCount);
			ImGui::Text("Cavalry %.d", calavaryCount);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void DrawBox(Vector3 unitPos)
{
	Vector3 one3 = unitPos;
	one3.x -= 1;
	one3.z -= 1;
	Vector2 one = worldToScreen(one3);

	Vector3 two3 = unitPos;
	two3.x += 1;
	two3.z += 1;
	Vector2 two = worldToScreen(two3);

	Vector3 three3 = unitPos;
	three3.x -= 1;
	three3.z += 1;
	Vector2 three = worldToScreen(three3);

	Vector3 four3 = unitPos;
	four3.x += 1;
	four3.z -= 1;
	Vector2 four = worldToScreen(four3);
	
	ImVec2 ivOne = ImVec2(one.x, one.y);
	ImVec2 ivTwo = ImVec2(two.x, two.y);
	ImVec2 ivThree = ImVec2(three.x, three.y);
	ImVec2 ivFour = ImVec2(four.x, four.y);

	Renderer::Get()->RenderRect(ivOne, ivFour, ivTwo, ivThree,0xff0000ff);
}

void Esp()
{
	static Main* main = reinterpret_cast<Main*>((DWORD)GetModuleHandle(NULL) + Offsets::main);
	static int totalPlayers = *reinterpret_cast<int*>((DWORD)GetModuleHandle(NULL) + Offsets::totalPlayers);

	static GameData* gameData = main->GameData;
	static PlayerArray* playerArray = gameData->pPlayerArray;

	Renderer::Get()->BeginScene();

	for (int i = 0; i < totalPlayers; i++)
	{
		Player* player = playerArray->playerData[i].player;
		for (int i = 0; i < player->objectManager->iObjectCount; i++)
		{
			Unit* unit = player->objectManager->untis[i];
			if (!unit)
			{
				continue;
			}
			
			Vector3 unitPos = unit->vPos;
			DrawBox(unitPos);
			Vector2 screenPosition = worldToScreen(unit);
			Renderer::Get()->RenderCircle(ImVec2(screenPosition.x, screenPosition.y), 7, 0xffffffff, 1, 20);
			Renderer::Get()->RenderText(unit->pUnitData->name, ImVec2(screenPosition.x, screenPosition.y), 16, 0xffff0000, true);
		}
	}
	Renderer::Get()->EndScene();
}


void RunHack()
{
	static Main* main = reinterpret_cast<Main*>((DWORD)GetModuleHandle(NULL) + Offsets::main);
	static int totalPlayers = *reinterpret_cast<int*>((DWORD)GetModuleHandle(NULL) + Offsets::totalPlayers);
	
	static GameData* gameData = main->GameData;
	static PlayerArray* playerArray = gameData->pPlayerArray;

	static bool openOverlay = true;
	if (GetAsyncKeyState(VK_INSERT) & 1) { openOverlay = !openOverlay; }

	Esp();
	
	ImGui::SetNextWindowBgAlpha(0.35f);
	if (openOverlay)
	{
		if (ImGui::Begin("Age of Empires 2 HD", &openOverlay, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			for (int i = 0; i < totalPlayers; i++)
			{
				
				createPlayerTreeNode(playerArray->playerData[i].player);
			}
			if (ImGui::Button("Maphack"))
			{
				typedef int(__cdecl *tEnableCheats)();
				tEnableCheats oEnableCheats;
				oEnableCheats = (tEnableCheats)((DWORD)GetModuleHandle(NULL) + Offsets::maphack);
				oEnableCheats();
			}
			ImGui::SameLine();
			ImGui::Text("<-- Desync in multiplayer");
		}
		ImGui::End();
	}
}