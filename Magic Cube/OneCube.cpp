#include "OneCube.h"


#include <fbxsdk.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

OneCube::OneCube(std::shared_ptr<mGraphic>& gfx)
	:gfx{gfx}
{
	//vertex buffer
	FbxManager* g_pFbxSdkManager = nullptr;
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize(modleFileName, -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess) return;

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = pFbxChildNode->GetMesh();			//拿到 第一个 mesh的 子节点

			FbxVector4* pVertices = pMesh->GetControlPoints();
			FbxArray<FbxVector4> pNormal;
			pMesh->GetPolygonVertexNormals(pNormal);
			FbxArray<FbxVector2> pUVs;
			FbxStringList UVSetName;
			pMesh->GetUVSetNames(UVSetName);
			const char* UVset1Name = (const char*)UVSetName[0];
			pMesh->GetPolygonVertexUVs(NULL, pUVs, NULL);

			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);

				for (int k = 0; k < iNumVertices; k++) {
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					FbxVector4 theVertexNormal;
					pMesh->GetPolygonVertexNormal(j, k, theVertexNormal);
					FbxVector2 theUV;
					bool isUnmapped;
					pMesh->GetPolygonVertexUV(j, k, UVset1Name, theUV, isUnmapped);
					VertexInfo oneVertexInfo;
					oneVertexInfo.vertex.x = pVertices[iControlPointIndex][0];
					oneVertexInfo.vertex.y = pVertices[iControlPointIndex][1];
					oneVertexInfo.vertex.z = pVertices[iControlPointIndex][2];
					oneVertexInfo.normal.x = theVertexNormal[0];
					oneVertexInfo.normal.y = theVertexNormal[1];
					oneVertexInfo.normal.z = theVertexNormal[2];
					oneVertexInfo.texcoord.x = theUV[0];
					oneVertexInfo.texcoord.y = theUV[1];
					vertexBuffer.push_back(oneVertexInfo);
				}
			}
		}
	}

	D3D11_BUFFER_DESC bufDesc{};
	bufDesc.ByteWidth = sizeof(VertexInfo) * vertexBuffer.size();
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0u;
	bufDesc.MiscFlags = 0u;
	bufDesc.StructureByteStride = sizeof(VertexInfo);
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA initiaVertexBuffer{};
	initiaVertexBuffer.pSysMem = vertexBuffer.data();
	gfx->pDevice->CreateBuffer(&bufDesc, &initiaVertexBuffer, &pVertexBuffer);
	VBstrides = (UINT)sizeof(VertexInfo);
	VBoffsets = 0u;

	//vertex constant buffer
	VConstBuffer vertexCBuffer
	{
		//Object_To_World
		XMMatrixTranspose(
			XMMatrixScaling(scale.x, scale.y, scale.z)
			* XMMatrixRotationX(rotation.x)
			* XMMatrixRotationY(rotation.y)
			* XMMatrixRotationZ(rotation.z)
			* XMMatrixTranslation(translation.x, translation.y, translation.z)
		),
		//World_To_Object
		XMMATRIX{},
	};
	vertexCBuffer.World_To_Object = XMMatrixInverse(nullptr, vertexCBuffer.Object_To_World);

	D3D11_BUFFER_DESC VSCBufferDesc{};
	VSCBufferDesc.ByteWidth = sizeof(VConstBuffer);
	VSCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VSCBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE ;
	VSCBufferDesc.MiscFlags = 0u;
	VSCBufferDesc.StructureByteStride = 0u;
	VSCBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	D3D11_SUBRESOURCE_DATA initiaVCBuffer;
	initiaVCBuffer.pSysMem = &vertexCBuffer;
	gfx->pDevice->CreateBuffer(&VSCBufferDesc,&initiaVCBuffer,&pVConstBuffer);
	
	//vertex shader
	D3DReadFileToBlob(vertexShaderFileName, &pBlob);
	gfx->pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	//InputLayout
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]
	{
		{"POSITION",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u},
		{"NORMAL",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,12u,D3D11_INPUT_PER_VERTEX_DATA,0u},
		{"TEXCOORD",0u,DXGI_FORMAT_R32G32_FLOAT,0u,24u,D3D11_INPUT_PER_VERTEX_DATA,0u}
	};
	gfx->pDevice->CreateInputLayout(inputLayoutDesc,sizeof(inputLayoutDesc)/sizeof(D3D11_INPUT_ELEMENT_DESC),pBlob->GetBufferPointer(),pBlob->GetBufferSize(),&pInputLayout);
	//Pixel Shader
	D3DReadFileToBlob(pixelShaderFileName, &pBlob);
	gfx->pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	
	//Pixel Shader Texture
	ULONG_PTR token;
	Gdiplus::GdiplusStartupInput input{};
	Gdiplus::GdiplusStartup(&token,&input,NULL);
	Gdiplus::Bitmap tex{ texture2DFileName };
	struct mColor
	{
		unsigned char r, g, b, a;
	};
	std::vector<mColor> texPixelArray{ tex.GetWidth() * tex.GetHeight(),mColor{} };
	Gdiplus::Color temp;
 	for (int i = 0; i < tex.GetHeight(); ++i)
 	{
		for (int j = 0; j < tex.GetWidth(); ++j)
		{
			tex.GetPixel(j, i, &temp);
			texPixelArray[tex.GetWidth() * i + j].r = temp.GetR();
			texPixelArray[tex.GetWidth() * i + j].g = temp.GetG();
			texPixelArray[tex.GetWidth() * i + j].b = temp.GetB();
			texPixelArray[tex.GetWidth() * i + j].a = temp.GetA();
		}
 	}

	D3D11_TEXTURE2D_DESC t2dDesc{};
	t2dDesc.Width = tex.GetWidth();
	t2dDesc.Height = tex.GetHeight();
	t2dDesc.MipLevels = 1u;
	t2dDesc.ArraySize = 1u;
	t2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	t2dDesc.SampleDesc.Count = 1u;
	t2dDesc.SampleDesc.Quality = 0u;
	t2dDesc.Usage = D3D11_USAGE_DEFAULT;
	t2dDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	t2dDesc.MiscFlags = 0u;
	t2dDesc.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initiaTex{};
	initiaTex.pSysMem = texPixelArray.data();
	initiaTex.SysMemPitch = sizeof(mColor) * tex.GetWidth();
	ComPtr<ID3D11Texture2D> pTexture;
	gfx->pDevice->CreateTexture2D(&t2dDesc,&initiaTex,&pTexture);
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
	shaderViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderViewDesc.Texture2D.MostDetailedMip = 0u;
	shaderViewDesc.Texture2D.MipLevels = 1u;
	gfx->pDevice->CreateShaderResourceView(pTexture.Get(),&shaderViewDesc,&pShaderview);
	
}

