static const float FOG_SPEED = 0.3;


cbuffer OncePerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
};


cbuffer fog_timeBuffer : register(b1)
{
    float time;
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

    output.uv[0] = input.uv[0];
    output.uv[1] = input.uv[1] + FOG_SPEED * time;

    output.opacity = input.opacity;


    return output;
}
