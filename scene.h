
#ifndef __SCENE_H
#define __SCENE_H

#include <hgeSprite.h>
#include <hgedistort.h>
#include "Utility/GeometryTools.h"

#define RELEASE(x,y) delete x;hge->Texture_Free(y);
#define DIMENTION 10
#define MAX_OBJECTS 1000

#define WINDOW_W 800
#define WINDOW_H 600

#define SCENE_TOTALNUM 128

#define PTARRAY 32
#define PTUSED 4

#define SCENE_NODE 16
#define MESHCOL 16
#define MESHROW 16

#define SCROLL_INTERVAL_Y 2.0f
#define SCROLL_INTERVAL_X 2.0f

#define ABS(x)  (( x < 0 ) ? x : -x)


enum ENTITYBEHAVIOR
{
	DEFAULT = 0,
	LEFT_KEY = 256,
    RIGHT_KEY = 2,
    UP_KEY = 4,
    DOWN_KEY = 8,
    UNRESTRAINED = 16,
    IDLE = 32,
	FRAMEIMAGE = 64,
	COLLISION = 128,

	SCENE_SCROLL_UP = 512,
	SCENE_SCROLL_DOWN = 1024,
	SCENE_SCROLL_LEFT = 2048,
	SCENE_SCROLL_RIGHT = 4096
};

enum EETSTATUS
{
	INSCENE = 0,
    OUTSCENE = 2,

    HAVE_VOLUME = 4,
    NO_VOLUME = 8,
	LEAVING_SCENE = 16,
	MOVING,
	CEASE
};

enum EETLEVEL
{
	ONE = 0,
	TWO,
	SCENE
};

enum EENTITYS
{
	HGE_DISTORTIONMESH,
	HGE_SPRITE,
	
	ETBUBBLE,
    ETLEADER01,
	ETLEADER02,
	ETSCENE01,
	ETSCENE02
};


class XScene;
class XEntity
{
	friend class XScene;
public:
	XEntity( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, 
		int iFrame = 4,  EENTITYS eType = HGE_SPRITE, int iMeshCol = MESHCOL, int MeshRow = MESHROW );
	~XEntity();
	virtual bool update( ENTITYBEHAVIOR eBe )= 0;
	virtual void setScene( XScene* pScene);
	virtual bool render() = 0;
	void setPos( float fX, float fY );
	float getPosX() { return m_fPosX; }
	float getPosY() { return m_fPosY; }
	virtual ENTITYBEHAVIOR onBehaviors( int iBes ) = 0 ;
	virtual bool frameImage();
	virtual bool collisionChain( XEntity* pCollisionA );

protected:
	XEntity() {};
	XEntity* m_pEtNext;

	union Unode
	{
		hgeSprite* pSr;
		hgeDistortionMesh* pMesh;
	};

	Unode m_pSr; 
	XScene* m_pScene;
	float m_fPosX;
	float m_fPosY;
	float m_fRot;

	float m_fWidth;
	float m_fHeight;
	GeometryTools::Polyline m_vecPtList;

	float m_fStartXTex;
	float m_fStartYTex;
	EETSTATUS m_eStatus;

private:

	HTEXTURE m_tex; 
	float m_fmass;

	int m_iFrame;
};

class XScene
{
public:
	XScene();
	~XScene();
	bool addEntity( EENTITYS eType, EETLEVEL eLev );
	bool render();
    bool update( int iBe );
    void setPassTime( float fElapseT );
	float getElapseTime();
	float getTotalTime();
	XEntity* getCurrentScene();
	EETSTATUS getCurrentSceneStatus();
	void addScenetoList( XEntity* pScene );
	void setCurrentScene( int iPos );
	bool isInWalkingZone();
	bool isInWalkingZone( Point pt );

private:
	bool collisionDetect( XEntity* pEntityA, XEntity* pEntityB
		/*, int iEntityNumA = 1, int iEntityNumB = 1*/ );
	Point windowToScene( const Point pt );
	XEntity* m_pListEntity;
	XEntity* m_ppEtCollision[DIMENTION][MAX_OBJECTS] /*= { NULL }*/;
	XEntity* m_pSceneList[ SCENE_TOTALNUM ];
	float m_fTotalTime;
	float m_fElapse;
	int m_iBehaviors;

	GeometryTools::Polyline walkingZone;

	int m_iSceneNum;
	int m_iScenePos;
};

class XEtLder : public  XEntity
{
public:
	XEtLder( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight,
		int iFrame = 4, EENTITYS eType = HGE_DISTORTIONMESH );
	~XEtLder();
	virtual bool update( ENTITYBEHAVIOR eBe );
	virtual bool render();
	virtual ENTITYBEHAVIOR onBehaviors( int iBes );
	virtual bool frameImage();
private:
	hgeDistortionMesh* m_pParticialMesh;
	HTEXTURE m_partTex;

	float m_fPartPosX;
	float m_fPartPosY;
	int m_iCols;
	int m_iRows;
	float m_fCellW;
	float m_fCellH;
};

class XEtBubble: public XEntity
{
public:
	XEtBubble( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, int iFrame = 4 );
	~XEtBubble( );
	virtual bool update( ENTITYBEHAVIOR eBe);
	virtual bool render();
	virtual ENTITYBEHAVIOR onBehaviors( int iBes );
	virtual bool frameImage();
private:
	float m_fScale;
	float m_fInterval;
    float m_fVelocity;
	float m_fAmplitude;

	float m_fElapseT[2];
	float m_fDy;
	float m_fTime;
};

class XEtScene: public XEntity
{
public:
	XEtScene( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, int iFrame = 1);
	~XEtScene( );
	virtual bool update( ENTITYBEHAVIOR eBe);
	virtual bool render();
	virtual ENTITYBEHAVIOR onBehaviors( int iBes );
	virtual bool frameImage();
    bool addMeshNode ( const char* szTexPath, float fMeshX, float fMeshY, float fMeshW, float fMeshH,
		float fPosX, float fPosY );
private:
	hgeDistortionMesh* m_meshArray[ SCENE_NODE ];
	int m_iMesh;
	HTEXTURE m_texArray[ SCENE_NODE ];
	Point m_meshPosArray[ SCENE_NODE ];
};
#endif