// Scene_MapTick.cpp
// Per-frame map/tile update and walk-mode tile dispatch.
//
// Scene_MapTick @ 0x004f64d0 — Scene_MapTick
//
// Manages the map-tile hash table for the current player position.
// Uses HashTable at MAIN_HASH_CLASS and player position at CharacterMachinePtr.
// Dispatches to RenderItemInfo (walk map) or RenderRepairInfo (alt map)
// based on boundary conditions.

#include "stdafx.h"


// Scene_MapTick — Scene_MapTick
// Updates the tile hash table entry for the local player's tile position.
// If the player has moved outside the current map tile bounds, reloads the map:
//   - Checks bounds: x in [InventoryStartY..0x27f], y in [InventoryStartX..0x1df]
//   - Calls RenderItemInfo or RenderRepairInfo to reload walk/alt map
// Also handles HoveredSkillIndex as a pending map-change request.
void Scene_MapTick(void)
{
  uint uVar2;
  void *pvVar3;
  undefined4 *puVar5;

  GL_ResetState();
  puVar5 = (undefined4*)CharacterMachinePtr;
  HudTextVisibleFlag = 0;
  // IDA Scene_MapTick L32-38: RepairEnable_0 sólo se NORMALIZA a 1; nunca se
  // apaga acá (RepairEnable, 0x07EAA138, vale 0 siempre que la tienda está
  // abierta, y apagarlo cortaría el modo reparación del herrero).
  if (RepairEnableFlag != 0) {
    RepairEnableFlag = 1;
  }
  if (CheckInventoryPtr == 0) {
    if ((((ItemActionMode == 1) && (InvCursorBoundXMin <= (float)MouseX)) &&
        ((float)MouseX < InvCursorBoundXMax)) &&
       ((115.0f <= (float)MouseY && ((float)MouseY < InvCursorBoundYMax)))) {
      uVar2 = HashTable_GetIndex(&MAIN_HASH_CLASS,CharacterMachinePtr);
      if (uVar2 == 0xffffffff) {
        pvVar3 = AntiTamper_HashNode();
        static_cast<BYTE*>(pvVar3)[0x584] = 1;
        HashTable_Insert(&MAIN_HASH_CLASS,pvVar3,puVar5);
      }
      RenderItemInfo((void*)0x109,(void*)0x78,PickedItemData,'\0');
      puVar5 = (undefined4*)CharacterMachinePtr;
    }
    goto LAB_004f6824;
  }
  if ((((int)MouseX < (int)InventoryStartX) || ((int)MouseX >= gWindow.PanelOrigin(Ui::WindowRight))) ||
     ((MouseY < InventoryStartY || ((0x1df < MouseY || (RepairEnableFlag == 0)))))) {
    uVar2 = HashTable_GetIndex(&MAIN_HASH_CLASS,CharacterMachinePtr);
    if (uVar2 == 0xffffffff) {
      pvVar3 = AntiTamper_HashNode();
      static_cast<BYTE*>(pvVar3)[0x584] = 1;
      HashTable_Insert(&MAIN_HASH_CLASS,pvVar3,puVar5);
    }
    RenderItemInfo((void*)(uintptr_t)TooltipPosX,(void*)(uintptr_t)TooltipPosY,(void*)(uintptr_t)CheckInventoryPtr,(int)ItemRenderMode);
    puVar5 = (undefined4*)CharacterMachinePtr;
  }
  else {
    uVar2 = HashTable_GetIndex(&MAIN_HASH_CLASS,CharacterMachinePtr);
    if (uVar2 == 0xffffffff) {
      pvVar3 = AntiTamper_HashNode();
      static_cast<BYTE*>(pvVar3)[0x584] = 1;
      HashTable_Insert(&MAIN_HASH_CLASS,pvVar3,puVar5);
    }
    RenderRepairInfo((void*)(uintptr_t)TooltipPosX,(int)TooltipPosY,(void*)(uintptr_t)CheckInventoryPtr);
    puVar5 = (undefined4*)CharacterMachinePtr;
  }
LAB_004f6824:
  // Clamp de HoveredSkillIndex al rango de slots de skill (0..19) antes de llamar al
  // tooltip: un valor basura haría lecturas fuera de rango.
  if ((int)HoveredSkillIndex >= 0 && (int)HoveredSkillIndex < 20) {
    // Skill tooltip render. Args resolved from IDA: (float Y, int X, int hoveredSkillIdx).
    // Ghidra mis-decoded the 3rd arg as a stack write; it is the real 3rd param.
    RenderSkillTooltip(*(float*)&TooltipPosX, (int)TooltipPosY, (int)HoveredSkillIndex);
  }
  return;
}
