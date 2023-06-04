#pragma once

	class PostColorGrading : public PostProcessingMaterial
	{
	public:
		PostColorGrading();
		~PostColorGrading() override = default;
		PostColorGrading(const PostColorGrading& other) = delete;
		PostColorGrading(PostColorGrading&& other) noexcept = delete;
		PostColorGrading& operator=(const PostColorGrading& other) = delete;
		PostColorGrading& operator=(PostColorGrading&& other) noexcept = delete;
		
	protected:
		void Initialize(const GameContext& /*gameContext*/) override;

	private:
		TextureData* m_pLUT{};
	};

