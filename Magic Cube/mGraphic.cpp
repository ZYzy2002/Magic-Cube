#include "mGraphic.h"


#include <DirectXMath.h>
#include "fbxsdk.h"
#include "vector"


mGraphic::mGraphic(HWND theWindow)
{
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = 0u;
	scd.BufferDesc.Height = 0u;
	scd.BufferDesc.RefreshRate.Denominator = 0u;
	scd.BufferDesc.RefreshRate.Numerator = 0u;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1u;
	scd.SampleDesc.Quality = 0u;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1u;
	scd.OutputWindow = theWindow;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0u;
	D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 0u, nullptr, 0u, 
		D3D11_SDK_VERSION, &scd, 
		&pSwapChain, &pDevice, nullptr, &pContext);
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	pSwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBackBuffer);
	
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTarget);

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	D3D11_TEXTURE2D_DESC t2dDesc{};
	t2dDesc.Width = 800;                                       
	t2dDesc.Height = 600;                                      
	t2dDesc.MipLevels = 1u;
	t2dDesc.ArraySize = 1u;
	t2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	t2dDesc.SampleDesc.Count = 1u;
	t2dDesc.SampleDesc.Quality = 0u;
	t2dDesc.Usage = D3D11_USAGE_DEFAULT;
	t2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthTex;
	pDevice->CreateTexture2D(&t2dDesc, nullptr, &pDepthTex);
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(pDepthTex.Get(), &DSVDesc, &pDepthStencilView);
	pContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(),pDepthStencilView.Get());

	D3D11_VIEWPORT viewport{};
	viewport.Height = 600;
	viewport.Width = 800;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	pContext->RSSetViewports(1u, &viewport);

	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mGraphic::Clear()
{
	float color[4]{ 0.5f, 0.5f, 0.5f, 1.0f };
	pContext->ClearRenderTargetView(pRenderTarget.Get(),color); 
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1u, 0u);
}

void mGraphic::Swap()
{
	pSwapChain->Present(1u, 0u);
}

void mGraphic::TestDraw()
{

// 	pDevice->CheckCounter();
// 	pContext->OMSetRenderTargetsAndUnorderedAccessViews()

// 	struct MyVertex
// 	{
// 		DirectX::XMFLOAT3 Pos;
// 		//DirectX::XMFLOAT2 Tex;
// 	};
// 	std::vector<MyVertex> myVertexBuffer;
// 
// 	FbxManager* g_pFbxSdkManager = nullptr;
// 	if (g_pFbxSdkManager == nullptr)
// 	{
// 		g_pFbxSdkManager = FbxManager::Create();
// 
// 		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
// 		g_pFbxSdkManager->SetIOSettings(pIOsettings);
// 	}
// 
// 	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
// 	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");
// 
// 	bool bSuccess = pImporter->Initialize("Modle\\OneCube.fbx", -1, g_pFbxSdkManager->GetIOSettings());
// 	if (!bSuccess) return;
// 
// 	bSuccess = pImporter->Import(pFbxScene);
// 	if (!bSuccess) return;
// 
// 	pImporter->Destroy();
// 
// 	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
// 
// 	if (pFbxRootNode)
// 	{
// 		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)		//遍历fbx文件子节点
// 		{
// 			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
// 
// 			if (pFbxChildNode->GetNodeAttribute() == NULL)
// 				continue;
// 
// 			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
// 
// 			if (AttributeType != FbxNodeAttribute::eMesh)
// 				continue;
// 
// 			FbxMesh* pMesh =pFbxChildNode->GetMesh();
// 
// 			FbxVector4* pVertices = pMesh->GetControlPoints();
// 
// 			FbxArray<FbxVector4> pNormal;
// 			pMesh->GetPolygonVertexNormals(pNormal);
// 
// 			for (int j = 0; j < pMesh->GetPolygonCount(); j++)		//如果是 mesh 遍历
// 			{
// 				int iNumVertices = pMesh->GetPolygonSize(j);
// 				assert(iNumVertices == 3);
// 
// 				for (int k = 0; k < iNumVertices; k++) {
// 					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
// 
// 					MyVertex vertex;
// 					vertex.Pos.x = (float)pVertices[iControlPointIndex].mData[0];
// 					vertex.Pos.y = (float)pVertices[iControlPointIndex].mData[1];
// 					vertex.Pos.z = (float)pVertices[iControlPointIndex].mData[2];
// 					oneVertexInfo.normal.x = theVertexNormal[0];
// 					oneVertexInfo.normal.y = theVertexNormal[1];
// 					oneVertexInfo.normal.z = theVertexNormal[2];
// 					myVertexBuffer.push_back(vertex);
// 				}
// 			}
// 		}
// 	}
// 
	
	struct VertexInfo
	{
		DirectX::XMFLOAT3 vertex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	std::vector<VertexInfo> vertexBuffer;

	FbxManager* g_pFbxSdkManager = nullptr;
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize("C:Modle\\OneCube.fbx", -1, g_pFbxSdkManager->GetIOSettings());
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
			const char* UVset1Name =(const char*) UVSetName[0];
			pMesh->GetPolygonVertexUVs(NULL,pUVs,NULL);

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




//*******************************vertex const buffer;
	struct VertexCBuffer
	{
		DirectX::XMMATRIX OBJECT_TO_WORLD;
		DirectX::XMMATRIX WORLD_TO_OBJECT;
		DirectX::XMMATRIX MATRIX_V;
		DirectX::XMMATRIX MATRIX_P;
		DirectX::XMMATRIX MATRIX_MV;
		DirectX::XMMATRIX MATRIX_MVP;
	}vertexCBuf;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	//pDevice->CreateBuffer(, , &pVertexBuffer);









	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	
	



	
}

