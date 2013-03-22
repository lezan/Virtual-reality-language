#include "waterEffect.h"
#include "DarkGDK.h"

#define firstImplementation 1
#define secondImplementation 0

#if(firstImplementation==1) 
	waterEffect::waterEffect(int iObj,int iShader,int iImg,int iImgSize,float iHeight) {
		WaterImage=iImg;
		WaterObject=iObj;
		WaterShader=iShader;
		WaterHeight=iHeight;

		//Setup Cameras
		float tSw=dbScreenWidth();
		float tSh=dbScreenHeight();
		for(int c=1;c<3;c++) 		{
			dbMakeCamera(c);
			dbSetCameraRange(c,1,15000);
			dbSetCameraAspect(c,tSw/tSh);
			dbBackdropOff(c);
			dbSetCameraToImage(c,iImg+(c-1),iImgSize,iImgSize);
			dbSetCameraFOV(c,85);
		}

		//Make Water plain
		dbLoadImage("water\\waves.dds",iImg+2);
		dbLoadImage("water\\Fresnel.bmp",iImg+3);
		dbLoadImage("mappa\\1\\WaterMask.bmp",iImg+4);
		dbMakeObjectPlain(WaterObject,5000,5000);
		dbTextureObject(WaterObject,0,iImg);
		dbTextureObject(WaterObject,1,iImg+1);
		dbTextureObject(WaterObject,2,iImg+2);
		dbTextureObject(WaterObject,3,iImg+3);
		dbTextureObject(WaterObject,4,iImg+4);
		dbXRotateObject(WaterObject,270);
		dbLoadEffect("water\\Water.fx",WaterShader,0);
		dbSetObjectEffect(WaterObject,WaterShader);
		dbSetObjectTransparency(WaterObject,1);

		dbPositionObject(WaterObject,0,0,0);
	}

	//----------------------------------------------------
	// Delete Water (Destructor)
	//----------------------------------------------------
	waterEffect::~waterEffect() {
		dbDeleteCamera(1);
		dbDeleteCamera(2);
		dbDeleteObject(WaterObject);
		dbDeleteEffect(WaterShader);
		dbDeleteEffect(WaterShader);
		dbDeleteImage(WaterImage);
		dbDeleteImage(WaterImage+1);
		dbDeleteImage(WaterImage+2);
		dbDeleteImage(WaterImage+3);
		dbDeleteImage(WaterImage+4);
	}

	//----------------------------------------------------
	// Update Water
	//----------------------------------------------------
	void waterEffect::UpdateWater() {
		//UpDate Water plain
		dbPositionObject(WaterObject,dbObjectPositionX(WaterObject),WaterHeight,dbObjectPositionZ(WaterObject));
		dbHideObject(WaterObject);

		//Upade Refract/Reflect cameras
		dbSetEffectTechnique(WaterShader,"Refract");
		UpdateRefraction();
		if(dbCameraPositionY()>WaterHeight) 		{
			dbSetEffectTechnique(WaterShader,"ReflectRefract");
			UpdateReflection();
		}
		else {
			dbFogOn();
			dbFogColor(dbRGB(55,65,75));
			dbFogDistance(2000);
		}

		//Show Water
		dbShowObject(WaterObject);
		dbSyncMask(1 << 0);
	}

	//----------------------------------------------------
	// Refraction Update
	//----------------------------------------------------
	void waterEffect::UpdateRefraction() {
		dbPositionCamera(1,dbCameraPositionX(),dbCameraPositionY(),dbCameraPositionZ());
		dbRotateCamera(1,dbCameraAngleX(),dbCameraAngleY(),dbCameraAngleZ());
		if(dbCameraPositionY()>WaterHeight) {
			dbSetCameraClip(1,1,0,WaterHeight+30,0,0,-1,0);
			dbFogOn();
			dbFogColor(dbRGB(55,65,75));
			dbFogDistance(250+(dbCameraPositionY()-WaterHeight)*10);
		}
		else {
			dbFogOn();
			dbFogColor(dbRGB(55,65,75));
			dbFogDistance(2000);
		}
		dbSetCameraClip(2,0,0,0,0,0,0,0);
		dbSyncMask(1 << 1);
		dbFastSync();
		dbFogOff();
	}

	//----------------------------------------------------
	// Reflection Update
	//----------------------------------------------------
	void waterEffect::UpdateReflection() {
		dbPositionCamera(2,dbCameraPositionX(),WaterHeight-(dbCameraPositionY()-WaterHeight),dbCameraPositionZ());
		dbRotateCamera(2,-dbCameraAngleX(),dbCameraAngleY(),dbCameraAngleZ());
		dbSetCameraClip(1,0,0,0,0,0,0,0);
		dbSetCameraClip(2,1,0,WaterHeight-15,0,0,1,0);
		dbSyncMask(1 << 2);
		dbFastSync();
	}


