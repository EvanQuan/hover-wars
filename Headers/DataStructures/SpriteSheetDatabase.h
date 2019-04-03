#pragma once
#include "stdafx.h"

#define FLAME_DURATION 5.0f
// Name: SpriteSheetDatabase
// Written by: James Coté
// Description: Contains a hardcoded database of Sprite information that can be used
//  for sprite animation.
namespace SpriteSheetDatabase
{
    // Sprite Sheet Information
    struct sSpriteSheetInfo
    {
        string sSheetLocation;
        vec2 vUVSpriteSize, vUVBorderSize;
        int iNumSpritesX, iNumSpritesY;
        float fDuration;
    };

    // Database
    const vector<sSpriteSheetInfo> vSpriteInformation{
        { "textures/matty-wyett-simmonds-flamevfxlooping.jpg",  // FIRE_SPRITE
           vec2(0.125f, 0.125f), vec2(0.005f, 0.0f), 8, 8, FLAME_DURATION },
        { "textures/Smoke15Frames.png",                         // SMOKE_SPRITE
           vec2(0.2f, 0.3333333f), vec2(0.0f), 5, 3, 15.0f }
    };

    // Enum to index and get necessary Sprite information.
    enum eSpriteEnum
    {
        FIRE_SPRITE = 0,
        SMOKE_SPRITE,
        MAX_SPRITES
    };
}
