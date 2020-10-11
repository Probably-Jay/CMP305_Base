#include "TerrainMesh.h"

TerrainMesh::TerrainMesh( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution ) :
	PlaneMesh( device, deviceContext, lresolution ) 
{
	Resize( resolution );
	BuildHeightMap();
	Regenerate( device, deviceContext );
}

//Cleanup the heightMap
TerrainMesh::~TerrainMesh() {
	delete[] heightMap;
	heightMap = 0;
}


//Fill an array of floats that represent the height values at each grid point.
//Here we are producing a Sine wave along the X-axis
void TerrainMesh::BuildHeightMap() {
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale =  terrainSize / (float)resolution;

	//TODO: Give some meaning to these magic numbers! What effect does changing them have on terrain?
	for( int j = 0; j < ( resolution ); j++ ) {
		for( int i = 0; i < ( resolution ); i++ ) {
			height = ( sin( (float)i * 0.1f * scale ) ) * 10.0f;
			height += ( cos( (float)j * 0.033f * scale ) ) * 10.0f;
			height += ( cos( (float)j * 0.066f * scale ) ) * 3.0f;
			heightMap[( j * resolution ) + i] = height;
		}
	}	
}


//Fill an array of floats that represent the height values at each grid point.
//Here we are producing a Sine wave along the X-axis
void TerrainMesh::BuildFlatHeightMap() {
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale =  terrainSize / (float)resolution;

	//TODO: Give some meaning to these magic numbers! What effect does changing them have on terrain?
	for( int j = 0; j < ( resolution ); j++ ) {
		for( int i = 0; i < ( resolution ); i++ ) {
			heightMap[( j * resolution ) + i] = height;
		}
	}	
}


//Fill an array of floats that represent the height values at each grid point.
//Here we are producing a Sine wave along the X-axis
void TerrainMesh::BuildRandHeightMap() {
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale =  terrainSize / (float)resolution;
	
	//TODO: Give some meaning to these magic numbers! What effect does changing them have on terrain?
	for( int j = 0; j < ( resolution ); j++ ) {
		for( int i = 0; i < ( resolution ); i++ ) {
			heightMap[( j * resolution ) + i] = rand()%1000 / 100.f;
		}
	}	
}



void TerrainMesh::Smooth(int itterations) {
	for (size_t ittr = 0; ittr < itterations; ittr++)
	{

	float height = 0.0f;

	float * tempHeightMap = new float[resolution * resolution];

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale =  terrainSize / (float)resolution;
	
	//TODO: Give some meaning to these magic numbers! What effect does changing them have on terrain?


	for( int j = 0; j < ( resolution ); j++ ) {
		for( int i = 0; i < ( resolution ); i++ ) {

			int divCount = 1;
			int index = (j * resolution) + i;
			
			tempHeightMap[index] = heightMap[index];

			if ((j - 1) > 0) {
				tempHeightMap[index] += heightMap[((j - 1) * resolution) + i];
				divCount++;
			}
			
			if ((i - 1) > 0) {
				tempHeightMap[index] += heightMap[(j * resolution) + (i-1)];
				divCount++;
			}

			if ((j + 1) < resolution) {
				tempHeightMap[index] += heightMap[((j + 1) * resolution) + i];
				divCount++;
			}
			
			if ((i + 1) < resolution) {
				tempHeightMap[index] += heightMap[(j * resolution) + (i + 1)];
				divCount++;
			}

			tempHeightMap[index] /= divCount;

		}
	}	

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			int index = (j * resolution) + i;

			heightMap[index] = tempHeightMap[index];

		}
	}

	}
}

