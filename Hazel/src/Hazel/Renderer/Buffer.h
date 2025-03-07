#pragma once
#include "hzpch.h"

namespace Hazel {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t GetShdaerDataTypeSize(ShaderDataType type) 
	{
		switch (type)
		{
		case ShaderDataType::None: return 0;
			break;
		case ShaderDataType::Float: return 4;
			break;
		case ShaderDataType::Float2:return 4 * 2;
			break;
		case ShaderDataType::Float3:return 4 * 3;
			break;
		case ShaderDataType::Float4:return 4 * 4;
			break;
		case ShaderDataType::Mat3:return 4 * 3 * 3;
			break;
		case ShaderDataType::Mat4:return 4* 4 * 4;
			break;
		case ShaderDataType::Int:return 4 * 1;
			break;
		case ShaderDataType::Int2:return 4 * 2;
			break;
		case ShaderDataType::Int3:return 4 * 3;
			break;
		case ShaderDataType::Int4:return 4 * 4;
			break;
		case ShaderDataType::Bool:return 1;
			break;
		default:
			break;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;
		uint32_t CoordIndex;

		BufferElement() {}

		BufferElement(ShaderDataType type,const std::string& name,uint32_t coordIndex, bool normalized = false)
			: Name(name), Type(type), Size(GetShdaerDataTypeSize(type)), Offset(0), Normalized(normalized), CoordIndex(coordIndex)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3 * 3;
			case ShaderDataType::Mat4:    return 4 * 4;
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout 
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; };
		inline const uint32_t GetStride() const { return m_Stride; };

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
		inline int GetCount() const { return m_Elements.size(); }

	private:
		void CalculateOffsetsAndStride() 
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		//inline const BufferLayout& GetLayout() { return m_Layout; };
		//inline virtual void SetLayout(const BufferLayout& layout) = 0;
		inline const uint32_t GetBufferSize() { return m_BufferSize; };
		inline uint32_t GetCount() const { return m_BufferSize; }
		inline uint32_t GetStride() const { return m_BufferStride; }
		inline const int SetStride(uint32_t stride) { m_BufferStride = stride; }
		inline const int SetSize(uint32_t size) { m_BufferSize = size; }

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size, uint32_t stride);
	protected:
		uint32_t m_BufferSize;
		uint32_t m_BufferStride;
	};

	class IndexBuffer 
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const  = 0;

		virtual uint32_t GetCount() const = 0;
		static Ref<IndexBuffer> Create(uint16_t* indices, uint32_t size);

	};

	class ConstantBuffer
	{
	public:
		ConstantBuffer() 
		{
#ifdef RENDER_API_OPENGL
			m_CpuHandle  = 0
			m_BufferResource = 0;
#elif RENDER_API_DIRECTX12 // RENDER_API_OPENGL
			m_CpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{};
			m_BufferResource = Microsoft::WRL::ComPtr<ID3D12Resource>{};
#endif // DEBUG
		};
		virtual ~ConstantBuffer() {}

		virtual void SetData(void* srcData, int length) = 0;
		//virtual void Unbind() const = 0;
		//
		//virtual uint32_t GetCount() const = 0;
		// 根据buffer的element个数创建一个大的buffer
		ConstantBuffer& operator=(const ConstantBuffer& rhs) = delete;
		static Ref<ConstantBuffer> Create(uint32_t bufferSize);
		inline boost::uuids::uuid GetUUID() const { return m_UUID; }
		inline uint32_t GetBufferSize() const { return m_BufferSize; }

		template<typename T>
		T getCpuHandle() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, CD3DX12_CPU_DESCRIPTOR_HANDLE>) {
				return std::get<T>(m_CpuHandle);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or CD3DX12_CPU_DESCRIPTOR_HANDLE");
			}
		}

		template<typename T>
		T getResource() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, Microsoft::WRL::ComPtr<ID3D12Resource>>) {
				return std::get<T>(m_BufferResource);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or ID3D12Resource");
			}
		}

	protected:

		std::variant<uint32_t, CD3DX12_CPU_DESCRIPTOR_HANDLE> m_CpuHandle;
		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> m_BufferResource;
		
		boost::uuids::uuid m_UUID;
		uint32_t m_BufferSize;
	};

}