void OneCube::Draw()
{
	gfx->pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &VBstrides, &VBoffsets);

	gfx->pContext->VSSetConstantBuffers(0u, 1u, pVConstBuffer.GetAddressOf());

	gfx->pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	gfx->pContext->IASetInputLayout(pInputLayout.Get());
	
	gfx->pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	gfx->pContext->PSSetShaderResources(0u, 1u,pShaderview.GetAddressOf());

	gfx->pContext->Draw((UINT)vertexBuffer.size(), 0u);
}

void OneCube::UpDate()
{
	VConstBuffer newConstVBuffer
	{
		//MATIRX_M
		XMMatrixTranspose(
			XMMatrixScaling(scale.x, scale.y, scale.z)
 			* XMMatrixRotationX(rotation.x)
 			* XMMatrixRotationY(rotation.y)
 			* XMMatrixRotationZ(rotation.z)
			* XMMatrixTranslation(translation.x, translation.y, translation.z)
		),
		
		//World_To_Object
		XMMATRIX {},
	};
	newConstVBuffer.World_To_Object = XMMatrixInverse(nullptr, newConstVBuffer.Object_To_World);


	D3D11_MAPPED_SUBRESOURCE mapSubsource{};
	gfx->pContext->Map(pVConstBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapSubsource);
	memcpy(mapSubsource.pData,&newConstVBuffer,sizeof(VConstBuffer));
	gfx->pContext->Unmap(pVConstBuffer.Get(), 1u);
}

