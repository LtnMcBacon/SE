#include "Tools.h"

DirectX::XMMATRIX SE::Core::Tools::getLocalMatrix(XMMATRIX worldM, XMMATRIX viewM)
{
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(viewM), viewM);
	XMMATRIX invWorl = XMMatrixInverse(&XMMatrixDeterminant(worldM), worldM);
	return XMMatrixMultiply(invView, invWorl);
}

DirectX::XMFLOAT3 SE::Core::Tools::rayToWorld(int sx, int sy, float height, float width)
{
	float aro = width / height;
	float x = aro * ((2 * sx / width) - 1);
	float y = -((2 * sy / height) + 1);

	return XMFLOAT3(x, y, 1.f);
}
DirectX::XMVECTOR SE::Core::Tools::getLocalRayDir(XMVECTOR rayO, XMVECTOR rayD, XMMATRIX localM)
{
	rayO = XMVector3Transform(rayO, localM);
	rayD = XMVector3TransformNormal(rayD, localM);

	return XMVector3Normalize(rayD);
}
