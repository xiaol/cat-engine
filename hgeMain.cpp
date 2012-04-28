/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hge_tut01 - Minimal HGE application
*/


#include <hge.h>
#include <hgeSprite.h>
#include <stdio.h>
#include "scene.h"

#define PLANT_NUM 3

HGE *hge = 0;
hgeSprite* gSpleader;
hgeSprite* gSpBack;
hgeSprite* gSpPlants[PLANT_NUM];
HTEXTURE gTex;
HTEXTURE gBackTex;
HTEXTURE gPlantsTex[PLANT_NUM];

XScene* g_pScene = NULL;

#define RELEASE(x,y) delete x;hge->Texture_Free(y);

const float fLeaderW = 42.0f;
const float fLeaderH = 20.0f;
const int iLeaderFrame = 8;
float fArray[] = { 0,0, 0,20,0,40,0,60 };
float fPos[2] = { 600.0f,400.0f };
float fLeaderRot = 0.0f;
bool g_bLeaderInit = false;
float fStep = 0.5;

bool initScene()
{
	gTex = hge->Texture_Load("media/leader03.png");
	gSpleader = new hgeSprite( gTex,0,0,40,80 );
	gSpleader->SetBlendMode( BLEND_DEFAULT_Z );
    gSpleader->SetZ( 0.5 );

    gBackTex = hge->Texture_Load( "media/scene02.png" );
    gSpBack = new hgeSprite( gBackTex, 0,0,800,600 );
    gSpBack->SetBlendMode( BLEND_DEFAULT_Z );
	//gSpleader->SetTexture( gTex );
	gSpBack->SetZ( 1.0 );

	char szFileName[256] = "media/";
	for( int i = 0; i < PLANT_NUM; i++ )
    {
		sprintf_s( szFileName, "media/plants0%d.png", i+1 );
		gPlantsTex[i] = hge->Texture_Load( szFileName );
        gSpPlants[i] = new hgeSprite( gPlantsTex[i], 0,0, WINDOW_W, WINDOW_H );
		gSpPlants[i]->SetBlendMode( BLEND_DEFAULT_Z );
		//gSpPlants[i]->SetZ( 0.1f + 0.5f*i  );
    }

	g_pScene = new XScene;
	g_pScene->addEntity( ETSCENE01, SCENE);
	
	for ( int i = 0; i < 20; i++ )
	{
		g_pScene->addEntity( ETBUBBLE, ONE );
	}
	
	g_pScene->addEntity( ETLEADER01, TWO );

	return false;
}

bool endScene()
{
	RELEASE( gSpleader, gTex )
	RELEASE( gSpBack, gBackTex )
    for( int i = 0; i < PLANT_NUM; i++ )
	{ RELEASE( gSpPlants[i], gPlantsTex[i] ) }
    return false;
}
// This function will be called by HGE once per frame.
// Put your game loop code here. In this example we
// just check whether ESC key has been pressed.
bool FrameFunc()
{
	// By returning "true" we tell HGE
	// to stop running the application.
    static float sfIntervalTime = 0.0;
    sfIntervalTime += hge->Timer_GetDelta();

	float fElapse = hge->Timer_GetDelta();
	g_pScene->setPassTime( fElapse );
	g_pScene->update( IDLE );

	int iBe = 0;
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_GetKeyState(HGEK_LEFT)) 
	{ 
		fPos[0] -= fStep;
		if ( !g_pScene->isInWalkingZone() )
			fPos[0] += fStep;
		
		if ( fLeaderRot > PI ) 
			fLeaderRot += (2.0f*PI - fLeaderRot)/4.0f;
		else
			fLeaderRot += (0.0f - fLeaderRot)/4.0f; 
		sfIntervalTime = 0.0f;
		iBe |= LEFT_KEY;
	}
	if (hge->Input_GetKeyState(HGEK_RIGHT)) 
	{ 
		fPos[0] += fStep;
		if ( !g_pScene->isInWalkingZone() )
			fPos[0] -= fStep;
		fLeaderRot += (PI - fLeaderRot)/4.0f; 
		sfIntervalTime = 0.0f;
		
		iBe |= RIGHT_KEY;
	}
	if (hge->Input_GetKeyState(HGEK_UP))
	{ 
		fPos[1] -= fStep;
		
		if( fLeaderRot > 1.5f*PI && fLeaderRot < 1.8f*PI )
			fLeaderRot += (2.0f*PI - fLeaderRot)/4.0f;
		else if ( fLeaderRot >= 1.8f*PI )
		{	
			fLeaderRot = 0.0f;
			//fLeaderRot += (0.5f*PI - fLeaderRot)/16.0f; 
		}
		else
			fLeaderRot += (0.5f*PI - fLeaderRot)/4.0f; 
		sfIntervalTime = 0.0f;

		iBe |= UP_KEY;
	}
	if (hge->Input_GetKeyState(HGEK_DOWN))
	{ 
		fPos[1] += fStep;
		
		if ( !g_pScene->isInWalkingZone() )
			fPos[1] -= fStep; 
		if ( fLeaderRot > 0.1f*PI && fLeaderRot < 0.5f*PI )
			fLeaderRot += (0.0f - fLeaderRot)/4.0f;
		else if ( fLeaderRot <= 0.1f*PI )
			fLeaderRot = 2.0*PI;
		else
			fLeaderRot += ( (1.5f)*PI - fLeaderRot)/4.0f;
		sfIntervalTime = 0.0f;

		iBe |= DOWN_KEY;
	}

	g_pScene->update( iBe );

	static float eclapseTime = 0.0;
    eclapseTime += hge->Timer_GetDelta(); 


    static int iCount = 2;
    if ( eclapseTime > ( 0.04f + sfIntervalTime*(0.5f - 0.07f)/10.0f ) && sfIntervalTime < 2.0f )
    {
		gSpleader->SetTextureRect( fArray[iCount],fArray[iCount+1],fLeaderW, fLeaderH  );
		//gSpleader->SetHotSpot(  fArray[iCount] + (fLeaderW / 2),fArray[iCount+1] + (fLeaderH / 2 )  );
        iCount += 2;
        iCount %= iLeaderFrame;
		eclapseTime = 0.0;
    } 
    	// Continue execution
	return false;
}

