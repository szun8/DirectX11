
struct VS_INPUT
{	// 처음 vertex shader에 들어오는 정보의 구조
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// IA -> VS -> RS(보간) -> PS -> OM
// vertex shader, 정점 단위로 아래 함수가 실행됨
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.color = input.color;
    return output;
}

// P(ixel)S의 결과를 마지막으로 SV_Target에 써줘야해서 
float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.color;
}