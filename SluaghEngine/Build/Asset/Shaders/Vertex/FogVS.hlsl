static const float mapPadding = 5.f;
static const float planeHeight = 2.f;


static const float2 mapCenter = { 12.5f, 12.5f };
static const float planeWidth = mapCenter.x + mapPadding;


static const float3 vertexPositions[6] =
{
    float3(mapCenter.x - planeWidth, planeHeight, mapCenter.y + planeWidth),
    float3(mapCenter.x + planeWidth, planeHeight, mapCenter.y + planeWidth),
    float3(mapCenter.x + planeWidth, planeHeight, mapCenter.y - planeWidth),
    float3(mapCenter.x - planeWidth, planeHeight, mapCenter.y + planeWidth),
    float3(mapCenter.x + planeWidth, planeHeight, mapCenter.y - planeWidth),
    float3(mapCenter.x - planeWidth, planeHeight, mapCenter.y - planeWidth)
};

static const float2 vertexUvs[6] =
{
    float2(0.f, 1.f),
    float2(1.f, 1.f),
    float2(1.f, 0.f),
    float2(0.f, 1.f),
    float2(1.f, 0.f),
    float2(0.f, 0.f)
};


cbuffer OncePerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


VS_OUT VS_main(uint vertexIndex : SV_VertexID)
{
    VS_OUT output;

    output.position = mul(float4(vertexPositions[vertexIndex], 1), viewProjectionMatrix);
    output.uv = vertexUvs[vertexIndex];

    return output;
}
