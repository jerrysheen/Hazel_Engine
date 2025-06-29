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
		// stbi_load��ȡ��ͼ
		// 3D��ͼ��ô��stbi��ȡ��
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
		
		// upload buffer����, ���Ĭ���� RGBA32UNORM
		D3D12_RESOURCE_DESC uploadHeapDesc = {};
		uploadHeapDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;  // ��������Դ
		uploadHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;  // Ĭ�϶���
		uploadHeapDesc.Width = width * height * chanels;  // �������Ĵ�С����λ�ֽ�
		uploadHeapDesc.Height = 1;  // ���ڻ�������Height �̶�Ϊ 1
		uploadHeapDesc.DepthOrArraySize = 1;  // ���ڻ�������Depth �̶�Ϊ 1
		uploadHeapDesc.MipLevels = 1;  // ���ڻ�������Mipmap �����޹�
		uploadHeapDesc.Format = DXGI_FORMAT_UNKNOWN;  // ��ʽ�޹أ�����������Ҫ�����ʽ
		uploadHeapDesc.SampleDesc.Count = 1;  // �޶��ز���
		uploadHeapDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;  // ���ַ�ʽ��������ͨ��������˳��
		uploadHeapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;  // �������־

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),  // �ϴ����͵��ڴ�
			D3D12_HEAP_FLAG_NONE,  // �������־
			&uploadHeapDesc,  // �ϴ�������������
			D3D12_RESOURCE_STATE_GENERIC_READ,  // ��ʼ״̬��ͨ�ö�ȡ
			nullptr,  // ��Դ�ĳ�ʼ״̬����ָ���ʾĬ�ϣ�
			IID_PPV_ARGS(&uploadHeap)  // ����ϴ���������Դָ��
		);

		// map��ͼ���ݵ�uploadbuffer
		// uploadbuffer -> texturebuffer;



		// ���ϼ����Ǵ������̣�bind��ʱ����ȥ����desc���ƺ�Ҳ���ԡ�
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
