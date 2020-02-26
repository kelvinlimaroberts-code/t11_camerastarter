#ifndef FX_H
#define FX_H

#include <string>
#include <d3d11.h>

//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a pixel shader
void CreatePixelShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS);
//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a vertex shader
void CreateVertexShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS);
//the input assembler needs to know what is in the vertex buffer, how to get the data out and which vertex shader to give it to
void CreateInputLayout(ID3D11Device& d3dDevice, const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout);

void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer);
void CheckShaderModel5Supported(ID3D11Device& d3dDevice);

char* ReadAndAllocate(const std::string& fileName, unsigned int& bytesRead);


#endif
