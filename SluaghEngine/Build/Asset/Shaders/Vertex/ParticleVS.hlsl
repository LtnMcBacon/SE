struct VS_IN {
	float3 pos : POSITION;
	float pad : PAD1;
	float3 velocity : VELOCITY;
	float pad2 : PAD2;
	float3 startEmitPos : ORIGIN;
	float pad5 : PAD5;
	float size : SIZE;
	float pad4 : PAD4;
	float opacity : OPACITY;
	float age : AGE;
	uint type : TYPE;
	uint bloom : BLOOM;
};
struct VS_OUT {
	float3 pos : POSITION;
	float pad : PAD1;
	float3 velocity : VELOCITY;
	float pad2 : PAD2;
	float3 startEmitPos : ORIGIN;
	float pad5 : PAD5;
	float size : SIZE;
	float pad4 : PAD4;
	float opacity : OPACITY;
	float age : AGE;
	uint type : TYPE;
	uint bloom : BLOOM;
};
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.velocity = input.velocity;
	output.size = input.size;
	output.opacity = input.opacity;
	output.startEmitPos = input.startEmitPos;
	output.age = input.age;
	output.type = input.type;
	output.pos = input.pos;
	output.bloom = input.bloom;
	
	return output;
}