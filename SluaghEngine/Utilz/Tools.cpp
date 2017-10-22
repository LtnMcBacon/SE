#include "Tools.h"
using namespace DirectX;



void SE::Utilz::Tools::GetLocalMatrix(const DirectX::XMMATRIX & worldM, const DirectX::XMMATRIX & invViewM, DirectX::XMMATRIX & out)
{
	auto invWorl = XMMatrixInverse(&XMMatrixDeterminant(worldM), worldM);
	out =  XMMatrixMultiply(invViewM, invWorl);
}

void SE::Utilz::Tools::RayToView(int sx, int sy, float height, float width, DirectX::XMVECTOR & out)
{
	float x = ((2.0f * sx / width) - 1.0f) / (1.f / ((width / height)*tan(1.570796f / 2.f)));
	float y = ((-2.0f * sy / height) + 1.0f) / (1.f / tan(1.570796f / 2.f));

	out = { x, y, 1.0f, 0.0f };
}

void SE::Utilz::Tools::GetLocalRayDir(const DirectX::XMVECTOR & rayD, const DirectX::XMMATRIX & localM, DirectX::XMVECTOR & out)
{
	auto rayDD = XMVector3TransformNormal(rayD, localM);

	out = XMVector3Normalize(rayDD);
}

void SE::Utilz::Tools::GetLocalRayOrigin(const DirectX::XMVECTOR & rayD, const DirectX::XMMATRIX & localM, DirectX::XMVECTOR & out)
{
	out = XMVector3TransformCoord(rayD, localM);
}
