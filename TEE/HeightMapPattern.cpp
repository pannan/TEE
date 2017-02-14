#include "HeightMapPattern.h"
#include "noise.h"
#include "noiseutils.h"

using namespace noise;

namespace TEE
{
	void HeightMapPattern::make(int width, int height, bool seamless)
	{
		module::Perlin myModule;
		utils::NoiseMap heightMap;
		utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(myModule);
		heightMapBuilder.SetDestNoiseMap(heightMap);
		heightMapBuilder.SetDestSize(width, height);
		heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
		heightMapBuilder.Build();

		utils::RendererImage renderer;
		utils::Image image;
		renderer.SetSourceNoiseMap(heightMap);
		renderer.SetDestImage(image);
		renderer.Render();

		utils::WriterBMP writer;
		writer.SetSourceImage(image);
		writer.SetDestFilename("heightmap.bmp");
		writer.WriteDestFile();
	}
}