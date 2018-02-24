#pragma once
#include <vector>
#include "renderpass.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass():RenderPass(RENDERPASS_FORWARD){}
			void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		protected:
			struct RenderItem
			{
				RenderItem(const SharedGeometryPtr& geo, const SharedMaterialPtr& mtl):geometry(geo), material(mtl){}
				SharedGeometryPtr geometry;
				SharedMaterialPtr material;
			};
			using RenderItemList = std::vector<RenderItem>;
			RenderItemList m_renderItems;
		};
	}
}
