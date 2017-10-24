#include "Tools.h"
using namespace DirectX;



void SE::Utilz::Tools::GetLocalMatrix(const DirectX::XMMATRIX & worldM, const DirectX::XMMATRIX & invViewM, DirectX::XMMATRIX & out)
{
	auto invWorl = XMMatrixInverse(&XMMatrixDeterminant(worldM), worldM);
	out =  XMMatrixMultiply(invViewM, invWorl);
}

void SE::Utilz::Tools::RayToView(const Rectr & rect, float aspect, DirectX::XMVECTOR & out)
{
	float x = ((2.0f * rect.sx / rect.width) - 1.0f) / (1.f / ((rect.width / rect.height)*tan(aspect / 2.f)));
	float y = ((-2.0f * rect.sy / rect.height) + 1.0f) / (1.f / tan(aspect / 2.f));

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
