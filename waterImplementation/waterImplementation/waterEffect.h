#pragma once

class waterEffect {
	public:

		waterEffect(int iObj,int iShader,int iImg,int iImgSize,float iHeight);
		~waterEffect(void);

		void UpdateWater();
		void UpdateRefraction();
		void UpdateReflection();

	private:

		int	WaterImage;
		int WaterObject;
		int WaterShader;
		float WaterHeight;
};
