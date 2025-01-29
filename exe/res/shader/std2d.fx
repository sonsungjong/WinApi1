#ifndef _STD2D
#define _STD2D

// ���̴� ���� ��Ŭ�� �� HLSL �����Ϸ����� ���̴� ������ ȿ��(/fx) �� ����
// ���̴� ���� ��Ŭ�� �� HLSL �����Ϸ����� Shader Model 5.0(/5_0) �� ����
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
	
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);			// ����� ���� �� 3���� ��ǥ�� 4�������� Ȯ�� (������ǥ)
    float4 vViewPos = mul(vWorldPos, g_matView);							// ī�޶� ������ �佺���̽�
    float4 vProjPos = mul(vViewPos, g_matProj);								// ������� �ݿ�
	
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
        vColor = float4(1.f, 1.f, 0.f, 1.f);				// �����
    }
    else if (g_mat_1[0][0] == 2.1f)
    {
        vColor = float4(0.f, 1.f, 1.f, 1.f);				// û�ϻ�
    }

	return vColor;
}

#endif