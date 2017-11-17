cbuffer OncePerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
};


struct VS_IN
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


VS_OUT VS_main(VS_IN input)
{
    VS_OUT output;

    output.position = mul(float4(input.position, 1), viewProjectionMatrix);
    output.uv = input.uv;
    output.opacity = input.opacity;

    return output;
}
