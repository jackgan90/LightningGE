#pragma once
#include <d3d12.h>
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "ITexture.h"
#include "Sampler.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12TypeMapper
		{
		public:
			static D3D12_BLEND_OP MapBlendOp(const BlendOperation& op)
			{
				switch (op)
				{
				case BlendOperation::ADD:
					return D3D12_BLEND_OP_ADD;
				case BlendOperation::SUBTRACT:
					return D3D12_BLEND_OP_SUBTRACT;
				case BlendOperation::REVERSE_SUBTRACT:
					return D3D12_BLEND_OP_REV_SUBTRACT;
				case BlendOperation::MIN:
					return D3D12_BLEND_OP_MIN;
				case BlendOperation::MAX:
					return D3D12_BLEND_OP_MAX;
				default:
					return D3D12_BLEND_OP_ADD;
				}
			}

			static D3D12_BLEND MapBlendFactor(const BlendFactor& factor)
			{
				switch (factor)
				{
				case BlendFactor::ZERO:
					return D3D12_BLEND_ZERO;
				case BlendFactor::ONE:
					return D3D12_BLEND_ONE;
				case BlendFactor::SRC_COLOR:
					return D3D12_BLEND_SRC_COLOR;
				case BlendFactor::INV_SRC_COLOR:
					return D3D12_BLEND_INV_SRC_COLOR;
				case BlendFactor::SRC_ALPHA:
					return D3D12_BLEND_SRC_ALPHA;
				case BlendFactor::INV_SRC_ALPHA:
					return D3D12_BLEND_INV_SRC_ALPHA;
				case BlendFactor::DEST_COLOR:
					return D3D12_BLEND_DEST_COLOR;
				case BlendFactor::INV_DEST_COLOR:
					return D3D12_BLEND_INV_DEST_COLOR;
				case BlendFactor::DEST_ALPHA:
					return D3D12_BLEND_DEST_ALPHA;
				case BlendFactor::INV_DEST_ALPHA:
					return D3D12_BLEND_INV_DEST_ALPHA;
				default:
					return D3D12_BLEND_ZERO;
				}
			}

			static D3D12_COMPARISON_FUNC MapCmpFunc(const CmpFunc& func)
			{
				switch (func)
				{
				case CmpFunc::NEVER:
					return D3D12_COMPARISON_FUNC_NEVER;
				case CmpFunc::LESS:
					return D3D12_COMPARISON_FUNC_LESS;
				case CmpFunc::EQUAL:
					return D3D12_COMPARISON_FUNC_EQUAL;
				case CmpFunc::LESS_EQUAL:
					return D3D12_COMPARISON_FUNC_LESS_EQUAL;
				case CmpFunc::GREATER:
					return D3D12_COMPARISON_FUNC_GREATER;
				case CmpFunc::NOT_EQUAL:
					return D3D12_COMPARISON_FUNC_NOT_EQUAL;
				case CmpFunc::GREATER_EQUAL:
					return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
				case CmpFunc::ALWAYS:
					return D3D12_COMPARISON_FUNC_ALWAYS;
				default:
					return D3D12_COMPARISON_FUNC_LESS;
				}
			}

			static D3D12_STENCIL_OP MapStencilOp(const StencilOp& op)
			{
				switch (op)
				{
				case StencilOp::KEEP:
					return D3D12_STENCIL_OP_KEEP;
				case StencilOp::ZERO:
					return D3D12_STENCIL_OP_ZERO;
				case StencilOp::REPLACE:
					return D3D12_STENCIL_OP_REPLACE;
				case StencilOp::INCREASE_CLAMP:
					return D3D12_STENCIL_OP_INCR_SAT;
				case StencilOp::DECREASE_CLAMP:
					return D3D12_STENCIL_OP_DECR_SAT;
				case StencilOp::INVERT:
					return D3D12_STENCIL_OP_INVERT;
				case StencilOp::INCREASE_WRAP:
					return D3D12_STENCIL_OP_INCR;
				case StencilOp::DECREASE_WRAP:
					return D3D12_STENCIL_OP_DECR;
				default:
					return D3D12_STENCIL_OP_KEEP;
				}
			}

			static D3D12_FILL_MODE MapFillMode(const FillMode& mode)
			{
				switch (mode)
				{
				case FillMode::SOLID:
					return D3D12_FILL_MODE_SOLID;
				case FillMode::WIREFRAME:
					return D3D12_FILL_MODE_WIREFRAME;
				default:
					return D3D12_FILL_MODE_SOLID;
				}
			}

			static D3D12_CULL_MODE MapCullMode(const CullMode& mode)
			{
				switch (mode)
				{
				case CullMode::NONE:
					return D3D12_CULL_MODE_NONE;
				case CullMode::FRONT:
					return D3D12_CULL_MODE_FRONT;
				case CullMode::BACK:
					return D3D12_CULL_MODE_BACK;
				default:
					return D3D12_CULL_MODE_BACK;
				}
			}

			static DXGI_FORMAT MapRenderFormat(RenderFormat format)
			{
				switch (format)
				{
				case RenderFormat::R32G32_FLOAT:
					return DXGI_FORMAT_R32G32_FLOAT;
				case RenderFormat::R32G32B32_FLOAT:
					return DXGI_FORMAT_R32G32B32_FLOAT;
				case RenderFormat::R8G8B8A8_UNORM:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case RenderFormat::R32G32B32A32_FLOAT:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case RenderFormat::D24_S8:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				default:
					return DXGI_FORMAT_R32G32B32_FLOAT;
				}
			}

			static RenderFormat MapRenderFormat(DXGI_FORMAT format)
			{
				switch (format)
				{
				case DXGI_FORMAT_R32G32B32_FLOAT:
					return RenderFormat::R32G32B32_FLOAT;
				case DXGI_FORMAT_R8G8B8A8_UNORM:
					return RenderFormat::R8G8B8A8_UNORM;
				case DXGI_FORMAT_R32G32B32A32_FLOAT:
					return RenderFormat::R32G32B32A32_FLOAT;
				case DXGI_FORMAT_D24_UNORM_S8_UINT:
					return RenderFormat::D24_S8;
				default:
					return RenderFormat::R32G32B32_FLOAT;
				}
			}

			static DXGI_FORMAT MapIndexType(IndexType type)
			{
				switch (type)
				{
				case IndexType::UINT16:
					return DXGI_FORMAT_R16_UINT;
				case IndexType::UINT32:
					return DXGI_FORMAT_R32_UINT;
				default:
					return DXGI_FORMAT_R16_UINT;
				}
			}

			static D3D12_PRIMITIVE_TOPOLOGY_TYPE MapPrimitiveType(PrimitiveType type)
			{
				switch (type)
				{
				case PrimitiveType::POINT_LIST:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
				case PrimitiveType::LINE_LIST:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case PrimitiveType::LINE_STRIP:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case PrimitiveType::TRIANGLE_LIST:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				case PrimitiveType::TRIANGLE_STRIP:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				default:
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
				}
			}

			static D3D12_RESOURCE_DIMENSION MapTextureDimension(TextureDimension dimension)
			{
				switch (dimension)
				{
				case TEXTURE_DIMENSION_1D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				case TEXTURE_DIMENSION_1D_ARRAY:
					return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				case TEXTURE_DIMENSION_2D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				case TEXTURE_DIMENSION_2D_ARRAY:
					return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				case TEXTURE_DIMENSION_3D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				default:
					return D3D12_RESOURCE_DIMENSION_UNKNOWN;
				}
			}

			static D3D12_SRV_DIMENSION MapSRVDimension(TextureDimension dimension, bool multisample)
			{
				switch (dimension)
				{
				case TEXTURE_DIMENSION_1D:
					return D3D12_SRV_DIMENSION_TEXTURE1D;
				case TEXTURE_DIMENSION_1D_ARRAY:
					return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				case TEXTURE_DIMENSION_2D:
				{
					if (multisample)
					{
						return D3D12_SRV_DIMENSION_TEXTURE2DMS;
					}
					else
					{
						return D3D12_SRV_DIMENSION_TEXTURE2D;
					}
				}
				case TEXTURE_DIMENSION_2D_ARRAY:
				{
					if (multisample)
					{
						return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
					}
					else
					{
						return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					}
				}
				case TEXTURE_DIMENSION_3D:
					return D3D12_SRV_DIMENSION_TEXTURE3D;
				default:
					return D3D12_SRV_DIMENSION_UNKNOWN;
				}
			}

			static D3D12_FILTER MapSamplerFilterMode(SamplerFilterMode mode)
			{
				switch (mode)
				{
				case SamplerFilterMode::Point:
					return D3D12_FILTER_MIN_MAG_MIP_POINT;
				case SamplerFilterMode::Linear:
					return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				case SamplerFilterMode::Trilinear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				case SamplerFilterMode::Anisotropic:
					return D3D12_FILTER_ANISOTROPIC;
				default:
					return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				}
			}

			static D3D12_TEXTURE_ADDRESS_MODE MapAddressMode(AddressMode mode)
			{
				switch (mode)
				{
				case AddressMode::Wrap:
					return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				case AddressMode::Mirror:
					return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				case AddressMode::Clamp:
					return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				case AddressMode::Border:
					return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				default:
					return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				}
			}
		};
	}
}