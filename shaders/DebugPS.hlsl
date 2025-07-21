struct Light {
    int type;
    float3 position;
    float range;
    float3 direction;
    float3 color;
    float intensity;
};

StructuredBuffer<Light> lights : register(t0);
Texture2D tex : register(t1);
SamplerState samp : register(s0);

cbuffer MaterialData : register(b0)
{
    float3 diffuseColor;
}

float4 main(float4 position : SV_POSITION, float3 worldNormal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET {

    return float4(diffuseColor.rgb, 1.0);
}