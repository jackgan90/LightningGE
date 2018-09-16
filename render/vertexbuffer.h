#pragma once
#include <vector>
#include <boost/functional/hash.hpp>
#include <memory>
#include "ivertexbuffer.h"
#include "gpubuffer.h"

namespace Lightning
{
	namespace Render
	{

		class LIGHTNING_RENDER_API VertexBuffer : public IVertexBuffer
		{
		public:
			const VertexComponent& GetComponentInfo(size_t attributeIndex)override;
			~VertexBuffer()override;
			//get vertex attribute count associate with this vertex buffer
			std::uint8_t GetComponentCount()override;
			//get vertices count contained within this vertex buffer
			std::uint32_t GetVertexCount()const override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()const override;
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
		protected:
			VertexBuffer(uint32_t bufferSize, const VertexComponent *components, std::uint8_t componentCount);
			void CalculateVertexSize();
			VertexComponent *m_components;
			std::uint8_t m_componentCount;
			std::uint32_t m_vertexCount;
			std::uint32_t m_vertexSize;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::VertexComponent>
	{
		size_t operator()(const Lightning::Render::VertexComponent& component)const noexcept
		{
			return component.GetHash();
		}
	};
}
