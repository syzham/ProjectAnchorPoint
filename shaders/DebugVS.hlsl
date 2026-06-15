cbuffer ObjectData : register(b1)
{
    row_major float4x4 worldViewProj;
    row_major float4x4 world;
}

struct VSInput {
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

// Debug line vertices are already in world space, so worldViewProj is set to
// the view-projection matrix (model = identity) before drawing the lines.
VSOutput main(VSInput input) {
    VSOutput output;
    output.pos = mul(worldViewProj, float4(input.pos, 1.0f));
    output.color = input.color;
    return output;
}
