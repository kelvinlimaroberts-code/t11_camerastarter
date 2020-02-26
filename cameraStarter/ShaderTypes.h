#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include <d3d11.h>

#include "SimpleMath.h"

/*
This is what our vertex data will look like
*/
struct VertexPosColour
{
	DirectX::SimpleMath::Vector3 Pos;
	DirectX::SimpleMath::Vector3 Colour;

	static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[2];
};


struct VertexPos
{
	DirectX::SimpleMath::Vector3 Pos;

	static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[1];
};

/*
This is a constant buffer providing important information
to the shader. It's size must be a multiple of 16bytes (16byte aligned).
*/
struct GfxParamsPerObj
{
	DirectX::SimpleMath::Matrix wvp;
};


static_assert((sizeof(GfxParamsPerObj) % 16) == 0, "CB size not padded correctly");


#endif
