#pragma 

namespace TEE
{
	class HeightMapPattern
	{
	public:

		HeightMapPattern() :m_noiseTexSRVID(0), m_width(256),m_height(256){}
		~HeightMapPattern(){}

		void		make(int width, int height, bool seamless);

		void		render();

		void		createNoiseTexture();

	protected:



	protected:

		int			m_width;
		int			m_height;

		Glyph3::ResourcePtr		m_noiseTex;
		int								m_noiseTexSRVID;

	private:
	};
}