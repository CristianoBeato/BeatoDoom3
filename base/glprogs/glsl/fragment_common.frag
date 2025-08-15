/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2025 Cristiano B. Santos (https://github.com/CristianoBeato/BeatoDoom3)

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#version 460 core

//######################################################
//
//  FRAGMENT SHADER UTILS
//
//######################################################

// we can acess the max of 8 textures from the sampler buffer 
struct samplersLocation
{
    uint    samp0;  // texture sampler index 0
    uint    samp1;  // texture sampler index 1
    uint    samp2;  // texture sampler index 2
    uint    samp3;  // texture sampler index 3
    uint    samp4;  // texture sampler index 4
    uint    samp5;  // texture sampler index 5
    uint    samp6;  // texture sampler index 6
    uint    samp7;  // texture sampler index 7
};

// fragment shader storage structure 
struct fragmentTransfom
{
    vec4    rpScreenCorrectionFactor;
    vec4    rpWindowCoord;

    vec4    rpOverbright;   // the max overbright
    vec4    rpAlphaTest;    // alpha test enabled 
};

vec3 toOpenGlCorrdinates( vec3 v )
{
	return vec3( -v.y, v.z, -v.x );
}
