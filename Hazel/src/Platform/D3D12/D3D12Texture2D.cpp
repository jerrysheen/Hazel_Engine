#include "hzpch.h"
#include "Hazel.h"
#include "D3D12Texture2D.h"
#include "stb_image.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"

namespace Hazel 
{
	D3D12Texture2D::D3D12Texture2D(const std::string& path)
		:m_RendererID(0)
	{
		// stbi_load读取贴图
		// 3D贴图怎么靠stbi读取？
		int width, height, chanels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &chanels, 0);
		HZ_CORE_ASSERT(data, "Failed to load the image");

		m_Width = width;
		m_Height = height;
		
		//GLenum internalFormat = 0, dataFormat = 0;
		//if (chanels == 4)
		//{
		//	internalFormat = GL_RGBA8;
		//	dataFormat = GL_RGBA;
		//}
		//else if (chanels == 3)
		//{
		//	internalFormat = GL_RGB8;
		//	dataFormat = GL_RGB;
		//}
		
		// upload buffer创建, 这个默认是 RGBA32UNORM
		D3D12_RESOURCE_DESC uploadHeapDesc = {};
		uploadHeapDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;  // 缓冲区资源
		uploadHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;  // 默认对齐
		uploadHeapDesc.Width = width * height * chanels;  // 缓冲区的大小，单位字节
		uploadHeapDesc.Height = 1;  // 对于缓冲区，Height 固定为 1
		uploadHeapDesc.DepthOrArraySize = 1;  // 对于缓冲区，Depth 固定为 1
		uploadHeapDesc.MipLevels = 1;  // 对于缓冲区，Mipmap 层数无关
		uploadHeapDesc.Format = DXGI_FORMAT_UNKNOWN;  // 格式无关，缓冲区不需要纹理格式
		uploadHeapDesc.SampleDesc.Count = 1;  // 无多重采样
		uploadHeapDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;  // 布局方式，缓冲区通常是行主顺序
		uploadHeapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;  // 无特殊标志

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),  // 上传类型的内存
			D3D12_HEAP_FLAG_NONE,  // 无特殊标志
			&uploadHeapDesc,  // 上传缓冲区描述符
			D3D12_RESOURCE_STATE_GENERIC_READ,  // 初始状态：通用读取
			nullptr,  // 资源的初始状态（空指针表示默认）
			IID_PPV_ARGS(&uploadHeap)  // 输出上传缓冲区资源指针
		);

		// map贴图数据到uploadbuffer
		// uploadbuffer -> texturebuffer;



		// 以上几步是创建流程，bind的时候，再去开辟desc，似乎也可以。
	}

	D3D12Texture2D::~D3D12Texture2D()
	{
	}

	void D3D12Texture2D::SetData(void* data, uint32_t size) const
	{
	}

	void D3D12Texture2D::Bind(uint32_t slot) const
	{
	}
}
