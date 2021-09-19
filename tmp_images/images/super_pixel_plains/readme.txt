///////////////////////////
//HOW TO USE THIS TILESET//
///////////////////////////

Hello, fellow game developer! Thank you for downloading the Super Pixel Plains tileset. Here are a few pointers to help you navigate and make sense of this zip file.
This is version 1.1 of Super Pixel Plains. A changelog is located at the bottom of this file.

Tips:

- You can find all the pieces to put together your levels in the "pieces" folder. There are three different styles.

- I've also included sprite sheets for each style in the "spritesheet" folder, along with two metadata files. (Pick the format you like.)

- Many of the pieces are set up to align to an 8x8 grid. Remember this when you're putting it together! (You can use a 16x16 grid for just the terrain, and the clouds don't have to be aligned to anything.)

- The layer order from front to back is: terrain, midground, bg1, bg2, clouds, bg3.

- All tileset pieces are descriptively named with easily-understandable shorthand. You can learn the naming pattern using the following guide, which applies to all my tilesets.

File name definitions:
	- "terrain"			Part of the tileset's terrain. The player probably stands on this.
	- "left/right/top/bottom"	The left/right/top/bottom wall or edge of something. For example, "terrain_top_left" would be the top-left corner of the terrain.
	- "center"			Context-specific. Usually means the middle of something.
	- "fill"			Indicates an object used as "filler," i.e. to fill an area. It should never be on the outside. Sometimes interchangeable with "center."
	- "inner_corner"		The inner corner of terrain or other object. For example, to transition from a platform to a wall, you need an inner corner piece.
	- "bg1", "bg2", etc.		Background. Higher number indicates further back.
	- "fg1", "fg2", etc.		Foreground. Higher number indicates further forwards.
	- "_f1", "_f2", "_f3", etc.	Suffixes with "f" and a number indicate a frame of an animation.
	- "_1", "_2", "_3", etc.	A suffix with a number indicates there are multiple variants of this object.
	- "A1", "B1", etc.		"A," "B," "C," and so on also indicate different patterns, styles, or objects.
					The number following the letter indicates a variant.

- Don't know how to put something together? Take a look at the example/wallpaper images and they might be able to help you.

- Really curious how it was all made? Need some hints for layer ordering? Check out the PSDs or the .pyxel files.

Got questions? Hit me up at:
@untiedgames (twitter)
contact@untiedgames.com (email)

Thanks again!
-Will

/////////////
//CHANGELOG//
/////////////

Version 1.1
	- Removed some weird-looking grass backlighting on the terrain_top_left and terrain_platform_left pieces.
	- Added missing terrain fill elements that I forgot to export:
		- terrain_fill_corner_top_left
		- terrain_fill_corner_top_right
		- terrain_fill_corner_bottom_left
		- terrain_fill_corner_bottom_right
		- terrain_fill_hole_A
		- terrain_fill_hole_B
	- Fixed file name typos in spritesheet/style_A. ("syper" -> "super")
	- Updated spritesheets and wallpaper images.

Version 1.0
	- Initial release.