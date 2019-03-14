#pragma once

/*
    Represents possible types of entities for differentiating the base class.
*/
enum eEntityType
{
    ENTITY_DIRECTIONAL_LIGHT,
    ENTITY_SPOT_LIGHT,
    ENTITY_POINT_LIGHT,
    ENTITY_PLANE,
    ENTITY_INTERACTABLE,
    ENTITY_STATIC,
    ENTITY_PICKUP,
    ENTITY_HOVERCRAFT,
    ENTITY_CAMERA
};

/*
    Represents different Interactable Entities
*/
enum eInteractType
{
    INTER_FLAME_TRAIL = 0,
    INTER_SPIKES,
    INTER_ROCKET,
    INTER_POWERUP
};
