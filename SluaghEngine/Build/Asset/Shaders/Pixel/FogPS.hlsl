static const float fogSize = 1;


Texture2D fogTexture : register(t0);

SamplerState samplerState : register(s0);


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    return float4(fogTexture.Sample(samplerState, (25 / fogSize) * input.uv));
}
