#pragma once

	class PostChromatic : public PostProcessingMaterial
	{
	public:
		PostChromatic();
		~PostChromatic() override = default;
		PostChromatic(const PostChromatic& other) = delete;
		PostChromatic(PostChromatic&& other) noexcept = delete;
		PostChromatic& operator=(const PostChromatic& other) = delete;
		PostChromatic& operator=(PostChromatic&& other) noexcept = delete;
		
	protected:
		void Initialize(const GameContext& /*gameContext*/) {};
	private:
	};