void OneCube::ChangeTransform(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	OneCube::translation = translation;
	OneCube::rotation = rotation;
	OneCube::scale = scale;
}

void OneCube::RotateAroundWorldCenter(XMFLOAT3 DeltaRotate)
{
	
	XMMATRIX transform = XMMatrixRotationX(DeltaRotate.x) * XMMatrixRotationY(DeltaRotate.y) * XMMatrixRotationZ(DeltaRotate.z);

	//change the cube translation
	XMVECTOR loadedTranslation = XMLoadFloat3(&translation);
	XMVECTOR newTranslation = XMVector3Transform(loadedTranslation, transform);
	XMStoreFloat3(&translation,newTranslation);

	// change thecube rotation
	XMVECTOR loadedRotation = XMLoadFloat3(&rotation);
	XMVECTOR newRotation = XMVectorAdd(loadedRotation , XMLoadFloat3(&DeltaRotate));
	XMStoreFloat3(&rotation,newRotation);
}

bool OneCube::RotationApproachPIDIV2()
{
	float roll, floor, top;
	bool alignX = false, alignY = false, alignZ = false;

	//x
	roll = rotation.x / XM_PIDIV2;
	if (rotation.x < 0)
	{
		floor = (int(roll) - 1) * XM_PIDIV2;
		top = int(roll) * XM_PIDIV2;
	}
	else
	{
		floor = int(roll) * XM_PIDIV2;
		top = (int(roll) + 1) * XM_PIDIV2;
	}

	if ((top - rotation.x) > (rotation.x - floor))
	{
		RotateAroundWorldCenter(XMFLOAT3{ -(rotation.x - floor) / 2,0,0 });
	}
	else
	{
		RotateAroundWorldCenter(XMFLOAT3{ (top - rotation.x) / 2,0,0 });
	}

	//y
	roll = rotation.y / XM_PIDIV2;
	if (rotation.y < 0)
	{
		floor = (int(roll) - 1) * XM_PIDIV2;
		top = int(roll) * XM_PIDIV2;
	}
	else
	{
		floor = int(roll) * XM_PIDIV2;
		top = (int(roll) + 1) * XM_PIDIV2;
	}


	if ((top - rotation.y) > (rotation.y - floor))
	{
		RotateAroundWorldCenter(XMFLOAT3{ 0,-(rotation.y - floor) / 2,0 });
	}
	else
	{
		RotateAroundWorldCenter(XMFLOAT3{ 0,(top - rotation.y) / 2,0 });
	}
	//z
	roll = rotation.z / XM_PIDIV2;
	if (rotation.z < 0)
	{
		floor = (int(roll) - 1) * XM_PIDIV2;
		top = int(roll) * XM_PIDIV2;
	}
	else
	{
		floor = int(roll) * XM_PIDIV2;
		top = (int(roll) + 1) * XM_PIDIV2;
	}


	if ((top - rotation.z) > (rotation.z - floor))
	{
		RotateAroundWorldCenter(XMFLOAT3{ 0,0,-0.01 });
	}
	else
	{
		RotateAroundWorldCenter(XMFLOAT3{ 0,0,0.01 });
	}

	if (alignX && alignY && alignZ)
	{
		return true;
	}
	return false;
}

