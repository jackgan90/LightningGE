#pragma once
#include "renderpass.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API DeferedRenderPass : public RenderPass
		{
		public:	
			DeferedRenderPass():RenderPass(RenderPassType::DEFERED){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
			void OnAddRenderNode(const RenderNode& node)override;
			void OnBeginFrame()override;
		};
	}
}
