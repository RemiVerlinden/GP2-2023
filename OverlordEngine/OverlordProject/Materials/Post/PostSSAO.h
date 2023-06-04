#pragma once

	class PostSSAO : public PostProcessingMaterial
	{
	public:
		PostSSAO();
		~PostSSAO() override = default;
		PostSSAO(const PostSSAO& other) = delete;
		PostSSAO(PostSSAO&& other) noexcept = delete;
		PostSSAO& operator=(const PostSSAO& other) = delete;
		PostSSAO& operator=(PostSSAO&& other) noexcept = delete;
		
	protected:
		void Initialize(const GameContext& /*gameContext*/) {};
		virtual void UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource) override;
	private:
	};