void TerrainMesh::Fault(float amount)
{
	// pick two sides at random
	int side1 = rand() % 4;
	int side2 = (side1 + rand() % 3 + 1) % 4;

	// helper function

	auto GetIndexOfSide = [=](int side) {
		int sideIndex;
		switch (side)
		{
		case 0:
			sideIndex = ((rand() % resolution) * resolution) + 0;
			break;
		case 1:
			sideIndex = (0 * resolution) + (rand() % resolution);
			break;
		case 2:
			sideIndex = ((rand() % resolution) * resolution) + (resolution - 1);
			break;
		case 3:
			sideIndex = ((resolution - 1) * resolution) + (rand() % resolution);
			break;
		default:
			sideIndex = -1;
			break;
		}
		return sideIndex;
	};
	
	auto GetPosOfSide = [=](int side) {
		XMFLOAT3 point;
		switch (side)
		{
		case 0:
			point = XMFLOAT3{ 0,0, (float)(rand() % resolution) };//  ((rand() % resolution) * resolution) + 0;
			break;
		case 1:
			point = XMFLOAT3{ (float)(rand() % resolution),0,0 }; // (0 * resolution) + (rand() % resolution);
			break;
		case 2:
			point = XMFLOAT3{ (float)(resolution-1) ,0,(float)(rand() % resolution) }; // ((rand() % resolution) * resolution) + (resolution - 1);
			break;
		case 3:
			point = XMFLOAT3{ (float)(rand() % resolution),0,(float)(resolution -1) }; // ((resolution - 1) * resolution) + (rand() % resolution);
			break;
		default:
			point = XMFLOAT3{ 0,0,0 };
			break;
		}
		return point;
	};
	
	//int side1Index = GetIndexOfSide(side1);
	//int side2Index = GetIndexOfSide(side2);

	XMFLOAT3 p1, p2, v, l1, l2;

	p1 = GetPosOfSide(side1);
	p2 = GetPosOfSide(side2);

	l1 = XMFLOAT3{ p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			v = XMFLOAT3{ (float) i, 0, (float)j };
			l2 = XMFLOAT3{ v.x - p1.x, v.y - p1.y, v.z - p1.z };

			XMFLOAT3 cross;
			//cross.x = l1.y * l2.z - l1.z * l2.y;
			cross.y = l1.z * l2.x - l1.x * l2.z;
			//cross.z = l1.x * l2.y - l1.y * l2.x;

			if (cross.y > 0) {
				heightMap[(j * resolution) + i] += amount ;
			}
			else {
				heightMap[(j * resolution) + i] -= amount;

			}
		}
	}


}



void TerrainMesh::Resize( int newResolution ) {
	resolution = newResolution;
	heightMap = new float[resolution * resolution];
	if( vertexBuffer != NULL ) {
		vertexBuffer->Release();
	}
	vertexBuffer = NULL;
}

