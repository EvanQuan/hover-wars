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

    // Default Constructor for the Object Info Structure
    //  Loads the default settings for the object info structure.
    ObjectInfo()
    {
        loadDefaults();
    }

    // Name: loadDefaults
    // Written by: James Cote
    // Description: Load the Defaults for the Object Info;
    void loadDefaults()
    {
        // Clear Material Properties
        sObjMaterial.fShininess = 1.0f;
        sObjMaterial.sDiffuseMap = sObjMaterial.sOptionalSpecMap = "";
        sObjMaterial.vOptionalDiffuseColor = vec4(0.0f);
        sObjMaterial.vOptionalSpecShade = vec4(0.0f);

        // Clear Bounding Box Properties
        sObjBoundingBox.eType = DEFAULT_TYPE;
        sObjBoundingBox.vDimensions = vec3(0.0f);

        // Reset Position
        vPosition = vec3(0.0f);
    }

    // Assignment Operator overload for Copying
    ObjectInfo& operator=(const ObjectInfo& pCopy)
    {
        vPosition       = pCopy.vPosition;
        sObjBoundingBox = pCopy.sObjBoundingBox;
        sObjMaterial    = pCopy.sObjMaterial;

        return *this;
    }
};
