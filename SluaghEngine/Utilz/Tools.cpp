#include "Tools.h"

DirectX::XMMATRIX SE::Tools::Tools::getLocalMatrix(XMMATRIX worldM, XMMATRIX invViewM)
{
	XMMATRIX invWorl = XMMatrixInverse(&XMMatrixDeterminant(worldM), worldM);
	return XMMatrixMultiply(invViewM, invWorl);
}

DirectX::XMVECTOR SE::Tools::Tools::rayToView(int sx, int sy, float width, float height)
{
	float x = ((2.0f * sx / width) - 1.0f) / (1.f / ((width / height)*tan(1.570796f / 2.f)));
	float y = ((-2.0f * sy / height) + 1.0f) / (1.f / tan(1.570796f/2.f));
	XMVECTOR ray = { x, y, 1.0f, 0.0f};
	return ray;
}
DirectX::XMVECTOR SE::Tools::Tools::getLocalRayDir(XMVECTOR rayD, XMMATRIX localM)
{
	rayD = XMVector3TransformNormal(rayD, localM);

	return XMVector3Normalize(rayD);
}
DirectX::XMVECTOR SE::Tools::Tools::getLocalRayOrigin(XMVECTOR rayO, XMMATRIX localM)
{
	return XMVector3TransformCoord(rayO, localM);
}
