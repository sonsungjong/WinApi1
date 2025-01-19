#ifndef _STD2D
#define _STD2D

cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
};

SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

Texture2D g_tex : register(t0);

struct VS_IN
{
	float3 vPos : POSITION;
	float4 vColor : COLOR;
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float4 vColor : COLOR;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_Std2D(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.0f;
	
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);			// 행렬을 곱할 때 3차원 좌표를 4차원으로 확장 (동차좌표)
    float4 vViewPos = mul(vWorldPos, g_matView);							// 카메라 원점인 뷰스페이스
    float4 vProjPos = mul(vViewPos, g_matProj);								// 투영행렬 반영
	
    output.vPosition = vProjPos;
	output.vColor = _in.vColor;
	output.vUV = _in.vUV;

	return output;
}

float4 PS_Std2D(VS_OUT _in) : SV_Target
{
	float4 vColor = g_tex.Sample(g_sam_0, _in.vUV);

	return vColor;
}

#endif