bool OneCube::IsRayCrossCollisionBox(XMFLOAT3 WorldSpaceDirection, XMFLOAT3 WorldSpacePointPosition)
{

	XMMATRIX Object_To_World_SR =
		/*XMMatrixTranspose(*/		//不用绑定到 渲染管线， 所以不用 转置
		XMMatrixScaling(scale.x, scale.y, scale.z)
		* XMMatrixRotationX(rotation.x)
		* XMMatrixRotationY(rotation.y)
		* XMMatrixRotationZ(rotation.z);
			//* XMMatrixTranslation(translation.x, translation.y, translation.z);
	XMMATRIX Object_To_World =
		/*XMMatrixTranspose(*/		//不用绑定到 渲染管线， 所以不用 转置
		Object_To_World_SR
		* XMMatrixTranslation(translation.x, translation.y, translation.z);
	XMMATRIX World_To_Object_SR = XMMatrixInverse(nullptr, Object_To_World_SR);
	XMMATRIX World_To_Object = XMMatrixInverse(nullptr, Object_To_World);

	XMFLOAT3 MSDirect;
	XMStoreFloat3(&MSDirect,XMVector3Transform(XMLoadFloat3(&WorldSpaceDirection), World_To_Object_SR));
 	XMFLOAT3 MSPoint;
 	XMStoreFloat3(&MSPoint, XMVector3Transform(XMLoadFloat3(&WorldSpacePointPosition), World_To_Object));  //由于没有看到有 float4 * matrix 所以使用float3 * matrix再加上位移
	float x, y, z, t;
	const float a = MSPoint.x, b = MSPoint.y, c = MSPoint.z, i = MSDirect.x, j = MSDirect.y, k = MSDirect.z;	// line Function: x=i*t+a, y=j*t+b, z=k*t+c

	x = centerBoxSideLength / 2.0f;  // 与x=1面，求交点，判断交点是否在Cube上
	if (i != 0)				//防止 Dir与面平行
	{
		t = (x - a) / i;
		y = j * t + b;
		z = k * t + c;
		if (y > -centerBoxSideLength / 2.0f && y <centerBoxSideLength / 2.0f && z> -centerBoxSideLength / 2.0f && z < centerBoxSideLength / 2.0f)return true;
	}
	x = -centerBoxSideLength / 2.0f;
	if (i != 0)
	{
		t = (x - a) / i;
		y = j * t + b;
		z = k * t + c;
		if (y > -centerBoxSideLength / 2.0f && y <centerBoxSideLength / 2.0f && z> -centerBoxSideLength / 2.0f && z < centerBoxSideLength / 2.0f)return true;
	}
	y = centerBoxSideLength / 2.0f;
	if (j != 0)
	{
		t = (y - b) / j;
		x = i * t + a;
		z = k * t + c;
		if (x > -centerBoxSideLength / 2.0f && x <centerBoxSideLength / 2.0f && z> -centerBoxSideLength / 2.0f && z < centerBoxSideLength / 2.0f)return true;
	}
	y = -centerBoxSideLength / 2.0f;
	if (j != 0)
	{
		t = (y - b) / j;
		x = i * t + a;
		z = k * t + c;
		if (x > -centerBoxSideLength / 2.0f && x <centerBoxSideLength / 2.0f && z> -centerBoxSideLength / 2.0f && z < centerBoxSideLength / 2.0f)return true;
	}
	z = centerBoxSideLength / 2.0f;
	if (k != 0)
	{
		t = (z - c) / k;
		x = i * t + a;
		y = j * t + b;
		if (x > -centerBoxSideLength / 2.0f && x <centerBoxSideLength / 2.0f && y> -centerBoxSideLength / 2.0f && y < centerBoxSideLength / 2.0f)return true;
	}
	z = -centerBoxSideLength / 2.0f;
	if (k != 0)
	{
		t = (z - c) / k;
		x = i * t + a;
		y = j * t + b;
		if (x > -centerBoxSideLength / 2.0f && x <centerBoxSideLength / 2.0f && y> -centerBoxSideLength / 2.0f && y < centerBoxSideLength / 2.0f)return true;
	}

	return false;
}