#endif
#if(secondImplementation==1) 
	//`----------------------------------------------------
	//` Create Water
	//`----------------------------------------------------
	//function MakeWater(Obj,Shader,Img,ImgSize,Height)
	void waterEffect::waterEffect(int Obj, int Shader, int Img, int ImgSize, int Height) {
	   //`Globals
		  WaterObj=Obj;
		  WaterHeight=Height;
		  Fx=Shader;

	  // `Setup Cameras
		  Sw = dbScreenWidth(); 
		  Sh = dbScreenHeight();
		  for(int c = 1; c <= 2; c++) {
			 dbMakeCamera(c);
			 dbSetCameraRange(c, 1, 15000);
			 dbSetCameraAspect(c, (Sw / Sh));
			 dbBackdropOff(c);
			 dbSetCameraToImage(c, (Img+(c-1)), ImgSize, ImgSize);
			 dbSetCameraFOV(c, 70);
		  }

	   //`Make Water plain
		  dbLoadImage("water\\Waves.dds", Img+2);
		  dbLoadImage("water\\Fresnel.bmp", Img+3);
		  dbLoadImage("mappa\\1\\WaterMask.bmp", Img+4);
		  dbMakeObjectPlain(WaterObj, 5000, 5000);
		  dbTextureObject(WaterObj, 0, Img);
		  dbTextureObject(WaterObj, 1, Img+1);
		  dbTextureObject(WaterObj, 2, Img+2);
		  dbTextureObject(WaterObj, 3, Img+3);
		  dbTextureObject(WaterObj, 4, Img+4);
		  dbXRotateObject(WaterObj, 270);
		  dbLoadEffect("water\\Water.fx", Fx, 0);
		  dbSetObjectEffect(WaterObj, Fx);
		  dbSetObjectTransparency(WaterObj, 1);
		  dbSetObjectDiffuse(WaterObj, dbRGB(25, 125, 150));

	}

	//`----------------------------------------------------
	//` Update Water
	//`----------------------------------------------------
	//function UpDateWater()
	void waterEffect::UpdateWater(void) {

	   //`UpDate Water plain
		  dbPositionObject(WaterObj, dbObjectPositionX(WaterObj), WaterHeight, dbObjectPositionZ(WaterObj));
		  dbHideObject(WaterObj);

	   //`Upade Refract/Reflect cameras
		  dbSetEffectTechnique(Fx, "Refract");
		  UpDateRefraction();
		  if(dbCameraPositionY() > WaterHeight) {
			 dbSetEffectTechnique(Fx, "ReflectRefract");
			 UpDateReflection();
		  }
		  else {
			 dbFogOn(); 
			 dbFogColor(dbRGB(55, 65, 75));
			 dbFogDistance(2000);
		  }

	   //`Show Water
		  dbShowObject(WaterObj);
		  dbSyncMask( 0x00000001 );
		  //dbSyncMask(2^0);
	}

	//`----------------------------------------------------
	//` Refraction Update
	//`----------------------------------------------------
	//function UpDateRefraction()
	void waterEffect::UpdateRefraction(void) {
	   dbPositionCamera(1, dbCameraPositionX(), dbCameraPositionY(), dbCameraPositionZ());
	   dbRotateCamera(1, dbCameraAngleX(), dbCameraAngleY(), dbCameraAngleZ());

	   if(dbCameraPositionY() > WaterHeight) {
		  dbSetCameraClip(1, 1, 0, (WaterHeight + 30), 0, 0, -1, 0);
		  dbFogOn();
		  dbFogColor(dbRGB(55, 65, 75));
		  dbFogDistance(250 + (dbCameraPositionY() - WaterHeight) * 10);
	   }

	   if(dbCameraPositionY() < WaterHeight) {
		  dbSetCameraClip(1, 1, 0, (WaterHeight - 30), 0, 0, 1, 0);
		  dbFogOn();
		  dbFogColor(dbRGB(55, 65, 75));
		  dbFogDistance(2000);
	   }
	   dbSetCameraClip(2, 0, 0, 0, 0, 0, 0, 0);
	   dbSyncMask( 0x00000006 );
	   dbFastSync();
	   dbFogOff();
	}

	//`----------------------------------------------------
	//` Reflection Update
	//`----------------------------------------------------
	//function UpDateReflection()
	void waterEffect::UpdateReflection(void) {
	   dbPositionCamera(2, dbCameraPositionX(), (WaterHeight - (dbCameraPositionY() - WaterHeight)), dbCameraPositionZ());
	   dbRotateCamera(2, -dbCameraAngleX(), dbCameraAngleY(), dbCameraAngleZ());
	   dbSetCameraClip(1, 0, 0, 0, 0, 0, 0, 0);
	   dbSetCameraClip(2, 1, 0, (WaterHeight - 15), 0, 0, 1, 0);
	   dbSyncMask( 0x00000006 );
	   dbFastSync();
	}
#endif