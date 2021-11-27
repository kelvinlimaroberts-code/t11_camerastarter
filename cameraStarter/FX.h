#ifndef FX_H
#define FX_H
 
#include <string>
#include <d3d11.h>
 
/*we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a pixel shader
* d3dDevice - IN the gpu device to hold this pixel shader
* pBuff - IN compiled program code
* buffSz - IN how big the program code is
* pPS - OUT a pointer reference to a D3D pixel shader object
*/ 
void CreatePixelShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS);
/*we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a vertex shader
* d3ddevice 
* pBuff - IN compiled program code
* buffSz - IN how big the program code is
* pVS - OUT a pointer reference to a D3D vertex shader object
*/
void CreateVertexShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS);
/*the input assembler needs to know what is in the vertex buffer, how to get the data outand which vertex shader to give it to
* d3dDevice
* vdesc - IN a vertex description
* numElements - IN how many items are in the vertex description
* pBuff - IN compiled program code this input layout matches (vertex shader)
* buffSz - IN how big the program code is
* pLayout - OUT pointer reference to a D3D input layout object
*/
void CreateInputLayout(ID3D11Device& d3dDevice, const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout);
/*
* A constant buffer object can pass constants to a gpu
* d3dDevice
* sizeOfBuffer - IN usually this is the size of some struct containing constants to shift to the GPU
* pBuffer - OUT a d3d object that can hold the constant data we will eventually be passing over
*/
void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer);

//any PC within the last 5+ years should be shader model 5 compatible, if not then it's too old
void CheckShaderModel5Supported(ID3D11Device& d3dDevice);

/*
* Read data from a file, usually used to load in compiled shader code
* fileName - IN file to load
* bytesRead - OUT how much data we got
*/
char* ReadAndAllocate(const std::string& fileName, unsigned int& bytesRead);


#endif
