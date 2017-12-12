static const float CYCLE_DURATION = 12;
static const float FADE_DURATION = 1;


static const float FOG_SPEED = 2.5 / CYCLE_DURATION;
static const float2 ROOM_CENTER = float2(12.5, 12.5);


cbuffer fog_timeBuffer : register(b0)
{
    float time;
};


struct GS_IN
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


[maxvertexcount(6)]
void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output;


    float clock = fmod(time, CYCLE_DURATION);


    float swapMultiplier = min(1, clock / FADE_DURATION);

    float swapMultiplier_mirrored = 1 - (swapMultiplier * swapMultiplier * swapMultiplier);

    swapMultiplier = 1 - swapMultiplier;
    swapMultiplier = 1 - (swapMultiplier * swapMultiplier * swapMultiplier);


    uint planeCount = 1 + (clock < FADE_DURATION);

    for (uint planeI = 0; planeI < planeCount; ++planeI)
    {
        for (uint vertexI = 0; vertexI < 3; ++vertexI)
        {
            float2 uvOffset = input[vertexI].uv - ROOM_CENTER;
            float2 uvOffset_signs = float2(sign(uvOffset.x), sign(uvOffset.y));

            uvOffset = normalize(uvOffset);
            uvOffset *= uvOffset;
            uvOffset *= uvOffset_signs;

            uvOffset *= FOG_SPEED *
                        (!planeI * clock +
                        planeI * (clock + CYCLE_DURATION));


            output.position = input[vertexI].position;

            output.uv = input[vertexI].uv + uvOffset;

            output.opacity = input[vertexI].opacity *
                             (!planeI * swapMultiplier +
                             planeI * swapMultiplier_mirrored);


            outputStream.Append(output);
        }


        outputStream.RestartStrip();
    }
}
