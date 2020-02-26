#include "D3DUtil.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

static float gTime = 0;

float GetClock()
{
	return gTime;
}

void AddSecToClock(float sec)
{
	gTime += sec;
}

void CreateProjectionMatrix(Matrix& projM, float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
	projM = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
}

void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const DirectX::SimpleMath::Vector3& camPos, const DirectX::SimpleMath::Vector3& camTgt, const DirectX::SimpleMath::Vector3& camUp)
{
	viewM = XMMatrixLookAtLH(camPos, camTgt, camUp);
}

void CreateVertexBuffer(ID3D11Device& d3dDevice, UINT bufferSize, void *pSourceData, ID3D11Buffer* &pVB)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = bufferSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vinitData.pSysMem = pSourceData;
	HR(d3dDevice.CreateBuffer(&vbd, &vinitData, &pVB));
}

void CreateIndexBuffer(ID3D11Device& d3dDevice, UINT bufferSize, void *pSourceData, ID3D11Buffer* &pIB)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = bufferSize;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pSourceData;
	HR(d3dDevice.CreateBuffer(&ibd, &iinitData, &pIB));
}


