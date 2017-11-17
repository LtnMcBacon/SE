static const float fogSize = 1;

static const float fogPlaneSize = 25 + 2 * 1;


Texture2D fogTexture : register(t0);

SamplerState samplerState : register(s0);


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    return float4(fogTexture.Sample(samplerState, (fogPlaneSize / fogSize) * input.uv));
}