// Set up the heightmap and create or update the appropriate buffers
void TerrainMesh::Regenerate( ID3D11Device * device, ID3D11DeviceContext * deviceContext ) {

	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, height, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	
	//Calculate and store the height values
	

	// Calculate the number of vertices in the terrain mesh.
	// We share vertices in this mesh, so the vertex count is simply the terrain 'resolution'
	// and the index count is the number of resulting triangles * 3 OR the number of quads * 6
	vertexCount = resolution * resolution;

	indexCount = ( ( resolution - 1 ) * ( resolution - 1 ) ) * 6;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	index = 0;

	// UV coords.
	u = 0;
	v = 0;
	increment = m_UVscale / resolution;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;

	//Set up vertices
	for( j = 0; j < ( resolution ); j++ ) {
		for( i = 0; i < ( resolution ); i++ ) {
			positionX = (float)i * scale;
			positionZ = (float)( j ) * scale;

			height = heightMap[index];
			vertices[index].position = XMFLOAT3( positionX, height, positionZ );
			vertices[index].texture = XMFLOAT2( u, v );

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
	}

	//Set up index list
	index = 0;
	for( j = 0; j < ( resolution - 1 ); j++ ) {
		for( i = 0; i < ( resolution - 1 ); i++ ) {

			//Build index array
			indices[index] = ( j*resolution ) + i;
			indices[index + 1] = ( ( j + 1 ) * resolution ) + ( i + 1 );
			indices[index + 2] = ( ( j + 1 ) * resolution ) + i;

			indices[index + 3] = ( j * resolution ) + i;
			indices[index + 4] = ( j * resolution ) + ( i + 1 );
			indices[index + 5] = ( ( j + 1 ) * resolution ) + ( i + 1 );
			index += 6;
		}
	}

	//Set up normals
	for( j = 0; j < ( resolution - 1 ); j++ ) {
		for( i = 0; i < ( resolution - 1 ); i++ ) {
			//Calculate the plane normals
			XMFLOAT3 a, b, c;	//Three corner vertices
			a = vertices[j * resolution + i].position;
			b = vertices[j * resolution + i + 1].position;
			c = vertices[( j + 1 ) * resolution + i].position;

			//Two edges
			XMFLOAT3 ab( c.x - a.x, c.y - a.y, c.z - a.z );
			XMFLOAT3 ac( b.x - a.x, b.y - a.y, b.z - a.z );
			
			//Calculate the cross product
			XMFLOAT3 cross;
			cross.x = ab.y * ac.z - ab.z * ac.y;
			cross.y = ab.z * ac.x - ab.x * ac.z;
			cross.z = ab.x * ac.y - ab.y * ac.x;
			float mag = ( cross.x * cross.x ) + ( cross.y * cross.y ) + ( cross.z * cross.z );
			mag = sqrtf( mag );
			cross.x/= mag;
			cross.y /= mag;
			cross.z /= mag;
			vertices[j * resolution + i].normal = cross;
		}
	}

	//Smooth the normals by averaging the normals from the surrounding planes
	XMFLOAT3 smoothedNormal( 0, 1, 0 );
	for( j = 0; j < resolution; j++ ) {
		for( i = 0; i < resolution; i++ ) {
			smoothedNormal.x = 0;
			smoothedNormal.y = 0;
			smoothedNormal.z = 0;
			float count = 0;
			//Left planes
			if( ( i - 1 ) >= 0 ) {
				//Top planes
				if( ( j ) < ( resolution - 1 ) ) {
					smoothedNormal.x += vertices[j * resolution + ( i - 1 )].normal.x;
					smoothedNormal.y += vertices[j * resolution + ( i - 1 )].normal.y;
					smoothedNormal.z += vertices[j * resolution + ( i - 1 )].normal.z;
					count++;
				}
				//Bottom planes
				if( ( j - 1 ) >= 0 ) {
					smoothedNormal.x += vertices[( j - 1 ) * resolution + ( i - 1 )].normal.x;
					smoothedNormal.y += vertices[( j - 1 ) * resolution + ( i - 1 )].normal.y;
					smoothedNormal.z += vertices[( j - 1 ) * resolution + ( i - 1 )].normal.z;
					count++;
				}
			}
			//right planes
			if( ( i ) <( resolution - 1 ) ) {

				//Top planes
				if( ( j ) < ( resolution - 1 ) ) {
					smoothedNormal.x += vertices[j * resolution + i].normal.x;
					smoothedNormal.y += vertices[j * resolution + i].normal.y;
					smoothedNormal.z += vertices[j * resolution + i].normal.z;
					count++;
				}
				//Bottom planes
				if( ( j - 1 ) >= 0 ) {
					smoothedNormal.x += vertices[( j - 1 ) * resolution + i].normal.x;
					smoothedNormal.y += vertices[( j - 1 ) * resolution + i].normal.y;
					smoothedNormal.z += vertices[( j - 1 ) * resolution + i].normal.z;
					count++;
				}
			}
			smoothedNormal.x /= count;
			smoothedNormal.y /= count;
			smoothedNormal.z /= count;

			float mag = sqrt( ( smoothedNormal.x * smoothedNormal.x ) + ( smoothedNormal.y * smoothedNormal.y ) + ( smoothedNormal.z * smoothedNormal.z ) );
			smoothedNormal.x /= mag;
			smoothedNormal.y /= mag;
			smoothedNormal.z /= mag;

			vertices[j * resolution + i].normal = smoothedNormal;
		}
	}
	//If we've not yet created our dyanmic Vertex and Index buffers, do that now
	if( vertexBuffer == NULL ) {
		CreateBuffers( device, vertices, indices );
	}
	else {
		//If we've already made our buffers, update the information
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory( &mappedResource, sizeof( D3D11_MAPPED_SUBRESOURCE ) );

		//  Disable GPU access to the vertex buffer data.
		deviceContext->Map( vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		//  Update the vertex buffer here.
		memcpy( mappedResource.pData, vertices, sizeof( VertexType ) * vertexCount );
		//  Reenable GPU access to the vertex buffer data.
		deviceContext->Unmap( vertexBuffer, 0 );
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

//Create the vertex and index buffers that will be passed along to the graphics card for rendering
//For CMP305, you don't need to worry so much about how or why yet, but notice the Vertex buffer is DYNAMIC here as we are changing the values often
void TerrainMesh::CreateBuffers( ID3D11Device* device, VertexType* vertices, unsigned long* indices ) {

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer( &vertexBufferDesc, &vertexData, &vertexBuffer );

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long ) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer( &indexBufferDesc, &indexData, &indexBuffer );
}