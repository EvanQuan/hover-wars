#pragma once
#include "stdafx.h"

// Name: ObjectInfo
// Written by: James Coté
// Description: Universal Structure of information loaded from Scene Loader containing:
//              * Material Information
//              * Bounding Box Information
//              * Position of Object
struct ObjectInfo
{
    /* Material Structure */
    struct Material
    {
        string sDiffuseMap;
        vec4 vOptionalDiffuseColor;
        string sOptionalSpecMap;
        vec4 vOptionalSpecShade;
        float fShininess;
    } sObjMaterial;

    /* Bounding Box Structure */
    struct BoundingBox
    {
        eBoundingBoxTypes eType;
        vec3 vDimensions;
    } sObjBoundingBox;

    /* Position of Object */
    vec3 vPosition;
};
