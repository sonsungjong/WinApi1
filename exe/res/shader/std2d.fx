#ifndef _STD2D
#define _STD2D

// 쉐이더 파일 우클릭 후 HLSL 컴파일러에서 셰이더 형식을 효과(/fx) 로 변경
// 쉐이더 파일 우클릭 후 HLSL 컴파일러에서 Shader Model 5.0(/5_0) 로 변경
#include "value.fx"

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
    float4 vColor = (float4) 0.f;
	
	if(g_int_0 == 0)
    {
        vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
	else if(g_vec4_3.y == 3.14f)
    {
        vColor = float4(1.f, 1.f, 0.f, 1.f);				// 노란색
    }
    else if (g_mat_1[0][0] == 2.1f)
    {
        vColor = float4(0.f, 1.f, 1.f, 1.f);				// 청록색
    }

	return vColor;
}

#endif