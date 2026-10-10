#pragma once

typedef float float3[3];

enum TopologicalStructure
{
	INTERSECT, NONINTERSECT
};

struct MyTriangle
{
	//float3 Normal_0;
	float3 Vertex_1, Vertex_2, Vertex_3;
	MyTriangle()
	{
		Vertex_1[0]=0;
		Vertex_1[1]=0;
		Vertex_1[2]=0;

		Vertex_2[0]=0;
		Vertex_2[1]=0;
		Vertex_2[2]=0;

		Vertex_3[0]=0;
		Vertex_3[1]=0;
		Vertex_3[2]=0;
	}
};

/*******************************************************************************************************/
//Devillers算法主函数  
TopologicalStructure judge_triangle_topologicalStructure(MyTriangle* tri1, MyTriangle* tri2);

//返回bool值
bool isTriangleTntersect(MyTriangle* tri1, MyTriangle* tri2);


