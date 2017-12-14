struct VS_IN {
	float3 pos : POSITION;
	float size : SIZE;
	float3 velocity : VELOCITY;
	float opacity : OPACITY;
	float3 startEmitPos : ORIGIN;
	float age : AGE;
	uint type : TYPE;
};
struct VS_OUT {
	float3 pos : POSITION;
	float size : SIZE;
	float3 velocity : VELOCITY;
	float opacity : OPACITY;
	float3 startEmitPos : ORIGIN;
	float age : AGE;
	uint type : TYPE;
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
//	output.bloom = input.bloom;
	
	return output;
}