#include <iostream>
#include <fstream>

#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"

using namespace std;



void CheckShaderModel5Supported(ID3D11Device& d3dDevice)
{
	D3D_FEATURE_LEVEL featureLevel = d3dDevice.GetFeatureLevel();
	bool featureLevelOK = true;
	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
		break;
	default:
		featureLevelOK = false;
	}

	if (!featureLevelOK)
	{
		DBOUT("feature level too low for shader model 5");
		assert(false);
	}
}

void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer)
{
	// Create the constant buffers for the variables defined in the vertex shader.
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeOfBuffer;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	HR(d3dDevice.CreateBuffer(&constantBufferDesc, nullptr, pBuffer));

}

char* ReadAndAllocate(const string& fileName, unsigned int& bytesRead)
{
	//open the file for reading
	ifstream infile;
	infile.open(fileName, ios::binary | ios::in);
	if (!infile.is_open() || infile.fail())
	{
		DBOUT("failed to open file: " << fileName);
		assert(false);
		return nullptr;
	}

	//read the whole contents
	infile.seekg(0, ios::end);
	streampos size = infile.tellg();
	if (size > INT_MAX || size <= 0)
	{
		DBOUT("failed to get size of file: " << fileName);
		assert(false);
	}
	char* pBuff = new char[(int)size];
	infile.seekg(0, ios::beg);
	infile.read(pBuff, size);
	if (infile.fail())
	{
		DBOUT("failed to read file: " << fileName);
		assert(false);
	}
	infile.close();
	bytesRead = (int)size;
	return pBuff;
}


void CreateInputLayout(ID3D11Device& d3dDevice,const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout)
{
	assert(pBuff);
	HR(d3dDevice.CreateInputLayout(vdesc, numElements, pBuff, buffSz, pLayout));

}

void CreateVertexShader(ID3D11Device& d3dDevice,char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS)
{
	assert(pBuff);
	HR(d3dDevice.CreateVertexShader(pBuff,
		buffSz,
		nullptr,
		&pVS));
	assert(pVS);
}

void CreatePixelShader(ID3D11Device& d3dDevice,char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS)
{
	assert(pBuff);
	HR(d3dDevice.CreatePixelShader(pBuff,
		buffSz,
		nullptr,
		&pPS));
	assert(pPS);
}