bool gradiantAlpha( hgeSprite* pSr, bool bDirect )
{
	static float s_fTime = 0.0f;
	static bool s_bInit = true;
	static int s_iAlpha = bDirect?0:255;

	s_fTime += hge->Timer_GetDelta();
	int iInterval = 30;
	int iDirect = bDirect?1:(-1);
	bool bEnd = false;
	

	if ( s_bInit )
	{
		s_fTime = 0.0f;
		s_iAlpha = bDirect?0:255;
		pSr->SetColor( SETA( pSr->GetColor(), s_iAlpha ) );
		s_bInit = false;
		return bEnd;
	}

	if ( s_fTime > 0.2f  )
	{
		s_iAlpha +=  iDirect*iInterval;
		if ( bDirect )
		{
			if ( s_iAlpha > 255 )
			{
				s_iAlpha = 255;
				bEnd = true;
				s_bInit = true;
			}
		}
		else
		{
			if ( s_iAlpha < 0 )
			{
				s_iAlpha = 0;
				bEnd = true;
				s_bInit = true;
			}
		}
		pSr->SetColor( SETA( pSr->GetColor(), s_iAlpha ) );
		s_fTime = 0.0f;
	}
	return bEnd;
}

bool renderFunc()
{
	hge->Gfx_BeginScene(); 
	hge->Gfx_Clear( 0 );
	//gSpBack->Render( 0.0 , 0.0 );
	
	g_pScene->render();

	
	{
		static bool s_bEnd = false;
		if ( !s_bEnd && g_bLeaderInit )
			s_bEnd = gradiantAlpha( gSpleader, true );

		if ( g_bLeaderInit )
			gSpleader->RenderEx( fPos[0],fPos[1], fLeaderRot );
	}
	
// 	for( int i = 0; i < PLANT_NUM; i++ )
// 		gSpPlants[i]->Render( 0.0,0.0 );
	hge->Gfx_EndScene();
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Here we use global pointer to HGE interface.
	// Instead you may use hgeCreate() every
	// time you need access to HGE. Just be sure to
	// have a corresponding hge->Release()
	// for each call to hgeCreate()
	hge = hgeCreate(HGE_VERSION);

	// Set our frame function
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, renderFunc);

	// Set the window title
	hge->System_SetState(HGE_TITLE, "Mercury");
	
	// Run in windowed mode
	// Default window size is 800x600
	hge->System_SetState(HGE_WINDOWED, true);

	// Don't use BASS for sound
	hge->System_SetState(HGE_USESOUND, false);

    //hge->System_SetState( HGE_ZBUFFER, true );

	// Tries to initiate HGE with the states set.
	// If something goes wrong, "false" is returned
	// and more specific description of what have
	// happened can be read with System_GetErrorMessage().
	if(hge->System_Initiate())
	{
		// Starts running FrameFunc().
		// Note that the execution "stops" here
		// until "true" is returned from FrameFunc().
        initScene();
		hge->System_Start();
	}
	else
	{	
		// If HGE initialization failed show error message
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	endScene();
	// Now ESC has been pressed or the user
	// has closed the window by other means.
	
	// Restore video mode and free
	// all allocated resources
	hge->System_Shutdown();
	
	// Release the HGE interface.
	// If there are no more references,
	// the HGE object will be deleted.
	hge->Release();

	return 0;
}
