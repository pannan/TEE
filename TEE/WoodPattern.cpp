#include "WoodPattern.h"
#include "noise.h"
#include "noiseutils.h"

using namespace noise;

namespace TEE
{
	void	WoodPattern::make(int width, int height,bool seamless)
	{
		module::Cylinders baseWood;
		baseWood.SetFrequency(16.0);

		// Perlin noise to use for the wood grain.
		module::Perlin woodGrainNoise;
		woodGrainNoise.SetSeed(0);
		woodGrainNoise.SetFrequency(48.0);
		woodGrainNoise.SetPersistence(0.5);
		woodGrainNoise.SetLacunarity(2.20703125);
		woodGrainNoise.SetOctaveCount(3);
		woodGrainNoise.SetNoiseQuality(QUALITY_STD);

		// Stretch the Perlin noise in the same direction as the center of the
		// log.  This produces a nice wood-grain texture.
		module::ScalePoint scaledBaseWoodGrain;
		scaledBaseWoodGrain.SetSourceModule(0, woodGrainNoise);
		scaledBaseWoodGrain.SetYScale(0.25);

		// Scale the wood-grain values so that they may be added to the base wood
		// texture.
		module::ScaleBias woodGrain;
		woodGrain.SetSourceModule(0, scaledBaseWoodGrain);
		woodGrain.SetScale(0.25);
		woodGrain.SetBias(0.125);

		// Add the wood grain texture to the base wood texture.
		module::Add combinedWood;
		combinedWood.SetSourceModule(0, baseWood);
		combinedWood.SetSourceModule(1, woodGrain);

		// Slightly perturb the wood texture for more realism.
		module::Turbulence perturbedWood;
		perturbedWood.SetSourceModule(0, combinedWood);
		perturbedWood.SetSeed(1);
		perturbedWood.SetFrequency(4.0);
		perturbedWood.SetPower(1.0 / 256.0);
		perturbedWood.SetRoughness(4);

		// Cut the wood texture a small distance from the center of the "log".
		module::TranslatePoint translatedWood;
		translatedWood.SetSourceModule(0, perturbedWood);
		translatedWood.SetZTranslation(1.48);

		// Cut the wood texture on an angle to produce a more interesting wood
		// texture.
		module::RotatePoint rotatedWood;
		rotatedWood.SetSourceModule(0, translatedWood);
		rotatedWood.SetAngles(84.0, 0.0, 0.0);

		// Finally, perturb the wood texture to produce the final texture.
		module::Turbulence finalWood;
		finalWood.SetSourceModule(0, rotatedWood);
		finalWood.SetSeed(2);
		finalWood.SetFrequency(2.0);
		finalWood.SetPower(1.0 / 64.0);
		finalWood.SetRoughness(4);

		// Given the wood noise module, create a non-seamless texture map, a
		// seamless texture map, and a spherical texture map.
		//CreatePlanarTexture(finalWood, false, TEXTURE_HEIGHT,
		//	"textureplane.bmp");
		//CreatePlanarTexture(finalWood, true, TEXTURE_HEIGHT,
		//	"textureseamless.bmp");
		//CreateSphericalTexture(finalWood, TEXTURE_HEIGHT,
		//	"texturesphere.bmp");
		utils::NoiseMapBuilderPlane plane;
		utils::NoiseMap noiseMap;
		plane.SetBounds(-1.0, 1.0, -1.0, 1.0);
		plane.SetDestSize(width, height);
		plane.SetSourceModule(finalWood);
		plane.SetDestNoiseMap(noiseMap);
		plane.EnableSeamless(seamless);
		plane.Build();

		//RenderTexture(noiseMap, filename);
		// Create the color gradients for the texture.
		utils::RendererImage textureRenderer;
		//CreateTextureColor(textureRenderer);
		textureRenderer.ClearGradient();
		textureRenderer.AddGradientPoint(-1.00, utils::Color(189, 94, 4, 255));
		textureRenderer.AddGradientPoint(0.50, utils::Color(144, 48, 6, 255));
		textureRenderer.AddGradientPoint(1.00, utils::Color(60, 10, 8, 255));

		// Set up us the texture renderer and pass the noise map to it.
		utils::Image destTexture;
		textureRenderer.SetSourceNoiseMap(noiseMap);
		textureRenderer.SetDestImage(destTexture);
		textureRenderer.EnableLight(false);

		// Render the texture.
		textureRenderer.Render();

		// Write the texture as a Windows bitmap file (*.bmp).
		utils::WriterBMP textureWriter;
		textureWriter.SetSourceImage(destTexture);
		textureWriter.SetDestFilename("test_noise.bmp");
		textureWriter.WriteDestFile();
	}
}
