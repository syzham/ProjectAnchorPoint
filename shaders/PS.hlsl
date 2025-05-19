cbuffer MaterialData : register(b0)
{
    float4 diffuseColor;
}

float4 main(float4 position : SV_POSITION) : SV_TARGET {
    return diffuseColor;
}