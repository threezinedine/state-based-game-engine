#pragma once

/**
 * Module contains the simple APIs for drawing the graphics on the screen.
 * The module contains several sub-modules:
 *      - Graphic Interface (Low level renderer): Drawing the object based
 *          on some information like position, size, color, etc.
 *
 *      - Scene Graph: Managing the hierarchy of objects in the scene.
 *          It will consider which object should be drawn first, which object
 *          should be drawn later, irgnore some objects, etc.
 *
 *      - Front end: Contains some layers which are above the Scene Layer such as:
 *          + Full Motion Video (FMV) Layer: Drawing the pre-recorded video on the screen.
 *          + In-Game Menu: The menu of the game which can have some game settings.
 *          + In-Game GUI: Where the user can interact with game characters, objects, etc.
 *
 */
namespace ntt::renderer
{

} // namespace ntt::renderer

#include "Geometry.hpp"
#include "TextureComponent.hpp"
#include "Sprite.hpp"
#include "Hovering.hpp"
#include "Text.hpp"